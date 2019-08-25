/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>

/* Put tea configuration code in structure here */

tea_cfg_t tea_cfg = {
	.log_timestamp = 1,
};

int tea_config_init(void)
{
	tea_cfg.log_timestamp = 1;
	return 0;
}