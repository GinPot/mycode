#include "config.h"




#define GIC_MSI_BASE 0x8020000
#define GICV2M_MSI_TYPER	0x008
#define GICV2M_MSI_SETSPI	0x040
#define GICV2M_MSI_IIDR		0xfcc

#define GICV2M_SPI_MASK		0x3ff
#define GICV2M_MSI_TYPER_VAL(start, nr)	\
	(((start) & GICV2M_SPI_MASK) << 16 | ((nr) & GICV2M_SPI_MASK))






void mgic_mmio(__u64 addr, __u8 *data, __u32 len, __u8 is_write)
{
	if (is_write)
		return;
	
	addr -= GIC_MSI_BASE;

	switch (addr) {
		case GICV2M_MSI_TYPER:
			*(__u32 *)data = GICV2M_MSI_TYPER_VAL(32, 64);
			break;
		case GICV2M_MSI_IIDR:
			*(__u32 *)data = 0x0;
			break;
	}
}







