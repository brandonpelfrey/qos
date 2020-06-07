
extern __kernel_main
global __kernel_copy_target

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Okay, now that we don't need to worry about everything fitting in just 512 bytes,
; we can continue booting.

bits 16
__kernel_copy_target:

; Ask the BIOS about the memory map on this system, store in RAM for later reading.
; https://wiki.osdev.org/Detecting_Memory_(x86)#Getting_an_E820_Memory_Map

setup_mmap:
  mov di, mmap_array_start
  xor ebx, ebx
  xor bp,bp
  mov edx, 0x534D4150

  .read_mmap_entry:
    mov eax, 0xE820
    mov ecx, 24
    int 0x15
    ; (Side note: Technically, we should check EAX=534D4150, CF clear, but every modern system handles this BIOS procedure.)
    
    ; If ebx==0, then we're done reading the list
    test ebx, ebx
    jz .read_mmap_done

    ; Got a valid entry, increment the entry count and move our di pointer along by the number of bytes read.
    ; Entries might be 20 or 24 bytes, but all we care about is the first 20 bytes. Keep everything aligned
    ; by assuming 24 the whole time.
    inc bp
    add di, 24
    jmp .read_mmap_entry

  .read_mmap_done:
  mov [mmap_array_size], bp


setup_long_mode:
; We're going to go directly to long mode (bypass intermediate 32b protected mode)

  ; 1. Disable paging (But, we never set it up anyway, so, let's continue on)
  ; TODO : We're assuming that 0x1000 is write-able. We should really detect usable RAM...

  ; 2. Create PML4 paging data structure in memory
  ;    This will be a really simple mapping that we will manipulate later when we jump
  ;    into a higher-level language. (Keeping the assembly here to a minimum..)

  ; 2.1 : PML4 Table
  mov edi, 0x1000
  mov cr3, edi
  xor eax, eax
  mov ecx, 4096
  rep stosd
  mov edi, cr3 ; the stosd before this trashed EDI, Reset EDI back to 0x1000

  ; 2.2 Now we have 4K zero'd out at 0x1000. Let's create a single chain of PML4T/PDPT/PDT/PT
  ;     (Note: '3' here indicates readable/writeable/present pages)
  mov dword [edi], 0x2003
  add edi, 0x1000
  mov dword [edi], 0x3003
  add edi, 0x1000
  mov dword [edi], 0x4003
  add edi, 0x1000

  ; 2.3 Now edi is pointing at our first page table entry in our only page table.
  ;     There are 512 entries in a page table, each an 8-byte pointer which we want to identity-map.
  ;     This means we'll be identity-mapping 2MB of space.
  mov ebx, 0x00000003
  mov ecx, 512
  .SetEntry:
    mov dword [edi], ebx
    add ebx, 0x1000
    add edi, 8
    loop .SetEntry

  ; 3. Enable Physical Address Extension (PAE)
  mov eax, cr4
  or  eax, 0x00000020
  mov cr4, eax

  ; 4. . Enable long mode by setting EFER.LME flag in MSR 0xC00000080
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1 << 8
  wrmsr

  ; 5. Enable paging + protected mode
  mov eax, cr0
  or eax, 1<<31 | 1<<0
  mov cr0, eax

  ; 6. We're now in Long Mode, but a 32-bit compatibility version
  ; We need to load a 64-bit GDT and then do a long jump there
  lgdt [GDT64.Pointer]
  mov ax, GDT64.Data
  jmp GDT64.Code:LongModeStart

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 64-bit Global Descriptor Table (GDT)
; Contains one null descriptor, and one code and data segment which identity-maps the entire range
; There isn't an interesting structure to this table since segmentation isn't used in x86-64.
GDT64:
  .Null: equ $ - GDT64
    dw 0xFFFF
    dw 0
    db 0
    db 0
    db 1
    db 0
  .Code: equ $ - GDT64
    dw 0
    dw 0
    db 0
    db 1001_1010b
    db 1010_1111b
    db 0
  .Data: equ $ - GDT64
    dw 0
    dw 0
    db 0
    db 10010010b
    db 00000000b
  db 0
  .Pointer:
    dw $ - GDT64 - 1
    dq GDT64

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; We're in Long Mode now, yay! Let's jump into a higher level language.

bits 64
LongModeStart:
  cli

  ; Load data segments with offset to our single data segment selector in the GDT.
  mov ax, GDT64.Data
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov edi, 0xB8000
  mov rax, 0x1F211F201F201F20
  mov ecx, 500
  rep stosq

  ; Setup a small initial stack for the kernel
  mov rsp, small_stack_top

  ; Pass in the memory map to __kernel_main
  mov dword rdi, [mmap_array_size]
  mov rsi, mmap_array_start
  
  ; Jump into the 
  call __kernel_main

  ; If we get back here, halt indefinitely (to leave any messages etc on the screen)
  die:
    cli
    hlt
    jmp die

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Memory map entries. We just read it from the BIOS here in the bootloader,
; but it will be interpreted later when we first get into the kernel.

MMAP_MAX_ENTRIES equ 64
mmap_array_size:
  dw 0

section .bss
mmap_array_start:
  resq (MMAP_MAX_ENTRIES)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Small initial 16KiB stack for the kernel startup

align 16
section .bss
small_stack_reservation:
  resb (1024 * 16)
small_stack_top: