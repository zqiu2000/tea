/*
 * Copyright (C) 2018 Intel Corporation.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _MEMORY_H_
#define _MEMORY_H_

void *memcpy(void *d, size_t dmax, const void *s, size_t slen);
void *memset(void *base, uint8_t v, size_t n);

#endif
