/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#ifndef _RAW_IO_H_
#define _RAW_IO_H_

#include <types.h>

#define readb(_a) (*(volatile const unsigned char *) (_a))
#define readw(_a) (*(volatile const unsigned short *) (_a))
#define readl(_a) (*(volatile const unsigned int *) (_a))

#define writeb(_v, _a) (*(volatile unsigned char *) (_a) = (_v))
#define writew(_v, _a) (*(volatile unsigned short *) (_a) = (_v))
#define writel(_v, _a) (*(volatile unsigned int *) (_a) = (_v))

static inline __attribute__((always_inline)) uint8_t read8(const volatile void *addr)
{
	return *((volatile uint8_t *)(addr));
}

static inline __attribute__((always_inline)) uint16_t read16(const volatile void *addr)
{
	return *((volatile uint16_t *)(addr));
}

static inline __attribute__((always_inline)) uint32_t read32(const volatile void *addr)
{
	return *((volatile uint32_t *)(addr));
}

static inline __attribute__((always_inline)) void write8(volatile void *addr, uint8_t value)
{
	*((volatile uint8_t *)(addr)) = value;
}

static inline __attribute__((always_inline)) void write16(volatile void *addr, uint16_t value)
{
	*((volatile uint16_t *)(addr)) = value;
}

static inline __attribute__((always_inline)) void write32(volatile void *addr, uint32_t value)
{
	*((volatile uint32_t *)(addr)) = value;
}

static inline unsigned int inl(int port)
{
	unsigned long val;
	__asm__ __volatile__("inl %w1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

static inline unsigned short inw(int port)
{
	unsigned short val;
	__asm__ __volatile__("inw %w1, %w0" : "=a"(val) : "Nd"(port));
	return val;
}

static inline unsigned char inb(int port)
{
	unsigned char val;
	__asm__ __volatile__("inb %w1, %b0" : "=a"(val) : "Nd"(port));
	return val;
}

static inline void outl(unsigned int val, int port)
{
	__asm__ __volatile__("outl %0, %w1" : : "a"(val), "Nd"(port));
}

static inline void outw(unsigned short val, int port)
{
	__asm__ __volatile__("outw %w0, %w1" : : "a"(val), "Nd"(port));
}

static inline void outb(unsigned char val, int port)
{
	__asm__ __volatile__("outb %b0, %w1" : : "a"(val), "Nd"(port));
}

static inline void outsl(int port, const void *addr, unsigned long count)
{
	__asm__ __volatile__("rep; outsl" : "+S"(addr), "+c"(count) : "d"(port));
}

static inline void outsw(int port, const void *addr, unsigned long count)
{
	__asm__ __volatile__("rep; outsw" : "+S"(addr), "+c"(count) : "d"(port));
}

static inline void outsb(int port, const void *addr, unsigned long count)
{
	__asm__ __volatile__("rep; outsb" : "+S"(addr), "+c"(count) : "d"(port));
}

static inline void insl(int port, void *addr, unsigned long count)
{
	__asm__ __volatile__("rep; insl" : "+D"(addr), "+c"(count) : "d"(port)
			     : "memory");
}

static inline void insw(int port, void *addr, unsigned long count)
{
	__asm__ __volatile__("rep; insw" : "+D"(addr), "+c"(count) : "d"(port)
			     : "memory");
}

static inline void insb(int port, void *addr, unsigned long count)
{
	__asm__ __volatile__("rep; insb" : "+D"(addr), "+c"(count) : "d"(port)
			     : "memory");
}

#endif
