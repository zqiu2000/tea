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

static int tea_config_init(void)
{
	return 0;
}

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
	return 0;
}

static int arch_init(void)
{
	return 0;
}

void c_entry(void)
{
	printf("c entry !!\n");
	/* Init BSS? */

	load_ucode();

	setup_mtrr();

	earlycall_init();

	arch_early_init();

	local_irq_enable();

	tea_config_init();

	arch_init();

	modulecall_init();

	devcall_init();

	printf("will test fault!\n");

	asm volatile("int $3");

	printf("will halt\n");

	panic("die loop @ %p \n", c_entry);
	asm volatile("hlt");
}
