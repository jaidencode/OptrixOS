#include <stdint.h>
#include "keyboard.h"
#include "mouse.h"
#include "fabric.h"
#include "graphics.h"
#include "hardware.h"
#include "font8x8.h"

static const unsigned char cursor_bitmap[8] = {
    0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFE
};

static uint8_t cursor_back[8][8];

static void save_cursor_back(int x, int y) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            cursor_back[row][col] = graphics_get_pixel(x + col, y + row);
        }
    }
}

static void restore_cursor_back(int x, int y) {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            graphics_put_pixel(x + col, y + row, cursor_back[row][col]);
        }
    }
}

static void draw_cursor(int x, int y, uint8_t color) {
    for (int row = 0; row < 8; ++row) {
        unsigned char bits = cursor_bitmap[row];
        for (int col = 0; col < 8; ++col) {
            if (bits & (1 << (7 - col)))
                graphics_put_pixel(x + col, y + row, color);
        }
    }
}

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
    graphics_draw_rect(0, 0, screen_width, screen_height, color);
}

static void draw_terminal_window(int term_x, int term_y, int term_w, int term_h) {
    graphics_draw_rect(term_x, term_y, term_w, term_h, 1);
    for (int i = 0; i < term_w; ++i) {
        graphics_put_pixel(term_x + i, term_y, 15);
        graphics_put_pixel(term_x + i, term_y + term_h - 1, 15);
    }
    for (int j = 0; j < term_h; ++j) {
        graphics_put_pixel(term_x, term_y + j, 15);
        graphics_put_pixel(term_x + term_w - 1, term_y + j, 15);
    }

    graphics_draw_rect(term_x + 1, term_y + 1, term_w - 2, 10, 8);
    graphics_draw_rect(term_x + 4, term_y + 3, 4, 4, 12);
    graphics_draw_rect(term_x + 12, term_y + 3, 4, 4, 14);
    graphics_draw_rect(term_x + 20, term_y + 3, 4, 4, 10);
}

void fabric_ui(uint8_t color) {
    graphics_clear(color);
    draw_window(color);

    int term_x = 30;
    int term_y = 40;
    int term_w = screen_width - 60;
    int term_h = screen_height - 80;
    draw_terminal_window(term_x, term_y, term_w, term_h);

    int mouse_x = screen_width / 2;
    int mouse_y = screen_height / 2;
    save_cursor_back(mouse_x, mouse_y);
    draw_cursor(mouse_x, mouse_y, 15);
    int cur_col = 0;
    int cur_row = 0;

    while (1) {
        ps2_flush_buffers();

        uint8_t packet[3];
        if (mouse_available() && mouse_read_packet(packet)) {
            int dx = (int8_t)packet[1];
            int dy = (int8_t)packet[2];
            restore_cursor_back(mouse_x, mouse_y);
            mouse_x += dx;
            mouse_y -= dy;
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_x >= screen_width) mouse_x = screen_width - 8;
            if (mouse_y >= screen_height) mouse_y = screen_height - 8;
            save_cursor_back(mouse_x, mouse_y);
            draw_cursor(mouse_x, mouse_y, 15);
        }

        uint8_t sc = keyboard_read_scan();
        if (sc && sc < 0x80) {
            char ch = scancode_ascii[sc];
            if (ch == 27)
                break;
            if (ch == '\b') {
                if (cur_col > 0) cur_col--;
            } else if (ch == '\n') {
                cur_col = 0;
                cur_row++;
            } else if (ch >= ' ' && ch <= '~') {
                graphics_draw_char(term_x + 4 + cur_col * 8,
                                   term_y + 12 + cur_row * 8,
                                   ch, 15);
                cur_col++;
                if (cur_col * 8 > term_w - 8) {
                    cur_col = 0;
                    cur_row++;
                }
            }
            if (cur_row * 8 > term_h - 16) {
                cur_row = 0;
                graphics_draw_rect(term_x + 1, term_y + 11,
                                  term_w - 2, term_h - 12, 1);
            }
        }
    }
}

