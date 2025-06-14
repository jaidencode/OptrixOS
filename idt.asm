; idt.asm
BITS 32

global idt_load
%macro ISR_NOERR 1
global isr%1
isr%1:
    push dword 0          ; error code
    push dword %1         ; interrupt number
    call isr_handler
    add esp, 8
    iret
%endmacro

; Declare all ISR stubs 0-31
%assign i 0
%rep 32
    ISR_NOERR i
%assign i i+1
%endrep

extern isr_handler

section .text

; Load the IDT (argument: pointer in [esp+4])
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret

