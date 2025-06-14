#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <stdint.h>

#define DEFAULT_SCREEN_WIDTH 1920
#define DEFAULT_SCREEN_HEIGHT 1080

extern int screen_width;
extern int screen_height;
extern uint32_t framebuffer_addr;

void graphics_init(uint32_t fb_addr);
void graphics_clear(uint32_t color);
void graphics_put_pixel(int x, int y, uint32_t color);
uint32_t graphics_get_pixel(int x, int y);
void graphics_draw_rect(int x, int y, int w, int h, uint32_t color);
void graphics_draw_char(int x, int y, char c, uint32_t color);
void graphics_draw_string(int x, int y, const char* str, uint32_t color);

#endif
