#ifndef __VECTOR_HADNLER_H_
#define __VECTOR_HADNLER_H_

struct pt_regs
{
	unsigned long r0;
	unsigned long r1;
	unsigned long r2;
	unsigned long r3;
	unsigned long r4;
	unsigned long r5;
	unsigned long r6;
	unsigned long r7;
	unsigned long r8;
	unsigned long r19;
	unsigned long r20;
	unsigned long r21;
	unsigned long r22;
	unsigned long r23;
	unsigned long r24;
	unsigned long r25;
	unsigned long r26;
	unsigned long r27;
	unsigned long r28;
	unsigned long hae;
/* JRP - These are the values provided to a0-a2 by PALcode */
	unsigned long trap_a0;
	unsigned long trap_a1;
	unsigned long trap_a2;
/* This makes the stack 16-byte aligned as GCC expects */
	unsigned long __pad0;
/* These are saved by PAL-code: */
	unsigned long ps;
	unsigned long pc;
	unsigned long gp;
	unsigned long r16;
	unsigned long r17;
	unsigned long r18;
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