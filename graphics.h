#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h>

#define DEFAULT_SCREEN_WIDTH 1920
#define DEFAULT_SCREEN_HEIGHT 1080

extern int screen_width;
extern int screen_height;

void graphics_init(void);
void graphics_clear(uint8_t color);
void graphics_put_pixel(int x, int y, uint8_t color);
uint8_t graphics_get_pixel(int x, int y);
void graphics_draw_rect(int x, int y, int w, int h, uint8_t color);
void graphics_draw_char(int x, int y, char c, uint8_t color);
void graphics_draw_string(int x, int y, const char* str, uint8_t color);

#endif
