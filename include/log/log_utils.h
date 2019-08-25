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
void print_prefix_flavor(void);
void printf(const char *fmt, ...);

#define pr_err(fmt, args...) \
	do {					\
		print_prefix_flavor();		\
		printf(fmt, ##args);		\
	} while (0)

#define pr_info(fmt, args...) \
	do {					\
		print_prefix_flavor();		\
		printf(fmt, ##args);		\
	} while (0)

#define pr_debug(fmt, args...) \
	do {					\
		print_prefix_flavor();		\
		printf(fmt, ##args);		\
	} while (0)

#define pr_verb(fmt, args...) \
	do {					\
		print_prefix_flavor();		\
		printf(fmt, ##args);		\
	} while (0)

#define panic(fmt, args...) \
	do {									\
		pr_info("Fatal error @ %s-line %d\n", __func__, __LINE__);	\
		pr_info(fmt, ##args);						\
		while(1) {asm volatile("hlt");}					\
	} while(0)

#endif
