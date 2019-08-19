/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <mod_init.h>

extern initcall_t _initcall0_start[];
extern initcall_t _initcall0_end[];
extern initcall_t _initcall1_start[];
extern initcall_t _initcall1_end[];
extern initcall_t _initcall2_start[];
extern initcall_t _initcall2_end[];

int earlycall_init(void)
{
	initcall_t *fn;

	for (fn = _initcall0_start; fn < _initcall0_end; fn++) {
		pr_debug("%s: calling initcall0-%p\n", __func__, fn);
		if ((*fn)())
			pr_err("initcall0-%p failed\n", fn);
	}

	return 0;
}

int modulecall_init(void)
{
	initcall_t *fn;

	for (fn = _initcall1_start; fn < _initcall1_end; fn++) {
		pr_debug("%s: calling initcall1-%p\n", __func__, fn);
		if ((*fn)())
			pr_err("initcall1-%p failed\n", fn);
	}

	return 0;
}

int devcall_init(void)
{
	initcall_t *fn;

	for (fn = _initcall2_start; fn < _initcall2_end; fn++) {
		pr_debug("%s: calling initcall2-%p\n", __func__, fn);
		if ((*fn)())
			pr_err("initcall2-%p failed\n", fn);
	}

	return 0;
}
