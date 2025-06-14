#include "graphics.h"

static volatile uint8_t* const video = (volatile uint8_t*)0xA0000;

int screen_width = DEFAULT_SCREEN_WIDTH;
int screen_height = DEFAULT_SCREEN_HEIGHT;

void graphics_init(void) {
    // graphics mode already set by bootloader
}

void graphics_clear(uint8_t color) {
    for (int i = 0; i < screen_width * screen_height; ++i)
        video[i] = color;
}

void graphics_put_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height)
        return;
    video[y * screen_width + x] = color;
}

uint8_t graphics_get_pixel(int x, int y) {
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height)
        return 0;
    return video[y * screen_width + x];
}

void graphics_draw_rect(int x, int y, int w, int h, uint8_t color) {
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            graphics_put_pixel(x + i, y + j, color);
        }
    }
}

#include "font8x8.h"

void graphics_draw_char(int x, int y, char c, uint8_t color) {
    const unsigned char *glyph = font8x8_basic[(unsigned char)c];
    for (int row = 0; row < 8; ++row) {
        unsigned char bits = glyph[row];
        for (int col = 0; col < 8; ++col) {
            if (bits & (1 << col)) {
                graphics_put_pixel(x + col, y + row, color);
            }
        }
    }
}

void graphics_draw_string(int x, int y, const char *str, uint8_t color) {
    int cx = x;
    while (*str) {
        graphics_draw_char(cx, y, *str++, color);
        cx += 8;
    }
}
