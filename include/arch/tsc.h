#ifndef _TSC_H_
#define _TSC_H_

#include <types.h>

static inline uint64_t rdtsc(void)
{
	uint32_t lo, hi;

	asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
	return ((uint64_t)hi << 32U) | lo;
}

uint64_t ticks_2_ns(uint64_t ticks);

uint32_t get_tsc_khz(void);

uint64_t us_to_ticks(uint32_t us);

uint64_t ticks_to_us(uint64_t ticks);

uint64_t ticks_to_ms(uint64_t ticks);

void udelay(uint32_t us);

int tsc_init(void);

#endif
