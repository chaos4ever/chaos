// Abstract: Types used by the sound library.
// Author: Erik Moren <nemo@chaosdev.org>

// © Copyright 1999 chaos development

#pragma once

// FIXME: Sound message structure.
typedef struct
{
    unsigned int frequency;
    unsigned int bits;

    // This is the size of the next sample played, or the size of the
    // buffers used when streaming data */
    unsigned int length;

    // An array of the data sent. The actual size of this will be
    // `length` bytes.
    uint8_t data[0];
} __attribute__ ((packed)) sound_message_type;
