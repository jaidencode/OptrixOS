#include <stdint.h>
#include "hardware.h"
#include "fabric.h"
#include "vga.h"
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

void kprint_dec(uint32_t n, uint8_t color) {
    char buf[16];
    int i = 0;
    if (n == 0) {
        buf[i++] = '0';
    } else {
        char tmp[16];
        int j = 0;
        while (n && j < 15) {
            tmp[j++] = '0' + (n % 10);
            n /= 10;
        }
        while (j > 0)
            buf[i++] = tmp[--j];
    }
    buf[i] = 0;
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

struct BootInfo {
    uint32_t mem_kb;
};

static void wait_2s(void) {
    for (volatile uint32_t i = 0; i < 50000000; ++i);
}

static void boot_sequence(struct BootInfo* boot, uint8_t color) {
    int row = 2;
    vga_set_default_color(color);
    vga_clear(color);
    vga_enable_cursor();
    vga_center_puts(row++, "=== OptrixOS Boot ===", color);

    char mem_msg[] = "BIOS Memory KB: ";
    vga_center_puts(row, mem_msg, color);
    int col = (VGA_WIDTH - (kstrlen(mem_msg) + 10)) / 2 + kstrlen(mem_msg);
    vga_move_cursor(row++, col);
    kprint_dec(boot ? boot->mem_kb : 0, color);
    wait_2s();

    vga_center_puts(row++, "Initializing IDT...", color);
    wait_2s();
    idt_init();
    vga_center_puts(row++, "IDT ready", color);
    wait_2s();

    vga_center_puts(row++, "Detecting input devices...", color);
    wait_2s();
    hardware_init();
    const char* kmsg = keyboard_available() ? "Keyboard detected" : "Keyboard missing";
    const char* mmsg = mouse_available() ? "Mouse detected" : "Mouse missing";
    vga_center_puts(row++, kmsg, color);
    wait_2s();
    vga_center_puts(row++, mmsg, color);
    wait_2s();
}

// === Kernel main ===
void kmain(struct BootInfo* boot) {
    uint8_t blue_white = (1 << 4) | 0xF;

    boot_sequence(boot, blue_white);

    // Launch mouse/keyboard test UI
    fabric_ui(blue_white);
}
