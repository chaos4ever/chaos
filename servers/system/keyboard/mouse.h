// Abstract: Mouse commands. Probably inspired by the Linux kernel.
// Author: Per Lundberg <per@chaosdev.io>
//
// © Copyright 2000 chaos development
// © Copyright 2007 chaos development
// © Copyright 2013 chaos development
// © Copyright 2015-2016 chaos development

#pragma once

#include <system/system.h>

// Constants.
// How many times per second the mouse reports updates.
#define MOUSE_SAMPLES_PER_SECOND        100

// Mouse commands.
// Set resolution.
#define MOUSE_SET_RESOLUTION            0xE8

// Set 1:1 scaling.
#define MOUSE_SET_SCALE11               0xE6

// Set 2:1 scaling.
#define MOUSE_SET_SCALE21               0xE7

// Get scaling factor.
#define MOUSE_GET_SCALE                 0xE9

// Set stream mode.
#define MOUSE_SET_STREAM                0xEA

// Set sample rate (number of times the controller will poll the port per second).
#define MOUSE_SET_SAMPLE_RATE           0xF3

// Enable mouse device.
#define MOUSE_ENABLE_DEVICE             0xF4

// Disable mouse device.
#define MOUSE_DISABLE_DEVICE            0xF5

// Reset aux device.
#define MOUSE_RESET                     0xFF

// Command byte ACK.
#define MOUSE_ACK                       0xFA

#define MOUSE_INTERRUPTS_OFF            (CONTROLLER_MODE_KCC | \
                                         CONTROLLER_MODE_DISABLE_MOUSE | \
                                         CONTROLLER_MODE_SYS | \
                                         CONTROLLER_MODE_KEYBOARD_INTERRUPT)

#define MOUSE_INTERRUPTS_ON             (CONTROLLER_MODE_KCC | \
                                         CONTROLLER_MODE_SYS | \
                                         CONTROLLER_MODE_MOUSE_INTERRUPT | \
                                         CONTROLLER_MODE_KEYBOARD_INTERRUPT)

// Mouse movement flags.
#define MOUSE_NEGATIVE_Y                0x20
#define MOUSE_NEGATIVE_X                0x10

// And buttons...
#define MOUSE_BUTTON_MASK               7
#define MOUSE_BUTTON_RIGHT              1
#define MOUSE_BUTTON_MIDDLE             4
#define MOUSE_BUTTON_LEFT               2

// Some aux operations take long time.
#define MAX_RETRIES                     60

// Hardware defines.
#define MOUSE_IRQ                       12

// Function prototypes.
extern void mouse_handle_event(u8 scancode);
extern bool mouse_init(void);
extern void mouse_irq_handler(void *argument);
extern void mouse_main(void *argument);

// Common variables.
extern bool has_mouse;

// Mouse structure.
typedef struct
{
    unsigned int button_state;
    int delta_x;
    int delta_y;
    int x;
    int y;
} mouse_type;
