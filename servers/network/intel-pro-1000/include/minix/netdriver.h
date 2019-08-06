// Based on the file with the same name from the Minix 3 source code

#pragma once

#include <stddef.h> // size_t

typedef struct
{
} netdriver_addr_t;

/* Opaque data structure for copying in and out actual packet data. */
struct netdriver_data;

/* Information and function call table for network drivers. */
struct netdriver {
	const char *ndr_name;
	int (* ndr_init)(unsigned int instance, netdriver_addr_t * hwaddr,
	    uint32_t * caps, unsigned int * ticks);
	void (* ndr_stop)(void);
	void (* ndr_set_mode)(unsigned int mode,
	    const netdriver_addr_t * mcast_list, unsigned int mcast_count);
	void (* ndr_set_caps)(uint32_t caps);
	void (* ndr_set_flags)(uint32_t flags);
	void (* ndr_set_media)(uint32_t media);
	void (* ndr_set_hwaddr)(const netdriver_addr_t * hwaddr);
	ssize_t (* ndr_recv)(struct netdriver_data * data, size_t max);
	int (* ndr_send)(struct netdriver_data * data, size_t size);
	unsigned int (* ndr_get_link)(uint32_t * media);
	void (* ndr_intr)(unsigned int mask);
	void (* ndr_tick)(void);
	void (* ndr_other)(const message * m_ptr, int ipc_status);
};

/* Functions defined by libnetdriver. */
void netdriver_task(const struct netdriver * ndp);
