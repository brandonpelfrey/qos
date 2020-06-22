global isr_table
extern global_interrupt_handler

NUM_IRQS: equ 64

%macro save_registers 0
  push rax
  push rbx
  push rcx
  push rdx
  push rbp
  push rsi
  push rdi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
%endmacro

%macro restore_registers 0
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rdi
  pop rsi
  pop rbp
  pop rdx
  pop rcx
  pop rbx
  pop rax
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

%macro ISR 1
align 16
isr_stub_%1:
  save_registers
  mov rdi, %1
  mov rsi, rsp
  call global_interrupt_handler
  restore_registers
  iretq
%endmacro

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%assign i 0
%rep NUM_IRQS
  ISR i
%assign i i+1
%endrep

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
%macro isr_table_entry 1
  dq isr_stub_%1
%endmacro

align 16
isr_table:
  %assign i 0
  %rep NUM_IRQS
    isr_table_entry i
  %assign i i+1
  %endrep

