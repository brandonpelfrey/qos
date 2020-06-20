extern __kernel_main

; This function exists just as a convenience.
; By making this the first code in the kernel binary image,
; We can jump to a known address and then jump into __kernel_main
; which is at some location only known at link-time
boot3_entry:
  call __kernel_main