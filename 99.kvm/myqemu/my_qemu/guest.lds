OUTPUT_ARCH(aarch64)
ENTRY(text_)

SECTIONS
{
	text_ = .;
	.start.text	: { KEEP(*(.start.text)) }
	.text 		: { KEEP(*(.text*)) }
	.rodata		: { . = ALIGN(8); KEEP(*(.rodata*)) }
	.data		: { . = ALIGN(8); KEEP(*(.data*)) }
	.bss		: { . = ALIGN(8); KEEP(*(.bss*)) KEEP(*(COMMON)) }
}
