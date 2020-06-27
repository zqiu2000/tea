/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#include <tea.h>
#include <string.h>
#include <vectors.h>
#include <lapic.h>
#include <idt.h>
#include <mod_init.h>

#define MSG_BYTES	(16)
#define MSG_CNT		(32)

enum {
	MESG_TYPE_ASYNC = 0,
	MESG_TYPE_SYNC,
};

enum {
	MESG_TEST,
};

struct mesg_payload {
	uint32_t mesg_type;
	union _mesg_data {
		uint8_t raw_dat[MSG_BYTES];
		struct dat {
			uint32_t dat0;
			uint32_t dat1;
			uint32_t dat2;
			uint32_t dat3;
		} mesg_dat;
	} mesg_data;
};

typedef struct _mesg_channel{
	uint32_t tx_id;
	uint32_t rx_id;
	struct mesg_payload mesg[MSG_CNT];
} mesg_channel;

extern uint8_t share_mem;

static mesg_channel *mesg_tx;
static mesg_channel *mesg_rx;

int send_msg_async(void)
{
	struct mesg_payload *mesg = &mesg_tx->mesg[mesg_tx->tx_id];
	char buff[MSG_BYTES];
	static int cnt = 0;

	sprintf(buff, "abcd%d", cnt++);

	mesg->mesg_type = MESG_TYPE_ASYNC;
	strncpy((char *)mesg->mesg_data.raw_dat, MSG_BYTES, buff, strnlen(buff, MSG_BYTES));

	mesg_tx->tx_id++;
	mesg_tx->tx_id %= MSG_CNT;
	if (mesg_tx->tx_id == mesg_tx->rx_id) {
		pr_err("ipc: tx channel overflow!!");
	}

	/* Ring the bell */
	apic_send_ipi(0, NMI_VECTOR);

	return 0;
}

int recv_msg_async(void *buf)
{
	struct mesg_payload *mesg = &mesg_rx->mesg[mesg_rx->rx_id];

	/* mesg->mesg_type = MESG_TYPE_ASYNC; */
	memcpy(buf, MSG_BYTES, mesg->mesg_data.raw_dat, MSG_BYTES);

	mesg_rx->rx_id++;
	mesg_rx->rx_id %= MSG_CNT;

	return 0;
}

int ipc_irq(struct irq_regs *reg, void *data)
{
	char buff[MSG_BYTES];

	while (mesg_rx->rx_id != mesg_rx->tx_id) {
		int cnt = 0;
		recv_msg_async((void*)buff);
		pr_debug("ipc: received %d msg %s\n", ++cnt, buff);
	}

	return 0;
}


static int ipc_init(void)
{
	int ret;

	memset(&share_mem, 0, SHARE_MEM_SIZE);

	pr_verb("ipc init: share mem %p - %p\n!\n", &share_mem, &share_mem + SHARE_MEM_SIZE);

	mesg_tx = (mesg_channel*)&share_mem;
	mesg_rx = mesg_tx + 1;

	ret = register_irq(I_X86_PLATFORM_IPI, ipc_irq, (void*)mesg_rx, "ipc");
	if (ret) {
		pr_err("Failed to register ipc irq!\n");
		return ret;
	}

	return 0;
}

module_init(ipc_init);
