/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

/* ISR low level hander code */

.altmacro
.section .isr_handle, "ax"
	.code32
.macro declare_isr num
	.align(16)
	.globl isr_\num
	isr_\num:
.if (\num==8 || \num==10 || \num==11 || \num==12 || \num==13 || \num==14 || \num==17)
	/* Error code will be pushed by processor */
.else
	pushl $0
.endif
	pushl $\num
	jmp isr_common_entry
.endm

.align(8)
.globl isr_common_entry
isr_common_entry:
	pushl %ss
	pushl %gs
	pushl %fs
	pushl %es
	pushl %ds
	pushl %esp
	pushl %eax
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
	movl %esp, %eax
	pushl %eax
	call isr_handler
	addl $4, %esp
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popl %eax
	popl %esp
	popl %ds
	popl %es
	popl %fs
	popl %gs
	popl %ss
	add $8, %esp
	iret

vector=0
.rept (0xff)
	declare_isr %vector
	vector=vector+1
.endr
