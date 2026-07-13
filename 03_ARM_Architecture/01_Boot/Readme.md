# ARM 
ARM is a RISC Processor

## Characteristics

- Few instructions
- Simple instructions
- Most instructions complete in one cycle
- Fixed instruction size
- Load/Store architecture

## Example
Instead of
```C
ADD MEMORY1, MEMORY2
```

ARM does
```asm
LDR X0, [addr1]
LDR X1, [addr2]
ADD X2, X0, X1
STR X2, [addr3]
```

Memory is accessed only using
```asm
LDR
STR
```
Everything else happens in registers.

# ARM Core Components:
```bash
+----------------------------+
|        CPU Core            |
|                            |
| Registers                  |
| ALU                        |
| Pipeline                   |
| Decoder                    |
| Branch Predictor           |
| Exception Unit             |
+-------------+--------------+
              |
        L1 Cache
              |
        L2 Cache
              |
        MMU
              |
        AXI Bus
              |
     DDR Controller
              |
           RAM
```
# ARM V8 OVERVIEW:
In the ARMv8 architecture, specifically in the AArch64 execution state, the register file is divided into general-purpose registers and system registers, each serving distinct roles in data processing and processor control. 

## General-Purpose Registers (GPRs)
- General-purpose registers (X0-X30) are used by your programs.
- AArch64 provides 31 × 64-bit integer general-purpose registers, labeled X0 to X30. 

- 32-bit Aliases: Each 64-bit register has a corresponding 32-bit alias (W0 to W30); writing to a W register zero-extends the upper 32 bits of the X register. 

### Specialized Roles:
- X0–X7: Used for passing arguments to functions and holding return values.
- X8: Indirect result register for large return structures.
- X9–X15: Temporary registers, free for the callee to use without saving.
- X16–X17: Intra-procedure call temporary registers (IP0/IP1).
- X18: Platform register, often reserved for OS use (e.g., thread-local storage).
- X19–X28: Callee-saved registers; the function must preserve their values if used.
- X29: Frame pointer (FP), pointing to the base of the current stack frame.
- X30: Link register (LR), storing the return address after a branch with link (BL).

- Stack Pointer (SP): A special register associated with each Exception Level (EL), not a general-purpose register.

- Zero Register (XZR/WZR): A logical register that always reads as zero and discards writes. 
System Registers

## System registers:
System registers are used to configure the processor, control the Memory Management Unit (MMU), and manage exception handling. 

- Access: They cannot be accessed directly by data processing or load/store instructions.
Instead, they must be read into or written from an X register using the MRS (Move to Register from System) and MSR (Move to System from Register) instructions. 

- Naming Convention: Names end with _ELx, where x denotes the minimum privilege level (Exception Level) required to access the register.

- Banking: While conceptual system registers may be banked across exception levels (e.g., TTBR0_EL1 vs TTBR0_EL2), each physical core maintains its own set of core system registers. 

- Exception State: Dedicated registers like the Exception Link Register (ELR_ELx) and Saved Program Status Register (SPSR_ELx) hold state for returning from exceptions at each level. 
Program Counter (PC)

### System registers control the CPU itself.Linux kernel uses these registers constantly.

| Register    | Purpose                           |
| ----------- | --------------------------------- |
| `CurrentEL` | Current exception level           |
| `VBAR_EL1`  | Address of exception vector table |
| `SPSR_EL1`  | Saved program status              |
| `ELR_EL1`   | Return address after exception    |
| `TTBR0_EL1` | Page table base                   |
| `MAIR_EL1`  | Memory attributes                 |
| `SCTLR_EL1` | MMU/cache control                 |
| `ESR_EL1`   | Exception Syndrome Register       |
| `FAR_EL1`   | Fault Address Register            |

### Reading System Registers:



## Booting ELF:

