// idt.c

#include <stdint.h>

#define IDT_ENTRIES 256

struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed));

struct IDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// --- Forward declarations for ASM symbols ---
extern void isr0(void);
extern void idt_load(struct IDTPointer* idt_ptr);

// --- IDT table ---
static struct IDTEntry idt[IDT_ENTRIES];

// Set an entry in the IDT
void idt_set_gate(int n, uint32_t handler) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = 0x08;
    idt[n].zero        = 0;
    idt[n].type_attr   = 0x8E;
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

// --- Main exception handler called from ASM ---
void isr_handler_c(int int_no) {
    extern void vga_clear(uint8_t color);
    extern void vga_center_puts(int row, const char* str, uint8_t color);

    vga_clear((4 << 4) | 0xF); // red background, white
    char msg[40] = "Exception: Interrupt #00";
    msg[21] = '0' + (int_no / 10);
    msg[22] = '0' + (int_no % 10);
    msg[23] = 0;
    vga_center_puts(12, msg, (4 << 4) | 0xF);
    for(;;); // halt forever
}

// --- Symbol called from ASM ---
void isr_handler() {
    isr_handler_c(0); // Just pass 0 for now (interrupt number)
}

// --- Initialize IDT ---
void idt_init() {
    for (int i = 0; i < IDT_ENTRIES; ++i)
        idt_set_gate(i, (uint32_t)isr0);

    struct IDTPointer idt_ptr;
    idt_ptr.limit = sizeof(struct IDTEntry) * IDT_ENTRIES - 1;
    idt_ptr.base  = (uint32_t)&idt;
    idt_load(&idt_ptr);
}
