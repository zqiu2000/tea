/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>


extern uint8_t mem_log;
static int index = 0;

int memlog_write(uint8_t byte)
{
	uint8_t *mem_log_p = &mem_log;

	if (!tea_cfg.memlog_dis) {
		index %= MEM_LOG_SIZE;
		mem_log_p[index++] = byte;
	}

	return 0;
}
