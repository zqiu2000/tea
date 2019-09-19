#ifndef _TIMER_H_
#define _TIMER_H_

#include <list.h>

struct timer {
	int (*t_call)(void *data);
	uint64_t tick;
	struct list_head list;
	void *data;
};

int timer_add(struct timer *p_timer);

#endif
