
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
  ; We've read all the real entries. Input a bogus one at the end as a marker. (Type=0)
  mov ecx, 6
  xor eax, eax
  rep stosd


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

  ; 3. Enable Physical Address Extension (PAE). 
  ; NOTE: This will cause the GDT to be interpreted as 512 64bit entries, even before we
  ;       jump to actual 64bit code.
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

  ; 6. We're now in Long Mode, but we're in a 32-bit compatibility mode.
  ; We need to load a 64-bit GDT and then do a long jump there to get to real 64bit code.
  lgdt [GDT64.Pointer]
  mov ax, GDT64.Data
  jmp GDT64.Code:LongModeStart

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 64-bit Global Descriptor Table (GDT)
; Contains one null descriptor, and one code and data segment which identity-maps the entire range
; There isn't any interesting structure to this table since segmentation isn't used in x86-64.
align 64
GDT64:
  .Null: equ $ - GDT64
    dw 0
    dw 0
    db 0
    db 0
    db 0
    db 0
  .Code: equ $ - GDT64
    dw 0
    dw 0
    db 0
    db 1001_1000b ; Access Byte
    db 0010_0000b ; Flags, Limit
    db 0
  .Data: equ $ - GDT64
    dw 0
    dw 0
    db 0
    db 1001_0010b ; Access Byte
    db 0000_0000b ; Flags, Limit
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

  ; Setup a small initial stack for the kernel
  mov rsp, small_stack_top

  ; Setup additional paging with highest 2GB pointing at beginning of physical memory
  call setup_high_memory_paging

  ; Copy the kernel to the beginning of that area  
  call install_kernel_to_high_memory

  ; Okay, now let's get our real stack
  ; (kernel_layout.h) KERNEL_STACK_TOP = KERNEL_STACK_BASE + KERNEL_STACK_SIZE - 16;
  mov rsp, 0xFFFFFFFF80000000 + ((32 + 8) * 1024 * 1024) - 16

  ; Pass in the memory map to __kernel_main, and jump in
  lea rdi, [mmap_array_start]

  ; Jump to high mem kernel stuff
  mov rax, 0xFFFFFFFF80000000
  call rax

  ; If we get back here, halt indefinitely (to leave any messages etc on the screen)
  die:
    cli
    hlt
    jmp die

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

setup_high_memory_paging:

  ;;;;;;
  ; Get address of the largest type=1 physical memory segment

  xor rcx, rcx ; rcx = Current SMAP entry address
  xor rdx, rdx ; rdx = Address of largest memory segment found
  xor rbx, rbx ; rbx = Size of largest memory segment found

  lea ecx, [mmap_array_start]
  check_entry:
    mov eax, dword [ecx + 16]

    ; Are we at the end of the list?
    test eax, eax
    jz .done

    ; Is this a type-1 memory segment?
    cmp eax, 1
    jne .next_entry

    ; This is a type-1 entry: let's see if it's the largest so far.
    mov rax, [rcx + 12] ; Load high bytes of entry.Length
    shl rax, 32

    mov r9, [rcx + 8]
    and r9, 0xFFFFFFFF
    or rax, r9  ; Load low bytes of entry.Length

    cmp rax, rbx
    jle .next_entry

    ; This entry is the largest so far
    mov rdx, rcx
    mov rbx, rax

    .next_entry:
      add rcx, 24
      jmp check_entry
  .done:

  ;;;;;;
  ; Paging : Setup the first 512GB as identity-mapped
  mov rdi, 0x2000
  mov rax, 0x0000000000000000 | 3 | 128
  mov rcx, 512
  setup_bottom_identity_paging:
    mov qword [rdi], rax
    add rdi, 8
    add rax, 1024 * 1024 * 1024
    loop setup_bottom_identity_paging

  ;;;;;;
  ; Let's setup a new paging entry that starts at the largest entry

  ; rdx is currently pointing to the beginning of our largest memory segment
  ; but need to make sure address is 2MB aligned for using large pages
  Two_MiB equ (2 * 1024 * 1024)

  add rdx, (Two_MiB-1)
  and rdx, Two_MiB

  ; Append a few tables to our existing 4-level paging data structure
  ; 0x1000 holds the top level, and 0x2/3/4000 were already setup to
  ; describe the bottom of physical memory, which is where we're executing
  ; right now. We'll add entries at 0x5/6000.

  setup_high_pages:
    mov qword [0x1000 + 511*8], 0x5000 | 3
    mov qword [0x5000 + 510*8], 0x6000 | 3 ; Final 2GiB

    mov rax, rdx
    or rax, (3 | 128)  ; Large Pages (2MB), Writeable, Present

    mov rdi, 0x6000
    mov rcx, 512

    .next_entry:
      mov qword [rdi], rax
      add rax, Two_MiB
      add rdi, 8

      loop .next_entry

    .done:
  ret
  

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Copies 512KB of kernel code that was loaded from disk and places
; it into the -2GB space in virtual memory
install_kernel_to_high_memory:
  KERNEL_CODE_SOURCE equ (0x7c00 + 4096)
  KERNEL_CODE_DEST equ 0xFFFFFFFF80000000

  ; 2. Copy the first 1MB of memory  to KERNEL_BASE
  mov rdi, 0
  mov rcx, (512 * 1024) ; 512kb
  .next_byte:
    test rcx, rcx
    jz .done

    mov rax, qword [KERNEL_CODE_SOURCE + rdi]
    mov qword [KERNEL_CODE_DEST + rdi], rax
    add rdi, 8
    sub rcx, 8
    jmp .next_byte

  .done:
  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Memory map entries. We just read it from the BIOS here in the bootloader,
; but it will be interpreted later when we first get into the kernel.

section .bss
MMAP_MAX_ENTRIES equ 32
mmap_array_start:
  resq (MMAP_MAX_ENTRIES * 3)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Small initial 16KiB stack for the kernel startup

align 16
section .bss
small_stack_reservation:
  resb (1024 * 16)
small_stack_top: