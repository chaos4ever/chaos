// Abstract: Set up the machine to a usable state and pass on control to the kernel.
// Author: Johannes Ridderstedt <jojo@chaosdev.io>
//         Per Lundberg <per@chaosdev.io>
//
// © Copyright 2016 chaos development.

#include <gpio.h>
#include <debug.h>

int _start(void) __attribute__((naked));
int _start(void)
{
    // Make debug_print output over both the serial port and to the screen
    
    debug_init();
    debug_print_simple("Raspberry Pi booting up!\n");
    
    // For our first boot, blink LEDs and cycle a few pixels on the screen (TODO)

    gpio_init();
    gpio_enable_output(16);

    while (1)
    {
        gpio_set_pin(16);

        for (int i = 0; i < 1500000; i++);

        gpio_clear_pin(16);

        for (int i = 0; i < 1500000; i++);
        
        debug_print_simple("GPIO 16 / LED toggled! :-)\n");
    }
}
