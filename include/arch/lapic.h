/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#ifndef _LAPIC_H_
#define _LAPIC_H_

#define LAPIC_BASE			0xfee00000
#define LAPIC_ID			(LAPIC_BASE + 0x20)
#define LAPIC_VER			(LAPIC_BASE + 0x30)
#define LAPIC_TPR			(LAPIC_BASE + 0x80)
#define LAPIC_APR			(LAPIC_BASE + 0x90)
#define LAPIC_PPR			(LAPIC_BASE + 0xa0)
#define LAPIC_EOI			(LAPIC_BASE + 0xb0)
#define LAPIC_LDR			(LAPIC_BASE + 0xd0)
#define LAPIC_DFR			(LAPIC_BASE + 0xe0)
#define LAPIC_SIVR			(LAPIC_BASE + 0xf0)
#define LAPIC_ISR			(LAPIC_BASE + 0x100)
#define LAPIC_TMR			(LAPIC_BASE + 0x180)
#define LAPIC_IRR			(LAPIC_BASE + 0x200)
#define LAPIC_ESR			(LAPIC_BASE + 0x280)
#define LAPIC_LVT_CMCIR			(LAPIC_BASE + 0x2f0)
#define LAPIC_ICR			(LAPIC_BASE + 0x300)
#define LAPIC_ICR2			(LAPIC_BASE + 0x310)
#define LAPIC_LVT_TIMER			(LAPIC_BASE + 0x320)
#define LAPIC_LVT_TSR			(LAPIC_BASE + 0x330)
#define LAPIC_LVT_PMCR			(LAPIC_BASE + 0x340)
#define LAPIC_LVT_LINT0R		(LAPIC_BASE + 0x350)
#define LAPIC_LVT_LINT1R		(LAPIC_BASE + 0x360)
#define LAPIC_LVT_ER			(LAPIC_BASE + 0x370)
#define LAPIC_TIMER_ICR			(LAPIC_BASE + 0x380)
#define LAPIC_TIMER_CCR			(LAPIC_BASE + 0x390)
#define LAPIC_DCR			(LAPIC_BASE + 0x3e0)

#define LAPIC_ID_SHIFT			24

#define LAPIC_VER_MASK			0xff
#define LAPIC_SIVR_EN			(1 << 8)

#define LAPIC_FLAT_MODEL		(0xf << 28)

#define LAPIC_TIMER_MODE_MASK		(0x3 << 17)
#define LAPIC_TIMER_MODE_ONESHOT	0
#define LAPIC_TIMER_MODE_PERIODIC	(1 << 17)
#define LAPIC_TIMER_MODE_TSD_DEADLINE	(2 << 17)
#define LAPIC_TIMER_MASK		(1 << 16)

#define LAPIC_ICR_SHORTHAND_MASK	(3 << 18)
#define LAPIC_ICR_SHORTHAND_SELF	(1 << 18)
#define LAPIC_ICR_SHORTHAND_ALL		(2 << 18)
#define LAPIC_ICR_SHORTHAND_REST	(3 << 18)

#define LAPIC_ICR_MODE_MASK		(7 << 8)
#define LAPIC_ICR_MODE_FIXED		(0 << 8)
#define LAPIC_ICR_MODE_LOWPRIORITY	(1 << 8)
#define LAPIC_ICR_MODE_SMI		(2 << 8)
#define LAPIC_ICR_MODE_NMI		(4 << 8)
#define LAPIC_ICR_MODE_INIT		(5 << 8)
#define LAPIC_ICR_MODE_STARTUP		(6 << 8)
#define LAPIC_ICR_DST_MODE_LOGICAL	(1 << 11)
#define LAPIC_ICR_DST_MODE_PHYSICAL	(0 << 11)

#define LAPIC_ICR_LEVEL_ASSERT		(1 << 14)
#define LAPIC_ICR_DELIVERY_STATUS	(1 << 12)

#define LAPIC_VEC_MASK			(0xff)

#define LAPIC_LVT_MASK			(1 << 16)

enum lapic_mode {
	LAPIC_MODE_ONESHOT,
	LAPIC_MODE_PERIODIC,
	LAPIC_MODE_TSC_DEADLINE
};

void early_apic_init(void);

int apic_send_ipi(uint32_t cpuid, uint32_t vector);

int apic_send_self(uint32_t vector);

void apic_eoi(void);
#endif
