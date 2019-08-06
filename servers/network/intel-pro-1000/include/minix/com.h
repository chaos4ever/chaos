// Based on https://github.com/Stichting-MINIX-Research-Foundation/minix/blob/03ac74ede908465cc64c671bbd209e761dc765dc/minix/include/minix/com.h

/* This file defines constants for use in message communication (mostly)
 * between system processes. */

#pragma once

/* Values for the 'link' field of initialization and status replies. */
#define NDEV_LINK_UNKNOWN   0   /* link status is unknown, assume up */
#define NDEV_LINK_UP        1   /* link is up */
#define NDEV_LINK_DOWN      2   /* link is down */

/* Bits in the 'caps' field of initialization and configuration requests. */
#define NDEV_CAP_CS_IP4_TX  0x01        /* IPv4 header checksum generation */
#define NDEV_CAP_CS_IP4_RX  0x02        /* IPv4 header checksum verification */
#define NDEV_CAP_CS_UDP_TX  0x04        /* UDP header checksum generation */
#define NDEV_CAP_CS_UDP_RX  0x08        /* UDP header checksum verification */
#define NDEV_CAP_CS_TCP_TX  0x10        /* TCP header checksum generation */
#define NDEV_CAP_CS_TCP_RX  0x20        /* TCP header checksum verification */
#define NDEV_CAP_MCAST      0x20000000  /* init only: mcast capable */
#define NDEV_CAP_BCAST      0x40000000  /* init only: bcast capable */
#define NDEV_CAP_HWADDR     0x80000000  /* init only: can set hwaddr */
