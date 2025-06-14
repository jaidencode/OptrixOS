#ifndef VGA_H
#define VGA_H

#include <stdint.h>

// VGA text mode constants
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

// Basic VGA text functions
void vga_enable_cursor(void);
void vga_move_cursor(int row, int col);
void vga_clear(uint8_t color);
void vga_putc(char c, uint8_t color);
void vga_puts(const char* str, uint8_t color);
void vga_write_str(int row, int col, const char* str, uint8_t color);
void vga_set_default_color(uint8_t color);
uint16_t vga_get_cell(int row, int col);
void vga_set_cell(int row, int col, uint16_t val);
void vga_center_puts(int row, const char* str, uint8_t color);

#endif // VGA_H
