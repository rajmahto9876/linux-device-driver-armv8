#include "vectorHandler.h"

.equ PT_REGS_SIZE, 288

.macro VECTOR_ENTRY handler
    b \handler
    .space 128 - 4
.endm

/* Save Context */
.macro save_context
   
    sub sp, sp, #PT_REGS_SIZE

    stp x0, x1, [sp, #16*0]
    stp x2, x3, [sp, #16*1]
    stp x4, x5, [sp, #16*2]
    stp x6, x7, [sp, #16*3]
    stp x8, x9, [sp, #16*4]
    stp x10, x11, [sp, #16*5]
    stp x12, x13, [sp, #16*6]
    stp x14, x15, [sp, #16*7]
    stp x16, x17, [sp, #16*8]
    stp x18, x19, [sp, #16*9]
    stp x20, x21, [sp, #16*10]
    stp x22, x23, [sp, #16*11]
    stp x24, x25, [sp, #16*12]
    stp x26, x27, [sp, #16*13]
    stp x28, x29, [sp, #16*14]

    /* Save Link Register */
    str x30,[sp, #16*15]

    /* Save system Registers in caller Regs */
    mrs x20, ELR_EL1
    str x20, [sp, #8 *31]

    mrs x21, SPSR_EL1
    str x21, [sp, #8* 32]

    mrs x22, ESR_EL1
    str x22,[sp,#8*33]

    mrs x23, FAR_EL1
    str x23,[sp,#8*34]

    /*  Move Sp to x0 as it is the first args */
    mov x0, sp

    /* call C functions after this*/
.endm

.macro restore_context
/*
    
*/
  /* Restore the System Registers*/
    ldr x20, [sp, #8 *31]
    msr ELR_EL1, x20

    ldr x21, [sp, #8 *32]
    msr SPSR_EL1, x21

    ldr x22, [sp,#8*32] 
    msr ESR_EL1,x22

    ldr x23, [sp,#8*34] 
    msr FAR_EL1,x23

    ldr x30,[sp, #16*15]

    ldp x28, x29, [sp, #16 * 14]
    ldp x26, x27, [sp, #16 * 13]
    ldp x24, x25, [sp, #16 * 12]
    ldp x22, x23, [sp, #16 * 11]
    ldp x20, x21, [sp, #16 * 10]
    ldp x18, x19, [sp, #16 * 9]
    ldp x16, x17, [sp, #16 * 8]
    ldp x14, x15, [sp, #16 * 7]
    ldp x12, x13, [sp, #16 * 6]
    ldp x10, x11, [sp, #16 * 5]
    ldp x8, x9, [sp, #16 * 4]
    ldp x6, x7, [sp, #16 * 3]
    ldp x4, x5, [sp, #16 * 2]
    ldp x2, x3, [sp, #16 * 1]
    ldp x0, x1, [sp, #16* 0]

    add sp, sp, #PT_REGS_SIZE
.endm

.section .text.vectors
.align 11
.global vectorTable
//------------------------------------------------------------
// Exception Vector Table
//------------------------------------------------------------

vectorTable:

    /* Current EL with SP0 */
    VECTOR_ENTRY sync_current_sp0
    VECTOR_ENTRY irq_current_sp0
    VECTOR_ENTRY fiq_current_sp0
    VECTOR_ENTRY serr_current_sp0

    /* Current EL with SPx */
    VECTOR_ENTRY sync_current_spx
    VECTOR_ENTRY irq_current_spx
    VECTOR_ENTRY fiq_current_spx
    VECTOR_ENTRY serr_current_spx

    /* Lower EL AArch64 */
    VECTOR_ENTRY sync_lower64
    VECTOR_ENTRY irq_lower64
    VECTOR_ENTRY fiq_lower64
    VECTOR_ENTRY serr_lower64

    /* Lower EL AArch32 */
    VECTOR_ENTRY sync_lower32
    VECTOR_ENTRY irq_lower32
    VECTOR_ENTRY fiq_lower32
    VECTOR_ENTRY serr_lower32


/* Definitions */

/* Current EL with SP0 */
sync_current_sp0:
    b sync_current_sp0_handler
    eret

irq_current_sp0:
    b irq_current_sp0_handler
    eret

fiq_current_sp0:
    b fiq_current_sp0_handler
    eret

serr_current_sp0:
    b serr_current_sp0_handler
    eret


    /* Current EL with SPx */
sync_current_spx:
    
    save_context
    
    bl sync_current_spx_handler
    
    restore_context
        
    eret

irq_current_spx:
    b irq_current_spx_handler
    eret

fiq_current_spx:
    b fiq_current_spx_handler
    eret

serr_current_spx:
    b serr_current_spx_handler
    eret


    /* Lower EL AArch64 */
sync_lower64:
    b sync_lower64_handler
    eret 

irq_lower64:
    b irq_lower64_handler
    eret 

fiq_lower64:
    b fiq_lower64_handler
    eret 

serr_lower64:
    b serr_lower64_handler
    eret 


    /* Lower EL AArch32 */
sync_lower32:
    b sync_lower32_handler
    eret

irq_lower32:
    b irq_lower32_handler
    eret

fiq_lower32:
    b fiq_lower32_handler
    eret

serr_lower32:
    b serr_lower32_handler
    eret
