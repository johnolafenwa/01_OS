; boot.asm

BITS 32
SECTION .multiboot
align 4
    dd 0x1BADB002             ; Magic number
    dd 0x00                   ; Flags
    dd -(0x1BADB002 + 0x00)   ; Checksum

SECTION .text
extern kernel_main
global _start

_start:
    ; Call the kernel main function
    call kernel_main
    ; Halt the CPU
    cli
halt:
    hlt
    jmp halt
