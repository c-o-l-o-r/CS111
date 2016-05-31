#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>

int thread_count=1;
int iteration_count=1;
long long counter = 0;
int opt_yield=0, debugtest=0, sync_m=0, sync_s=0, sync_c=0;
int flag_error = 0;

#define DEBUG_threads 1
#define DEBUG_iterations 1
#define DEBUG_yield 0

/***** Original add *****/
void add(long long *pointer, long long value) 
{
	long long sum = *pointer + value;
	if (opt_yield)
		pthread_yield();
	*pointer = sum;
}

/***** Add with mutex *****/
pthread_mutex_t lock_m;
void add_m(long long *pointer, long long value) 
{
	pthread_mutex_lock(&lock_m);

	long long sum = *pointer + value;
	if (opt_yield)
		pthread_yield();
	*pointer = sum;
	
	pthread_mutex_unlock(&lock_m);
}

/***** Add with spinlock *****/
int flag_s=0;
void lock_s() 
{
	while (__sync_lock_test_and_set (&flag_s, 1) == 1)
		; // spin-wait (do nothing)
}

void unlock_s() 
{
	flag_s=0;
}

void add_s(long long *pointer, long long value) 
{
	lock_s();

	long long sum = *pointer + value;
	if (opt_yield)
		pthread_yield();
	*pointer = sum;
	
	unlock_s();
}

/***** Add with compare_and_swap *****/
void add_c(long long *pointer, long long value) 
{
	
	long long old;
	long long sum;
	int flag=0;
	while (1)
	{
		old = *pointer;
		sum = old + value;
		
		if (opt_yield)
			pthread_yield();
		
		if (__sync_val_compare_and_swap(pointer,old,sum)==old) break;
	}
}

/***** Worker thread *****/
void* worker(void* arg)
{
	int iter = *((int*)arg);
	for (int i=0;i<iter;i++)
	{
		if (sync_m)
		{
			add_m(&counter,1);
			add_m(&counter,-1);			
		}
		else if (sync_s)
		{
			add_s(&counter,1);
			add_s(&counter,-1);				
		}
		else if (sync_c)
		{
			add_c(&counter,1);
			add_c(&counter,-1);				
		}
		else
		{
			add(&counter,1);
			add(&counter,-1);
		}
	}
}

int main(int argc, char **argv)
{
	thread_count = DEBUG_threads;
	iteration_count = DEBUG_iterations;
	opt_yield = DEBUG_yield;
	
	// parse options
	int c;
	
	while (1)
    {
		static struct option long_options[] =
        {
			{"threads", required_argument, 0, 't'},
			{"iterations", required_argument, 0, 'i'},
			{"sync", required_argument, 0, 's'},
			{"yield", no_argument, 0, 'y'},
			{"debugtest", no_argument, 0, 'd'},
			{0, 0, 0, 0}
        };
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "t:i:s:ed",
                       long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) break;
		
		switch (c)
        {
		case 't':
			thread_count = atoi(optarg);
			break;
		case 'i':
			iteration_count = atoi(optarg);
			break;
		case 's':
			switch (*optarg)
			{
			case 'm':
				sync_m=1;
				break;
			case 's':
				sync_s=1;
				break;
			case 'c':
				sync_c=1;
				break;
			default:
				fprintf(stderr,"Error options --sync\n");
				exit(1);
			}
			break;
		case 'y':
			opt_yield = 1;
			break;
		case 'd':
			debugtest = 1;
			break;
		default:
			fprintf(stderr,"Error parsing options\n");
			exit(1);
		}
	}
	
	// Thread init
	if (sync_m)
	{
		if (pthread_mutex_init(&lock_m, NULL) != 0)
		{
			printf("\n mutex init failed\n");
			exit(-1);
		}
	}
	
	pthread_t* threads = malloc(thread_count*sizeof(pthread_t));
	if (threads == NULL) 
	{
		fprintf(stderr, "malloc failed\n");
		exit(-1);
	}
	
	// Clock starts
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	
	// Threading
	for (int t=0;t<thread_count;t++)
	{
		pthread_create(&threads[t],NULL,worker,(void*)&iteration_count);
	}
	for (int t=0;t<thread_count;t++)
	{
		pthread_join(threads[t],NULL);
	}

	// Clock ends
	clock_gettime(CLOCK_MONOTONIC, &end);
	
	int operation_count = thread_count*iteration_count;
	double elapsed = (double)(end.tv_sec - start.tv_sec)*1000000000 + (double)end.tv_nsec - (double)start.tv_nsec;
	
	if (debugtest)
	{
		printf("%d\t%d\t%d\t%d\t%.0f\n",thread_count,iteration_count,operation_count,counter,elapsed);
		exit(0);
	}
	
	printf("%d threads x %d iterations x (add + subtract) = %d operations\n",thread_count,iteration_count,operation_count);
	if (counter != 0) 
	{
		fprintf(stderr, "ERROR: final counter = %d\n", counter);
		flag_error = 1;
	}
	printf("elapsed time: %.0f ns\n",elapsed);
	printf("per operation: %.0f ns\n",elapsed/operation_count);
	
	if (flag_error) exit(1);
	else exit(0);
}
