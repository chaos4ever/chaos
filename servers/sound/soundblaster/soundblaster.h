// Authors: Per Lundberg <per@chaosdev.io>
//          Erik Moren <nemo@chaosdev.org>
//
// © Copyright 1999 chaos development
//
// Most of the information in this server is based on information
// found in sblaster.doc, which can be found in the same directory as
// this source code.

#pragma once

// Data ports.
#define DSP_MIXER_REGISTER              (base_port + 0x04)
#define DSP_MIXER_DATA                  (base_port + 0x05)
#define DSP_RESET                       (base_port + 0x06)
#define DSP_DATA_READ                   (base_port + 0x0A)
#define DSP_DATA_WRITE                  (base_port + 0x0C)
#define DSP_DATA_AVAILABLE              (base_port + 0x0E)

// Commands (sent to DSP_DATA_WRITE).
#define DSP_VERSION                     (0xE1)
#define DSP_SPEAKER_ON                  (0xD1)
#define DSP_SPEAKER_OFF                 (0xD3)
#define DSP_MODE_DIRECT_OUTPUT          (0x10)
#define DSP_MODE_DMA_8BIT_DAC           (0x14)
#define DSP_MODE_DMA_8BIT_AUTOINIT_DAC  (0x1C)
#define DSP_MODE_DIRECT_INPUT           (0x20)
#define DSP_MODE_DMA_8BIT_ADC           (0x24)
#define DSP_SET_TIME_CONSTANT           (0x40)
#define DSP_SET_DMA_BLOCK_SIZE          (0x48)

typedef struct
{
    unsigned int irq;
    unsigned int base_port;
    unsigned int dma_channel;
    unsigned int min_frequency_output;
    unsigned int max_frequency_output;
    bool supports_8bit_output;
    bool supports_16bit_output;
    bool supports_autoinit_dma;
    unsigned char *device_name;
} soundblaster_device_type;

typedef struct
{
    unsigned int frequency;

    // FIXME: Should this really be a bool ?
    bool _8bits;

    // TRUE if streaming audio, FALSE if just a single sample
    bool streaming;

    // TRUE if playing at the moment
    bool is_playing;
} soundblaster_event_type;

typedef struct
{
    uint8_t *data;
    bool is_full;
} double_buffer_type;

typedef struct
{
    ipc_structure_type *ipc_structure;
    unsigned int irq_number;
} irq_handler_data_type;

void irq_handler(irq_handler_data_type *irq_handler_data);
void handle_connection(mailbox_id_type reply_mailbox_id);
