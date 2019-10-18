#
# $QNXLicenseA:
# Copyright 2007, QNX Software Systems. All Rights Reserved.
# 
# You must obtain a written license from and pay applicable license fees to QNX 
# Software Systems before you may reproduce, modify or distribute this software, 
# or any work that includes all or part of this software.   Free development 
# licenses are available for evaluation and non-commercial purposes.  For more 
# information visit http://licensing.qnx.com or email licensing@qnx.com.
#  
# This file may contain contributions from others.  Please review this entire 
# file for other proprietary rights or license notices, as well as the QNX 
# Development Suite License Guide at http://licensing.qnx.com/license-guide/ 
# for other information.
# $
#

 
#
# void atomic_set( volatile unsigned *loc, unsigned bits )
# unsigned atomic_set_value( volatile unsigned *loc, unsigned bits )
#
	.section 	".text"
atomic_set:
atomic_set_value:
	#
	#	if (__cpu_flags & SH_CPU_FLAG_MOVLICO)
	#		use mov.li/mov.co to perform atomic op
	#	else
	#		use kernel cmpxchg emulation
	#
.ifdef	__PIC__
	mova	.Lgot, r0
	mov.l	.Lgot, r6
	add		r0, r6
	mov.l	.Lcpu, r0
	mov.l	@(r0, r6), r0
.else
	mov.l	.Lcpu, r0
.endif
	mov.l	@r0, r0
	tst		#1, r0				! SH_CPU_FLAG_MOVLICO
	bt		0f

1:	.word	0x0463				! mov.li @r4, r0
	mov		r0, r1
	or		r5, r0
	.word	0x0473				! mov.co r0, @r4
	bf		1b
	rts
	mov		r1, r0

	.align 2

.ifdef	__PIC__
.Lgot:	.long	_GLOBAL_OFFSET_TABLE_
.Lcpu:	.long	__cpu_flags@GOT
.else
.Lcpu:	.long	__cpu_flags
.endif

0:	mov		r5, r1
	mov.l	@r4, r0
1:	mov		r0, r5
	or		r1, r5
	trapa	#0xff
	bf		1b
	rts
	nop
	
	.globl	atomic_set
	.type	atomic_set,@function
	.size	atomic_set,.-atomic_set

	.globl	atomic_set_value
	.type	atomic_set_value,@function
	.size	atomic_set_value,.-atomic_set_value
