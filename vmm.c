#include "vmm.h"
#include "pmm.h"

#define PAGE_PRESENT 0x1
#define PAGE_WRITE   0x2
#define PAGE_USER    0x4

static uint32_t* kernel_directory;

static inline void load_cr3(uint32_t val) {
    __asm__ volatile("mov %0, %%cr3" :: "r"(val));
}

static inline uint32_t read_cr0(void) {
    uint32_t val; __asm__ volatile("mov %%cr0, %0" : "=r"(val)); return val; }

static inline void write_cr0(uint32_t val) {
    __asm__ volatile("mov %0, %%cr0" :: "r"(val));
}

static void enable_paging(uint32_t* pd) {
    load_cr3((uint32_t)pd);
    uint32_t cr0 = read_cr0();
    cr0 |= 0x80000000; // PG bit
    write_cr0(cr0);
}

void init_vmm(void) {
    kernel_directory = (uint32_t*)alloc_frame();
    uint32_t* first_table = (uint32_t*)alloc_frame();
    for (int i = 0; i < 1024; ++i) {
        first_table[i] = (i * 0x1000) | PAGE_PRESENT | PAGE_WRITE;
        kernel_directory[i] = 0;
    }
    kernel_directory[0] = ((uint32_t)first_table) | PAGE_PRESENT | PAGE_WRITE;
    enable_paging(kernel_directory);
}

uint32_t* vmm_create_address_space(void) {
    uint32_t* pd = (uint32_t*)alloc_frame();
    for (int i = 0; i < 1024; ++i)
        pd[i] = kernel_directory[i];
    return pd;
}

void vmm_switch(uint32_t* pd_phys) {
    load_cr3((uint32_t)pd_phys);
}

