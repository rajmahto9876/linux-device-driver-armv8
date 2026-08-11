#include "vectorHandler.h"

struct pt_regs;

static int test_function(int x, int y)
{
	return x + y;
}

/* Current EL with SP0 */
void sync_current_sp0_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void irq_current_sp0_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void fiq_current_sp0_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void serr_current_sp0_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

/* Current EL with SPx */
void sync_current_spx_handler(struct pt_regs *regs)
{
	int ret = 0;
	uint64_t esr = regs->esr_el1;
    uint64_t ec = (esr >> 26) & 0x3F;
    uint64_t il = (esr >> 25) & 0x01;
    uint64_t iss = (esr & 0x1FFFFFF);
    uint64_t dfsc = iss & 0x3F;
    uint64_t wnr = (iss >> 6) & 1;

    if(il == 1)
	{
		/*
			Just so compiler don't remove during optimisation.
			32-bit instruction, For AArch64 instructions it is 32 bits,
		*/
		(void) ret;
    }

    if (ec == 0x15)
    {
        // SVC
    }
    else if (ec == 0x24)
    {
        // Data Abort from same EL
    }
    else if (ec == 0x25)
    {
        // Data Abort from lower EL
    }

    /*
    	Just so compiler don't remove during optimisation.
    */
    (void)iss;
    (void)dfsc;
    (void)wnr;
}

void irq_current_spx_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void fiq_current_spx_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void serr_current_spx_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}


/* Lower EL AArch64 */
void sync_lower64_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void irq_lower64_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void fiq_lower64_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void serr_lower64_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}


/* Lower EL AArch32 */
void sync_lower32_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void irq_lower32_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void fiq_lower32_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}

void serr_lower32_handler(struct pt_regs *regs)
{
	int ret = 0;
	ret = test_function(1, 2);
	(void)ret;
}