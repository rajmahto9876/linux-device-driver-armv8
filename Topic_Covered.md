# Module 1 – ARM Architecture

Topics:
1. ARM Introduction
2. ARMv7 vs ARMv8
3. Registers
4. Privilege Levels
5. Exception Handling
6. Memory System
7. ARM Bus Architecture (AXI/AHB/APB)
8. Interrupt Controller (GIC)

Labs:
- ARM64 QEMU
- Register Inspection
- Exception Tracing
- GDB Practice

---

# Module 2 – ARM Assembly

Topics:
- ARM64 Instructions
- Stack
- Calling Convention
- Branches
- Functions
- Inline Assembly
- NEON Basics

Labs:
- Assembly Programs
- Mixed C + Assembly
- GDB Instruction Stepping

---

# Module 3 – Linux Boot Process

Topics:
- Boot ROM
- TF-A
- SPL
- U-Boot
- Kernel Entry
- Device Tree
- Initramfs
- RootFS
- systemd/init

Labs:
- Boot Logs
- Boot Arguments
- U-Boot Commands
- Device Tree Loading

---

# Module 4 – Linux Kernel Internals

Topics:
- Kernel Layout
- Scheduler
- Processes & Threads
- Context Switching
- System Calls
- Kernel Memory
- VFS

Labs:
- Trace start_kernel()
- Trace System Calls
- Kernel Debugging

---

# Module 5 – Device Tree

Topics:
- DTS / DTSI / DTB
- compatible
- reg
- interrupts
- GPIO
- clocks
- phandles
- overlays

Labs:
- QEMU Device Tree
- BeagleBone Device Tree
- Custom Device Node

---

# Module 6 – Character Drivers

Topics:
- Kernel Modules
- file_operations
- cdev
- Major/Minor
- ioctl
- poll
- mmap

Labs:
- LED Driver
- Memory Driver
- IOCTL Driver

---

# Module 7 – Platform Drivers

Topics:
- Platform Bus
- Probe/Remove
- Match Table
- Resources
- devm APIs

Labs:
- QEMU Platform Driver
- BeagleBone Platform Driver

---

# Module 8 – Interrupts

Topics:
- ISR
- Top Half
- Bottom Half
- Threaded IRQ
- Workqueues
- SoftIRQ
- Tasklets

Labs:
- GPIO Interrupt
- ISR Debugging

---

# Module 9 – Synchronization

Topics:
- Mutex
- Spinlock
- Semaphore
- Atomic Operations
- Completions
- RCU

Labs:
- Race Conditions
- Deadlocks
- SMP Testing

---

# Module 10 – Memory Management

Topics:
- kmalloc
- vmalloc
- Buddy Allocator
- Slab
- Page Tables
- MMU
- Cache

Labs:
- Memory Allocation
- Page Walk
- Cache Experiments

---

# Module 11 – DMA

Topics:
- DMA API
- Coherent DMA
- Streaming DMA
- DMA Engine
- Scatter-Gather
- IOMMU

Labs:
- DMA Driver
- Performance Measurement

---

# Module 12 – Linux Subsystems

Topics:
- GPIO
- Pinctrl
- Clock Framework
- Reset Framework
- I2C
- SPI
- UART
- MMC
- USB
- Ethernet
- PCIe

Labs:
- One Driver Per Subsystem

---

# Module 13 – Build Systems

Topics:
- Buildroot
- Yocto
- BitBake
- Recipes
- Layers
- SDK

Labs:
- Custom Layer
- Custom Package
- Custom Image

---

# Module 14 – BSP Development

Topics:
- Board Bring-up
- Pinmux
- Clock Tree
- DDR
- Bootloader
- Kernel Configuration
- RootFS

Labs:
- BeagleBone BSP
- Custom BSP
- QEMU BSP

---

# Module 15 – Kernel Debugging

Topics:
- printk
- Dynamic Debug
- ftrace
- perf
- kgdb
- crash
- addr2line
- objdump
- readelf

Labs:
- Panic Analysis
- Stack Trace
- Memory Leak Debugging

---

# Module 16 – Open Source Contribution

Topics:
- Kernel Coding Style
- Git Workflow
- Patch Submission
- LKML
- Review Process

Labs:
- First Kernel Patch
- Documentation Patch

---