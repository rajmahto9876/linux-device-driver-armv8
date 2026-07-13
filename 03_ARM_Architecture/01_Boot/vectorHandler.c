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
	ret = test_function(1, 2);
	(void)ret;
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