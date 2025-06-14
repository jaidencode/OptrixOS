#include "hardware.h"

#define PS2_DATA    0x60
#define PS2_STATUS  0x64
#define PS2_CMD     0x64

static bool has_keyboard = true;
static bool has_mouse = false;

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ volatile ("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

static void ps2_wait_input(void) {
    for (int i = 0; i < 100000; ++i)
        if (!(inb(PS2_STATUS) & 0x02))
            return;
}

static void ps2_wait_output(void) {
    for (int i = 0; i < 100000; ++i)
        if (inb(PS2_STATUS) & 0x01)
            return;
}

bool keyboard_available(void) { return has_keyboard; }

uint8_t keyboard_read_scan(void) {
    if (inb(PS2_STATUS) & 0x01) {
        return inb(PS2_DATA);
    }
    return 0;
}

bool mouse_available(void) { return has_mouse; }

void mouse_enable(void) {
    ps2_wait_input();
    outb(PS2_CMD, 0xA8);
    ps2_wait_input();
    outb(PS2_CMD, 0x20);
    ps2_wait_output();
    uint8_t status = inb(PS2_DATA);
    status |= 0x02;
    ps2_wait_input();
    outb(PS2_CMD, 0x60);
    ps2_wait_input();
    outb(PS2_DATA, status);
    ps2_wait_input();
    outb(PS2_CMD, 0xD4);
    ps2_wait_input();
    outb(PS2_DATA, 0xF6);
    ps2_wait_output();
    inb(PS2_DATA);
    ps2_wait_input();
    outb(PS2_CMD, 0xD4);
    ps2_wait_input();
    outb(PS2_DATA, 0xF4);
    ps2_wait_output();
    inb(PS2_DATA);
    has_mouse = true;
}

bool mouse_read_packet(uint8_t packet[3]) {
    static int phase = 0;
    static uint8_t bytes[3];
    if (!(inb(PS2_STATUS) & 0x01))
        return false;
    uint8_t data = inb(PS2_DATA);
    if (!has_mouse)
        return false;
    bytes[phase++] = data;
    if (phase == 3) {
        for (int i = 0; i < 3; ++i) packet[i] = bytes[i];
        phase = 0;
        return true;
    }
    return false;
}

// Detect basic PS/2 devices. Call only after the IDT is installed to
// avoid unexpected interrupts resetting the CPU.
void hardware_init(void) {
    has_keyboard = true;
    mouse_enable();
}
