#include <stdio.h>

static int factorial (int n)
{
	if(n == 0)
	{
		return 1;
	}

	return n * factorial(n-1);
}

int main(void)
{
	int n = 5;
	int fact = factorial(n);

	while(1)
	{
		
	}
	return 0;
}