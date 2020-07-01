/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef ASSEMBLER

typedef struct _tea_cfg {
	int log_timestamp;
	int uart_log;
	int memlog_dis;
} tea_cfg_t;

extern tea_cfg_t tea_cfg;

int tea_config_init(void);

#endif

/* Memory layout */
#define TEA_STACK_SIZE			(0x1000)
#define TEA_ENTRY_ADDR			(0x1000)
#define MEM_LOG_ADDR			(0xA000)
#define MEM_LOG_SIZE			(0x2000)
#define SHARE_MEM_ADDR			(MEM_LOG_ADDR+MEM_LOG_SIZE)
#define SHARE_MEM_SIZE			(0x1000)

#define TEA_SIZE				(64*1024) /* 64K */

#endif
