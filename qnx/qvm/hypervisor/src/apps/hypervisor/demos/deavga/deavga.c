#include <sys/iomgr.h>
#include <sys/siginfo.h>
#include <hyp_shm.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <screen/screen.h>
#include <sys/dispatch.h>
#include <dlfcn.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <stdbool.h>

#include <WF/wfd.h>

#include <wfdqnx/wfdcfg.h>
#include <wfdqnx/wfdcfg_rcar3.h>

#include <sys/slog.h>
#include <sys/slogcodes.h>

#define SLOG_DEBUG(x, ...)      //slogf(_SLOGC_GRAPHICS_DISPLAY, _SLOG_DEBUG1,  "[c5p49]DEBG: " x, ##__VA_ARGS__)
#define SLOG_INFO(x, ...)       slogf(_SLOGC_GRAPHICS_DISPLAY, _SLOG_INFO,    "[c5p49]INFO: " x, ##__VA_ARGS__)
#define SLOG_WARNING(x, ...)    slogf(_SLOGC_GRAPHICS_DISPLAY, _SLOG_WARNING, "[c5p49]WARN: " x, ##__VA_ARGS__)
#define SLOG_ERROR(x, ...)      slogf(_SLOGC_GRAPHICS_DISPLAY, _SLOG_ERROR,   "[c5p49]ERR : " x, ##__VA_ARGS__)

/* Product Register */
#define RCAR_PRR                          0xFFF00044
#define RCAR_PRR_REGSIZE          0x04
#define RCAR_PRODUCT_ID(x)        ((x & 0x7F00) >> 8)
#define RCAR_PRODUCT_H3           0x4F
#define RCAR_PRODUCT_M3W        0x52
#define RCAR_PRODUCT_M3N         0x55

#define RCAR_REVISION_MASK      0xFF
#define RCAR_PRODUCT_REV(x)     (x & 0xFF)
#define RCAR_REVISION_ES10        0x00
#define RCAR_REVISION_ES11        0x01
#define RCAR_REVISION_ES20        0x10

static const char   *devname = "/dev/i2c4";  /* name registered by i2c */
static int          idt_i2c_fd = -1;

struct OMSG
{
	i2c_send_t  hdr;        /* Message header */
	uint8_t     bytes[15];   /* Transmit buffer with (optional) extra space */
} omsg_t;

struct IMSG
{
	i2c_recv_t  hdr;        /* Message header */
	uint8_t     bytes[15];  /* Receive buffer */
} imsg_t;

enum
{
	C5P49_CLK_OUT1      = 2,
	C5P49_CLK_OUT2      = 3,
	C5P49_CLK_OUT3      = 4,
	C5P49_CLK_OUT4      = 5,
};

#define C5P49_I2C_SLAVE_ADDR    0x6A

/* some registers */
#define C5P49_CLK_OE_SHUTDOWN   0x68
#define C5P49_FB_INT_DIV_REG1   0x17
#define C5P49_FB_INT_DIV_REG0   0x18

/* offset address*/
#define C5P49_OUT_DIV_CTRL      0x01
#define C5P49_DIV_FRAC_29_22    0x02
#define C5P49_DIV_FRAC_21_14    0x03
#define C5P49_DIV_FRAC_13_6     0x04
#define C5P49_DIV_FRAC_5_0      0x05
#define C5P49_DIV_INTEGER_11_4  0x0D
#define C5P49_DIV_INTEGER_3_0   0x0E

#define INPUT_CLK_5p49v5923a    25000000
#define INPUT_CLK_5p49v6901a    50000000
#define OUTPUT_DIVIDER_MODE     0x81

#define clk_5p49_write(index, addr, val) \
	clk_5p49_i2c_write(addr + (0x10 * index), val)

#define clk_5p49_read(index, addr) \
	clk_5p49_i2c_read(addr + (0x10 * index))

int clk_5p49_i2c_write(uint8_t reg, uint8_t val)
{
	int         status;
	struct OMSG omsg;

	if(idt_i2c_fd < 0)
		return -1;

	omsg.hdr.slave.addr = C5P49_I2C_SLAVE_ADDR;
	omsg.hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	omsg.hdr.len = 2;
	omsg.hdr.stop = 1;
	omsg.bytes[0] = reg;
	omsg.bytes[1] = val;

	status = devctl(idt_i2c_fd, DCMD_I2C_SEND, &omsg, sizeof(omsg.hdr) + omsg.hdr.len, NULL);

	if (status != EOK)
	{
		errno = status;
		perror("I2C_SEND");
		exit(-1);
	}

	return 0;
}

