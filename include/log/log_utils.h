/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#ifndef _LOG_UTILS_H_
#define _LOG_UTILS_H_

#include <types.h>

int sprintf(char *buf, const char *fmt, ...);
int snprintf(char *buf, size_t size, const char *fmt, ...);
void printf(const char *fmt, ...);

#define pr_err printf
#define pr_info printf
#define pr_debug printf
#define pr_verb printf

#define panic(fmt, args...) \
	do {									\
		printf("Fatal error @ %s-line %d\n", __func__, __LINE__);	\
		printf(fmt, ##args);						\
		while(1) {asm volatile("hlt");}					\
	} while(0)

#endif
