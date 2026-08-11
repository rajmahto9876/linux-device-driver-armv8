# ARMv8-A & Embedded Linux Learning Roadmap

> **Goal:** Become an Embedded Linux / ARM64 Kernel & Driver Engineer by understanding the architecture from the hardware level to the Linux kernel internals.

---

# Module 1 – ARMv8 Architecture Fundamentals

## Objective

Understand the ARMv8 architecture, execution model, privilege levels, registers, and exception handling from the hardware perspective.

### Theory

* ARM Architecture Overview
* ARMv7 vs ARMv8
* AArch32 vs AArch64
* Register Architecture
* Program Counter & Stack Pointer
* CurrentEL Register
* Exception Levels (EL0–EL3)
* ARM Execution Model
* Memory System Overview
* ARM Bus Architecture

  * AXI
  * AHB
  * APB
* Generic Interrupt Controller (GIC)

---

## Labs

### Lab 1.1

Bare-metal ARM64 Boot on QEMU

### Lab 1.2

Vector Table (VBAR_EL1)

### Lab 1.3

Supervisor Call (SVC)

### Lab 1.4

Data Abort Exception

### Lab 1.5

Exception Return (ERET)

### Lab 1.6

Calling C from Exception Handler

#### Lab 1.6.1

AAPCS64 Calling Convention

#### Lab 1.6.2

Stack Frames

#### Lab 1.6.3

Caller Saved Registers

#### Lab 1.6.4

Callee Saved Registers

#### Lab 1.6.5

Saving Context

### Lab 1.7

Linux-style `struct pt_regs`

### Lab 1.8

Exception Syndrome Register (ESR_EL1)

#### Lab 1.8.1

ESR Decoding

#### Lab 1.8.2

ISS, EC, DFSC, IFSC, WnR

#### Lab 1.8.3

Instruction Abort

#### Lab 1.8.4

Undefined Instruction

---

**Status:** ✅ Completed

---

# Module 2 – Linux ARM64 Exception Handling

## Objective

Understand how Linux implements ARM64 exception handling internally.

---

## Lab 2.1

Linux ARM64 Directory Structure

* arch/arm64/
* include/
* kernel/

---

## Lab 2.2

Understanding `entry.S`

Topics

* kernel_ventry
* kernel_entry
* kernel_exit
* Exception Vectors
* Stack Switching

---

## Lab 2.3

Linux `struct pt_regs`

Compare

* Linux
* Your implementation

---

## Lab 2.4

System Call Flow

```
User
 ↓
svc
 ↓
entry.S
 ↓
el0_sync
 ↓
el0_svc
 ↓
invoke_syscall()
 ↓
sys_write()
```

---

## Lab 2.5

Page Fault Flow

```
Invalid Address
      ↓
Data Abort
      ↓
entry.S
      ↓
do_mem_abort()
      ↓
handle_mm_fault()
```

---

## Lab 2.6

Interrupt Entry

* IRQ
* FIQ
* SError
* Synchronous Exception

---

## Lab 2.7

Complete Exception Walkthrough

```
printf()
 ↓
write()
 ↓
svc
 ↓
Kernel
 ↓
UART Driver
```

---

# Module 3 – ARM64 MMU & Virtual Memory

## Objective

Understand virtual memory and page translation.

---

## Theory

* Virtual Memory
* Physical Memory
* Address Translation
* Translation Regimes
* Stage-1 Translation
* Stage-2 Translation
* Page Tables
* TLB
* Memory Attributes

---

## Registers

* SCTLR_EL1
* TTBR0_EL1
* TTBR1_EL1
* MAIR_EL1
* TCR_EL1

---

## Labs

### Lab 3.1

Page Table Format

### Lab 3.2

Build Translation Tables

### Lab 3.3

Enable MMU

### Lab 3.4

Translation Faults

### Lab 3.5

Page Table Walk

### Lab 3.6

TLB Operations

---

# Module 4 – ARM64 Cache Architecture

## Theory

* L1 Cache
* L2 Cache
* Cache Hierarchy
* Cache Coherency
* Instruction Cache
* Data Cache

---

