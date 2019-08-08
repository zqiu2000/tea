/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <idt.h>
#include <tea.h>
#include <spinlock.h>

struct idt_entry idt_table[256];
struct idt_ptr idt_p;
struct irq_action {
	irq_call_t irq_call;
	void *data;
	spinlock_t lock;
	char *name;
} irq_actions[256 - 32];

static const char *exceptions[] = {
	"Devide Error",
	"Debug Exception",
	"NMI Interrupt",
	"Breakpoint",
	"Overflow",
	"BOUND Range Exceeded",
	"Invalid Opcode (Undefined Opcode)",
	"Device Not Available",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Invalid TSS",
	"Segment Not Present",
	"Stack-Segment Fault",
	"General Protection",
	"Page Fault",
	"Reserved",
	"x87 FPU Floating-Point Error"
	"Alignment Check",
	"Machine Check",
	"SIMD Floating-Point Exception",
	"Virtualization Exception"
};

static inline void load_idt(const struct idt_ptr *idt)
{
	asm volatile("cs lidt %0" : : "m" (*idt));
}

void idt_init(void)
{
	int irq;
	int isr_entry_size = isr_1 - isr_0;
	void *isr_entry = (void*)isr_0;

	for (irq = 0; irq < 256; irq++) {
		idt_table[(irq)].base_low = ((uint32_t)isr_entry) & 0xFFFF;
		idt_table[(irq)].base_hi = (((uint32_t)isr_entry)>> 16) & 0xFFFF;
		idt_table[(irq)].sel = 0x10;
		idt_table[(irq)].reserved = 0;
		idt_table[(irq)].flags = (0x8E) | 0x60;
		isr_entry += isr_entry_size;

		if (irq >= 0x20)
			spinlock_init(&irq_actions[irq - 0x20].lock);
	}

	idt_p.limit = 8 * 256 - 1;
	idt_p.base = (uint32_t)idt_table;

	load_idt(&idt_p);
}

int register_irq(uint32_t irq, irq_call_t irq_call, void *data, char *name)
{
	uint32_t vector = irq + 0x20;
	unsigned long flags;

	if (vector > 255) {
		pr_err("Invalid irq %d\n", irq);
		return -1;
	}

	flags = spinlock_irq_save(&irq_actions[irq].lock);
	if (irq_actions[irq].irq_call) {
		pr_err("Irq %d already registered as function %p\n", irq, irq_actions[irq].irq_call);
		spinlock_irq_restore(&irq_actions[irq].lock, flags);
		return -1;
	}

	irq_actions[irq].irq_call = irq_call;
	irq_actions[irq].data = data;
	irq_actions[irq].name = name;

	spinlock_irq_restore(&irq_actions[irq].lock, flags);
	return 0;
}

int free_irq(uint32_t irq)
{
	uint32_t vector = irq + 0x20;
	unsigned long flags;

	if (vector > 255) {
		pr_err("Invalid irq %d\n", irq);
		return -1;
	}

	flags = spinlock_irq_save(&irq_actions[irq].lock);
	if (!irq_actions[irq].irq_call) {
		pr_err("Double free irq %d\n", irq);
		spinlock_irq_restore(&irq_actions[irq].lock, flags);
		return -1;
	}

	irq_actions[irq].irq_call = NULL;
	irq_actions[irq].data = NULL;
	irq_actions[irq].name = NULL;
	spinlock_irq_restore(&irq_actions[irq].lock, flags);

	return 0;
}

int irq_handler(struct irq_regs *reg)
{
	uint32_t hw_irq = reg->vector - 0x20;

	if (hw_irq > (255 - 32))
		panic("Irq %d overflow\n", hw_irq);

	if (irq_actions[hw_irq].irq_call)
		return irq_actions[hw_irq].irq_call(reg, irq_actions[hw_irq].data);

	pr_info("Irq %d unhandled!\n", hw_irq);

	return -1;
}

void dump_regs(struct irq_regs *reg)
{
	printf("EIP: %04x:[<%08x>] EFLAGS: %08x\n",
			(u16)reg->xcs, reg->eip, reg->eflags);

	printf("EAX: %08x EBX: %08x ECX: %08x EDX: %08x\n",
			reg->eax, reg->ebx, reg->ecx, reg->edx);
	printf("ESI: %08x EDI: %08x EBP: %08x ESP: %08x\n",
			reg->esi, reg->edi, reg->ebp, reg->esp);
	printf(" DS: %04x ES: %04x FS: %04x GS: %04x SS: %04x\n",
			(u16)reg->xds, (u16)reg->xes, (u16)reg->xfs,
			(u16)reg->xgs, (u16)reg->xss);
}

void isr_handler(struct irq_regs *reg)
{
	switch (reg->vector) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
		printf("CPU Exception: %s\n", exceptions[reg->vector]);
		dump_regs(reg);
		break;
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
		printf("CPU Exception: Reserved fault %d\n", reg->vector);
		dump_regs(reg);
		break;
	default:
		pr_debug("%d handler\n", reg->vector);
		irq_handler(reg);
		break;
	}
}
