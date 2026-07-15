#ifndef __VECTOR_HADNLER_H_
#define __VECTOR_HADNLER_H_

#include <stdint.h>

struct pt_regs
{
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t x30;


    uint64_t elr_el1;
    uint64_t spsr_el1;
    uint64_t esr_el1;
    uint64_t far_el1;
};

/* Current EL with SP0 */
void sync_current_sp0_handler(struct pt_regs *regs);
void irq_current_sp0(struct pt_regs *regs);
void fiq_current_sp0_handler(struct pt_regs *regs);
void serr_current_sp0_handler(struct pt_regs *regs);

/* Current EL with SPx */
void sync_current_spx_handler(struct pt_regs *regs);
void irq_current_spx_handler(struct pt_regs *regs);
void fiq_current_spx_handler(struct pt_regs *regs);
void serr_current_spx_handler(struct pt_regs *regs);

/* Lower EL AArch64 */
void sync_lower64_handler(struct pt_regs *regs);
void irq_lower64_handler(struct pt_regs *regs);
void fiq_lower64_handler(struct pt_regs *regs);
void serr_lower64_handler(struct pt_regs *regs);

/* Lower EL AArch32 */
void sync_lower32_handler(struct pt_regs *regs);
void irq_lower32_handler(struct pt_regs *regs);
void fiq_lower32_handler(struct pt_regs *regs);
void serr_lower32_handler(struct pt_regs *regs);

#endif /* __VECTOR_HADNLER_H_ */