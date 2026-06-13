	.file	"main.c"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$48, %rsp
	.seh_stackalloc	48
	.seh_endprologue
	call	__main
	movl	$5, -4(%rbp)
	movl	-4(%rbp), %eax
	cmpl	$9, %eax
	jg	.L2
	movl	$0, -4(%rbp)
.L2:
/APP
 # 7 "main.c" 1
	nop
 # 0 "" 2
 # 8 "main.c" 1
	nop
 # 0 "" 2
 # 9 "main.c" 1
	nop
 # 0 "" 2
/NO_APP
	movl	$0, %eax
	addq	$48, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev8, Built by MSYS2 project) 15.2.0"
