#define _GNU_SOURCE
#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

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

void SortedList_insert_mutex(SortedList_t *list, SortedListElement_t *element)
{
	pthread_mutex_lock(&lock_m);
	
	// Empty list, just insert
	if (list->next==NULL)
	{	
		list->next = element;
		
		if (opt_yield & INSERT_YIELD)
			pthread_yield();
		
		element->prev = list;
		
		pthread_mutex_unlock(&lock_m);
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
			
			pthread_mutex_unlock(&lock_m);
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
			
			pthread_mutex_unlock(&lock_m);
			return;
		}
		
		// iterate
		iteration = iteration->next;
	}
}

int SortedList_delete_mutex( SortedListElement_t *element)
{
	pthread_mutex_lock(&lock_m);
	
	// Check prev/next corruption
	if (element->next!=NULL)
	{
		if (element->next->prev!=element) 
		{
			pthread_mutex_unlock(&lock_m);
			return 1;
		}
	}
	if (element->prev!=NULL)
	{
		if (element->prev->next!=element) 
		{
			pthread_mutex_unlock(&lock_m);
			return 1;
		}
	}
	
	if (element->next!=NULL) element->next->prev = element->prev;
	
	if (opt_yield & DELETE_YIELD)
		pthread_yield();
			
	if (element->prev!=NULL) element->prev->next = element->next;
	free(element);
	
	pthread_mutex_unlock(&lock_m);
	return 0;
}

SortedListElement_t *SortedList_lookup_mutex(SortedList_t *list, const char *key)
{
	pthread_mutex_lock(&lock_m);
	
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Found key, return
		if (iteration->key!=NULL)
			if (strcmp(iteration->key,key)==0)
			{
				pthread_mutex_unlock(&lock_m);
				return iteration;
			}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();	
		
		// iterate
		iteration = iteration->next;
	}
	
	pthread_mutex_unlock(&lock_m);
	return NULL;
}

int SortedList_length_mutex(SortedList_t *list)
{
	pthread_mutex_lock(&lock_m);
	int count = 0;
	
	// Check list corruption for the head
	if (list->prev!=NULL) 
	{
		pthread_mutex_unlock(&lock_m);
		return -1; 
	}
	
	SortedListElement_t* previous_iteration = NULL;
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Check list corruption
		if (iteration->prev!=previous_iteration) 
		{
			pthread_mutex_unlock(&lock_m);
			return -1;
		}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();
		
		if (previous_iteration!=NULL)
			if (previous_iteration->next!=iteration) 
			{
				pthread_mutex_unlock(&lock_m);
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
		pthread_mutex_unlock(&lock_m);
		return -1; 
	}
	
	// Excluding list head
	pthread_mutex_unlock(&lock_m);
	return count-1;
}

/***************************** TEST-AND-SET *****************************/
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

void SortedList_insert_tas(SortedList_t *list, SortedListElement_t *element)
{
	lock_s();
	
	// Empty list, just insert
	if (list->next==NULL)
	{	
		list->next = element;
		
		if (opt_yield & INSERT_YIELD)
			pthread_yield();
		
		element->prev = list;
		
		unlock_s();
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
			
			unlock_s();
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
			
			unlock_s();
			return;
		}
		
		// iterate
		iteration = iteration->next;
	}
}

int SortedList_delete_tas( SortedListElement_t *element)
{
	lock_s();
	
	// Check prev/next corruption
	if (element->next!=NULL)
	{
		if (element->next->prev!=element) 
		{
			unlock_s();
			return 1;
		}
	}
	if (element->prev!=NULL)
	{
		if (element->prev->next!=element) 
		{
			unlock_s();
			return 1;
		}
	}
	
	if (element->next!=NULL) element->next->prev = element->prev;
	
	if (opt_yield & DELETE_YIELD)
		pthread_yield();
			
	if (element->prev!=NULL) element->prev->next = element->next;
	free(element);
	
	unlock_s();
	return 0;
}

SortedListElement_t *SortedList_lookup_tas(SortedList_t *list, const char *key)
{
	lock_s();
	
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Found key, return
		if (iteration->key!=NULL)
			if (strcmp(iteration->key,key)==0)
			{
				unlock_s();
				return iteration;
			}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();	
		
		// iterate
		iteration = iteration->next;
	}
	
	unlock_s();
	return NULL;
}

int SortedList_length_tas(SortedList_t *list)
{
	lock_s();
	int count = 0;
	
	// Check list corruption for the head
	if (list->prev!=NULL) 
	{
		unlock_s();
		return -1; 
	}
	
	SortedListElement_t* previous_iteration = NULL;
	SortedListElement_t* iteration = list;
	while (iteration!=NULL)
	{
		// Check list corruption
		if (iteration->prev!=previous_iteration) 
		{
			unlock_s();
			return -1;
		}
		
		if (opt_yield & SEARCH_YIELD)
			pthread_yield();
		
		if (previous_iteration!=NULL)
			if (previous_iteration->next!=iteration) 
			{
				unlock_s();
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
		unlock_s();
		return -1; 
	}
	
	// Excluding list head
	unlock_s();
	return count-1;
}