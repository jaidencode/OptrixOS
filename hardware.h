#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdbool.h>
#include <stdint.h>

// Initialize keyboard/mouse. IDT should be ready before calling.
void hardware_init(void);

// Keyboard
uhu5r1-codex/fix-mouse-and-keyboard-functionality
bool keyboard_available(void);
void keyboard_enable(void);
uint8_t keyboard_read_scan(void);
=======
bool keyboard_available(void);
void keyboard_enable(void);
uint8_t keyboard_read_scan(void);
main

// Mouse
bool mouse_available(void);
void mouse_enable(void);
bool mouse_read_packet(uint8_t packet[3]); // Returns true if a packet was read

#endif
