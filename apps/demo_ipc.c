/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <ipc/ipc.h>


int demo_ipc(void)
{
	int msg_cnt = 0;

	while (msg_cnt++ < 80) {
		pr_info("demo ipc, send %d msg\n");
		send_msg_async();
		//udelay(1000*1000);
	}

	return 0;
}
