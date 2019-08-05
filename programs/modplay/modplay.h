// Abstract: Module player.
// Author: Erik Moren <nemo@chaosdev.org>
//
// © Copyright 1999 chaos development

#pragma once

// The number of samples per buffer.
#define NUMBER_OF_SAMPLES   2000
#define NUM_CHANNELS        1       // 1 = mono, 2 = stereo
#define BYTES_PER_SAMPLE    1       // 1 = 8-bit, 2 = 16-bit
#define BUFFER_SIZE         (NUMBER_OF_SAMPLES * NUM_CHANNELS * BYTES_PER_SAMPLE)

// Note: hxcmod_setcfg must be called if you wish to use something different than this.
#define FREQUENCY 44100

#define NUM_INSTRUMENTS     31

// Gets the smallest one of two values
#define MIN_OF_TWO(a, b)                 ((a) < (b) ? (a) : (b))

// Gets the largest one of two values
#define MAX_OF_TWO(a, b)                 ((a) > (b) ? (a) : (b))

//
// Colors
//

// Gray on black
#define DEFAULT_COLOR                   CONSOLE_BUFFER_COLOUR_LIGHT_GRAY

#define DEFAULT_INSTRUMENT_COLOR        CONSOLE_BUFFER_COLOUR_LIGHT_GRAY
#define BRIGHT_INSTRUMENT_COLOR         CONSOLE_BUFFER_COLOUR_BRIGHT_WHITE
