/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <tsc.h>
#include <time/timer.h>

static struct timer time;
static uint64_t min, max;

static int timer_callback(void *data)
{
	uint64_t now = rdtsc();
	struct timer *t = (struct timer *)data;
	uint64_t delt = ticks_2_ns(now - t->tick);
	min = delt < min ? delt : min;
	max = delt > max ? delt : max;

	pr_info("%s fired, delt %ld ns, min %ld ns, max %ld ns\n", __func__, delt, min, max);

	time.tick = us_to_ticks(500*1000) + rdtsc();
	timer_add(&time);
	return 0;
}

int timer_demo(void)
{
	INIT_LIST_HEAD(&time.list);
	min = -1;
	time.t_call = timer_callback;
	time.data = (void *)&time;
	time.tick = us_to_ticks(500*1000) + rdtsc();
	timer_add(&time);

	return 0;
}
