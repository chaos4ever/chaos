// Abstract: Module player.
// Author: Erik Moren <nemo@chaosdev.org>
//
// © Copyright 1999 chaos development

#pragma once

#include <system/system.h>

#define BUFFER_SIZE 2000
#define FREQUENCY 22000
#define CHANNELS 4
#define ROWS 64
#define BYTES_PER_PATTERN ROWS*CHANNELS*4
//#define PAL 3546894
#define PAL 0x361F0E00     /* 3546894 << 2 */

typedef struct
{
    uint8_t name[20];
    uint8_t song_length;
    uint8_t play_sequence[128];
    uint8_t id[4];
    unsigned int physical_patterns;
} module_type;

typedef struct
{
    uint8_t name[22];
    uint16_t length;
    uint8_t finetune;
    uint8_t volume;
    uint16_t repeat_point;
    uint16_t repeat_length;
    uint8_t *data;
} sample_type;

typedef struct
{
    unsigned int sample_number;
    uint16_t length;
    uint8_t finetune;
    uint8_t volume;
    uint16_t repeat_point;
    uint16_t repeat_length;
    uint32_t scaling_factor;
    uint8_t *sample_data;
    uint16_t period_frequency;
    uint32_t ticks;
} channel_type;

typedef struct
{
    uint8_t sample_number : 8;
    uint16_t period_frequency : 12;
    uint8_t effect_number : 4;
    uint8_t effect_parameter : 8;
} __attribute__((packed)) note_type;
