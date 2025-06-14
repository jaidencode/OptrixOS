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
};

// === Kernel main ===
void kmain(struct BootInfo* boot) {
    (void)boot;
    extern uint32_t kernel_end;
    idt_init();
    init_pmm((uint32_t)&kernel_end);
    init_vmm();
    hardware_init();
    graphics_init();
    graphics_clear(0);
    fabric_ui(12);
}
