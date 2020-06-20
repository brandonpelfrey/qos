; QOS Bootloader
; This bootloader is based on https://wiki.osdev.org/Setting_Up_Long_Mode
; It takes a system from booting in Real Mode to Long Mode with a simple page table setup.
; This bootloader lives entirely in low-memory at 0x7C00

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; == ORDER OF BOOT EVENTS == 
; 1. Only the first 1st sector (512 bytes) were loaded by BIOS, load the rest of the bootloader.
; 2. Enable A20.
; 3. Read the memory map into RAM for later use.
; 3. Setup an initial paging data structure.
; 5. Enable PAE/Protected Mode/Paging
; 6. Load identity-mapped 64-bit ("Long Mode") GDT, and jump to Kernel start

; TL;DR: Get us out of 70s compatibility mode, and into the modern era :)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; ** Initial entry during system boot
; - Initially we boot in legacy 16 bit mode
; - The BIOS will recognize the boot sector flag we've setup (at the end of this file)
;   and will locate the start of this code to 0x7c00 in memory. (org 0x7c00 lets the assembler know this)

global __boot
global __boot_disk_identifier
extern __kernel_copy_target

bits 16
__boot:
  cli
  
  ; Save the disk number we booted from (BIOS placed it in the dl register for us)
  mov [__boot_disk_identifier], dl

  ; Ready additional sectors from disk into the area above 0x7c00
  mov ah, 0x2         ; read sectors
  mov al, 50          ; sectors to read << TODO : Yes, this is abitrary right now..
  mov ch, 0           ; cylinder idx
  mov dh, 0           ; head idx
  mov cl, 2           ; sector idx
  mov dl, [__boot_disk_identifier]      ; disk idx
  mov bx, __kernel_copy_target        ; target pointer
  int 0x13

  ; Utilize BIOS call to enable A20 (There are lots of ways). 
  mov ax, 0x2401
  int 0x15

  ; Jump into second boot sector that we loaded from disc sector
  jmp __kernel_copy_target

__boot_disk_identifier:
  db 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
