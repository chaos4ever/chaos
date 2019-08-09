// Abstract: Intel Pro/1000 PCI driver
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 2019 chaos development

#include <log/log.h>
#include <memory/memory.h>
#include <pci/pci.h>

#include "intel-pro-1000.h"

#define INTEL_VENDOR_ID                 0x8086

// Globals.
static pci_device_probe_type pci_device_probe[] =
{
    { INTEL_VENDOR_ID, E1000_DEV_ID_82540EM },
    { INTEL_VENDOR_ID, E1000_DEV_ID_82545EM },
    { INTEL_VENDOR_ID, E1000_DEV_ID_82540EP_LP },
    { INTEL_VENDOR_ID, E1000_DEV_ID_82541GI_LF },
    { INTEL_VENDOR_ID, E1000_DEV_ID_82573L },
    { INTEL_VENDOR_ID, E1000_DEV_ID_82574L },
    { INTEL_VENDOR_ID, E1000_DEV_ID_82571EB_COPPER },
    { INTEL_VENDOR_ID, E1000_DEV_ID_82583V },

    // Not supported for now - uses a different way of reading EEPROM data, like MAC address. The
    // Minix driver has it, so supporting these is easy, but I feel a bit hesitant about writing
    // code for hardware I don't have access to. (The VirtualBox driver doesn't use any of the
    // devices below)
    // { INTEL_VENDOR_ID, 0x10CD },
    // { INTEL_VENDOR_ID, 0x10DE },

    // End of list
    { 0xFFFF, 0xFFFF }
};

static log_structure_type log_structure;
static ipc_structure_type pci_structure;

// An empty tag list.
static tag_type empty_tag =
{
    0, 0, ""
};

// static int e1000_init(unsigned int instance, netdriver_addr_t *addr,
// 	uint32_t *caps, unsigned int *ticks);
// static void e1000_stop(void);
// static void e1000_set_mode(unsigned int, const netdriver_addr_t *,
// 	unsigned int);
static void e1000_set_hwaddr(e1000_t *device, netdriver_address_type *hwaddr);
// static int e1000_send(struct netdriver_data *data, size_t size);
// static ssize_t e1000_recv(struct netdriver_data *data, size_t max);
// static unsigned int e1000_get_link(uint32_t *);
// static void e1000_intr(unsigned int mask);
// static void e1000_tick(void);
// static int e1000_probe(e1000_t *e, int skip);
static void e1000_init_hw(e1000_t *device);
// static uint32_t e1000_reg_read(e1000_t *e, uint32_t reg);
static void e1000_reg_write(e1000_t *device, uint32_t reg, uint32_t value);
static void e1000_reg_set(e1000_t *device, uint32_t reg, uint32_t value);
static void e1000_reg_unset(e1000_t *device, uint32_t reg, uint32_t value);
static uint16_t eeprom_eerd(e1000_t *device, int reg);

// static int e1000_instance;
// static e1000_t e1000_state;

// static const struct netdriver e1000_table = {
// 	.ndr_name	= "em",
// 	.ndr_init	= e1000_init,
// 	.ndr_stop	= e1000_stop,
// 	.ndr_set_mode	= e1000_set_mode,
// 	.ndr_set_hwaddr	= e1000_set_hwaddr,
// 	.ndr_recv	= e1000_recv,
// 	.ndr_send	= e1000_send,
// 	.ndr_get_link	= e1000_get_link,
// 	.ndr_intr	= e1000_intr,
// 	.ndr_tick	= e1000_tick
// };

// /*
//  * The e1000 driver.
//  */
// int
// main(int argc, char * argv[])
// {

// 	env_setargs(argc, argv);

// 	/* Let the netdriver library take control. */
// 	netdriver_task(&e1000_table);

// 	return 0;
// }

// /*
//  * Initialize the e1000 driver and device.
//  */
// static int
// e1000_init(unsigned int instance, netdriver_addr_t * addr, uint32_t * caps,
// 	unsigned int * ticks)
// {
// 	e1000_t *e;
// 	int r;

// 	e1000_instance = instance;

// 	/* Clear state. */
// 	memset(&e1000_state, 0, sizeof(e1000_state));

// 	e = &e1000_state;

// 	/* Perform calibration. */
// 	if ((r = tsc_calibrate()) != OK)
// 		panic("tsc_calibrate failed: %d", r);

// 	/* See if we can find a matching device. */
// 	if (!e1000_probe(e, instance))
// 		return ENXIO;

// 	/* Initialize the hardware, and return its ethernet address. */
// 	e1000_init_hw(e, addr);

// 	*caps = NDEV_CAP_MCAST | NDEV_CAP_BCAST | NDEV_CAP_HWADDR;
// 	*ticks = sys_hz() / 10; /* update statistics 10x/sec */
// 	return OK;
// }

// /*
//  * Map flash memory.  This step is optional.
//  */
// static void
// e1000_map_flash(e1000_t * e, int devind, int did)
// {
// 	u32_t flash_addr, gfpreg, sector_base_addr;
// 	size_t flash_size;

// 	/* The flash memory is pointed to by BAR2.  It may not be present. */
// 	if ((flash_addr = pci_attr_r32(devind, PCI_BAR_2)) == 0)
// 		return;

// 	/* The default flash size. */
// 	flash_size = 0x10000;

// 	switch (did) {
// 	case E1000_DEV_ID_82540EM:
// 	case E1000_DEV_ID_82545EM:
// 	case E1000_DEV_ID_82540EP:
// 	case E1000_DEV_ID_82540EP_LP:
// 		return; /* don't even try */

// 	/* 82566/82567/82562V series support mapping 4kB of flash memory. */
// 	case E1000_DEV_ID_ICH10_D_BM_LM:
// 	case E1000_DEV_ID_ICH10_R_BM_LF:
// 		flash_size = 0x1000;
// 		break;
// 	}

// 	e->flash = vm_map_phys(SELF, (void *)flash_addr, flash_size);
// 	if (e->flash == MAP_FAILED)
// 		panic("e1000: couldn't map in flash");

// 	/* sector_base_addr is a "sector"-aligned address (4096 bytes). */
// 	gfpreg = E1000_READ_FLASH_REG(e, ICH_FLASH_GFPREG);
// 	sector_base_addr = gfpreg & FLASH_GFPREG_BASE_MASK;

// 	/* flash_base_addr is byte-aligned. */
// 	e->flash_base_addr = sector_base_addr << FLASH_SECTOR_ADDR_SHIFT;
// }

// Resets the card
static void e1000_reset_hw(e1000_t *device)
{
    // Assert a Device Reset signal.
    e1000_reg_set(device, E1000_REG_CTRL, E1000_REG_CTRL_RST);

    // Wait a millisecond. (One microsecond would be enough but we don't have any such kernel-level
    // calls in chaos at the moment.)
    system_sleep(1);
}

// Initialize and return the card's ethernet address.
static void e1000_init_addr(e1000_t *device, netdriver_address_type *network_address)
{
    // Read Ethernet Address from EEPROM. (The original Minix code had
    // support here for being able to override the MAC address using
    // an env variable as well; we ignore this for now.)
    for (int i = 0; i < 3; i++) {
        uint16_t word = eeprom_eerd(device, i);
        network_address->address[i * 2]     = (word & 0x00FF);
        network_address->address[i * 2 + 1] = (word & 0xFF00) >> 8;
    }

    // Set Receive Address.
    e1000_set_hwaddr(device, &device->network_address);

    log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "Ethernet Address %x:%x:%x:%x:%x:%x\n",
        network_address->address[0], network_address->address[1], network_address->address[2],
        network_address->address[3], network_address->address[4], network_address->address[5]);
}

// /*
//  * Initialize receive and transmit buffers.
//  */
// static void
// e1000_init_buf(e1000_t * e)
// {
// 	phys_bytes rx_desc_p, rx_buff_p;
// 	phys_bytes tx_desc_p, tx_buff_p;
// 	int i;

