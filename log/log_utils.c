/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <stdarg.h>
#include <types.h>
#include <raw_io.h>
#include <string.h>
#include <memlog.h>
#include <div64.h>
#include <uart.h>

#define is_digit(c)	((c) >= '0' && (c) <= '9')
#define F_LONG	(1)

struct print_parm {
	void (*output_char)(char byte, void *data);
	void *data;
};

struct sprint_ctx {
	char *buf;
	size_t size;
};

static void buffer_saver(char byte, void *data)
{
	struct sprint_ctx *ctx = (struct sprint_ctx *)data;

	if (ctx->size) {
		*ctx->buf = byte;
		ctx->buf++;
		ctx->size--;
	}
}

static void log_distributer(char byte, void *data)
{
	uart_tx(byte);
	memlog_write(byte);
}

static int parse_number(uint64_t num, int base, int width, struct print_parm *parm)
{
	char *digit = "0123456789ABCDEF";
	char tmp[64];
	int i = 0, count = 0;

	do {
		tmp[i++] = digit[do_div(num, base)];
	} while (num);

	while (width > 0 && width-- > i)
		parm->output_char('0', parm->data), count++;

	while (i-- > 0)
		parm->output_char(tmp[i], parm->data), count++;

	return count;
}

static int skip_atoi(const char **s)
{
	int i = 0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

static int parse_print_args(const char *fmt, va_list args, void *data)
{
	struct print_parm *parm = (struct print_parm *)data;
	int count;
	char *s;
	int64_t va;
	int width;
	int flag;

	for (count = 0; *fmt != '\0'; fmt++) {
		if (*fmt != '%') {
			parm->output_char(*fmt, parm->data);
			count++;
			continue;
		}

		fmt++; /* This skips '%' */

		width = -1;
		if (is_digit(*fmt)) {
			width = skip_atoi(&fmt);
		}

		flag = 0;
		if (*fmt == 'l') {
			flag |= F_LONG;
			fmt++;
		}

		switch (*fmt) {
		case 'c':
			va = va_arg(args, int);
			parm->output_char((unsigned char)va, parm->data);
			count++;
			break;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			while (*s != '\0') {
				parm->output_char(*s++, parm->data);
				count++;
			}
			break;

		case 'd':
			if (flag & F_LONG)
				va = va_arg(args, int64_t);
			else
				va = va_arg(args, int);

			if (va < 0) {
				va = -va;
				parm->output_char('-', parm->data);
				count++;
			}
			count += parse_number(va, 10, width, parm);
			break;

		case 'u':
			if (flag & F_LONG)
				va = va_arg(args, uint64_t);
			else
				va = va_arg(args, unsigned int);

			count += parse_number(va, 10, width, parm);
			break;

		case 'x':
			if (flag & F_LONG)
				va = va_arg(args, uint64_t);
			else
				va = va_arg(args, unsigned int);

			count += parse_number(va, 16, width, parm);
			break;

		case 'o':
			if (flag & F_LONG)
				va = va_arg(args, uint64_t);
			else
				va = va_arg(args, unsigned int);

			count += parse_number(va, 8, width, parm);
			break;

		case 'p':
			width = 2*sizeof(void *);
			va = (unsigned long)va_arg(args, void *);
			count += parse_number(va, 16, width, parm);
			break;

		/* TODO: Add more formats */
		default:
			/* Unsupported format, warning by "#^!"*/
			parm->output_char('#', parm->data), count++;
			parm->output_char('^', parm->data), count++;
			parm->output_char('!', parm->data), count++;
			break;
		}
	}

	return count;
}

static int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
	struct print_parm parm;
	struct sprint_ctx ctx;

	if (buf) {
		ctx.buf = buf;
		ctx.size = size;
		parm.output_char = buffer_saver;
		parm.data = &ctx;
	} else {
		parm.output_char = log_distributer;
		parm.data = &parm;
	}

	return parse_print_args(fmt, args, &parm);
}

int snprintf(char *buf, size_t size, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, size, fmt, args);
	va_end(args);

	return i;
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, UINT32_MAX, fmt, args);
	va_end(args);

	return i;
}

void printf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsnprintf(NULL, 0, fmt, args);
	va_end(args);
}
