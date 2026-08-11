#include <stdio.h>
#include <stdint.h>

#include "kernel_mmu.h"
#include "gic.h"
#include "timer.h"

#ifndef __UNUSED__
#define __UNUSED__(x)  (void) (x) 
#endif

extern uint32_t __STACK_START__;
volatile unsigned long currentEl;
volatile unsigned long sctlr_el1;
volatile unsigned long ttbr0;
volatile unsigned long tcr;
volatile unsigned long mair;

static int factorial (int n)
{
	if(n == 0)
	{
		return 1;
	}

	return n * factorial(n-1);
}

static inline unsigned long read_currentEl(void)
{
    unsigned long val;
    __asm__ volatile( "mrs %0, CurrentEL": "=r"(val));
    return val;
}

static inline unsigned long read_sctlr_el1(void)
{
    unsigned long val;
    __asm__ volatile("mrs %0, SCTLR_EL1": "=r"(val));

    return val;
}

static inline unsigned long read_ttbr0_el1(void)
{
    unsigned long val;
    __asm__ volatile("mrs %0, TTBR0_EL1": "=r"(val));

    return val;
}

static inline unsigned long read_mair_el1(void)
{
    unsigned long val;
    __asm__ volatile("mrs %0, MAIR_EL1": "=r"(val));

    return val;
}

static inline unsigned long read_tcr_el1(void)
{
    unsigned long val;
    __asm__ volatile("mrs %0, TCR_EL1": "=r"(val));

    return val;
}

int main(void)
{
	int n = 5;
	int fact = 0;
	unsigned long tempEl;

	fact = factorial(n);
	__UNUSED__(fact);
	__UNUSED__(__STACK_START__);

	gic_init();
	gic_enable_timer_irq();
	timer_init();
	
	asm volatile("msr DAIFClr, #2");

	/*
		If CurrentEL = 0x4, 0x4 >> 2 = 1
		i.e., running  EL = 1 (EL1).
	*/

	// tempEl    = read_currentEl();
	// currentEl = tempEl>>2;

	// sctlr_el1 = read_sctlr_el1();
	// ttbr0 = read_ttbr0_el1();
	// tcr        = read_tcr_el1();
    // mair       = read_mair_el1();

    // build_translation_tables();

    // mmu_init();
	while(1)
	{
		asm volatile("wfi");
	}
	return 0;
}