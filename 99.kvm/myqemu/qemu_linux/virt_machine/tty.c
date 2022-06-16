#include "config.h"

#define TERM_FD_IN      0
#define TERM_FD_OUT     1
#define ARM_UART_MMIO_BASE     0x0000000001000000UL

#define FIFO_LEN		64
#define UART_IRQ		96
#define UART_IIR_TYPE_BITS	0xc0

static int term_fds[2];
static struct termios orig_term;
static pthread_t term_poll_thread;


extern int kvm_fd, vm_fd, vcpu_fd, gic_fd, guest_fd, guest_dtb_fd, guest_rootfs_fd;

//static __u8			irq;
static __u8			irq_state;
//static int			txcnt;
static int			rxcnt;
static int			rxdone;
//static char			txbuf[FIFO_LEN];
static char			rxbuf[FIFO_LEN];

static __u8			dll;
static __u8			dlm;
static __u8			iir = UART_IIR_NO_INT;
static __u8			ier;
static __u8			fcr;
static __u8			lcr;
static __u8			mcr = UART_MCR_OUT2;
static __u8			lsr = UART_LSR_TEMT | UART_LSR_THRE;
static __u8			msr = UART_MSR_DCD | UART_MSR_DSR | UART_MSR_CTS;
static __u8			scr;

/* Same as read(2) except that this function never returns EAGAIN or EINTR. */
ssize_t xread(int fd, void *buf, size_t count)
{
	ssize_t nr;

restart:
	nr = read(fd, buf, count);
	if ((nr < 0) && ((errno == EAGAIN) || (errno == EINTR)))
		goto restart;

	return nr;
}

ssize_t read_in_full(int fd, void *buf, size_t count)
{
	ssize_t total = 0;
	char *p = buf;

	while (count > 0) {
		ssize_t nr;

		nr = xread(fd, p, count);
		if (nr <= 0) {
			if (total > 0)
				return total;

			return -1;
		}

		count -= nr;
		total += nr;
		p += nr;
	}

	return total;
}

int term_readable(void)
{
	struct pollfd pollfd = (struct pollfd) {
		.fd	= term_fds[TERM_FD_IN],
		.events	= POLLIN,
		.revents = 0,
	};
	int err;

	err = poll(&pollfd, 1, 0);
	return (err > 0 && (pollfd.revents & POLLIN));
}

int term_getc(void)
{
	unsigned char c;

	if (read_in_full(term_fds[TERM_FD_IN], &c, 1) < 0)
		return -1;


	return c;
}

//term_putc("hello", 5);
int term_putc(char *addr, int cnt)
{
	int ret;
	int num_remaining = cnt;

	while (num_remaining) {
		ret = write(term_fds[TERM_FD_OUT], addr, num_remaining);
		if (ret < 0)
			return cnt - num_remaining;
		num_remaining -= ret;
		addr += ret;
	}

	return cnt;
}

#define KVM_ARM_IRQ_TYPE_SPI		1
#define KVM_ARM_IRQ_TYPE_SHIFT		24
#define KVM_ARM_IRQ_NUM_MASK		0xffff
#define KVM_IRQCHIP_IRQ(x) (KVM_ARM_IRQ_TYPE_SPI << KVM_ARM_IRQ_TYPE_SHIFT) |\
			   ((x) & KVM_ARM_IRQ_NUM_MASK)

void kvm__irq_line(int irq, int level)
{
	struct kvm_irq_level irq_level = {
		.irq	= KVM_IRQCHIP_IRQ(irq),
		.level	= !!level,
	};

	//printf("irq=%d", irq);

	if (ioctl(vm_fd, KVM_IRQ_LINE, &irq_level) < 0)
		printf("Could not KVM_IRQ_LINE for irq %d", irq);
}

static void serial8250_update_irq(void)
{
	__u8 iir_tmp = 0;

	/* Handle clear rx */
	//if (lcr & UART_FCR_CLEAR_RCVR) {
	//	lcr &= ~UART_FCR_CLEAR_RCVR;
	//	rxcnt = rxdone = 0;
	//	lsr &= ~UART_LSR_DR;
	//}
	//
	///* Handle clear tx */
	//if (lcr & UART_FCR_CLEAR_XMIT) {
	//	lcr &= ~UART_FCR_CLEAR_XMIT;
	//	txcnt = 0;
	//	lsr |= UART_LSR_TEMT | UART_LSR_THRE;
	//}
	//printf("(ier & UART_IER_RDI)=%d\n",(ier & UART_IER_RDI));
	//printf("(lsr & UART_LSR_DR)=%d\n",(lsr & UART_LSR_DR));
	/* Data ready and rcv interrupt enabled ? */
	if ((ier & UART_IER_RDI) && (lsr & UART_LSR_DR)){
		iir_tmp |= UART_IIR_RDI;
		
	}
		
	//printf("(ier & UART_IER_THRI)=%d\n",(ier & UART_IER_THRI));
	//printf("(lsr & UART_LSR_TEMT)=%d\n",(lsr & UART_LSR_TEMT));
	/* Transmitter empty and interrupt enabled ? */
	if ((ier & UART_IER_THRI) && (lsr & UART_LSR_TEMT))
		iir_tmp |= UART_IIR_THRI;

	/* Now update the irq line, if necessary */
	if (!iir_tmp) {
		iir = UART_IIR_NO_INT;
		if (irq_state)
			kvm__irq_line(UART_IRQ, 0);
	} else {
		iir = iir_tmp;
		if (!irq_state)
			kvm__irq_line(UART_IRQ, 1);
	}
	irq_state = iir_tmp;
}