// 	/* Number of descriptors. */
// 	e->rx_desc_count = E1000_RXDESC_NR;
// 	e->tx_desc_count = E1000_TXDESC_NR;

// 	/* Allocate receive descriptors. */
// 	if ((e->rx_desc = alloc_contig(sizeof(e1000_rx_desc_t) *
// 	    e->rx_desc_count, AC_ALIGN4K, &rx_desc_p)) == NULL)
// 		panic("failed to allocate RX descriptors");

// 	memset(e->rx_desc, 0, sizeof(e1000_rx_desc_t) * e->rx_desc_count);

// 	/* Allocate receive buffers. */
// 	e->rx_buffer_size = E1000_RXDESC_NR * E1000_IOBUF_SIZE;

// 	if ((e->rx_buffer = alloc_contig(e->rx_buffer_size, AC_ALIGN4K,
// 	    &rx_buff_p)) == NULL)
// 		panic("failed to allocate RX buffers");

// 	/* Set up receive descriptors. */
// 	for (i = 0; i < E1000_RXDESC_NR; i++)
// 		e->rx_desc[i].buffer = rx_buff_p + i * E1000_IOBUF_SIZE;

// 	/* Allocate transmit descriptors. */
// 	if ((e->tx_desc = alloc_contig(sizeof(e1000_tx_desc_t) *
// 	    e->tx_desc_count, AC_ALIGN4K, &tx_desc_p)) == NULL)
// 		panic("failed to allocate TX descriptors");

// 	memset(e->tx_desc, 0, sizeof(e1000_tx_desc_t) * e->tx_desc_count);

// 	/* Allocate transmit buffers. */
// 	e->tx_buffer_size = E1000_TXDESC_NR * E1000_IOBUF_SIZE;

// 	if ((e->tx_buffer = alloc_contig(e->tx_buffer_size, AC_ALIGN4K,
// 	    &tx_buff_p)) == NULL)
// 		panic("failed to allocate TX buffers");

// 	/* Set up transmit descriptors. */
// 	for (i = 0; i < E1000_TXDESC_NR; i++)
// 		e->tx_desc[i].buffer = tx_buff_p + i * E1000_IOBUF_SIZE;

// 	/* Set up the receive ring registers. */
// 	e1000_reg_write(e, E1000_REG_RDBAL, rx_desc_p);
// 	e1000_reg_write(e, E1000_REG_RDBAH, 0);
// 	e1000_reg_write(e, E1000_REG_RDLEN,
// 	    e->rx_desc_count * sizeof(e1000_rx_desc_t));
// 	e1000_reg_write(e, E1000_REG_RDH, 0);
// 	e1000_reg_write(e, E1000_REG_RDT, e->rx_desc_count - 1);
// 	e1000_reg_unset(e, E1000_REG_RCTL, E1000_REG_RCTL_BSIZE);
// 	e1000_reg_set(e, E1000_REG_RCTL, E1000_REG_RCTL_EN);

// 	/* Set up the transmit ring registers. */
// 	e1000_reg_write(e, E1000_REG_TDBAL, tx_desc_p);
// 	e1000_reg_write(e, E1000_REG_TDBAH, 0);
// 	e1000_reg_write(e, E1000_REG_TDLEN,
// 	    e->tx_desc_count * sizeof(e1000_tx_desc_t));
// 	e1000_reg_write(e, E1000_REG_TDH, 0);
// 	e1000_reg_write(e, E1000_REG_TDT, 0);
// 	e1000_reg_set(e, E1000_REG_TCTL,
// 	    E1000_REG_TCTL_EN | E1000_REG_TCTL_PSP);
// }

