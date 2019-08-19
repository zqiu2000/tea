/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#ifndef _MODULE_INIT_H_
#define _MODULE_INIT_H_

#include <types.h>

typedef int (*initcall_t)(void);

#define _initcall(fn, id) \
        static initcall_t __initcall_##fn##id __used \
        __attribute__((__section__(".initcall" #id))) = fn;

#define early_init(fn)		_initcall(fn, 0)

#define module_init(fn)		_initcall(fn, 1)

#define dev_init(fn)		_initcall(fn, 2)

int earlycall_init(void);
int modulecall_init(void);
int devcall_init(void);
#endif
