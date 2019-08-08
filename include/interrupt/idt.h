/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#ifndef _IDT_H_
#define _IDT_H_

#include <types.h>

struct idt_entry {
	uint16_t base_low;
	uint16_t sel;
	uint8_t reserved;
	uint8_t flags;
	uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

struct irq_regs {
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t eax;
	uint32_t esp;
	uint32_t xds;
	uint32_t xes;
	uint32_t xfs;
	uint32_t xgs;
	uint32_t xss;
	uint32_t vector; /* Pushed by vector handler */
	uint32_t errno;
	uint32_t eip;
	uint32_t xcs;
	uint32_t eflags;
}  __attribute__ ((packed));

extern void isr_0(void);
extern void isr_1(void);

void idt_init(void);

typedef int (*irq_call_t)(struct irq_regs *reg, void *data);

static inline void local_irq_disable(void)
{
	asm volatile("cli": : :"memory");
}

static inline void local_irq_enable(void)
{
	asm volatile("sti": : :"memory");
}

static inline unsigned long local_irq_save(void)
{
	unsigned long flags;

	asm volatile("pushf ; pop %0"
		     : "=rm" (flags)
		     : /* no input */
		     : "memory");
	local_irq_disable();

	return flags;
}

static inline void local_irq_restore(unsigned long flags)
{
	asm volatile("push %0 ; popf"
		     : /* no output */
		     :"g" (flags)
		     :"memory", "cc");
}

int register_irq(uint32_t irq, irq_call_t irq_call, void *data, char *name);
int free_irq(uint32_t irq);

#endif
