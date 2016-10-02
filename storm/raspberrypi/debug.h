// Abstract: Debug routines with both serial and display output
// Author: Johannes Ridderstedt <jojo@chaosdev.io>
//
// © Copyright 2016 chaos development.

#pragma once

#define UART0_DR   0x00
#define UART0_FR   0x18

void debug_init(void);
void debug_print_simple(const char *string);
