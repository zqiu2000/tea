/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <vectors.h>
#include <lapic.h>
#include <msr.h>
#include <idt.h>
#include <tsc.h>
#include <mod_init.h>
#include <time/timer.h>
#include <spinlock.h>

static struct list_head t_head;
static spinlock_t t_lock;
static int timer_pending = 0;

static int update_timer(void)
{
	unsigned long flags;

	flags = spinlock_irq_save(&t_lock);
	if (!timer_pending && !list_empty(&t_head)) {
		struct timer *t = get_first_item(&t_head, struct timer, list);

		wrmsr(MSR_IA32_TSC_DEADLINE, t->tick);
		timer_pending = 1;
		spinlock_irq_restore(&t_lock, flags);

		return 0;
	}

	timer_pending = 0;
	spinlock_irq_restore(&t_lock, flags);

	return -1;
}

static int apic_timer_call(struct irq_regs *reg, void *data)
{
	struct list_head *l, *nl;
	uint64_t now = rdtsc();
	unsigned long flags;

	pr_info("%s timer served!\n", __func__);

	flags = spinlock_irq_save(&t_lock);

	list_for_each_safe(l, nl, &t_head) {
		struct timer *t = list_entry(l, struct timer, list);

		if (t->tick < now && t->t_call) {

			list_del_init(l);

			spinlock_irq_restore(&t_lock, flags);

			t->t_call(t->data);

			flags = spinlock_irq_save(&t_lock);
		}
	}

	spinlock_irq_restore(&t_lock, flags);

	update_timer();

	return 0;
}

static int x2apic_timer_init(void)
{
	uint64_t val;

	val = V_LAPIC_TIMER | LAPIC_TIMER_MODE_TSD_DEADLINE;
	wrmsr(MSR_IA32_EXT_APIC_LVT_TIMER, val);

	/* mfence no need in x2apic mode */
	wrmsr(MSR_IA32_TSC_DEADLINE, 0);

	pr_info("TSC DEADLINE 0x%016lx\n, lvttimer 0x%016lx",
		rdmsr(MSR_IA32_TSC_DEADLINE),
		rdmsr(MSR_IA32_EXT_APIC_LVT_TIMER));

	return 0;
}

static int timer_init(void)
{
	int ret;

	ret = register_irq(I_LAPIC_TIMER, apic_timer_call, NULL, "lapic_timer");
	if (ret) {
		pr_err("Failed to register lapic timer irq!\n");
		return ret;
	}

	x2apic_timer_init();

	INIT_LIST_HEAD(&t_head);
	spinlock_init(&t_lock);
	timer_pending = 0;

	return 0;
}

int timer_add(struct timer *p_timer)
{
	struct list_head *l, *nl;
	unsigned long flags;

	if (!p_timer || !p_timer->t_call) {
		pr_err("Invalid timer param!\n");
		return -1;
	}

	flags = spinlock_irq_save(&t_lock);

	if (!list_empty(&t_head)) {
		list_for_each_safe(l, nl, &t_head) {
			struct timer *t = list_entry(l, struct timer, list);
			struct timer *nt = list_entry(nl, struct timer, list);

			if (p_timer->tick > t->tick && p_timer->tick < nt->tick)
				break;
		}
	} else
		l = &t_head;

	list_add(&p_timer->list, l);

	spinlock_irq_restore(&t_lock, flags);

	update_timer();

	return 0;
}

module_init(timer_init);