// Initializes the hardware
static void e1000_init_hw(e1000_t *device)
{
    int r, i;

    device->irq_hook = device->irq;

    /*
     * Set the interrupt handler and policy.  Do not automatically
     * reenable interrupts.  Return the IRQ line number on interrupts.
     */
    // if ((r = sys_irqsetpolicy(e->irq, 0, &e->irq_hook)) != OK)
    //     panic("sys_irqsetpolicy failed: %d", r);
    // if ((r = sys_irqenable(&e->irq_hook)) != OK)
    //     panic("sys_irqenable failed: %d", r);

    // Reset the hardware.
    e1000_reset_hw(device);

    // Initialize appropriately, according to section 14.3 General
    // Configuration of Intel's Gigabit Ethernet Controllers Software
    // Developer's Manual.
    e1000_reg_set(device, E1000_REG_CTRL,
                  E1000_REG_CTRL_ASDE | E1000_REG_CTRL_SLU);
    e1000_reg_unset(device, E1000_REG_CTRL, E1000_REG_CTRL_LRST);
    e1000_reg_unset(device, E1000_REG_CTRL, E1000_REG_CTRL_PHY_RST);
    e1000_reg_unset(device, E1000_REG_CTRL, E1000_REG_CTRL_ILOS);
    e1000_reg_write(device, E1000_REG_FCAL, 0);
    e1000_reg_write(device, E1000_REG_FCAH, 0);
    e1000_reg_write(device, E1000_REG_FCT, 0);
    e1000_reg_write(device, E1000_REG_FCTTV, 0);
    e1000_reg_unset(device, E1000_REG_CTRL, E1000_REG_CTRL_VME);

    // Clear Multicast Table Array (MTA).
    for (i = 0; i < 128; i++)
    {
        e1000_reg_write(device, E1000_REG_MTA + i * 4, 0);
    }

    // Initialize statistics registers.
    for (i = 0; i < 64; i++)
    {
        e1000_reg_write(device, E1000_REG_CRCERRS + i * 4, 0);
    }

    // Acquire MAC address and set up RX/TX buffers.
    e1000_init_addr(device, &device->network_address);
    e1000_init_buf(device);

    // Enable interrupts.
    e1000_reg_set(device, E1000_REG_IMS, E1000_REG_IMS_LSC | E1000_REG_IMS_RXO |
                  E1000_REG_IMS_RXT | E1000_REG_IMS_TXQE | E1000_REG_IMS_TXDW);
}

// /*
//  * Set receive mode.
//  */
// static void
// e1000_set_mode(unsigned int mode, const netdriver_addr_t * mcast_list __unused,
// 	unsigned int mcast_count __unused)
// {
// 	e1000_t *e;
// 	uint32_t rctl;

// 	e = &e1000_state;

// 	rctl = e1000_reg_read(e, E1000_REG_RCTL);

// 	rctl &= ~(E1000_REG_RCTL_BAM | E1000_REG_RCTL_MPE |
// 	    E1000_REG_RCTL_UPE);

// 	/* TODO: support for NDEV_MODE_DOWN and multicast lists */
// 	if (mode & NDEV_MODE_BCAST)
// 		rctl |= E1000_REG_RCTL_BAM;
// 	if (mode & (NDEV_MODE_MCAST_LIST | NDEV_MODE_MCAST_ALL))
// 		rctl |= E1000_REG_RCTL_MPE;
// 	if (mode & NDEV_MODE_PROMISC)
// 		rctl |= E1000_REG_RCTL_BAM | E1000_REG_RCTL_MPE |
// 		    E1000_REG_RCTL_UPE;

// 	e1000_reg_write(e, E1000_REG_RCTL, rctl);
// }

// Sets hardware address.
static void e1000_set_hwaddr(e1000_t *device, netdriver_address_type *hwaddr)
{
    e1000_reg_write(device, E1000_REG_RAL,
        *(const uint32_t *)(&hwaddr->address[0]));
    e1000_reg_write(device, E1000_REG_RAH,
        *(const uint16_t *)(&hwaddr->address[4]));
    e1000_reg_set(device, E1000_REG_RAH, E1000_REG_RAH_AV);
}

// /*
//  * Try to send a packet.
//  */
// static int
// e1000_send(struct netdriver_data * data, size_t size)
// {
// 	e1000_t *e;
// 	e1000_tx_desc_t *desc;
// 	unsigned int head, tail, next;
// 	char *ptr;

// 	e = &e1000_state;

// 	if (size > E1000_IOBUF_SIZE)
// 		panic("packet too large to send");

