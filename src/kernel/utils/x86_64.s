
global dump_register_states
global get_call_stack

dump_register_states:
  ; Pointer to structure is in RDI
  mov qword [rdi + 8*0], rax
  mov qword [rdi + 8*1], rbx
  mov qword [rdi + 8*2], rcx
  mov qword [rdi + 8*3], rdx
  
  mov qword [rdi + 8*4], rsp
  mov qword [rdi + 8*5], rbp
  mov qword [rdi + 8*6], rsi
  mov qword [rdi + 8*7], rdi

  mov qword [rdi + 8*8], r9
  mov qword [rdi + 8*9], r10
  mov qword [rdi + 8*10], r11
  mov qword [rdi + 8*11], r12
  mov qword [rdi + 8*12], r13
  mov qword [rdi + 8*13], r14
  mov qword [rdi + 8*15], r15

  push rax
  mov rax, cr0
  mov qword [rdi + 8*16], rax
  mov rax, cr2
  mov qword [rdi + 8*17], rax
  mov rax, cr3
  mov qword [rdi + 8*18], rax
  mov rax, cr4
  mov qword [rdi + 8*19], rax
  
  ; TODO : Actually get a meaningful RIP from the context of the caller
  mov rax, 0xDEADBEEF_DEADBEEF
  mov qword [rdi + 8*20], rax
  pop rax

  ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;struct CallStack {
;  u64 call_site;
;  u64 frame_pointer;
;  u64 caller_frame_pointer;
;};
; extern "C" int get_call_stack(CallStack*, u64 max_entries);

get_call_stack:
  push rbp ; We'll be jumping back up the stack using this
  push rcx ; This will count the number of stacks populated

  xor rcx, rcx
  .next_entry:
    test rsi, rsi
    jz .done

    ; Assumption here is that 5-byte call encoding was used, which is always true in the kernel
    ; Definitely a TODO here..
    mov rax, qword [rbp+8] ; Where was this function called from
    lea rax, [rax - 5]
    mov qword [rdi + 0], rax 

    mov rax, rbp
    mov qword [rdi + 8], rax ; Function Frame Pointer

    mov rax, qword [rbp + 0]
    mov qword [rdi + 16], rax ; Function Frame Pointer

    dec rsi
    inc rcx

    ; NOTE ASSUMPTION
    ; This is only used in the kernel code, and so the top stack pointer is the bootstrap code. Don't go past it.
    cmp qword [rdi + 0], 0xffffffff80000000
    je .done

    ; Setup pointers for next iteration
    add rdi, 8*3
    mov rbp, [rbp]

    jmp .next_entry

  .done:
    mov rax, rcx
    pop rcx
    pop rbp
    ret
