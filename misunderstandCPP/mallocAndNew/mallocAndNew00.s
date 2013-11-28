	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main
	.align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:                                ## %entry
	push	RBP
Ltmp3:
	.cfi_def_cfa_offset 16
Ltmp4:
	.cfi_offset rbp, -16
	mov	RBP, RSP
Ltmp5:
	.cfi_def_cfa_register rbp
	push	R14
	push	RBX
Ltmp6:
	.cfi_offset rbx, -32
Ltmp7:
	.cfi_offset r14, -24
	mov	EDI, 1024
	call	_malloc
	mov	R14, RAX
	mov	EBX, 1
	xor	EDI, EDI
	jmp	LBB0_1
	.align	4, 0x90
LBB0_2:                                 ## %for.body.for.body_crit_edge
                                        ##   in Loop: Header=BB0_1 Depth=1
	movsx	EDI, BYTE PTR [R14 + RBX]
	inc	RBX
LBB0_1:                                 ## %for.body
                                        ## =>This Inner Loop Header: Depth=1
	call	_putchar
	cmp	EBX, 1024
	jne	LBB0_2
## BB#3:                                ## %for.end
	mov	RDI, R14
	call	_free
	xor	EAX, EAX
	pop	RBX
	pop	R14
	pop	RBP
	ret
	.cfi_endproc


.subsections_via_symbols
