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
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
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
void isr_handler(int int_no, int err_code) {
    (void)err_code;
    isr_handler_c(int_no);
}

// --- Initialize IDT ---
static void (*isr_table[32])(void) = {
    isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,
    isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
};

void idt_init() {
    for (int i = 0; i < 32; ++i)
        idt_set_gate(i, (uint32_t)isr_table[i]);
    for (int i = 32; i < IDT_ENTRIES; ++i)
        idt_set_gate(i, (uint32_t)isr_table[0]);

    struct IDTPointer idt_ptr;
    idt_ptr.limit = sizeof(struct IDTEntry) * IDT_ENTRIES - 1;
    idt_ptr.base  = (uint32_t)&idt;
    idt_load(&idt_ptr);
}
