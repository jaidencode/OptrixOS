// vga.c - OptrixOS VGA text driver (C, 32-bit, for kernel dev)
#include <stdint.h>

// --- I/O PORT FUNCTIONS ---
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// --- VGA DEFINITIONS ---
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

enum VGAColor {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_YELLOW = 14,
    VGA_WHITE = 15,
};

static volatile uint16_t* const vga_buffer = (volatile uint16_t*) VGA_ADDRESS;
static int cursor_row = 0, cursor_col = 0;
static uint8_t default_color = (VGA_BLUE << 4) | VGA_WHITE;

// --- CURSOR CONTROL ---
void vga_move_cursor(int row, int col) {
    uint16_t pos = row * VGA_WIDTH + col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    cursor_row = row;
    cursor_col = col;
}

// --- CLEAR SCREEN ---
void vga_clear(uint8_t color) {
    for (int y = 0; y < VGA_HEIGHT; ++y)
        for (int x = 0; x < VGA_WIDTH; ++x)
            vga_buffer[y * VGA_WIDTH + x] = ((uint16_t)color << 8) | ' ';
    cursor_row = 0;
    cursor_col = 0;
    vga_move_cursor(0, 0);
}

// --- SCROLLING ---
void vga_scroll() {
    for (int y = 1; y < VGA_HEIGHT; ++y)
        for (int x = 0; x < VGA_WIDTH; ++x)
            vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
    for (int x = 0; x < VGA_WIDTH; ++x)
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ((uint16_t)default_color << 8) | ' ';
    if (cursor_row > 0) cursor_row--;
}

// --- CHARACTER OUTPUT WITH SCROLLING AND CURSOR ---
void vga_putc(char c, uint8_t color) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else if (c == '\r') {
        cursor_col = 0;
    } else {
        vga_buffer[cursor_row * VGA_WIDTH + cursor_col] = ((uint16_t)color << 8) | c;
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }
    if (cursor_row >= VGA_HEIGHT) {
        vga_scroll();
        cursor_row = VGA_HEIGHT - 1;
    }
    vga_move_cursor(cursor_row, cursor_col);
}

// --- STRING OUTPUT WITH COLOR ---
void vga_puts(const char* str, uint8_t color) {
    while (*str)
        vga_putc(*str++, color);
}

// --- WRITE STRING AT SPECIFIC ROW/COL ---
void vga_write_str(int row, int col, const char* str, uint8_t color) {
    int old_row = cursor_row, old_col = cursor_col;
    cursor_row = row;
    cursor_col = col;
    while (*str)
        vga_putc(*str++, color);
    cursor_row = old_row;
    cursor_col = old_col;
    vga_move_cursor(old_row, old_col);
}

// --- SET DEFAULT COLOR ---
void vga_set_default_color(uint8_t color) {
    default_color = color;
}

// --- DIRECT CELL ACCESS ---
uint16_t vga_get_cell(int row, int col) {
    return vga_buffer[row * VGA_WIDTH + col];
}

void vga_set_cell(int row, int col, uint16_t val) {
    vga_buffer[row * VGA_WIDTH + col] = val;
}