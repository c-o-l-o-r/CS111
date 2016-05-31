#define _GNU_SOURCE
#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>


// Return a hash from 0 to limit-1
int hash(const char* key, int limit)
{
	int sum=0;
	for (int i=0;i<strlen(key);i++)
	{
		sum += key[i];
	}
	return sum % limit;
}

// Which sublist is it?
int list_index(SortedList_t *list)
{
	for (int i=0;i<list_count;i++)
	{
		if (lists[i]==list) return i;
	}
	
	// Cannot find list, error
	fprintf(stderr, "Error: Cannot find list, in function list_index\n");
	exit(-1);
}
/***************************** ORIGINAL *****************************/

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *	The specified element will be inserted in to
 *	the specified list, which will be kept sorted
 *	in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 *
 * Note: if (opt_yield & INSERT_YIELD)
 *		call pthread_yield in middle of critical section
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
	// Empty list, just insert
	if (list->next==NULL)
	{	
		list->next = element;
		
		if (opt_yield & INSERT_YIELD)
			pthread_yield();
		
		element->prev = list;
		return;
	}

	// Not empty list, iterate
	// Algorithm: Insert if element->key before if it is smaller than current key
	SortedListElement_t* iteration = list->next;
	while (1)
	{
		if (strcmp(element->key,iteration->key)<=0)
		{
			// Insert
			element->next = iteration;
			
			if (opt_yield & INSERT_YIELD)
				pthread_yield();
		
			element->prev = iteration->prev;
			iteration->prev->next = element;
			iteration->prev = element;
			return;
		}
		
		// If this is the last iteration, element is simply the largest one, insert at the end
		if (iteration->next==NULL)
		{
			element->prev = iteration;
			
			if (opt_yield & INSERT_YIELD)
				pthread_yield();
			
			element->next = NULL;
			iteration->next = element;
			return;
		}
		
		// iterate
		iteration = iteration->next;
	}
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *	The specified element will be removed from whatever
 *	list it is currently in.
 *
 *	Before doing the deletion, we check to make sure that
 *	next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 * Note: if (opt_yield & DELETE_YIELD)
 *		call pthread_yield in middle of critical section
 */
int SortedList_delete( SortedListElement_t *element)
{
	// Check prev/next corruption
	if (element->next!=NULL)
	{
		if (element->next->prev!=element) return 1;
	}
	if (element->prev!=NULL)
	{
		if (element->prev->next!=element) return 1;
	}
	
	if (element->next!=NULL) element->next->prev = element->prev;
	
	if (opt_yield & DELETE_YIELD)
		pthread_yield();
			
	if (element->prev!=NULL) element->prev->next = element->next;
	free(element);
	return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *	The specified list will be searched for an
 *	element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 *
 * Note: if (opt_yield & SEARCH_YIELD)
 *		call pthread_yield in middle of critical section
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Found key, return
		if (iteration->key!=NULL)
			if (strcmp(iteration->key,key)==0)
			{
				return iteration;
			}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();	
		
		// iterate
		iteration = iteration->next;
	}
	return NULL;
}

/**
 * SortedList_length ... count elements in a sorted list
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *	   -1 if the list is corrupted
 *
 * Note: if (opt_yield & SEARCH_YIELD)
 *		call pthread_yield in middle of critical section
 */
int SortedList_length(SortedList_t *list)
{
	int count = 0;
	
	// Check list corruption for the head
	if (list->prev!=NULL) return -1; 
	
	SortedListElement_t* previous_iteration = NULL;
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Check list corruption
		if (iteration->prev!=previous_iteration) return -1;
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();
		
		if (previous_iteration!=NULL)
			if (previous_iteration->next!=iteration) return -1;
		
		// iterate
		previous_iteration = iteration;
		iteration = iteration->next;
		
		count++;
	}
	
	// Check list corruption for the last iteration
	if (previous_iteration->next!=NULL) return -1; 
	
	// Excluding list head
	return count-1;
}


/***************************** MUTEX *****************************/
pthread_mutex_t* lock_m;

void SortedList_mutex_init(int count)
{
	lock_m = malloc(sizeof(pthread_mutex_t)*count);
	if (lock_m==NULL)
	{
		printf("malloc failed\n");
		exit(-1);
	}
	for (int i=0;i<count;i++)
	{
		if (pthread_mutex_init(&lock_m[i], NULL) != 0)
		{
			printf("mutex init failed\n");
			exit(-1);
		}
	}
}

void SortedList_insert_mutex(SortedList_t *list, SortedListElement_t *element)
{
	// Empty list, just insert
	if (list->next==NULL)
	{	
		list->next = element;
		
		if (opt_yield & INSERT_YIELD)
			pthread_yield();
		
		element->prev = list;
		
		
		return;
	}

	// Not empty list, iterate
	// Algorithm: Insert if element->key before if it is smaller than current key
	SortedListElement_t* iteration = list->next;
	while (1)
	{
		if (strcmp(element->key,iteration->key)<=0)
		{
			// Insert
			element->next = iteration;
			
			if (opt_yield & INSERT_YIELD)
				pthread_yield();
		
			element->prev = iteration->prev;
			iteration->prev->next = element;
			iteration->prev = element;
			
			
			return;
		}
		
		// If this is the last iteration, element is simply the largest one, insert at the end
		if (iteration->next==NULL)
		{
			element->prev = iteration;
			
			if (opt_yield & INSERT_YIELD)
				pthread_yield();
			
			element->next = NULL;
			iteration->next = element;
			
			
			return;
		}
		
		// iterate
		iteration = iteration->next;
	}
}

