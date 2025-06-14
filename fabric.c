#include <stdint.h>
#include "keyboard.h"
#include "mouse.h"
#include "fabric.h"
#include "vga.h"

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
    const int top = 1;
    const int left = 1;
    const int bottom = VGA_HEIGHT - 2;
    const int right = VGA_WIDTH - 2;

    for (int x = left; x <= right; ++x) {
        vga_set_cell(top, x, ((uint16_t)color << 8) | '-');
        vga_set_cell(bottom, x, ((uint16_t)color << 8) | '-');
    }
    for (int y = top; y <= bottom; ++y) {
        vga_set_cell(y, left, ((uint16_t)color << 8) | '|');
        vga_set_cell(y, right, ((uint16_t)color << 8) | '|');
    }
    vga_set_cell(top, left, ((uint16_t)color << 8) | '+');
    vga_set_cell(top, right, ((uint16_t)color << 8) | '+');
    vga_set_cell(bottom, left, ((uint16_t)color << 8) | '+');
    vga_set_cell(bottom, right, ((uint16_t)color << 8) | '+');

    const char *title = "Fabric Terminal";
    vga_move_cursor(top, (VGA_WIDTH - 14) / 2);
    vga_puts(title, color);
}

void fabric_ui(uint8_t color) {
    vga_clear(color);
    draw_window(color);

    int mouse_row = VGA_HEIGHT / 2;
    int mouse_col = VGA_WIDTH / 2;
    uint16_t prev_cell = vga_get_cell(mouse_row, mouse_col);
    uint16_t cursor_val = (color << 8) | 'X';
    vga_set_cell(mouse_row, mouse_col, cursor_val);

    int term_row = VGA_HEIGHT - 3;
    int term_col = 2;
    vga_move_cursor(term_row, term_col);
    const char *prompt = "Press ESC to exit > ";
    vga_puts(prompt, color);
    term_col += 20; // strlen(prompt)
    char line[80];
    int linepos = 0;

    while (1) {
        uint8_t packet[3];
        if (mouse_available() && mouse_read_packet(packet)) {
            int dx = (int8_t)packet[1];
            int dy = (int8_t)packet[2];
            vga_set_cell(mouse_row, mouse_col, prev_cell);
            mouse_col += dx / 2;
            mouse_row -= dy / 2;
            if (mouse_col < 2) mouse_col = 2;
            if (mouse_row < 2) mouse_row = 2;
            if (mouse_col >= VGA_WIDTH - 2) mouse_col = VGA_WIDTH - 3;
            if (mouse_row >= VGA_HEIGHT - 3) mouse_row = VGA_HEIGHT - 4;
            prev_cell = vga_get_cell(mouse_row, mouse_col);
            vga_set_cell(mouse_row, mouse_col, cursor_val);
        }

        uint8_t sc = keyboard_read_scan();
        if (sc && sc < 0x80) {
            char ch = scancode_ascii[sc];
            if (ch == 27) {
                vga_set_cell(mouse_row, mouse_col, prev_cell);
                break;
            }
            if (ch) {
                if (ch == '\n') {
                    vga_puts("\n", color);
                    term_row++;
                    term_col = 2;
                    vga_move_cursor(term_row, term_col);
                    line[linepos] = 0;
                    linepos = 0;
                } else if (ch == '\b') {
                    if (linepos > 0 && term_col > 2) {
                        --linepos; --term_col; vga_move_cursor(term_row, term_col);
                        vga_puts(" ", color); vga_move_cursor(term_row, term_col);
                    }
                } else if (linepos < (int)(sizeof(line)-1)) {
                    line[linepos++] = ch;
                    char s[2] = {ch, 0};
                    vga_puts(s, color);
                    term_col++;
                }
            }
        }
    }
}

