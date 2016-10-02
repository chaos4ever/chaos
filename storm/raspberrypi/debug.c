// Abstract: Debug routines with both serial and display output
// Author: Johannes Ridderstedt <jojo@chaosdev.io>
//
// © Copyright 2016 chaos development.

#include <stdint.h>

#include <debug.h>

static volatile uint32_t *uart0_memory;

static void serial_init(void)
{
    uart0_memory = (uint32_t *) 0x20201000;
}

static void serial_write(uint8_t byte)
{
    while ((uart0_memory[UART0_FR] & (1 << 5)))
    {
    }
    
    uart0_memory[UART0_DR] = byte;

    // FIXME: This delay is arbitrary to not flood the UART
    for (int i = 0; i < 5000; i++);
}

static void serial_write_string(const char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        serial_write((uint8_t) string[i]);
    }
}

void debug_print_simple(const char *string)
{
    serial_write_string(string);
}

void debug_init(void)
{
    serial_init();
}
