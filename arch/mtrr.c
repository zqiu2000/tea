/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <msr.h>
#include <cpu.h>
#include <bitops.h>

#define MASK2SIZE(msk, max_phyaddr)	((~(msk)) & (RANGE2MASK(0, max_phyaddr)))

struct fixed_mtrr {
	uint8_t types[8];
} __packed;

static const char *mtrr_type[] = {
	"UC",
	"WC",
	"RV",
	"RV",
	"WT",
	"WP",
	"WB"
};

static int get_fixed_mtrr(void)
{
	int i, j;
	uint64_t va;
	struct fixed_mtrr *mtrr_f = (struct fixed_mtrr*)&va;
	uint32_t base, size;
	uint8_t prev_type;

	va = rdmsr(MSR_IA32_MTRR_FIX64K_00000);
	base = 0;
	size = 0;
	for (i = 0; i < 7; i++) {
		if (mtrr_f->types[i] != mtrr_f->types[i + 1]) {
			size += 0x10000;
			pr_info("0x%05x - 0x%05x: %s\n", base, base + size - 1, mtrr_type[mtrr_f->types[i]]);
			base += size;
			size = 0;
			continue;
		}
		size += 0x10000;
	}
	prev_type = mtrr_f->types[7];
	size += 0x10000;

	va = rdmsr(MSR_IA32_MTRR_FIX16K_80000);
	for (i = 0; i < 8; i++) {
		if (mtrr_f->types[i] != prev_type) {
			pr_info("0x%05x - 0x%05x: %s\n", base, base + size - 1, mtrr_type[prev_type]);
			base += size;
			size = 0x4000;
			prev_type = mtrr_f->types[i];
			continue;
		}
		size += 0x4000;
	}

	va = rdmsr(MSR_IA32_MTRR_FIX16K_A0000);
	for (i = 0; i < 8; i++) {
		if (mtrr_f->types[i] != prev_type) {
			pr_info("0x%05x - 0x%05x: %s\n", base, base + size - 1, mtrr_type[prev_type]);
			base += size;
			size = 0x4000;
			prev_type = mtrr_f->types[i];
			continue;
		}
		size += 0x4000;
	}

	for(j = 0; j < 8; j++) {
		va = rdmsr(MSR_IA32_MTRR_FIX4K_C0000 + j);
		for (i = 0; i < 8; i++) {
			if (mtrr_f->types[i] != prev_type) {
				pr_info("0x%05x - 0x%05x: %s\n", base, base + size - 1, mtrr_type[prev_type]);
				base += size;
				size = 0x1000;
				prev_type = mtrr_f->types[i];
				continue;
			}
			size += 0x1000;
		}
	}

	pr_info("0x%05x - 0x%05x: %s\n", base, base + size - 1, mtrr_type[prev_type]);

	return 0;
}

static inline int get_cpu_phy_addr_width(void)
{
	uint32_t eax, ebx, ecx, edx;

	cpuid(CPUID_EXTEND_ADDRESS_SIZE, &eax, &ebx, &ecx, &edx);

	return eax & 0xFF;
}

static int get_var_mtrr(int var_cnt)
{
	uint32_t phyaddr_width;
	int i;

	phyaddr_width = get_cpu_phy_addr_width();

	pr_info("Max phy addr %d\n", phyaddr_width);

	for (i = 0; i < var_cnt; i++) {
		uint64_t base, mask;

		base = rdmsr(MSR_IA32_MTRR_PHYSBASE_0 + i * 2);
		mask = rdmsr(MSR_IA32_MTRR_PHYSMASK_0 + i * 2);

		if (mask & BIT11) {
			uint8_t type = base & 0xF;
			uint64_t size;

			base &= RANGE2MASK(12, phyaddr_width);
			size = MASK2SIZE((mask & RANGE2MASK(12, phyaddr_width - 1)), phyaddr_width - 1);
			pr_info("0x%016lx - 0x%016lx: %s\n", base, base + size, mtrr_type[type]);
		}
	}

	return 0;
}

int setup_mtrr(void)
{
	uint32_t eax, ebx, ecx, edx;
	uint64_t va, vb;

	cpuid(CPUID_FEATURES, &eax, &ebx, &ecx, &edx);

	pr_verb("eax 0x%08x ebx 0x%08x ecx 0x%08x edx 0x%08x\n", eax, ebx, ecx, edx);
	if (!(edx & BIT12)) {
		pr_info("No mtrr on this cpu!\n");
		return 0;
	}

	/* TODO: set necessary mtrr here: */

	/* Get and tell mtrrs */

	va = rdmsr(MSR_IA32_MTRR_CAP);
	vb = rdmsr(MSR_IA32_MTRR_DEF_TYPE);

	if ((va & BIT8) && (vb & BIT10)) {
		pr_debug("MTRR fixed enabled.\n");
		get_fixed_mtrr();
	}

	if (vb & BIT11) {
		pr_info("Default MTRR type: %s\n", mtrr_type[vb & 0xF]);
		get_var_mtrr(va & 0xff);
	}

	return 0;
}

