# SENG21213 Operating System

A custom, bare-metal x86 operating system developed from scratch for the SENG21213 course at the University of Kelaniya. This project demonstrates the core principles of operating system design, progressing from a basic bootloader and shell to a fully functioning multi-threaded kernel with memory management and system calls.

## Features

This OS was developed in incremental stages, each adding critical functionality:

- **Stage 0: Boot & Shell**
  - Custom bootloader loading the kernel from disk.
  - Switch to 32-bit Protected Mode.
  - Basic VGA text driver and PS/2 keyboard driver.
  - Interactive shell.
- **Stage 1: Process Management**
  - Process Control Block (PCB) structure and process states.
  - Round-Robin Scheduler.
  - Assembly-level context switching (`switch_to`).
- **Stage 2: Synchronization & Threads**
  - Lightweight threads executing within the kernel address space.
  - Mutexes (spin-locks) for mutual exclusion.
  - Semaphores for signalling and synchronization.
- **Stage 3: Memory Management & System Calls**
  - **Memory Management:**
    - Physical Memory Manager (PMM) using a bitmap allocator (4 KB pages, 1 MB pool).
    - Heap Allocator (`kmalloc`/`kfree`) using a bump allocator with free-list reclaim.
  - **System Calls:**
    - Interrupt Descriptor Table (IDT) configuration.
    - Safe Timer IRQ (Interrupt Request) handling.
    - `INT 0x80` Syscall gate interface (`SYS_YIELD`, `SYS_EXIT`, `SYS_GETPID`, etc.).
- **Stage 4: RAM Disk & File System**
  - **RAM Disk:** 1 MB fixed-size in-memory storage (BSS segment) partitioned into 4 KB blocks.
  - **File System:** Custom file system with Superblock, Inode/Block bitmaps, and Inode table.
  - **POSIX-like API:** `fs_create`, `fs_open`, `fs_read`, `fs_write`, `fs_close`, `fs_unlink`.
- **Integration:**
  - Fully integrated shell capable of displaying process lists (`ps`), memory statistics (`memstat`), running background demo threads (`run`), changing text colours, and executing syscalls directly.
  - File system commands: list (`ls`), create empty file (`touch`), read file (`cat`), write to file (`write`), and delete file (`rm`).
## Technologies Used

- **C:** Core kernel logic, drivers, process management, and memory allocators.
- **Assembly (NASM):** Bootloader, low-level context switching, and ISR (Interrupt Service Routine) stubs.
- **Makefile:** Build automation and linking.
- **QEMU:** Emulation and testing environment.
- **i686-elf GCC Toolchain:** Cross-compiling the OS for the target architecture.

## Getting Started

### Prerequisites
You need the following installed on your system:
- `nasm` (Netwide Assembler)
- `i686-elf-gcc` (Cross-compiler)
- `qemu-system-i386` (Emulator)
- `make`

### Building and Running

To compile the OS, generate the disk image, and launch it in QEMU, simply run:

```bash
make run
```

To clean the build artifacts:
```bash
make clean
```

## Git Tags Breakdown

The project's evolution is captured through the following Git tags:

- `v0.1-stage0` - Initial bootloader, 32-bit switch, VGA/Keyboard drivers, and basic shell.
- `v0.2-stage1` - Process management, PCB, round-robin scheduler, and context switching.
- `v0.3-stage2` - Threads, mutexes, and semaphores implementation.
- `v0.4-stage3` - Memory management (PMM, Heap), IDT, timer IRQs, and system call interface.
- `v0.5-stage4` - Final integration: RAM disk, file system, POSIX API, and file-related shell commands.

## Output

<img width="926" height="581" alt="Screenshot 2026-09-04 at 16 58 34" src="https://github.com/user-attachments/assets/ce3f5896-36e7-4772-934a-ff8ba1f26ddc" />

<img width="926" height="593" alt="Screenshot 2026-09-04 at 16 58 56" src="https://github.com/user-attachments/assets/e9423e56-bece-4273-a64c-32dbd01c7449" />

<img width="1256" height="731" alt="Screenshot 2026-09-04 at 16 57 54" src="https://github.com/user-attachments/assets/a31cdde7-98ae-45ea-bf58-9b48c587325f" />



