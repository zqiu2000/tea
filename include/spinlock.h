/*
 * Copyright (C) 2018 Intel Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <tea.h>

typedef struct _spinlock {
	uint32_t head;
	uint32_t tail;

} spinlock_t;

/* Function prototypes */
static inline void spinlock_init(spinlock_t *lock)
{
	(void)memset(lock, 0U, sizeof(spinlock_t));
}

static inline void spinlock_lock(spinlock_t *lock)
{

	/* The lock function atomically increments and exchanges the head
	 * counter of the queue. If the old head of the queue is equal to the
	 * tail, we have locked the spinlock. Otherwise we have to wait.
	 */

	asm volatile ("   movl $0x1,%%eax\n"
		      "   lock xaddl %%eax,%[head]\n"
		      "   cmpl %%eax,%[tail]\n"
		      "   jz 1f\n"
		      "2: pause\n"
		      "   cmpl %%eax,%[tail]\n"
		      "   jnz 2b\n"
		      "1:\n"
		      :
		      :
		      [head] "m"(lock->head),
		      [tail] "m"(lock->tail)
		      : "cc", "memory", "eax");
}

static inline void spinlock_release(spinlock_t *lock)
{
	/* Increment tail of queue */
	asm volatile ("   lock incl %[tail]\n"
				:
				: [tail] "m" (lock->tail)
				: "cc", "memory");
}

static inline unsigned long spinlock_irq_save(spinlock_t *lock)
{
	unsigned long flags = local_irq_save();
	spinlock_lock(lock);
	return flags;
}

static inline void spinlock_irq_restore(spinlock_t *lock, unsigned long flags)
{
	spinlock_release(lock);
	local_irq_restore(flags);
}

#endif