// 	/*
// 	 * The queue tail must not advance to the point that it is equal to the
// 	 * queue head, since this condition indicates that the queue is empty.
// 	 */
// 	head = e1000_reg_read(e, E1000_REG_TDH);
// 	tail = e1000_reg_read(e, E1000_REG_TDT);
// 	next = (tail + 1) % e->tx_desc_count;

// 	if (next == head)
// 		return SUSPEND;

// 	/* The descriptor to use is the one pointed to by the current tail. */
// 	desc = &e->tx_desc[tail];

// 	/* Copy the packet from the caller. */
// 	ptr = e->tx_buffer + tail * E1000_IOBUF_SIZE;

// 	netdriver_copyin(data, 0, ptr, size);

// 	/* Mark this descriptor ready. */
// 	desc->status = 0;
// 	desc->length = size;
// 	desc->command = E1000_TX_CMD_EOP | E1000_TX_CMD_FCS | E1000_TX_CMD_RS;

// 	/* Increment tail.  Start transmission. */
// 	e1000_reg_write(e, E1000_REG_TDT, next);

// 	return OK;
// }

// /*
//  * Try to receive a packet.
//  */
// static ssize_t
// e1000_recv(struct netdriver_data * data, size_t max)
// {
// 	e1000_t *e;
// 	e1000_rx_desc_t *desc;
// 	unsigned int head, tail, cur;
// 	char *ptr;
// 	size_t size;

// 	e = &e1000_state;

// 	/* If the queue head and tail are equal, the queue is empty. */
// 	head = e1000_reg_read(e, E1000_REG_RDH);
// 	tail = e1000_reg_read(e, E1000_REG_RDT);

// 	E1000_DEBUG(4, ("%s: head=%u, tail=%u\n",
// 	    netdriver_name(), head, tail));

// 	if (head == tail)
// 		return SUSPEND;

// 	/* Has a packet been received? */
// 	cur = (tail + 1) % e->rx_desc_count;
// 	desc = &e->rx_desc[cur];

// 	if (!(desc->status & E1000_RX_STATUS_DONE))
// 		return SUSPEND;

// 	/*
// 	 * HACK: we expect all packets to fit in a single receive buffer.
// 	 * Eventually, some sort of support to deal with packets spanning
// 	 * multiple receive descriptors should be added.  For now, we panic,
// 	 * so that we can continue after the restart; this is already an
// 	 * improvement over freezing (the old behavior of this driver).
// 	 */
// 	size = desc->length;

// 	if (!(desc->status & E1000_RX_STATUS_EOP))
// 		panic("received packet too large");

// 	/* Copy the packet to the caller. */
// 	ptr = e->rx_buffer + cur * E1000_IOBUF_SIZE;

// 	if (size > max)
// 		size = max;

// 	netdriver_copyout(data, 0, ptr, size);

// 	/* Reset the descriptor. */
// 	desc->status = 0;

// 	/* Increment tail. */
// 	e1000_reg_write(e, E1000_REG_RDT, cur);

// 	/* Return the size of the received packet. */
// 	return size;
// }

// /*
//  * Return the link and media status.
//  */
// static unsigned int
// e1000_get_link(uint32_t * media)
// {
// 	uint32_t status, type;

// 	status = e1000_reg_read(&e1000_state, E1000_REG_STATUS);

// 	if (!(status & E1000_REG_STATUS_LU))
// 		return NDEV_LINK_DOWN;

// 	if (status & E1000_REG_STATUS_FD)
// 		type = IFM_ETHER | IFM_FDX;
// 	else
// 		type = IFM_ETHER | IFM_HDX;

// 	switch (status & E1000_REG_STATUS_SPEED) {
// 	case E1000_REG_STATUS_SPEED_10:
// 		type |= IFM_10_T;
// 		break;
// 	case E1000_REG_STATUS_SPEED_100:
// 		type |= IFM_100_TX;
// 		break;
// 	case E1000_REG_STATUS_SPEED_1000_A:
// 	case E1000_REG_STATUS_SPEED_1000_B:
// 		type |= IFM_1000_T;
// 		break;
// 	}

