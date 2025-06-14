#include "graphics.h"

static volatile uint8_t* const video = (volatile uint8_t*)0xA0000;

void graphics_init(void) {
    // graphics mode already set by bootloader
}

void graphics_clear(uint8_t color) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
        video[i] = color;
}

void graphics_put_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;
    video[y * SCREEN_WIDTH + x] = color;
}

void graphics_draw_rect(int x, int y, int w, int h, uint8_t color) {
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            graphics_put_pixel(x + i, y + j, color);
        }
    }
}
