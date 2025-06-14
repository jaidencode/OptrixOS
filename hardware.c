#include "hardware.h"
#include "keyboard.h"
#include "mouse.h"

// internal handlers from keyboard.c and mouse.c
extern void keyboard_handle_byte(uint8_t data);
extern void mouse_handle_byte(uint8_t data);

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_CMD 0x64

static bool has_keyboard = true;
static bool has_mouse = false;

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static void ps2_wait_input(void) {
    for (int i = 0; i < 100000; ++i) {
        if (!(inb(PS2_STATUS) & 0x02))
            return;
    }
}

static void ps2_wait_output(void) {
    for (int i = 0; i < 100000; ++i) {
        if (inb(PS2_STATUS) & 0x01)
            return;
    }
}

bool keyboard_available(void) { return has_keyboard; }

static void ps2_flush_output(void) {
    while (inb(PS2_STATUS) & 0x01)
        (void)inb(PS2_DATA);
}

static bool ps2_controller_self_test(void) {
    ps2_wait_input();
    outb(PS2_CMD, 0xAA);
    ps2_wait_output();
    return inb(PS2_DATA) == 0x55;
}

static bool ps2_port_test(int port) {
    ps2_wait_input();
    outb(PS2_CMD, port == 1 ? 0xAB : 0xA9);
    ps2_wait_output();
    return inb(PS2_DATA) == 0x00;
}

void ps2_flush_buffers(void) {
    while (inb(PS2_STATUS) & 0x01) {
        uint8_t status = inb(PS2_STATUS);
        uint8_t data = inb(PS2_DATA);
        if (status & 0x20) { // from mouse
            if (has_mouse)
                mouse_handle_byte(data);
        } else {
            if (has_keyboard)
                keyboard_handle_byte(data);
        }
    }
}


bool mouse_available(void) { return has_mouse; }

// Detect basic PS/2 devices. Call only after the IDT is installed to
// avoid unexpected interrupts resetting the CPU.
void hardware_init(void) {
    has_keyboard = false;
    has_mouse = false;

    /* Disable both PS/2 ports and flush any pending output */
    ps2_wait_input();
    outb(PS2_CMD, 0xAD); // disable first port
    ps2_wait_input();
    outb(PS2_CMD, 0xA7); // disable second port
    ps2_flush_output();

    ps2_controller_self_test();

    has_keyboard = ps2_port_test(1);
    has_mouse = ps2_port_test(2);

    if (has_keyboard)
        keyboard_enable();
    if (has_mouse)
        mouse_enable();
}