void serial8250__update_consoles(void)
{
	int c;

	while (term_readable() && rxcnt < FIFO_LEN) {

		c = term_getc();
		if (c < 0)
			break;
		rxbuf[rxcnt++] = c;
		lsr |= UART_LSR_DR;
	}

	serial8250_update_irq();
}

static void *term_poll_thread_loop(void *param)
{
	struct pollfd fds;

	//set_thread_name
	prctl(PR_SET_NAME, "term-poll");

	fds.fd = term_fds[TERM_FD_IN];
	fds.events = POLLIN;
	fds.revents = 0;

	while (1) {
		/* Poll with infinite timeout */
		if(poll(&fds, 1, -1) < 1)
			break;
		serial8250__update_consoles();
	}

	printf("term_poll_thread_loop: error polling device fds %d\n", strerror(errno));
	return NULL;
}


int ter_init(void)
{
	int r;
	struct termios term;

	if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO))
		return -1;
	
	term_fds[TERM_FD_IN] = STDIN_FILENO;
	term_fds[TERM_FD_OUT] = STDOUT_FILENO;

	r = tcgetattr(STDIN_FILENO, &orig_term);
	if (r < 0) {
		printf("unable to save initial standard input settings");
		return r;
	}
    
	term = orig_term;
	term.c_iflag &= ~(ICRNL);
	term.c_lflag &= ~(ICANON | ECHO);// | ISIG);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);

	/* Use our own blocking thread to read stdin, don't require a tick */
	if(pthread_create(&term_poll_thread, NULL, term_poll_thread_loop, NULL) != 0)
		printf("Unable to create console input poll thread\n");



	return 0;
}


static void serial8250_out(__u16 offset, void *data)
{
	char *addr = data;
	
	switch (offset) {
		case UART_TX:
				if (lcr & UART_LCR_DLAB) {
					dll = *(__u8 *)data;
					break;
				}
				lsr |= UART_LSR_TEMT | UART_LSR_THRE;
				term_putc(addr, 1);
		break;
		
		case UART_IER:
			if (!(lcr & UART_LCR_DLAB))
				ier = *(__u8 *)data & 0x0f;
			else
				dlm = *(__u8 *)data;
			break;
			
		case UART_FCR:
			fcr = *(__u8 *)data;
			break;
		case UART_LCR:
			lcr = *(__u8 *)data;
			break;
		case UART_MCR:
			mcr = *(__u8 *)data;
			break;
		case UART_LSR:
			/* Factory test */
			break;
		case UART_MSR:
			/* Not used */
			break;
		case UART_SCR:
			scr = *(__u8 *)data;
			break;
	}
	
	serial8250_update_irq();
}

static void serial8250_rx(void *data)
{
	if (rxdone == rxcnt)
		return;

	/* Break issued ? */
	if (lsr & UART_LSR_BI) {
		lsr &= ~UART_LSR_BI;
		*(__u8 *)data = 0;
		return;
	}

	*(__u8 *)data = rxbuf[rxdone++];
	if (rxcnt == rxdone) {
		lsr &= ~UART_LSR_DR;
		rxcnt = rxdone = 0;
	}
}

static void serial8250_in(__u16 offset, void *data)
{

	switch (offset) {
	case UART_RX:
		if (lcr & UART_LCR_DLAB)
			*(__u8 *)data = dll;
		else
			serial8250_rx(data);
		break;
	case UART_IER:
		if (lcr & UART_LCR_DLAB)
			*(__u8 *)data = dlm;
		else
			*(__u8 *)data = ier;
		break;
	case UART_IIR:
		*(__u8 *)data = iir | UART_IIR_TYPE_BITS;
		break;
	case UART_LCR:
		*(__u8 *)data = lcr;
		break;
	case UART_MCR:
		*(__u8 *)data = mcr;
		break;
	case UART_LSR:
		*(__u8 *)data = lsr;
		break;

	case UART_MSR:
		*(__u8 *)data = msr;
		break;
	case UART_SCR:
		*(__u8 *)data = scr;
		break;
	}
	
	serial8250_update_irq();
}

void serial8250_mmio(__u64 addr, __u8 *data, __u32 len, __u8 is_write)
{
	if(is_write){
		serial8250_out(addr - ARM_UART_MMIO_BASE, data);
	}else{
		//printf("read offset=%lld  data=%d\n", addr - ARM_UART_MMIO_BASE, *data);
		serial8250_in(addr - ARM_UART_MMIO_BASE, data);
	}
					
}
