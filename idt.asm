; idt.asm
BITS 32

global idt_load
global isr0

extern isr_handler

section .text

; Load the IDT (argument: pointer in [esp+4])
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; ISR0: Divide by zero handler
isr0:
    push 0         ; dummy error code
    push 0         ; interrupt number 0
    call isr_handler
    add esp, 8
    iret
