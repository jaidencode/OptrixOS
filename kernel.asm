; kernel.asm for OptrixOS (32-bit, flat binary)
BITS 32

global start
extern kmain

section .text
align 16
start:
    ; Entry point for 32-bit kernel.
    ; Bootloader has already set up segment registers and stack!
    call kmain

.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
resb 4096        ; 4K scratch space for C (optional)
kernel_end:
