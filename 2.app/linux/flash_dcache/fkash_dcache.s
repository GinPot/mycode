.section .text
.type __dma_inv_area,@function
.globl __dma_inv_area



.macro altinstruction_entry orig_offset alt_offset feature orig_len alt_len
	.word \orig_offset - .
	.word \alt_offset - .
	.hword \feature
	.byte \orig_len
	.byte \alt_len
.endm

/*
 * Begin an alternative code sequence.
 */
.macro alternative_if_not cap
	.set .Lasm_alt_mode, 0
	.pushsection .altinstructions, "a"
	altinstruction_entry 661f, 663f, \cap, 662f-661f, 664f-663f
	.popsection
661:
.endm

/*
 * Provide the other half of the alternative code sequence.
 */
.macro alternative_else
662:
	.if .Lasm_alt_mode==0
	.pushsection .altinstr_replacement, "ax"
	.else
	.popsection
	.endif
663:
.endm

/*
 * Complete an alternative code sequence.
 */
.macro alternative_endif
664:
	.if .Lasm_alt_mode==0
	.popsection
	.endif
	.org	. - (664b-663b) + (662b-661b)
	.org	. - (662b-661b) + (664b-663b)
.endm


/*
 * read_ctr - read CTR_EL0. If the system has mismatched register fields,
 * provide the system wide safe value from arm64_ftr_reg_ctrel0.sys_val
 */
    .macro  read_ctr, reg
alternative_if_not ARM64_MISMATCHED_CACHE_TYPE
    mrs \reg, ctr_el0           // read CTR
    nop
alternative_else
    ldr_l   \reg, arm64_ftr_reg_ctrel0 + ARM64_FTR_SYSVAL
alternative_endif
    .endm

/*
 * dcache_line_size - get the safe D-cache line size across all CPUs
 */
    .macro  dcache_line_size, reg, tmp
    read_ctr    \tmp
    ubfm        \tmp, \tmp, #16, #19    // cache line size encoding
    mov     \reg, #4        // bytes per word
    lsl     \reg, \reg, \tmp    // actual cache line size
    .endm

__dma_inv_area:
	add	x1, x1, x0
	dcache_line_size x2, x3
	sub	x3, x2, #1
	tst	x1, x3				// end cache line aligned?
	bic	x1, x1, x3
	b.eq	1f
	dc	civac, x1			// clean & invalidate D / U line
1:	tst	x0, x3				// start cache line aligned?
	bic	x0, x0, x3
	b.eq	2f
	dc	civac, x0			// clean & invalidate D / U line
	b	3f
2:	dc	ivac, x0			// invalidate D / U line
3:	add	x0, x0, x2
	cmp	x0, x1
	b.lo	2b
	dsb	sy
	ret
