#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>
#include <stdint.h>

// Initialize and enable mouse if present
void mouse_enable(void);
void mouse_init(void); // wrapper

bool mouse_available(void); // flag in hardware.c
bool mouse_read_packet(uint8_t packet[3]);

#endif

