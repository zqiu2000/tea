/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <raw_io.h>
#include <uart.h>
#include <mod_init.h>

/*TODO add uart init functions*/
int uart_tx(unsigned char byte)
{
	uint8_t reg;

	do {
		reg = inb(0x3f8 + UART16550_LSR);
	} while (!(reg & LSR_THRE) || !(reg & LSR_TEMT));

	outb(byte, 0x3f8);

	if (byte == '\n')
		outb('\r', 0x3f8);

	return 0;
}

int uart_init(void)
{
	pr_info("uart init!\n");

	/* TODO: add uart init code here */
	return 0;
}

early_init(uart_init);