uint8_t clk_5p49_i2c_read(uint8_t reg)
{
	int             status;
	iov_t           siov[2], riov[2];
	unsigned char   data;
	i2c_sendrecv_t  hdr;    /* Message header */

	if (idt_i2c_fd < 0)
		return -1;

	//Write register address which is need to be read.
	hdr.slave.addr = C5P49_I2C_SLAVE_ADDR;
	hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	hdr.stop = 1;

	hdr.send_len = 1;
	hdr.recv_len = 1;

	SETIOV(&siov[0], &hdr, sizeof(hdr) );
	SETIOV(&siov[1], &reg, 1 );

	SETIOV(&riov[0], &hdr, sizeof(hdr) );
	SETIOV(&riov[1], &data, 1 );
	status = devctlv(idt_i2c_fd, DCMD_I2C_SENDRECV, 2, 2, siov, riov, NULL);
	if (status != EOK)
	{
		errno = status;
		perror("I2C_READ");
		return 0xFF;
	}

	return data;
}

void clk_5p49_power(uint8_t index, bool power)
{
	uint8_t reg;

	if (power)
	{
		reg = clk_5p49_i2c_read(C5P49_CLK_OE_SHUTDOWN);
		reg |= (0x80 >> (index - 1));
		clk_5p49_i2c_write(C5P49_CLK_OE_SHUTDOWN, reg);

		/* Set Output Divider Control Register:
		 * VersaClock 5 User Guide says that the default setting for Output Divider Control Register 1, 2, 3 and 4 is 0x81,
		 * which means "use output's divider 1, 2, 3, 4 settings respectively". But in fact, by default Output Divider Control Register 4
		 * is set to 0x0C (use previous channel's clock output), other Output Divider Control Registers are set to 0x81.
		 * So we need to set all the Output Divider Control Registers to 0x81.
		 */
		clk_5p49_write(index, C5P49_OUT_DIV_CTRL, OUTPUT_DIVIDER_MODE);
	}
	else
	{
		reg = clk_5p49_i2c_read(C5P49_CLK_OE_SHUTDOWN);
		reg &= ~(0x80 >> (index - 1));
		clk_5p49_i2c_write(C5P49_CLK_OE_SHUTDOWN, reg);
	}
}