// 	*media = type;
// 	return NDEV_LINK_UP;
// }

// /*
//  * Handle an interrupt.
//  */
// static void
// e1000_intr(unsigned int __unused mask)
// {
// 	e1000_t *e;
// 	u32_t cause;

// 	E1000_DEBUG(3, ("e1000: interrupt\n"));

// 	e = &e1000_state;

// 	/* Reenable interrupts. */
// 	if (sys_irqenable(&e->irq_hook) != OK)
// 		panic("failed to re-enable IRQ");

// 	/* Read the Interrupt Cause Read register. */
// 	if ((cause = e1000_reg_read(e, E1000_REG_ICR)) != 0) {
// 		if (cause & E1000_REG_ICR_LSC)
// 			netdriver_link();

// 		if (cause & (E1000_REG_ICR_RXO | E1000_REG_ICR_RXT))
// 			netdriver_recv();

// 		if (cause & (E1000_REG_ICR_TXQE | E1000_REG_ICR_TXDW))
// 			netdriver_send();
// 	}
// }

// /*
//  * Do regular processing.
//  */
// static void
// e1000_tick(void)
// {
// 	e1000_t *e;

// 	e = &e1000_state;

// 	/* Update statistics. */
// 	netdriver_stat_ierror(e1000_reg_read(e, E1000_REG_RXERRC));
// 	netdriver_stat_ierror(e1000_reg_read(e, E1000_REG_CRCERRS));
// 	netdriver_stat_ierror(e1000_reg_read(e, E1000_REG_MPC));
// 	netdriver_stat_coll(e1000_reg_read(e, E1000_REG_COLC));
// }

// /*
//  * Stop the card.
//  */
// static void
// e1000_stop(void)
// {
// 	e1000_t *e;

// 	e = &e1000_state;

// 	E1000_DEBUG(3, ("%s: stop()\n", netdriver_name()));

// 	e1000_reset_hw(e);
// }

// Read from a memory mapped register
static uint32_t e1000_reg_read(e1000_t *device, uint32_t reg)
{
    uint32_t value;

    // Ensure register number is sane.
    if (reg >= 0x1FFFF)
    {
        log_print_formatted(&log_structure, LOG_URGENCY_ERROR, "Invalid register read attempt: %x", reg);
        return -1;
    }

    value = *(volatile uint32_t *)(device->registers + reg);

    return value;
}

// Writes to a register.
static void e1000_reg_write(e1000_t *device, uint32_t reg, uint32_t value)
{
    // Ensure register number is sane.
    if (reg >= 0x1FFFF)
    {
        log_print_formatted(&log_structure, LOG_URGENCY_ERROR, "Invalid register write attempt: %x", reg);
    }

    /* Write to memory mapped register. */
    *(volatile uint32_t *)(device->registers + reg) = value;
}

// Sets bits in a register.
static void e1000_reg_set(e1000_t *device, uint32_t reg, uint32_t value)
{
    uint32_t data = e1000_reg_read(device, reg);

    // Set bits, and write back.
    e1000_reg_write(device, reg, data | value);
}

// Clear bits in a register.
static void e1000_reg_unset(e1000_t * e, uint32_t reg, uint32_t value)
{
    uint32_t data = e1000_reg_read(e, reg);

    // Unset bits, and write back.
    e1000_reg_write(e, reg, data & ~value);
}

// Read from EEPROM.
static uint16_t eeprom_eerd(e1000_t *device, int reg)
{
    uint16_t data;

    // Request EEPROM read.
    e1000_reg_write(device, E1000_REG_EERD,
        (reg << device->eeprom_addr_off) | (E1000_REG_EERD_START));

    // Wait until ready.
    while (!((data = (e1000_reg_read(device, E1000_REG_EERD))) &
        device->eeprom_done_bit));

    return data >> 16;
}

NORETURN static void loop(void)
{
    // Must not return; I think returning here is what caused an earlier page fault.
    // See this issue: https://github.com/chaos4ever/chaos/issues/155
    while (TRUE)
    {
        system_call_dispatch_next();
    }
}

