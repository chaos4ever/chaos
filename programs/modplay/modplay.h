// Abstract: Module player.
// Author: Erik Moren <nemo@chaosdev.org>
//
// © Copyright 1999 chaos development

#pragma once

#define BUFFER_SIZE 2000
#define NUM_CHANNELS 1      // 1 = mono, 2 = stereo
#define BYTES_PER_SAMPLE 1  // 1 = 8-bit, 2 = 16-bit

// Note: hxcmod_setcfg must be called if you wish to use something different than this.
#define FREQUENCY 44100
