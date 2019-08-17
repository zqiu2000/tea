/*
 * Copyright (C) 2018 Intel Corporation.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

union u_qword {
	struct {
		uint32_t low;
		uint32_t high;
	} dwords;

	uint64_t qword;

};

/* MACRO related to string */
#define ULONG_MAX	((uint64_t)(~0UL))	/* 0xFFFFFFFF */
#define LONG_MAX	(ULONG_MAX >> 1U)	/* 0x7FFFFFFF */
#define LONG_MIN	(~LONG_MAX)		/* 0x80000000 */

static inline bool is_space(char c)
{
	return ((c == ' ') || (c == '\t'));
}

/* Function prototypes */
int32_t strcmp(const char *s1_arg, const char *s2_arg);
int32_t strncmp(const char *s1_arg, const char *s2_arg, size_t n_arg);
char *strncpy(char *d_arg, size_t dmax, const char *s_arg, size_t slen_arg);
char *strchr(char *s_arg, char ch);
size_t strnlen(const char *str_arg, size_t maxlen_arg);
int64_t strtol_deci(const char *nptr);
uint64_t strtoul_hex(const char *nptr);
char *strstr(const char *str1, size_t maxlen1,
			const char *str2, size_t maxlen2);

#endif
