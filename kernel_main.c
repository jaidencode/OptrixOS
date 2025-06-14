#include <stdint.h>
#include "hardware.h"
#include "keyboard.h"
#include "mouse.h"
#include "fabric.h"
#include "graphics.h"
// --- pmm.c prototypes ---
void init_pmm(uint32_t kernel_end_addr);
uint32_t alloc_frame(void);
void free_frame(uint32_t addr);

// --- idt.c prototype ---
void idt_init(void);


struct BootInfo {
    uint32_t mem_kb;
};

// === Kernel main ===
void kmain(struct BootInfo* boot) {
    (void)boot;
    idt_init();
    hardware_init();
    graphics_init();
    graphics_clear(0);
    fabric_ui(12);
}
