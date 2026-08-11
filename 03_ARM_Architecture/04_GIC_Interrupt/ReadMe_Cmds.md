==============================
QEMU CMD:
==============================
qemu-system-aarch64  -M virt -cpu cortex-a53 -nographic -kernel kernel.elf -S  -gdb tcp::1234

-smp 4
Increase the number io cores on QEMU

- Machine: virt
- CPU: Cortex-A53
- Architecture: ARMv8-A
- QEMU: virt
- GIC: GICv3 (default on modern QEMU virt)
- Timer: ARM Generic Timer (PPI 30)

==============================
GDB Other Terminal:
==============================
- gdb-multiarch kernel.elf

- target remote :1234

## 3. Registers Used to Debug This, and Why

| Register                         | What it told us                                                                                                                                                                                                                                                                                                                |
| -------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `ESR_EL1`<br>*(saved in `x22`)*  | **Exception Syndrome Register** — the most important register for identifying the exception. Bits `[31:26]` (`EC`) tell us the **exception class**. This allowed us to distinguish:<br><br>• Undefined Instruction — `EC = 0x00`<br>• Data Abort — `EC = 0x24 / 0x25`<br>• Trapped System Register Access — `EC = 0x18`        |
| `ELR_EL1`<br>*(saved in `x20`)*  | Contains the **address of the instruction where the exception occurred**. We matched this address with `objdump -d` output to identify the exact instruction. This confirmed that the exception occurred at the `mrs` instruction inside `gic_enable_sre()`, rather than at the `GICD_CTLR` store that we initially suspected. |
| `FAR_EL1`<br>*(saved in `x23`)*  | Contains the **faulting virtual address** for applicable abort exceptions, particularly Data Aborts and Instruction Aborts. It read `0x0` in this case, which was informative because it ruled out the suspected memory-address access problem.                                                                                |
| `SPSR_EL1`<br>*(saved in `x21`)* | Contains the **saved processor state from before the exception**. It allowed us to verify the execution mode/exception level. In our case, it confirmed that execution was in **EL1h**, as expected.                                                                                                                           |
| `CurrentEL`                      | Shows the **current Exception Level** at which the processor is executing. This is important because assumptions about **EL1, EL2, or EL3** affect which system registers, exception vectors, and exception-routing mechanisms are applicable.                                                                                 |

### Quick Debugging Flow

```text
Exception occurs
       ↓
Read ESR_EL1
       ↓
Decode EC [31:26]
       ↓
Identify exception class
       ↓
Read ELR_EL1
       ↓
Find faulting instruction
       ↓
Compare with objdump -d
       ↓
If Abort → Check FAR_EL1
       ↓
Check SPSR_EL1 / CurrentEL
       ↓
Confirm execution level and processor state
```

### GDB Register Mapping

During our exception-handler debugging, the registers were saved as:

| GDB Register | ARM64 System Register | Purpose                                |
| ------------ | --------------------- | -------------------------------------- |
| `x20`        | `ELR_EL1`             | Faulting instruction address           |
| `x21`        | `SPSR_EL1`            | Saved processor state                  |
| `x22`        | `ESR_EL1`             | Exception syndrome / exception class   |
| `x23`        | `FAR_EL1`             | Faulting address for applicable aborts |

### Key Lesson

> **Do not guess the cause of an ARM64 exception from where the code appears to fail.**

Use the exception registers to determine what actually happened:

```text
ESR_EL1  → What type of exception?
ELR_EL1  → Which instruction caused it?
FAR_EL1  → Which address was involved?
SPSR_EL1 → What was the processor state?
CurrentEL → Which Exception Level are we executing at?
```
