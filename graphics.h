#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

void graphics_init(void);
void graphics_clear(uint8_t color);
void graphics_put_pixel(int x, int y, uint8_t color);
void graphics_draw_rect(int x, int y, int w, int h, uint8_t color);

#endif
