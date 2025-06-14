#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>
#include <stdbool.h>

// Initialize keyboard/mouse. IDT should be ready before calling.
void hardware_init(void);

// Keyboard
bool keyboard_available(void);
uint8_t keyboard_read_scan(void);

// Mouse
bool mouse_available(void);
void mouse_enable(void);
bool mouse_read_packet(uint8_t packet[3]); // Returns true if a packet was read

#endif
