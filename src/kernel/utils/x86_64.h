#pragma once

#include <types.h>

namespace System {
struct RegisterStates {
  u64 rax, rbx, rcx, rdx;
  u64 rsp, rbp, rsi, rdi;
  u64 r8, r9, r10, r11, r12, r13, r14, r15;
  u64 cr0, cr2, cr3, cr4;
  u64 rip;
};

extern "C" void dump_register_states(RegisterStates*);

struct CallStack {
  u64 call_site;
  u64 frame_pointer;
  u64 caller_frame_pointer;
};

// Populate call stack information, returning the number of valid call stack structures populated.
extern "C" int get_call_stack(CallStack*, u64 max_entries);


};  // namespace System
