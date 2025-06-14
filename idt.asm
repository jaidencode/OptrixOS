; idt.asm
BITS 32

global idt_load

; Macro for interrupts without an error code pushed by the CPU
%macro ISR_NOERR 1
global isr%1
isr%1:
    push dword 0          ; dummy error code
    push dword %1         ; interrupt number
    call isr_handler
    add esp, 8
    iret
%endmacro

; Macro for interrupts where the CPU already pushes an error code
%macro ISR_ERR 1
global isr%1
isr%1:
    push dword %1         ; interrupt number
    call isr_handler
    add esp, 8            ; remove int_no + error_code
    iret
%endmacro

; Declare all ISR stubs 0-31
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR  10
ISR_ERR  11
ISR_ERR  12
ISR_ERR  13
ISR_ERR  14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR  17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR  21
%assign i 22
%rep 10
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

