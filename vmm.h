#ifndef VMM_H
#define VMM_H
#include <stdint.h>

void init_vmm(void);
uint32_t* vmm_create_address_space(void);
void vmm_switch(uint32_t* pd_phys);

#endif
