# Overview
QOS 

# Bootstrapping Sequence
1. In Bootloader Stage 2 (`boot2.s`)...
    1. An identity-mapped GDT for the entire physical address space is setup. 
    2. Details of available memory are read through calls to the BIOS.
    3. Setup long mode, Jump to higher-level code...
2. In (`entry.cpp`)...
    1. Setup -2GiB addressing for kernel, jump into next stage.
    2. Setup static allocations and other data structures for memory management.

# Kernel Area
- Start @ Virtual `0xFFFF_FFFF_8000_0000`
- Organized as...
```
BASE + 0     : Kernel Code
BASE + 8MiB  : Kernel Stack Start
BASE + 16MiB : Kernel Static Data Structures (Interrupts and other HW configuration, Static allocations, etc.)
Base + 32MiB : Remaining system memory, managed in static accounting
```

--------------------------------
