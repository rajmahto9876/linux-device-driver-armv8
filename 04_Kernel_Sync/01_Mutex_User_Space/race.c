#include <stdio.h>
#include <stdint.h>

#include <pthread.h>

#define ENABLE_MUTEX 	1

#define MAX_INCREMENT_COUNT 	100000
static int counter = 0;

#if (ENABLE_MUTEX == 1)
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
#endif

void *func(void *args)
{
	int i = 0;
	for(i = 0; i<MAX_INCREMENT_COUNT; i++)
	{
		#if ENABLE_MUTEX == 1
			pthread_mutex_lock(&lock);
			counter++;
			pthread_mutex_unlock(&lock);
		#else
			counter++;
		#endif
	}	
	return NULL;
}

int main()	
{
	pthread_t p1, p2;
	pthread_create(&p1, NULL, func, NULL);
	pthread_create(&p2, NULL, func, NULL);
	
	
	pthread_join(p1, NULL);
	pthread_join(p2, NULL);

	printf(" Counter Value: %d \n", counter);

	return 0;
}