int clk_5p49_div_calculation(int rate, uint8_t index)
{
	int integ_div, frac_div, div, vco_div, vco_clk;
	uint32_t shift_1kHz = 1000;
	uint8_t frac_0, frac_1, frac_2, frac_3;

	uint32_t *prr_reg;
	if ((prr_reg = (void*)mmap_device_io (RCAR_PRR_REGSIZE, RCAR_PRR)) == MAP_FAILED) {
		SLOG_ERROR("Unable mapping PRR.");
		return -1;
	}

	vco_div = ((clk_5p49_i2c_read(C5P49_FB_INT_DIV_REG0) & 0xF0) >> 4)
			+ (clk_5p49_i2c_read(C5P49_FB_INT_DIV_REG1) << 4);
	SLOG_DEBUG("vco_div value: %d", vco_div);

	/* disable CLK_OE */
	clk_5p49_power(index, false);

	if (RCAR_PRODUCT_ID(*prr_reg) == RCAR_PRODUCT_H3){
		if ((RCAR_PRODUCT_REV(*prr_reg) == RCAR_REVISION_ES10) || (RCAR_PRODUCT_REV(*prr_reg) == RCAR_REVISION_ES11)){
			vco_clk = INPUT_CLK_5p49v5923a * vco_div / shift_1kHz;
		}
		else if (RCAR_PRODUCT_REV(*prr_reg) == RCAR_REVISION_ES20){
			vco_clk = INPUT_CLK_5p49v6901a * vco_div / shift_1kHz;
		}
	}
	else if (RCAR_PRODUCT_ID(*prr_reg) == RCAR_PRODUCT_M3W){
		if (RCAR_PRODUCT_REV(*prr_reg) == RCAR_REVISION_ES10){
			vco_clk = INPUT_CLK_5p49v5923a * vco_div / shift_1kHz;
		}
		else if (RCAR_PRODUCT_REV(*prr_reg) == RCAR_REVISION_ES20){
			vco_clk = INPUT_CLK_5p49v6901a * vco_div / shift_1kHz;
		}
	}
	else if (RCAR_PRODUCT_ID(*prr_reg) == RCAR_PRODUCT_M3N){
		vco_clk = INPUT_CLK_5p49v6901a * vco_div / shift_1kHz;
	}

	SLOG_DEBUG("vco_clk value: %d", vco_clk);

	vco_clk = (vco_clk / 2);
	rate = (rate / shift_1kHz);
	integ_div = (vco_clk / rate);
	div = ((vco_clk * shift_1kHz) / rate);
	frac_div = (div - (integ_div * shift_1kHz));

	SLOG_DEBUG("clk_5p49 setting value: integ_div %d, div %d, frac_div %d",
		integ_div, div, frac_div);

	if (frac_div > 0x3fffffff)
		return -1;

	/* Integdiv setting */
	clk_5p49_write(index, C5P49_DIV_INTEGER_11_4,
			((0x0ff0 & (uint16_t)integ_div) >> 4));
	clk_5p49_write(index, C5P49_DIV_INTEGER_3_0,
			((0x000f & (uint16_t)integ_div) << 4));

	/* spread = 0.01% */
	frac_div = frac_div - ((div / (100 * 100 / 1)) / 2);
	frac_div = ((0x1000000 / shift_1kHz) * frac_div);

	frac_0 = (frac_div & 0x3fc00000) >> 22;
	frac_1 = (frac_div & 0x003fc000) >> 14;
	frac_2 = (frac_div & 0x00003fc0) >> 6;
	frac_3 = (frac_div & 0x0000003f) << 2;

	/* Fracdiv setting */
	clk_5p49_write(index, C5P49_DIV_FRAC_29_22, frac_0);
	clk_5p49_write(index, C5P49_DIV_FRAC_21_14, frac_1);
	clk_5p49_write(index, C5P49_DIV_FRAC_13_6, frac_2);
	clk_5p49_write(index, C5P49_DIV_FRAC_5_0, frac_3);

	/* enable CLK_OE */
	clk_5p49_power(index, true);

	return 0;
}

int clk_5p49_config(int du_index, int source_clk)
{

	if (idt_i2c_fd < 0)
	{
		if ((idt_i2c_fd = open(devname, O_RDWR)) < 0)
		{
			SLOG_ERROR("open(%s): %s\n", devname, strerror(errno));
			exit(-1);
		}
	}

	switch(du_index)
	{
		case 1:/* config CLK_OUT1 route to du_dotclkin0 */
			if (clk_5p49_div_calculation(source_clk, C5P49_CLK_OUT1) < 0)
			{
				SLOG_ERROR("clk_5p49: can not set divider for DU%d\n",du_index);
				return -1;
			}
			break;

		case 2:/* config CLK_OUT2 route to du_dotclkin2 */
			if (clk_5p49_div_calculation(source_clk, C5P49_CLK_OUT2) < 0)
			{
				SLOG_ERROR("clk_5p49: can not set divider for DU%d\n",du_index);
				return -1;
			}
			break;

		case 3:/* config CLK_OUT2 route to du_dotclkin3 */
			//XXX: is this correct? it's identical to DU_CH_2
			if (clk_5p49_div_calculation(source_clk, C5P49_CLK_OUT2) < 0)
			{
				SLOG_ERROR("clk_5p49: can not set divider for DU%d\n",du_index);
				return -1;
			}
			break;

		default:
			SLOG_ERROR("clk_5p49: Invalid DU index: %d\n",du_index);
			break;
	}

	return 0;
}


int main(int argc, char *argv[]) 
{
	uint32_t  otar_val;
	uintptr_t   pwr_base;
	
	//clk_5p49_config(2,74250);
	
	if((pwr_base = mmap_device_io(4, 0xFEB50004)) == (uintptr_t)MAP_FAILED){
		printf("mmap_device_io fail\n");
	}
	//out32(pwr_base,0x400000);
	printf("vaule=%x\n",in32(pwr_base));

	
    return 0;
}
