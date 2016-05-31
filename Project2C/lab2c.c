#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>
#include <string.h>

int thread_count=1;
int iteration_count=1;
int list_count=1;
long long counter = 0;
int opt_yield=0, debugtest=0, sync_m=0, sync_s=0;
int flag_error = 0;
SortedList_t* list;
SortedList_t** lists;

#define DEBUG_threads 1
#define DEBUG_iterations 1
#define DEBUG_lists 1
#define DEBUG_yield 0

/***** Helper functions *****/
SortedListElement_t* new_element(const char* key)
{
	SortedListElement_t* element = malloc(sizeof(SortedListElement_t));
	element->key = key;
	element->prev = NULL;
	element->next = NULL;
	return element;
}


/***** Worker thread *****/
void* worker(void* arg)
{
	char** string_array = (char**) arg;
	
	// Iterate through string array
	for (int i=0;i<iteration_count;i++)
	{
		// Create element
		SortedListElement_t *element = new_element(string_array[i]);
		
		// Which sublist it should be in
		int index = hash(string_array[i],list_count);
		
		// Lock before insert
		if (sync_m) pthread_mutex_lock(&lock_m[index]);
		else if (sync_s) lock_s(index);
		
		// Insert to sublist
		if (sync_m) SortedList_insert_mutex(lists[index],element);
		else if (sync_s) SortedList_insert_tas(lists[index],element);
		else SortedList_insert(lists[index],element);
		
		if (sync_m) pthread_mutex_unlock(&lock_m[index]);
		else if (sync_s) unlock_s(index);
	}
	
	// Check length of the lists
	int list_length=0;
	for (int i=0;i<list_count;i++)
	{
		if (sync_m) pthread_mutex_lock(&lock_m[i]);
		else if (sync_s) lock_s(i);
		
		int l = SortedList_length(lists[i]);
		if (l==-1)
		{
			fprintf(stderr, "Error: List corrupted\n");
			exit(1);
		}
		list_length += SortedList_length(lists[i]);
		
		if (sync_m) pthread_mutex_unlock(&lock_m[i]);
		else if (sync_s) unlock_s(i);
	}

	

	// Look up and delete
	for (int i=0;i<iteration_count;i++)
	{
		// Which sublist it should be in
		int index = hash(string_array[i],list_count);
		
		// Lock before lookup / delete
		if (sync_m) pthread_mutex_lock(&lock_m[index]);
		else if (sync_s) lock_s(index);
		
		SortedListElement_t *element;
		if (sync_m) element=SortedList_lookup_mutex(lists[index],string_array[i]);
		else if (sync_s) element=SortedList_lookup_tas(lists[index],string_array[i]);
		else element=SortedList_lookup(lists[index],string_array[i]);
		if (element==NULL)
		{
			fprintf(stderr, "Error: Element not found\n");
			exit(1);
		}
		
		int e;
		if (sync_m) e=SortedList_delete_mutex(element);
		else if (sync_s) e=SortedList_delete_tas(element);
		else e=SortedList_delete(element);
		
		if (e!=0)
		{
			fprintf(stderr, "Error: Delete element error\n");
			exit(1);
		}
		
		if (sync_m) pthread_mutex_unlock(&lock_m[index]);
		else if (sync_s) unlock_s(index);
	}
}

/***** Main *****/
int main(int argc, char **argv)
{
	thread_count = DEBUG_threads;
	iteration_count = DEBUG_iterations;
	list_count = DEBUG_lists;
	opt_yield = DEBUG_yield;
	
	// parse options
	int c;

	while (1)
    {
		static struct option long_options[] =
        {
			{"threads", required_argument, 0, 't'},
			{"iterations", required_argument, 0, 'i'},
			{"list", required_argument, 0, 'l'},
			{"sync", required_argument, 0, 's'},
			{"yield", required_argument, 0, 'y'},
			{"debugtest", no_argument, 0, 'd'},
			{0, 0, 0, 0}
        };
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "t:i:l:s:e:d",
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
		case 'l':
			list_count = atoi(optarg);
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
			default:
				fprintf(stderr,"Error options --sync\n");
				exit(1);
			}
			break;
		case 'y':
			for (int i=0;i<strlen(optarg);i++)
			{
				if (optarg[i]=='i') opt_yield+=INSERT_YIELD;
				if (optarg[i]=='d') opt_yield+=DELETE_YIELD;
				if (optarg[i]=='s') opt_yield+=SEARCH_YIELD;
			}
			break;
		case 'd':
			debugtest = 1;
			break;
		default:
			fprintf(stderr,"Error parsing options\n");
			exit(1);
		}
	}

	// Initialize an empty list
	lists = malloc(sizeof(SortedListElement_t*)*list_count);
	for (int i=0;i<list_count;i++)
	{
		lists[i] = new_element(NULL);
	}
	
	// Initialize threads*iterations random strings
	char** string_array = malloc(sizeof(char*)*thread_count*iteration_count);
	for (int i=0;i<thread_count*iteration_count;i++)
	{
		int len = (rand() % 100) + 8;

		static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		
		string_array[i] = malloc(sizeof(char)*(len+1));
		if (string_array[i]==NULL)
		{
			fprintf(stderr,"malloc failed\n");
			exit(1);
		}
		
		// To make each key unique: First 7 chars used as id
		string_array[i][0]=alphanum[0];
		string_array[i][1]=alphanum[0];
		string_array[i][2]=alphanum[0];
		string_array[i][3]=alphanum[0];
		string_array[i][4]=alphanum[0];
		string_array[i][5]=alphanum[0];
		string_array[i][6]=alphanum[0];
		int d = 0;
		int q = i;
		while (q)
		{
			string_array[i][d] = alphanum[q % (sizeof(alphanum) - 1)];
			q = q / (sizeof(alphanum) - 1);
			d++;
		}

		for (int j = 7; j < len; ++j) 
		{
			string_array[i][j] = alphanum[rand() % (sizeof(alphanum) - 1)];
		}

		// Add \0 at the end of string
		string_array[i][len] = 0;
	}

	// Thread init
	if (sync_m)
	{
		SortedList_mutex_init(list_count);
	}
	
	if (sync_s)
	{
		SortedList_tas_init(list_count);
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
		pthread_create(&threads[t],NULL,worker,(void*)(string_array+iteration_count*t));
	}
	for (int t=0;t<thread_count;t++)
	{
		pthread_join(threads[t],NULL);
	}
	
	// Clock ends
	clock_gettime(CLOCK_MONOTONIC, &end);
	
	int operation_count = thread_count*iteration_count*2;
	double elapsed = (double)(end.tv_sec - start.tv_sec)*1000000000 + (double)end.tv_nsec - (double)start.tv_nsec;
	
	if (debugtest)
	{
		printf("%d\t%d\t%d\t%d\t%.0f\n",thread_count,iteration_count,operation_count,list_count,elapsed);
		exit(0);
	}
	
	// Check length of the lists
	int list_length=0;
	for (int i=0;i<list_count;i++)
	{
		list_length += SortedList_length(lists[i]);
	}
	
	if (list_length!=0)
	{
		fprintf(stderr, "ERROR: list length = %d\n", list_length);
		flag_error = 1;
	}
	
	printf("%d lists\n",list_count);
	printf("%d threads x %d iterations x (insert + lookup/delete) = %d operations\n",thread_count,iteration_count,operation_count); 
	printf("elapsed time: %.0f ns\n",elapsed);
	printf("per operation: %.0f ns\n",elapsed/operation_count);
	
	if (flag_error) exit(1);
	else exit(0);
}
