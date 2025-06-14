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

// --- ring buffer for scancodes ---
#define KBD_BUF_SIZE 32
static uint8_t kbd_buf[KBD_BUF_SIZE];
static int kbd_head = 0, kbd_tail = 0;

static void kbd_buf_put(uint8_t sc) {
    int next = (kbd_head + 1) % KBD_BUF_SIZE;
    if (next != kbd_tail) {
        kbd_buf[kbd_head] = sc;
        kbd_head = next;
    }
}

void keyboard_handle_byte(uint8_t data) {
    kbd_buf_put(data);
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
    if (kbd_head == kbd_tail)
        return 0;
    uint8_t sc = kbd_buf[kbd_tail];
    kbd_tail = (kbd_tail + 1) % KBD_BUF_SIZE;
    return sc;
}