int SortedList_delete_mutex(SortedListElement_t *element)
{
	// Check prev/next corruption
	if (element->next!=NULL)
	{
		if (element->next->prev!=element) 
		{
			return 1;
		}
	}
	if (element->prev!=NULL)
	{
		if (element->prev->next!=element) 
		{
			return 1;
		}
	}
	
	if (element->next!=NULL) element->next->prev = element->prev;
	
	if (opt_yield & DELETE_YIELD)
		pthread_yield();
			
	if (element->prev!=NULL) element->prev->next = element->next;
	free(element);
	
	return 0;
}

SortedListElement_t *SortedList_lookup_mutex(SortedList_t *list, const char *key)
{
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Found key, return
		if (iteration->key!=NULL)
			if (strcmp(iteration->key,key)==0)
			{
				
				return iteration;
			}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();	
		
		// iterate
		iteration = iteration->next;
	}
	
	return NULL;
}

int SortedList_length_mutex(SortedList_t *list)
{
	
	int count = 0;
	
	// Check list corruption for the head
	if (list->prev!=NULL) 
	{
		return -1; 
	}
	
	SortedListElement_t* previous_iteration = NULL;
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Check list corruption
		if (iteration->prev!=previous_iteration) 
		{
			return -1;
		}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();
		
		if (previous_iteration!=NULL)
			if (previous_iteration->next!=iteration) 
			{
				return -1;
			}
		
		// iterate
		previous_iteration = iteration;
		iteration = iteration->next;
		
		count++;
	}
	
	// Check list corruption for the last iteration
	if (previous_iteration->next!=NULL) 
	{
		return -1; 
	}
	
	// Excluding list head
	return count-1;
}

/***************************** TEST-AND-SET *****************************/
int* flag_s;
void SortedList_tas_init(int count)
{
	flag_s = malloc(sizeof(int)*count);
	if (flag_s==NULL)
	{
		printf("malloc failed\n");
		exit(-1);
	}
	for (int i=0;i<count;i++)
	{
		flag_s[i]=0;
	}
}

void lock_s(int index) 
{
	while (__sync_lock_test_and_set (&flag_s[index], 1) == 1)
		; // spin-wait (do nothing)
}

void unlock_s(int index) 
{
	flag_s[index]=0;
}

void SortedList_insert_tas(SortedList_t *list, SortedListElement_t *element)
{
	
	// Empty list, just insert
	if (list->next==NULL)
	{	
		list->next = element;
		
		if (opt_yield & INSERT_YIELD)
			pthread_yield();
		
		element->prev = list;
		return;
	}

	// Not empty list, iterate
	// Algorithm: Insert if element->key before if it is smaller than current key
	SortedListElement_t* iteration = list->next;
	while (1)
	{
		if (strcmp(element->key,iteration->key)<=0)
		{
			// Insert
			element->next = iteration;
			
			if (opt_yield & INSERT_YIELD)
				pthread_yield();
		
			element->prev = iteration->prev;
			iteration->prev->next = element;
			iteration->prev = element;
			return;
		}
		
		// If this is the last iteration, element is simply the largest one, insert at the end
		if (iteration->next==NULL)
		{
			element->prev = iteration;
			
			if (opt_yield & INSERT_YIELD)
				pthread_yield();
			
			element->next = NULL;
			iteration->next = element;
			
			return;
		}
		
		// iterate
		iteration = iteration->next;
	}
}

int SortedList_delete_tas( SortedListElement_t *element)
{
	
	// Check prev/next corruption
	if (element->next!=NULL)
	{
		if (element->next->prev!=element) 
		{
			return 1;
		}
	}
	if (element->prev!=NULL)
	{
		if (element->prev->next!=element) 
		{
			return 1;
		}
	}
	
	if (element->next!=NULL) element->next->prev = element->prev;
	
	if (opt_yield & DELETE_YIELD)
		pthread_yield();
			
	if (element->prev!=NULL) element->prev->next = element->next;
	free(element);
	
	return 0;
}

SortedListElement_t *SortedList_lookup_tas(SortedList_t *list, const char *key)
{
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Found key, return
		if (iteration->key!=NULL)
			if (strcmp(iteration->key,key)==0)
			{
				return iteration;
			}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();	
		
		// iterate
		iteration = iteration->next;
	}
	
	return NULL;
}

int SortedList_length_tas(SortedList_t *list)
{
	int count = 0;
	
	// Check list corruption for the head
	if (list->prev!=NULL) 
	{
		return -1; 
	}
	
	SortedListElement_t* previous_iteration = NULL;
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Check list corruption
		if (iteration->prev!=previous_iteration) 
		{
			return -1;
		}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();
		
		if (previous_iteration!=NULL)
			if (previous_iteration->next!=iteration) 
			{
				return -1;
			}
		
		// iterate
		previous_iteration = iteration;
		iteration = iteration->next;
		
		count++;
	}
	
	// Check list corruption for the last iteration
	if (previous_iteration->next!=NULL) 
	{
		return -1; 
	}
	
	// Excluding list head
	return count-1;
}