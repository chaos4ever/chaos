
#pragma once

// For reference, refer to: http://pinout.xyz/ and BCM2835-ARM-Peripherals.pdf

void gpio_init (void);
void gpio_enable_output (int pin);
void gpio_set_pin (int pin);
void gpio_clear_pin (int pin);

#define GPIO_FUNCTION_SELECT   0
#define GPIO_SET_PIN           7
#define GPIO_CLEAR_PIN        10
