/*
 * $Tea$
 * SPDX-License-Identifier: BSD-3-Clause
 * Author:
 * 	Qiu Zanxiong <zqiu2000@126.com>
 */

#ifndef _MSG_DEFS_H_
#define _MSG_DEFS_H_


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

#endif
