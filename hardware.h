#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdbool.h>
#include <stdint.h>

// Initialize PS/2 controller and detect devices. IDT should be ready before
// calling.  This sets the availability flags for the keyboard and mouse and
// enables the devices if they are present.
void hardware_init(void);

// Query device availability after hardware_init()
bool keyboard_available(void);
bool mouse_available(void);

// Read all pending bytes from the PS/2 controller and route them
void ps2_flush_buffers(void);

#endif
