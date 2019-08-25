/*
 * Copyright (C) 2018 Intel Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Based on acrn timer.c
 */

#include <tea.h>
#include <tsc.h>
#include <raw_io.h>
#include <div64.h>

#define CAL_MS			10U
#define NS_PER_SEC		(1000000000)

static uint32_t tsc_khz = 0U;
static uint32_t tsc_mult, tsc_shift;

uint64_t ticks_2_ns(uint64_t ticks)
{
	return ticks * tsc_mult >> tsc_shift;
}

/* Borrowed from Linux kernel */
static void clocks_calc_mult_shift(u32 *mult, u32 *shift, u32 from, u32 to, u32 maxsec)
{
	u64 tmp;
	u32 sft, sftacc= 32;

	/*
	 * Calculate the shift factor which is limiting the conversion
	 * range:
	 */
	tmp = ((u64)maxsec * from) >> 32;
	while (tmp) {
		tmp >>=1;
		sftacc--;
	}

	/*
	 * Find the conversion shift/mult pair which has the best
	 * accuracy and fits the maxsec conversion range:
	 */
	for (sft = 32; sft > 0; sft--) {
		tmp = (u64) to << sft;
		tmp += from / 2;
		do_div(tmp, from);
		if ((tmp >> sftacc) == 0)
			break;
	}
	*mult = tmp;
	*shift = sft;
}

/* Return tsc khz */
static uint32_t pit_calibrate_tsc(uint32_t cal_ms_arg)
{
#define PIT_TICK_RATE	1193182U
#define PIT_TARGET	0x3FFFU
#define PIT_MAX_COUNT	0xFFFFU

	uint32_t cal_ms = cal_ms_arg;
	uint32_t initial_pit;
	uint16_t current_pit;
	uint32_t max_cal_ms;
	uint64_t time;
	uint8_t initial_pit_high, initial_pit_low;

	max_cal_ms = ((PIT_MAX_COUNT - PIT_TARGET) * 1000U) / PIT_TICK_RATE;
	cal_ms = min(cal_ms, max_cal_ms);

	/* Assume the 8254 delivers 18.2 ticks per second when 16 bits fully
	 * wrap.  This is about 1.193MHz or a clock period of 0.8384uSec
	 */
	initial_pit = (cal_ms * PIT_TICK_RATE) / 1000U;
	initial_pit += PIT_TARGET;
	initial_pit_high = (uint8_t)(initial_pit >> 8U);
	initial_pit_low = (uint8_t)initial_pit;

	/* Port 0x43 ==> Control word write; Data 0x30 ==> Select Counter 0,
	 * Read/Write least significant byte first, mode 0, 16 bits.
	 */

	outb(0x30U, 0x43U);
	outb(initial_pit_low, 0x40U);	/* Write LSB */
	outb(initial_pit_high, 0x40U);		/* Write MSB */

	time = rdtsc();

	do {
		/* Port 0x43 ==> Control word write; 0x00 ==> Select
		 * Counter 0, Counter Latch Command, Mode 0; 16 bits
		 */
		outb(0x00U, 0x43U);

		current_pit = (uint16_t)inb(0x40U);	/* Read LSB */
		current_pit |= (uint16_t)inb(0x40U) << 8U;	/* Read MSB */
		/* Let the counter count down to PIT_TARGET */
	} while (current_pit > PIT_TARGET);

	time = rdtsc() - time;

	return lldiv(time, cal_ms);
}

/*
 * Determine TSC frequency via CPUID 0x15 and 0x16.
 */
static uint32_t native_calibrate_tsc(void)
{
	uint32_t eax, ebx, ecx, edx;
	uint64_t tsc_hz = 0;

	cpuid(0, &eax, &ebx, &ecx, &edx);

	if (eax >= 0x15U) {
		uint32_t eax_denominator, ebx_numerator, ecx_hz, reserved;

		cpuid(0x15U, &eax_denominator, &ebx_numerator,
			&ecx_hz, &reserved);

		if ((eax_denominator != 0U) && (ebx_numerator != 0U)) {
			tsc_hz = lldiv((uint64_t) ecx_hz * ebx_numerator, eax_denominator);
		}
	}

	if ((tsc_hz == 0) && (eax >= 0x16U)) {
		uint32_t eax_base_mhz, ebx_max_mhz, ecx_bus_mhz, edx;
		cpuid(0x16U, &eax_base_mhz, &ebx_max_mhz, &ecx_bus_mhz, &edx);
		tsc_hz = (uint64_t) eax_base_mhz * 1000000U;
	}

	return lldiv(tsc_hz, 1000);
}

void calibrate_tsc(void)
{
	tsc_khz = native_calibrate_tsc();
	if (tsc_khz == 0U) {
		tsc_khz = pit_calibrate_tsc(CAL_MS);
	}

	pr_info("%s, tsc_khz=%u\n", __func__, tsc_khz);
}

uint32_t get_tsc_khz(void)
{
	return tsc_khz;
}

/* TODO: use mult&shift calculation method to eliminate this div */
uint64_t us_to_ticks(uint32_t us)
{
	return lldiv(((uint64_t)us * (uint64_t)tsc_khz), 1000);
}

uint64_t ticks_to_us(uint64_t ticks)
{
	return lldiv((ticks * 1000) , tsc_khz);
}

uint64_t ticks_to_ms(uint64_t ticks)
{
	return lldiv(ticks, tsc_khz);
}

void udelay(uint32_t us)
{
	uint64_t dest_tsc, delta_tsc;

	/* Calculate number of ticks to wait */
	delta_tsc = us_to_ticks(us);
	dest_tsc = rdtsc() + delta_tsc;

	/* Loop until time expired */
	while (rdtsc() < dest_tsc) {
	}
}

int tsc_init(void)
{
	calibrate_tsc();
	clocks_calc_mult_shift(&tsc_mult, &tsc_shift, tsc_khz * 1000, NS_PER_SEC, 2000);

	return 0;
}

/*early_init(tsc_init);*/