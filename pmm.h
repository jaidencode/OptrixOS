#ifndef PMM_H
#define PMM_H
#include <stdint.h>

void init_pmm(uint32_t kernel_end_addr);
uint32_t alloc_frame(void);
void free_frame(uint32_t addr);

#endif
