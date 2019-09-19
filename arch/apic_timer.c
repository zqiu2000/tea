
#include <tea.h>
#include <vectors.h>
#include <lapic.h>
#include <msr.h>
#include <idt.h>
#include <tsc.h>
#include <mod_init.h>

static int apic_timer_call(struct irq_regs *reg, void *data)
{
	pr_info("%s timer served!\n", __func__);

	return 0;
}

static int x2apic_timer_init(void)
{
	uint64_t val;

	val = V_LAPIC_TIMER | LAPIC_TIMER_MODE_TSD_DEADLINE;
	wrmsr(MSR_IA32_EXT_APIC_LVT_TIMER, val);

	/* mfence needed? */
	wrmsr(MSR_IA32_TSC_DEADLINE, 0);

	pr_info("TSC DEADLINE 0x%016lx\n, lvttimer 0x%016lx",
		rdmsr(MSR_IA32_TSC_DEADLINE),
		rdmsr(MSR_IA32_EXT_APIC_LVT_TIMER));

	return 0;
}

int timer_init(void)
{
	int ret;

	ret = register_irq(I_LAPIC_TIMER, apic_timer_call, NULL, "lapic_timer");
	if (ret) {
		pr_err("Failed to register lapic timer irq!\n");
		return ret;
	}

	x2apic_timer_init();

	return 0;
}

int timer_set(uint32_t us)
{
	uint64_t val = rdtsc();

	wrmsr(MSR_IA32_TSC_DEADLINE, val + us_to_ticks(us));

	pr_info("TSC DEADLINE 0x%016lx, ticks 0x%016lx, now 0x%016lx, added 0x%016lx\n",
		rdmsr(MSR_IA32_TSC_DEADLINE),
		us_to_ticks(us), val, val+us_to_ticks(us));
	return 0;
}

module_init(timer_init);