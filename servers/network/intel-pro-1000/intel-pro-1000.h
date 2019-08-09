// Abstract: Intel Pro/1000 PCI driver
// Author: Per Lundberg <per@chaosdev.io>

// © Copyright 2019 chaos development

#include <ipv4/ipv4.h>

#include "e1000_reg.h"
#include "e1000_pci.h"

typedef struct
{
    uint8_t address[IPV4_ETHERNET_ADDRESS_LENGTH];
} netdriver_address_type;

//
// Describes the state of an Intel Pro/1000 card.
//
typedef struct e1000
{
    int irq;
    int irq_hook;
    netdriver_address_type network_address;
    uint8_t *registers;     // Memory mapped hardware registers.

    // u8_t *flash;		  /**< Optional flash memory. */
    // u32_t flash_base_addr;	  /**< Flash base address. */

    int eeprom_done_bit;    // Offset of the EERD.DONE bit.
    int eeprom_addr_off;    // Offset of the EERD.ADDR field.

    // e1000_rx_desc_t *rx_desc;	  /**< Receive Descriptor table. */
    // int rx_desc_count;		  /**< Number of Receive Descriptors. */
    // char *rx_buffer;		  /**< Receive buffer returned by malloc(). */
    // int rx_buffer_size;		  /**< Size of the receive buffer. */

    // e1000_tx_desc_t *tx_desc;	  /**< Transmit Descriptor table. */
    // int tx_desc_count;		  /**< Number of Transmit Descriptors. */
    // char *tx_buffer;		  /**< Transmit buffer returned by malloc(). */
    // int tx_buffer_size;		  /**< Size of the transmit buffer. */
} e1000_t;