// Thread entry point. For each Intel Pro/1000 device detected, a thread is created with the
// according pci_device_info_type entry passed to the method.
static void handle_e1000(pci_device_info_type *device_info)
{
    // int r, devind, ioflag;
    // u16_t vid, did, cr;
    uint32_t status;
    // u32_t base, size;
    // const char *dname;
    e1000_t device;

    memory_set_uint8_t((uint8_t *) &device, 0, sizeof(e1000_t));

    // We found a matching card.  Set card-specific properties.

    switch (device_info->device_id) {
        case E1000_DEV_ID_82540EM:
        case E1000_DEV_ID_82545EM:
        case E1000_DEV_ID_82540EP_LP:
            device.eeprom_done_bit = (1 << 4);
            device.eeprom_addr_off = 8;
            break;

        default:
            device.eeprom_done_bit = (1 << 1);
            device.eeprom_addr_off = 2;
            break;
    }

    log_print_formatted(&log_structure, LOG_URGENCY_INFORMATIVE, "Intel PRO/1000 Gigabit Ethernet Card detected");

    // /* Reserve PCI resources found. */
    // pci_reserve(devind);

    if (device_info->resource[0].flags != PCI_RESOURCE_MEMORY)
    {
        log_print_formatted(&log_structure, LOG_URGENCY_EMERGENCY, "PCI bar is not for memory. This server only handles memory-mapped Intel PRO/100 cards.");

        // TODO: ugly workaround for #155
        loop();
    }

    // if ((e->regs = vm_map_phys(SELF, (void *)base, size)) == MAP_FAILED)
    //     panic("failed to map hardware registers from PCI");
    void *registers;
    int size = device_info->resource[0].end - device_info->resource[0].start;

    if (system_call_memory_reserve(device_info->resource[0].start, size, &registers) != STORM_RETURN_SUCCESS)
    {
        // TODO: ugly workaround for #155
        loop();
    }

    // TODO: Enable DMA bus mastering if necessary.
    // It can work without this, but quoting https://wiki.osdev.org/RTL8139:
    // "Some BIOS may enable Bus Mastering at startup, but some versions of qemu don't. You should thus be careful about this step. "

    // /* Optionally map flash memory. */
    // e1000_map_flash(e, devind, did);

    device.registers = registers;

    // Output debug information.
    status = e1000_reg_read(&device, E1000_REG_STATUS);
    log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "MEM at %lX, IRQ %d", device_info->resource[0].start, device_info->irq);
    log_print_formatted(&log_structure, LOG_URGENCY_DEBUG, "link %s, %s duplex",
                        status & 3 ? "up"   : "down", status & 1 ? "full" : "half");

    e1000_init_hw(&device);

    // TODO: ugly workaround for #155
    loop();
}

int main(void)
{
    pci_device_info_type *device_info;
    unsigned int number_of_devices;
    unsigned int counter;
    unsigned int probe_counter;

    system_process_name_set("intel-pro-1000");
    system_thread_name_set("Initialising");

    if (log_init(&log_structure, "intel-pro-1000", &empty_tag) != LOG_RETURN_SUCCESS)
    {
        return -1;
    }

    if (pci_init(&pci_structure, &empty_tag) != PCI_RETURN_SUCCESS)
    {
        log_print(&log_structure, LOG_URGENCY_EMERGENCY,
                  "Failed to create connection to PCI service.");
        return -1;
    }

    system_call_process_parent_unblock();

    for (probe_counter = 0; pci_device_probe[probe_counter].vendor_id !=
            0xFFFF; probe_counter++)
    {
        pci_device_exists(&pci_structure, &pci_device_probe[probe_counter],
                          &device_info, &number_of_devices);

        if (number_of_devices != 0)
        {
            for (counter = 0; counter < number_of_devices; counter++)
            {
                system_thread_create((thread_entry_point_type *) handle_e1000, &device_info[counter]);
            }
        }
    }

    return 0;
}
