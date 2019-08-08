/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

/* Trampoline code, the isolated core will be started here once SIPI is received */
#include <cpu.h>

.section .start16, "ax"
.code16
.globl start16
start16:
	cli
	xorl	%eax, %eax
	movl	%eax, %cr3

	mov	$0x3f8, %dx
	mov	$'O', %al
	out	%al, (%dx)

	movw	%cs, %ax
	movw	%ax, %ds

	mov	$0x3f8, %dx
	mov	$'o', %al
	out	%al, (%dx)

	/* load the temporary Global Descriptor Table */
#ifdef KVM
	movl	$idt_ptr, %ebx
	lidtl	(%ebx)
	movl	$(gdt_ptr), %ebx
	lgdtl	(%ebx)
#else
	movl	$(idt_ptr - start16), %ebx
	lidtl	(%ebx)
	movl	$(gdt_ptr - start16), %ebx
	lgdtl	(%ebx)
#endif

	/* Now, we enter protected mode */
	movl	%cr0, %eax
	orl	$X86_CR0_PE, %eax
	movl	%eax, %cr0

	mov	$0x3f8, %dx
	mov	$'l', %al
	out	%al, (%dx)

	ljmpl	$0x10, $code32start

.code32
code32start:
	mov	$0x3f8, %dx
	mov	$'o', %al
	out	%al, (%dx)

	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	mov	$0x3f8, %dx
	mov	$'n', %al
	out	%al, (%dx)

	movl	$_stack_start, %esp
	call	c_entry
	hlt

.align 4
idt_ptr:
	.word   0               /* limit */
	.long   0               /* base */

/*
 * Global gdt
 */
.align 4
gdt_ptr:
	.word   0x20 - 1		/* limit (32 bytes = 4 GDT entries) */
	.long   gdt			/* base */

.align 16
gdt:
	/*
	 * The GDT table
	 *
	 *       Selector       Type
	 *       0x00           NULL
	 *       0x08           Unused
	 *       0x10           32bit code
	 *       0x18           32bit data
	 *       0x20           64bit code TODO.
	 *       0x28           64bit data TODO.
	 */
	/* The NULL Desciptor - Mandatory */
	.word   0x0000          /* limit_low */
	.word   0x0000          /* base_low */
	.byte   0x00            /* base_middle */
	.byte   0x00            /* access */
	.byte   0x00            /* flags + limit_high */
	.byte   0x00            /* base_high */

	/* Unused Desciptor */
	.word   0x0000          /* limit_low */
	.word   0x0000          /* base_low */
	.byte   0x00            /* base_middle */
	.byte   0x00            /* access */
	.byte   0x00            /* flags + limit_high */
	.byte   0x00            /* base_high */

	/*
	 * The Code Segment Descriptor:
	 * - Base   = 0x00000000
	 * - Size   = 4GB
	 * - Access = Present, Ring 0, Exec (Code), Readable
	 * - Flags  = 4kB Granularity, 32-bit
	 */
	.word   0xffff          /* limit_low */
	.word   0x0000          /* base_low */
	.byte   0x00            /* base_middle */
	.byte   0x9b            /* access */
	.byte   0xcf            /* flags + limit_high */
	.byte   0x00            /* base_high */

	/*
	 * The Data Segment Descriptor:
	 * - Base   = 0x00000000
	 * - Size   = 4GB
	 * - Access = Present, Ring 0, Non-Exec (Data), Writable
	 * - Flags  = 4kB Granularity, 32-bit
	 */
	.word   0xffff          /* limit_low */
	.word   0x0000          /* base_low */
	.byte   0x00            /* base_middle */
	.byte   0x93            /* access */
	.byte   0xcf            /* flags + limit_high */
	.byte   0x00            /* base_high */

