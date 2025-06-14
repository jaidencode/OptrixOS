#include <stdint.h>
#include "hardware.h"
#include "keyboard.h"
#include "mouse.h"
#include "fabric.h"
#include "graphics.h"
#include "pmm.h"
#include "vmm.h"

// --- idt.c prototype ---
void idt_init(void);


struct BootInfo {
    uint32_t mem_kb;
    uint32_t width;
    uint32_t height;
    uint32_t framebuffer;
};

// === Kernel main ===
void kmain(struct BootInfo* boot) {
    screen_width = (boot && boot->width) ? boot->width : DEFAULT_SCREEN_WIDTH;
    screen_height = (boot && boot->height) ? boot->height : DEFAULT_SCREEN_HEIGHT;
    uint32_t fb_addr = (boot && boot->framebuffer) ? boot->framebuffer : 0xA0000;
    extern uint32_t kernel_end;
    idt_init();
    init_pmm((uint32_t)&kernel_end);
    init_vmm();
    hardware_init();
    graphics_init(fb_addr);
    graphics_clear(0);
    fabric_ui(0);
}
