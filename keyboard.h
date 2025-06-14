#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

// Initialize and enable keyboard if present
void keyboard_enable(void);
void keyboard_init(void); // simple wrapper for enable

bool keyboard_available(void); // defined in hardware.c
uint8_t keyboard_read_scan(void);

#endif

