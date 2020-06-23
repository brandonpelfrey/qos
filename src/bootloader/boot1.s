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
; 6. Load identity-mapped 64-bit ("Long Mode") GDT
; 7. Setup initial Paging: Bottom 2MB physical identity-mapped, Top 2GB mapped to beginning of RAM
; 8. Jump to Kernel in higher-level language

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

  mov bx, 0x7c00
  mov dh, 0           ; head idx

  ; Read additional sectors from disk into the area above 0x7c00
  read_chunk:
    mov ah, 0x2         ; command : read sectors
    mov al, 63          ; sectors to read
    mov ch, 0           ; cylinder idx

    mov cl, 1           ; sector idx
    mov dl, [__boot_disk_identifier]      ; disk idx
    int 0x13

    add bx, 512*63

    ; We'll read ~252 KiB (512 * 63 * 8) before we stop.
    ; A long time from now, the disk driver will load the rest.
    inc dh
    cmp dh, 8
    jne read_chunk

  ; Utilize BIOS call to enable A20 (There are lots of ways). 
  mov ax, 0x2401
  int 0x15

  ; Jump into second boot sector that we loaded from disc sector
  jmp __kernel_copy_target

__boot_disk_identifier:
  db 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
