#include <stdint.h>
#include "hardware.h"

// VGA dimensions
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// --- vga.c prototypes ---
void vga_clear(uint8_t color);
void vga_puts(const char* str, uint8_t color);
void vga_set_default_color(uint8_t color);
void vga_move_cursor(int row, int col);

// --- pmm.c prototypes ---
void init_pmm(uint32_t kernel_end_addr);
uint32_t alloc_frame(void);
void free_frame(uint32_t addr);

// --- idt.c prototype ---
void idt_init(void);

// --- Helpers ---
int kstrlen(const char* str) {
    int len = 0;
    while (str[len]) ++len;
    return len;
}

void vga_center_puts(int row, const char* str, uint8_t color) {
    int len = kstrlen(str);
    int col = (VGA_WIDTH - len) / 2;
    vga_move_cursor(row, col);
    vga_puts(str, color);
}

// Print hex address
void kprint_hex(uint32_t n, uint8_t color) {
    char buf[11] = "0x00000000";
    for (int i = 0; i < 8; ++i)
        buf[9-i] = "0123456789ABCDEF"[(n >> (i*4)) & 0xF];
    vga_puts(buf, color);
}

// === Simple keyboard scancode -> ASCII translation table (US QWERTY, non-shifted) ===
static const char scancode_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',   // 0x00 - 0x0F
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,   // 0x10 - 0x1F
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\',       // 0x20 - 0x2D
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',           // 0x2E - 0x3B
    0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',             // 0x3C - 0x4B (Numpad)
    '2','3','0','.','\\',0,0,0,0,0,0,0,0,0,0,0,                    // 0x4C - 0x5B (Numpad, extra)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                               // 0x5C - 0x6B
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                               // 0x6C - 0x7B
    // 0x7C - 0x7F: Extended and F-keys as readable placeholder codes
    0x81, /*F11*/ 0x82, /*F12*/ 0,0                                // 0x7C - 0x7F
};

// === Interactive command line shell ===
void command_line_shell(uint8_t color) {
    int row = 0;
    int col = 0;
    vga_clear(color);
    vga_move_cursor(row, col);
    vga_puts("optrix> ", color);
    col += 8;

    char line[80];
    int linepos = 0;

    while (1) {
        uint8_t sc = keyboard_read_scan();
        if (!sc) continue;

        // Only care about key-down codes, not releases (sc < 0x80)
        if (sc >= 0x80) continue;

        char ch = scancode_ascii[sc];
        if (!ch) continue;

        if (ch == '\n') { // Enter
            vga_puts("\n", color);
            row++;
            col = 0;
            vga_move_cursor(row, col);
            vga_puts("optrix> ", color);
            col += 8;
            line[linepos] = 0;
            linepos = 0;
        } else if (ch == '\b') { // Backspace
            if (linepos > 0 && col > 8) {
                --linepos;
                --col;
                vga_move_cursor(row, col);
                vga_puts(" ", color);
                vga_move_cursor(row, col);
            }
        } else if (linepos < (int)(sizeof(line)-1)) {
            line[linepos++] = ch;
            char s[2] = {ch, 0};
            vga_puts(s, color);
            col++;
        }
    }
}

// === Kernel main ===
void kmain() {
    uint8_t blue_white = (1 << 4) | 0xF;
    uint8_t red_yellow = (4 << 4) | 0xE;
    uint8_t green_black = (2 << 4) | 0xA;

    vga_set_default_color(blue_white);
    vga_clear(blue_white);

    // --- HARDWARE INIT ---
    hardware_init();

    int row = 2;
    vga_center_puts(row++, "=== Hardware Device Detection ===", blue_white);

    if (keyboard_available()) {
        vga_center_puts(row++, "Keyboard: Detected", blue_white);
    } else {
        vga_center_puts(row++, "Keyboard: NOT Detected", red_yellow);
    }

    if (mouse_available()) {
        vga_center_puts(row++, "Mouse: Detected", blue_white);
    } else {
        vga_center_puts(row++, "Mouse: NOT Detected", red_yellow);
    }

    // --- PHYSICAL MEMORY MANAGER TEST ---
    row++;
    vga_center_puts(row++, "Physical Memory Manager Test", blue_white);

    init_pmm(0x100000);

    vga_center_puts(row++, "Allocating 5 frames...", blue_white);
    uint32_t frames[5];
    for (int i = 0; i < 5; ++i) {
        frames[i] = alloc_frame();
        char msg[32] = "Frame allocated at ";
        int display_row = row + i;
        vga_center_puts(display_row, msg, blue_white);
        int hex_col = (VGA_WIDTH - (kstrlen(msg) + 10)) / 2 + kstrlen(msg);
        vga_move_cursor(display_row, hex_col);
        kprint_hex(frames[i], red_yellow);
    }
    row += 5;

    vga_center_puts(row++, "Freeing frame #3...", blue_white);
    free_frame(frames[2]);

    vga_center_puts(row++, "Allocating 1 more frame...", blue_white);
    uint32_t reused = alloc_frame();
    char msg2[32] = "Frame allocated at ";
    vga_center_puts(row, msg2, blue_white);
    int hex_col2 = (VGA_WIDTH - (kstrlen(msg2) + 10)) / 2 + kstrlen(msg2);
    vga_move_cursor(row, hex_col2);
    kprint_hex(reused, red_yellow);
    row++;

    if (reused == frames[2]) {
        vga_center_puts(row++, "PMM Test: PASS (frame reused)", green_black);
    } else {
        vga_center_puts(row++, "PMM Test: FAIL (no reuse)", red_yellow);
    }

    row++;
    vga_center_puts(row++, "IDT/Exception Test (Divide by Zero)", blue_white);
    vga_center_puts(row++, "Triggering exception in 2 seconds...", red_yellow);

    idt_init();

    for (volatile int i = 0; i < 50000000; ++i);

    // === SWITCH TO COMMAND LINE MODE ===
    command_line_shell(blue_white);
}
