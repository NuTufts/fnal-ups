/************************************************************************
 *
 * FILE:
 *       ups_memory.c
 * 
 * DESCRIPTION: 
 *       This file contains routines for memory management
 *
 * AUTHORS:
 *       Eileen Berman
 *       David Fagan
 *       Lars Rasmussen
 *
 *       Fermilab Computing Division
 *       Batavia, Il 60510, U.S.A.
 *
 * MODIFICATIONS:
 *       25-Jul-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <malloc.h>
#include <stdio.h>

/* ups specific include files */
#include "ups_error.h"
#include "ups_list.h"
#include "ups_memory.h"

/*
 * Definition of public variables.
 */

/*
 * Definition of global variables.
 */

/* header attached to each piece of memory requested with umem_malloc */
typedef struct umem_header {
  void *data;
  int  num_bytes;
  int  reference_counter;
} t_umem_header;

/* linked list of allocated memory blocks */
static t_ups_list_item *g_memory_list = 0;

/*
 * Declaration of private functions.
 */

static t_umem_header *find_saved_data(const void * const a_data);

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * umem_malloc
 *
 * Malloc the requested amount and save it on a linked list so we can keep
 * track of reference counts.
 *
 * Input : number of bytes to malloc
 * Output: none
 * Return: address of new memory.  NULL if malloc failed
 */
void *umem_malloc(const int a_bytes)
{
  void *dataPtr = 0;
  t_umem_header *memory;
  int hdr_and_data_bytes;

  /* Return if no memory requested */
  if (a_bytes > 0) {
    hdr_and_data_bytes = a_bytes + (int )sizeof(t_umem_header);
    dataPtr = (void *)malloc((unsigned int)hdr_and_data_bytes);
    if (dataPtr != 0) {
      /* We got the memory, initialize it */
      memory = (t_umem_header *)dataPtr;
      memory->reference_counter = 0;
      memory->data = (void *)(memory + (int )sizeof(t_umem_header));
      memory->num_bytes = a_bytes;
      dataPtr = (void *)memory->data;

      /* Add the memory to the linked list of stuff */
      if (g_memory_list == 0) {
	/* First time through - create the list */
	g_memory_list = ups_list_new((void *)memory);
      } else {
	g_memory_list = ups_list_insert(g_memory_list, (void *)memory);
      }

      /* Now make sure there was not an error mallocing the list element */
      if (g_memory_list == 0) {
	/* Yes there was an error, free what we have */
	free (memory);
	dataPtr = 0;
      }
    }
  }
  return dataPtr;
}

/*-----------------------------------------------------------------------
 * umem_free
 *
 * Free the passed memory if the reference count is zero.
 *
 * Input : address of memory to free
 * Output: none
 * Return: none
 */
void umem_free(void *a_data)
{
  t_umem_header *memory;

  /* See if the passed memory is saved in g_memory_list */
  memory = find_saved_data(a_data);

  if (memory != 0) {
    /* Yes the memory was on the list. check the reference counter.
       if it is <= 0, then free the data and the memory header, else do
       nothing */
    if (memory->reference_counter <= 0) {
      /* okay, remove this item from the list and free it */
      g_memory_list = ups_list_delete(g_memory_list, (void *)memory, ' ');
      free(memory);
      a_data = 0;
    }
  } else {
    /* No the memory was not on the list, free like normal */
    free(a_data);
    a_data = 0;
  }

}

/*-----------------------------------------------------------------------
 * umem_inc_refctr
 *
 * Increment the reference counter associated with the passed data
 *
 * Input : address of data whose reference counter is to be incremented
 * Output: none
 * Return: none
 */
void umem_inc_refctr(const void * const a_data)
{
  t_umem_header *memory;

  /* See if the passed memory is saved in g_memory_list */
  memory = find_saved_data(a_data);

  if (memory != 0) {
    /* Yes the memory was on the list. increment the reference counter. */
    ++(memory->reference_counter);
  }
}

/*-----------------------------------------------------------------------
 * umem_dec_refctr
 *
 * Decrement the reference counter associated with the passed data
 *
 * Input : address of data whose reference counter is to be decremented
 * Output: none
 * Return: none
 */
void umem_dec_refctr(const void * const a_data)
{
  t_umem_header *memory;

  /* See if the passed memory is saved in g_memory_list */
  memory = find_saved_data(a_data);

  if (memory != 0) {
    /* Yes the memory was on the list. decrement the reference counter. */
    --(memory->reference_counter);
  }
}

/*-----------------------------------------------------------------------
 * umem_print
 *
 * Print out g_memory_list.  Used mainly for debugging purposes.
 *
 * Input : none
 * Output: none
 * Return: none
 */
void umem_print(void)
{
  int i;
  t_ups_list_item *tempItem = 0;
  t_umem_header *memItem;

  tempItem = g_memory_list;
  if (tempItem == 0) {
    printf("No memory on the list\n");
  } else {
    for (i = 0; tempItem != 0; ++i) {
      memItem = tempItem->data;
      printf("Num of Bytes = %d, Reference Count = %d\n", memItem->num_bytes,
	     memItem->reference_counter);
      tempItem = tempItem->next;
    }
  }
}

/*
 * Definition of private functions.
 */

/*-----------------------------------------------------------------------
 * find_saved_data
 *
 * Given a pointer to user data, find the memory header list element that
 * corresponds to it
 *
 * Input : data pointer
 * Output: none
 * Return: pointer to the memory list element
 *
 */
static t_umem_header *find_saved_data(const void * const a_data)
{
  t_ups_list_item *list_item;
  t_umem_header *data_item;
  t_umem_header *data_header = 0;

  for (list_item = g_memory_list; list_item; list_item = list_item->next) {
    data_item = (t_umem_header *)list_item->data;
    if ((void *)data_item->data == a_data) {
      /* we found the match get out */
      data_header = data_item;
      break;
    }
  }
  return data_header;
}
