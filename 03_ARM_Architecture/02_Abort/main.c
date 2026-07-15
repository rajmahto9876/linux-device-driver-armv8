#include <stdio.h>
#include <stdint.h>

static int factorial (int n)
{
	if(n == 0)
	{
		return 1;
	}

	return n * factorial(n-1);
}

volatile uint64_t *ptr = (uint64_t *)0x12345678;
static void test_data_abort(void)
{
	*ptr = 5;
}

static void test_instruction_abort(void)
{
	void (*func)(void);
    func = (void *)0x12345678;
    func();
    while(1)
    {

    }
}

static void test_undefined_instruction(void)
{
    __asm__ volatile(
        ".word 0xFFFFFFFF\n"
    );
}

int main(void)
{
	int n = 5;
	int fact = factorial(n);

	test_undefined_instruction();
	test_instruction_abort();
	test_data_abort();
	while(1)
	{
		
	}
	return 0;
}