/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef struct _tea_cfg {
	int log_timestamp;
} tea_cfg_t;

extern tea_cfg_t tea_cfg;

int tea_config_init(void);

#endif
