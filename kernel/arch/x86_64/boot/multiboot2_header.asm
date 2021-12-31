MULTIBOOT2_HEADER_MAGIC equ 0xe85250d6

MULTIBOOT2_ARCHITECTURE_I386 equ 0

MULTIBOOT2_HEADER_TAG_OPTIONAL equ 1
MULTIBOOT2_HEADER_TAG_INFORMATION_REQUEST equ 1
MULTIBOOT2_HEADER_TAG_ADDRESS equ 2
MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS equ 3
MULTIBOOT2_HEADER_TAG_CONSOLE_FLAGS equ 4
MULTIBOOT2_HEADER_TAG_FRAMEBUFFER equ 5
MULTIBOOT2_HEADER_TAG_MODULE_ALIGN equ 6
MULTIBOOT2_HEADER_TAG_EFI_BS equ 7
MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS_EFI32 equ 8
MULTIBOOT2_HEADER_TAG_ENTRY_ADDRESS_EFI64 equ 9
MULTIBOOT2_HEADER_TAG_RELOCATABLE equ 10
MULTIBOOT2_HEADER_TAG_END equ 0

section .multiboot2
align 8
header_start:
    dd MULTIBOOT2_HEADER_MAGIC       
    dd MULTIBOOT2_ARCHITECTURE_I386  
    dd header_end - header_start     
    dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_ARCHITECTURE_I386 + (header_end - header_start)) 

align 8
    dw MULTIBOOT2_HEADER_TAG_FRAMEBUFFER
    dw MULTIBOOT2_HEADER_TAG_OPTIONAL
    dd 20
    dd 0 
    dd 0
    dd 32 
align 8
    dw MULTIBOOT2_HEADER_TAG_END
    dw 0
    dd 8

header_end: