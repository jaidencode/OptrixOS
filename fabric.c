#include <stdint.h>
#include "keyboard.h"
#include "mouse.h"
#include "fabric.h"
#include "graphics.h"
#include "hardware.h"

static const char scancode_ascii[128] = {
    0, 27,'1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',
    0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
    '2','3','0','.','\\',0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0x81,0x82,0,0
};

// Draw a simple bordered window filling most of the screen
static void draw_window(uint8_t color) {
    graphics_draw_rect(20, 20, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 40, color);
}

void fabric_ui(uint8_t color) {
    graphics_clear(0);
    draw_window(color);

    int mouse_x = SCREEN_WIDTH / 2;
    int mouse_y = SCREEN_HEIGHT / 2;
    graphics_put_pixel(mouse_x, mouse_y, 15);

    while (1) {
        ps2_flush_buffers();

        uint8_t packet[3];
        if (mouse_available() && mouse_read_packet(packet)) {
            int dx = (int8_t)packet[1];
            int dy = (int8_t)packet[2];
            graphics_put_pixel(mouse_x, mouse_y, 0);
            mouse_x += dx;
            mouse_y -= dy;
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_x >= SCREEN_WIDTH) mouse_x = SCREEN_WIDTH - 1;
            if (mouse_y >= SCREEN_HEIGHT) mouse_y = SCREEN_HEIGHT - 1;
            graphics_put_pixel(mouse_x, mouse_y, 15);
        }

        uint8_t sc = keyboard_read_scan();
        if (sc && sc < 0x80) {
            char ch = scancode_ascii[sc];
            if (ch == 27)
                break;
        }
    }
}

