#include "keyboard.h"

#define PS2_DATA 0x60
#define PS2_STATUS 0x64
#define PS2_CMD 0x64

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

void keyboard_enable(void) {
    ps2_wait_input();
    outb(PS2_CMD, 0xAE); // enable first PS/2 port
    ps2_wait_input();
    outb(PS2_DATA, 0xF4); // enable scanning
    ps2_wait_output();
    (void)inb(PS2_DATA); // ack
}

void keyboard_init(void) {
    keyboard_enable();
}

uint8_t keyboard_read_scan(void) {
    if (inb(PS2_STATUS) & 0x01) {
        return inb(PS2_DATA);
    }
    return 0;
}

