
#include <gpio.h>
#include <stdint.h>

static volatile uint32_t *gpio_memory;

void gpio_init (void)
{
    // FIXME: Detect which Raspberry Pi we are running on
    gpio_memory = (uint32_t *) 0x20200000;
}

void gpio_enable_output (int pin)
{
    int set = 0;
    int offset = pin;
    
    while (offset > 10) {
        set += 1;
        offset -= 10;
    }
    
    gpio_memory[GPIO_FUNCTION_SELECT+set] |= (1 << (offset*3));
}

void gpio_set_pin (int pin)
{
    int set = 0;
    int offset = pin;
    
    while (offset > 32) {
        set += 1;
        offset -= 32;
    }
    
    gpio_memory[GPIO_SET_PIN+set] |= (1 << offset);
}

void gpio_clear_pin (int pin)
{
    int set = 0;
    int offset = pin;
    
    while (offset > 32) {
        set += 1;
        offset -= 32;
    }
    
    gpio_memory[GPIO_CLEAR_PIN+set] |= (1 << offset);
}
