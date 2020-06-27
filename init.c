/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <mtrr.h>
#include <idt.h>
#include <mod_init.h>
#include <lapic.h>
#include <tsc.h>

static int load_ucode(void)
{
	/* TODO: load the ucode althought BIOS should
	 * have loaded it already.
	 */
	return 0;
}

static int arch_early_init(void)
{
	idt_init();
	/* Enable Cache? */
	/* Enable paging? */

	x86_enable_caches();

	tsc_init();

	early_apic_init();

	return 0;
}

static int arch_init(void)
{
	return 0;
}

int __weak timer_demo(void)
{
	pr_info("Fake timer demo!\n");
	return 0;
}

void c_entry(void)
{
	pr_info("c entry !!\n");
	/* Init BSS? */

	load_ucode();

	setup_mtrr();

	tea_config_init();

	earlycall_init();

	arch_early_init();

	local_irq_enable();

	arch_init();

	modulecall_init();

	devcall_init();

	pr_info("Call apps\n");

	//timer_demo();
	demo_ipc();

	panic("die loop @ %p \n", c_entry);
	asm volatile("hlt");
}
