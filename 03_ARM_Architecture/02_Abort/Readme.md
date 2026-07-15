# Data Abort
Every operating system spends a huge amount of time handling Data Aborts. For examples:

- NULL pointer dereference
- Invalid pointer
- Stack overflow
- Page fault
- Memory protection violation
All of these eventually become a Data Abort.

## CPU Generates Exception

```text
Data Abort

↓

Save ELR_EL1

↓

Save ESR_EL1

↓

Save FAR_EL1

↓

Jump to Vector

```

## Register To Check:
### FAR_EL1
- It literally means Fault Address Register.

### ELR_EL1
- Contains not the address being accessed, but the address of the instruction that caused the fault. 

### ESR_EL1
- Contains Reason for the Data Abort and much more.

```c
(gdb) print /t regs->esr_el1 
$4 = 10010110000000000000000001010000
```

As per ARM64 Doc `The AArch64 System Level Programmers’ Model
D1.10 Exception entry`:

- EC, bits[31:26] The Exception class field, that indicates the cause of the exception.
- IL, bit[25] The Instruction length bit, for synchronous exceptions, that indicates 		whether a trapped instruction was a 16-bit or a 32-bit instruction.
-ISS, bits[24:0] The Instruction specific syndrome field. Architecturally, this field 		can be defined independently for each defined Exception class. However, in practice, some ISS encodings are used for more than one Exception class.

```c
ec = (esr >> 26) & 0x3F;

0x25
```

## Note:
- Data Abort happens during `EXECUTE`.
- The instruction itself is perfectly valid.
- The memory address used by the instruction was invalid.

This is how Linux handles a page fault. When a user-space process does 
`*(int *)0x12345678 = 5;`
Linux receives the same information:
```c
ESR_EL1

↓

Reason

FAR_EL1

↓

Fault Address

ELR_EL1

↓

Faulting Instruction

```

From there, Linux decides whether to:

- map a missing page,
- send SIGSEGV,
- or panic (if it happened in kernel mode).

## ISS Layout for Data Abort (ESR_EL1 Bit)

For Data Abort exceptions, the ISS field has another format.

Simplified:

24                      10 9      6 5      0
+------------------------+---------+--------+
| Other Information      |  WnR    | DFSC   |
+------------------------+---------+--------+

### DFSC bits
- Data Fault Status Code. It tells why the access failed.
| DFSC | Meaning              |
| ---- | -------------------- |
| 0x04 | Translation Fault L0 |
| 0x05 | Translation Fault L1 |
| 0x06 | Translation Fault L2 |
| 0x07 | Translation Fault L3 |
| 0x09 | Access Flag Fault L1 |
| 0x0D | Permission Fault L1  |
| 0x11 | Alignment Fault      |
| 0x10 | TLB Conflict         |
| 0x18 | External Abort       |

Example:
```c
DFSC = 0x04

Translation Fault

↓

Level 0
```
Linux now knows there is no translation for this address.

### WnR bit
Bit 6
```c
uint64_t wnr = (iss >> 6) & 1;
```
```bash
Meaning

0

↓

Read
```

```bash
1

↓

Write
```

`*ptr = 5;` is Write, So WnR = 1

# Instruction Abort
Cause the CPU to fail while fetching an instruction, not while reading/writing data.

- Instruction Fetch
- PC as an address
- Prefetch Abort (ARMv7)
- Instruction Abort (ARMv8)
- How Linux handles bad program counters

## Example:
```c
mov x0,#0x12345678
br x0
```
i.e., 

```c
		CPU
		↓
		PC = 0x12345678
		↓
		Fetch next instruction
		↓
		No memory there
		↓
		Instruction Abort
```

- Nothing was wrong with the BR instruction.
- The problem is the CPU couldn't fetch the next instruction.
- Instruction Abort happens during `FETCH`

## NOTE (IMP):

```
ELR_EL1 = 0x12345678
FAR_EL1 = 0x12345678
```

i.e., Both are same. Both refer to the same address because the fault occurred while fetching the instruction located there.

but in data abort it is occured during the Execute, so PC is different so ELR_EL1 and FAR_EL1 is different.


# Undefined Instruction Exception
```bash
Does this match ADD?
No

Does it match SUB?
No

Does it match MOV?
No

Does it match BR?
No

Does it match any AArch64 instruction?
No


Then it is Undefined Instruction Exception

```
