
.equ    FIQSTACK0 , 0xf0000
.equ    IRQSTACK0 , 0xd0000
.equ    KSTACK0   , 0xd0000
.equ    USTACK0   , 0x80000

.equ	USR_mode,	0x10
.equ	FIQ_mode,	0x11
.equ	IRQ_mode,	0x12
.equ	SVC_mode,	0x13
.equ	HYP_mode,	0x1A
.equ	SYS_mode,	0x1F
.equ	No_Int,		0xC0

.globl _start
_start:
// check core ID
	mrc     p15, 0, r0, c0, c0, 5
	ubfx    r0, r0, #0, #2
	cmp     r0, #0					@ is it core 0?
	beq     core0

	@ it is not core0, so do things that are appropriate for SVC level as opposed to HYP
	@ like set up separate stacks for each core, etc.

	beq     hang

hang: b hang

core0:
//	mov 	r2, # No_Int | IRQ_mode
//	msr		cpsr_c, r2
//	mov		sp, # IRQSTACK0

//	mov 	r2, # No_Int | FIQ_mode
//	msr		cpsr_c, r2
//	mov		sp, # FIQSTACK0

//	mov 	r2, # No_Int | HYP_mode
//	msr		cpsr_c, r2
	mov		sp, # KSTACK0

	bl		notmain
	b hang
