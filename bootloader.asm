; bootloader.asm - OptrixOS (finalized, robust)
BITS 16
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; Save BIOS boot drive number
    mov [BOOT_DRIVE], dl

    ; Clear screen
    mov ah, 0x0
    mov al, 0x3
    int 0x10

    ; Print banner
    mov si, banner
    call print_str

    ; Get conventional memory size (KB) via BIOS
    int 0x12
    mov [BOOTINFO_ADDR], ax

    ; Load 8 sectors of kernel from LBA 1 (MBR=sector 0, kernel=sector 1+)
    mov ah, 0x02
    mov al, 32             ; SECTORS TO LOAD (tune as kernel grows)
    mov ch, 0
    mov cl, 2              ; Sector 2
    mov dh, 0
    mov dl, [BOOT_DRIVE]   ; BIOS passes boot drive in DL (save below)
    mov bx, 0x1000
    int 0x13
    jc disk_error

    ; Enable A20 line (for QEMU/Bochs)
    in  al, 0x92
    or  al, 2
    out 0x92, al

    ; Set up GDT (null, code, data)
    lgdt [gdt_desc]

    ; Enter protected mode
    mov eax, cr0
    or  eax, 1
    mov cr0, eax
    jmp 0x08:pm_entry      ; far jump to code selector:protected_mode

disk_error:
    mov si, err
    call print_str
    cli
    hlt

print_str:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    mov bh, 0
    int 0x10
    jmp print_str
.done:
    ret

banner db 'OptrixOS Bootloader',13,10,0
err    db 'DISK ERR!',0

BOOT_DRIVE: db 0
BOOTINFO_ADDR equ 0x9000


; --- GDT ---
gdt_start:
    dq 0x0000000000000000      ; null
    dq 0x00cf9a000000ffff      ; code
    dq 0x00cf92000000ffff      ; data
gdt_end:

gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; 32-bit code from here
[BITS 32]
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000      ; stack top (safe for early boot)

    ; Pass BootInfo pointer in stack and call kernel at 0x1000
    push dword BOOTINFO_ADDR
    call dword 0x1000

    ; If kernel returns, halt
.hang:  jmp .hang

times 510-($-$$) db 0
dw 0xAA55