/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <lapic.h>
#include <msr.h>
#include <raw_io.h>
#include <bitops.h>
#include <vectors.h>

struct apic_ops {
	void (*apic_init)(void);
	void (*apic_enable)(void);
	int (*apic_send_ipi)(uint32_t cpuid, uint32_t vector);
	int (*apic_sned_self)(uint32_t vector);
	void (*apic_eoi)(void);
};

static struct apic_ops lapic_ops;

static int x2apic_valid(void)
{
	uint32_t eax, ebx, ecx, edx;

	cpuid(1, &eax, &ebx, &ecx, &edx);

	return ecx & BIT21;
}

static void lapic_init(void)
{
	uint64_t val;

	val = rdmsr(MSR_IA32_APIC_BASE);
	val |= MSR_IA32_APIC_BASE_ENABLE;
	wrmsr(MSR_IA32_APIC_BASE, val);

	pr_debug("LAPIC version %x\n", readl(LAPIC_VER) & LAPIC_VER_MASK);

	/* set task priority to 0 to accept all interrupts */
	writel(0, LAPIC_TPR);

	pr_debug("LAPIC ID %x, logical ID %x,\n", readl(LAPIC_ID) >> LAPIC_ID_SHIFT,
			readl(LAPIC_LDR) >> LAPIC_ID_SHIFT);

	/* How to set the logical id? */

	/* use flat model */
	writel(LAPIC_FLAT_MODEL, LAPIC_DFR);
}

static void lapic_enable(void)
{
	writel(readl(LAPIC_SIVR) | LAPIC_SIVR_EN, LAPIC_SIVR);
}

static void x2apic_init(void)
{
	uint64_t val;

	val = rdmsr(MSR_IA32_APIC_BASE);
	val |= MSR_IA32_APIC_BASE_ENABLE;
	wrmsr(MSR_IA32_APIC_BASE, val);
	val |= MSR_IA32_X2APIC_ENABLE;
	wrmsr(MSR_IA32_APIC_BASE, val);

	pr_debug("LAPIC version %lx\n", rdmsr(MSR_IA32_EXT_APIC_VERSION) & LAPIC_VER_MASK);

	/* Mask all lapic lvt entires before enabling lapic */
#if 0
	wrmsr(MSR_IA32_EXT_APIC_LVT_CMCI, LAPIC_LVT_MASK);
	wrmsr(MSR_IA32_EXT_APIC_LVT_TIMER, LAPIC_LVT_MASK);
	wrmsr(MSR_IA32_EXT_APIC_LVT_THERMAL, LAPIC_LVT_MASK);
	wrmsr(MSR_IA32_EXT_APIC_LVT_PMI, LAPIC_LVT_MASK);
	wrmsr(MSR_IA32_EXT_APIC_LVT_LINT0, LAPIC_LVT_MASK);
	wrmsr(MSR_IA32_EXT_APIC_LVT_LINT1, LAPIC_LVT_MASK);
	wrmsr(MSR_IA32_EXT_APIC_LVT_ERROR, LAPIC_LVT_MASK);
#endif
	wrmsr(MSR_IA32_EXT_APIC_TPR, 0);

	pr_debug("LAPIC ID %lx, logical ID %lx,\n", rdmsr(MSR_IA32_EXT_XAPICID),
			rdmsr(MSR_IA32_EXT_APIC_LDR) >> LAPIC_ID_SHIFT);

	/* How to set the logical id? */

}

static void x2apic_enable(void)
{
	wrmsr(MSR_IA32_EXT_APIC_SIVR, LAPIC_SIVR_EN | rdmsr(MSR_IA32_EXT_APIC_SIVR));
}

static int x2apic_send_ipi(uint32_t cpuid, uint32_t vector)
{
	uint64_t val = 0;

	/* No smp support, cpuid have to be 0 */
	if (cpuid) {
		pr_err("No smp supported!\n");
		return -1;
	}

	if (vector == NMI_VECTOR)
		val |= LAPIC_ICR_MODE_NMI | LAPIC_ICR_DST_MODE_PHYSICAL;
	else
		val |= vector | LAPIC_ICR_MODE_FIXED | LAPIC_ICR_DST_MODE_PHYSICAL;

	/* send to tearget apic_id */
	/* TODO: now target to default cpu 0, need to pass target apic id accordingly */
	/* val |= rdmsr(MSR_IA32_EXT_XAPICID) << 32; */

	wrmsr(MSR_IA32_EXT_APIC_ICR, val);

	return 0;
}

static int x2apic_send_self(uint32_t vector)
{
	uint64_t val = 0;

	val |= vector | LAPIC_ICR_MODE_FIXED | LAPIC_ICR_DST_MODE_PHYSICAL;
	val |= LAPIC_ICR_SHORTHAND_SELF;

	wrmsr(MSR_IA32_EXT_APIC_ICR, val);

	return 0;
}

static void x2apic_eoi(void)
{
	wrmsr(MSR_IA32_EXT_APIC_EOI, 0);
}

void early_apic_init(void)
{
	if (x2apic_valid()) {
		pr_info("x2apic!\n");
		lapic_ops.apic_init = x2apic_init;
		lapic_ops.apic_enable = x2apic_enable;
		lapic_ops.apic_send_ipi = x2apic_send_ipi;
		lapic_ops.apic_sned_self = x2apic_send_self;
		lapic_ops.apic_eoi = x2apic_eoi;
	} else {
		pr_info("xapic!\n");
		lapic_ops.apic_init = lapic_init;
		lapic_ops.apic_enable = lapic_enable;
		/*FIXME..*/
	}

	lapic_ops.apic_init();
	lapic_ops.apic_enable();
}

int apic_send_ipi(uint32_t cpuid, uint32_t vector)
{
	return lapic_ops.apic_send_ipi(cpuid, vector);
}

int apic_send_self(uint32_t vector)
{
	return lapic_ops.apic_sned_self(vector);
}

void apic_eoi(void)
{
	lapic_ops.apic_eoi();
}
