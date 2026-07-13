.section .text
.global _start

.extern vectorTable
_start:

    // Initialize Stack Pointer
    ldr x0, =_stack_top
    mov sp, x0

    // Install Exception Vector Table
    ldr x0, =vectorTable
    msr VBAR_EL1, x0
    isb //Flush pipeline

    b _main
    b _loop

_loop:
    b _loop

_main:

    // Simple arithmetic
    mov x0, #5
    mov x1, #10
    add x2, x0, x1

    // Read Current Exception Level
    mrs x0, CurrentEL
    lsr x0, x0, #2

    // Read Multiprocessor Affinity Register
    mrs x1, MPIDR_EL1

    // Read Main ID Register (CPU identification)
    mrs x2, MIDR_EL1

    //Trigger Exception Here
    svc #0

    //Do simple atirhmetic again to just check
    mov x0, #5
    mov x1, #10
    add x2, x0, x1

    b main