## Labs

### Lab 4.1

Cache Maintenance

### Lab 4.2

Invalidate Cache

### Lab 4.3

Clean Cache

### Lab 4.4

DSB / DMB / ISB

### Lab 4.5

Performance Measurement

---

# Module 5 – Generic Interrupt Controller (GIC)

## Theory

* GIC Architecture
* Distributor
* CPU Interface
* Redistributor
* Priority
* Interrupt Routing

---

## Labs

### Lab 5.1

Configure GIC

### Lab 5.2

Software Generated Interrupts

### Lab 5.3

Private Peripheral Interrupts

### Lab 5.4

Shared Peripheral Interrupts

### Lab 5.5

Interrupt Priorities

---

# Module 6 – ARM Generic Timer

## Theory

* CNTFRQ_EL0
* CNTPCT_EL0
* CNTVCT_EL0
* Physical Timer
* Virtual Timer

---

## Labs

### Lab 6.1

Read System Counter

### Lab 6.2

Periodic Timer

### Lab 6.3

Timer Interrupt

### Lab 6.4

Scheduler Tick

---

# Module 7 – ARM64 Boot Process

## Theory

```
ROM
 ↓
Bootloader
 ↓
TF-A
 ↓
U-Boot
 ↓
Linux
 ↓
init
 ↓
systemd
```

---

## Labs

### Lab 7.1

Reset Sequence

### Lab 7.2

head.S

### Lab 7.3

Kernel Decompression

### Lab 7.4

Kernel Initialization

---

# Module 8 – Linux Process Management

## Theory

* task_struct
* thread_info
* Context Switching
* fork()
* execve()
* clone()

---

## Labs

### Lab 8.1

Context Switch

### Lab 8.2

Scheduler

### Lab 8.3

Kernel Threads

---

# Module 9 – Device Tree

## Theory

* DTS
* DTB
* FDT
* Device Tree Parsing

---

## Labs

### Lab 9.1

Build DTB

### Lab 9.2

GPIO Node

### Lab 9.3

UART Node

### Lab 9.4

Interrupt Properties

### Lab 9.5

Clock Framework

---

# Module 10 – Linux Driver Internals

## Theory

* Character Drivers
* Platform Drivers
* Driver Model
* Device Model
* DMA

---

## Labs

### Lab 10.1

Character Driver

### Lab 10.2

Platform Driver

### Lab 10.3

Interrupt Driver

### Lab 10.4

DMA Driver

---

# Module 11 – ARM64 Synchronization

## Theory

* Memory Ordering
* Atomics
* Exclusive Monitor
* Spinlocks
* Mutexes

---

## Labs

### Lab 11.1

LDXR/STXR

### Lab 11.2

Spinlock Implementation

### Lab 11.3

Barrier Instructions

---

# Module 12 – Virtualization

## Theory

* EL2
* Hypervisor
* Stage-2 MMU
* KVM

---

## Labs

### Lab 12.1

Hypervisor Registers

### Lab 12.2

Guest Entry

### Lab 12.3

VM Exit

---

# Module 13 – ARM64 Security

## Theory

* EL3
* Secure Monitor
* TrustZone
* Pointer Authentication (PAC)
* Branch Target Identification (BTI)
* Memory Tagging Extension (MTE)

---

## Labs

### Lab 13.1

SMC Instruction

### Lab 13.2

Secure World Overview

### Lab 13.3

Pointer Authentication

---

# Module 14 – Linux ARM64 Source Code Deep Dive

## Objective

Read the ARM64 Linux kernel source

---

## Files

* arch/arm64/kernel/head.S
* arch/arm64/kernel/entry.S
* arch/arm64/kernel/traps.c
* arch/arm64/mm/fault.c
* arch/arm64/kernel/process.c
* arch/arm64/kernel/syscall.c
* arch/arm64/kernel/smp.c

---

## Final Project

Build a minimal ARM64 kernel environment capable of:

* Booting on QEMU
* Handling exceptions
* Configuring the MMU
* Handling interrupts
* Scheduling simple tasks
* Understanding and debugging the Linux ARM64 exception and memory management paths
