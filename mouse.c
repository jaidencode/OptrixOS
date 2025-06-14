#include "mouse.h"

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

// --- ring buffer for mouse packets ---
#define MOUSE_BUF_SIZE 16
static uint8_t mouse_buf[MOUSE_BUF_SIZE][3];
static int mouse_head = 0, mouse_tail = 0;
static uint8_t mouse_bytes[3];
static int mouse_phase = 0;

static void mouse_buf_put(const uint8_t pkt[3]) {
    for (int i = 0; i < 3; ++i)
        mouse_buf[mouse_head][i] = pkt[i];
    mouse_head = (mouse_head + 1) % MOUSE_BUF_SIZE;
    if (mouse_head == mouse_tail)
        mouse_tail = (mouse_tail + 1) % MOUSE_BUF_SIZE; // overwrite oldest
}

void mouse_handle_byte(uint8_t data) {
    mouse_bytes[mouse_phase++] = data;
    if (mouse_phase == 3) {
        mouse_buf_put(mouse_bytes);
        mouse_phase = 0;
    }
}

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
}

void mouse_init(void) {
    mouse_enable();
}

bool mouse_read_packet(uint8_t packet[3]) {
    if (mouse_head == mouse_tail)
        return false;
    for (int i = 0; i < 3; ++i)
        packet[i] = mouse_buf[mouse_tail][i];
    mouse_tail = (mouse_tail + 1) % MOUSE_BUF_SIZE;
    return true;
}

