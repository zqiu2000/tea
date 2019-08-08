/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>

/* static defined this 4k memory for log buffer */
#define LOG_SIZE	(4*1024)

extern uint8_t mem_log;
static int index = 0;

int memlog_write(uint8_t byte)
{
	uint8_t *mem_log_p = &mem_log;

	index %= LOG_SIZE;
	mem_log_p[index++] = byte;

	return 0;
}
