/************************************************************************
 *
 * FILE:
 *       ups_list.c
 * 
 * DESCRIPTION: 
 *       A double linked list (ideas from old ups_list).
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
 *       23-Jun-1997, LR, first
 *       25-Jul-1997, LR, upslst_add is now as fast as upslst_insert:
 *                        upslst_add will now return the last element.
 *                        upslst_insert will return the first element.
 *                        In that way, successive calls using previous
 *                        return will be fast.  
 *                        Changed 'bot' to 'last' and 'top' to 'first'.
 *
 ***********************************************************************/

/* standard include files */
#include <stdlib.h>

/* ups specific include files */
#include "../inc/ups_list.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */

/*
 * Definition of global variables.
 */

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upslst_new
 *
 * Will create a new ups list and add first item.
 *
 * Input : void *, a data element
 * Output: none
 * Return: t_upslst_item *, pointer to top of list or NULL
 */
t_upslst_item *upslst_new( void *data_ptr )
{
  t_upslst_item *l_first = NULL;

  l_first = (t_upslst_item *)malloc( sizeof( t_upslst_item ) );

  if ( !l_first ) return NULL;
  
  l_first->data = data_ptr;
  l_first->prev = NULL;
  l_first->next = NULL;

  return( l_first );
}	

/*-----------------------------------------------------------------------
 * upslst_free
 *
 * Will free up all memory for ups list.
 * If option 'd' is passed the data elements are also freed.
 *
 * Input : t_upslst_item *, pointer to a list.
 *         char, copt = 'd', will also delete data elements.
 * Output: none
 * Return: t_upslst_item *, NULL
 */
t_upslst_item *upslst_free( t_upslst_item *list_ptr, char copt )
{
  t_upslst_item *l_ptr = NULL;
  t_upslst_item *l_tmp = NULL;
  t_upslst_item *l_first = upslst_first( list_ptr );

  if ( !l_first ) return NULL;

  l_ptr = l_first;
  while( l_ptr ) {
    if ( copt == 'd' && l_ptr->data ) free ( l_ptr->data );
    l_tmp = l_ptr;
    l_ptr = l_ptr->next;
    if ( l_ptr ) l_ptr->prev = NULL;
    free( l_tmp );
  }

  return NULL;
}

/*-----------------------------------------------------------------------
 * upslst_insert
 *
 * Will add an item as the first item to the list.
 *
 * Input : t_upslst_item *, pointer to a list, if NULL a new list will
 *         be created.
 * Output: none
 * Return: t_upslst_item *, pointer to the first item of the list or NULL
 *
 * NOTE: if passed list pointer, in successive calls, is the first item,
 * it should be pretty fast.
 */
t_upslst_item *upslst_insert( t_upslst_item *list_ptr, void *data_ptr )
{
  t_upslst_item *l_first = NULL;
  t_upslst_item *l_new = NULL;

  if ( ! list_ptr ) {
    list_ptr = upslst_new( data_ptr );
    return list_ptr;
  }

  l_first = upslst_first( list_ptr );

  if ( !l_first ) return NULL;
  
  l_new = (t_upslst_item *)malloc( sizeof( t_upslst_item ) );

  if ( !l_new ) return NULL;

  l_new->data = data_ptr;
  l_first->prev = l_new;
  l_new->next = l_first;
  l_new->prev = NULL;

  return l_new;
}

/*-----------------------------------------------------------------------
 * upslst_add
 *
 * Will add an item as the last item to the list.
 *
 * Input : t_upslst_item *, pointer to a list, if NULL a new list will
 *         be created.
 * Output: none
 * Return: t_upslst_item *, pointer to the last item of the list or NULL
 *
 * NOTE: if passed list pointer, in successive calls, is the last item,
 * it should be pretty fast.
 */
t_upslst_item *upslst_add( t_upslst_item *list_ptr, void *data_ptr )
{
  t_upslst_item *l_last = NULL;
  t_upslst_item *l_new = NULL;

  if ( ! list_ptr ) {
    list_ptr = upslst_new( data_ptr );
    return list_ptr;
  }
  
  l_last = upslst_last( list_ptr );

  if ( !l_last ) return NULL;
  
  l_new = (t_upslst_item *)malloc( sizeof( t_upslst_item ) );

  if ( !l_new ) return NULL;

  l_new->data = data_ptr;
  l_last->next = l_new;
  l_new->prev = l_last;
  l_new->next = NULL;

  return l_new;
}

/*-----------------------------------------------------------------------
 * upslst_delete
 *
 * Will delete item from list.
 * If option 'd' is passed the data elements are also freed.
 *
 * Input : t_upslst_item *, pointer to a list
 *         void *, data element of item to be deletet.
 *         char, copt = 'd', will also delete data elements.
 * Output: none
 * Return: t_upslst_item *, pointer to top of list
 */
t_upslst_item *upslst_delete( t_upslst_item *list_ptr, void *data_ptr, char copt )
{
  t_upslst_item *l_ptr = NULL;
  t_upslst_item *l_prev = NULL;
  t_upslst_item *l_first = NULL;

  l_first = upslst_first( list_ptr );
  
  for ( l_ptr = l_first; l_ptr; l_ptr = l_ptr->next ) {
    l_prev = l_ptr->prev;

    if ( l_ptr->data == data_ptr ) {
      if ( l_ptr->next ) {
	l_ptr->next->prev = l_prev;
	if ( l_prev ) l_prev->next = l_ptr->next;
      }
      else {
	if ( l_prev ) l_prev->next = NULL;
      }
      if ( copt == 'd' && l_ptr->data ) free( l_ptr->data );
      if ( l_ptr == l_first ) {
	l_first = l_ptr->next;
      }
      free( l_ptr );
      return l_first;
    }
  }

  /* item was not found */
  return NULL;
}

/*-----------------------------------------------------------------------
 * upslst_first
 *
 * Will return first item of passed list.
 *
 * Input : t_upslst_item *, pointer to a list
 * Output: none
 * Return: t_upslst_item *, pointer to first item of list
 */
t_upslst_item *upslst_first( t_upslst_item *list_ptr )
{
  t_upslst_item *l_ptr = NULL;
  
  if ( !list_ptr ) return NULL;

  for ( l_ptr = list_ptr; l_ptr->prev; l_ptr = l_ptr->prev ) {}

  return l_ptr;
}

/*-----------------------------------------------------------------------
 * upslst_last
 *
 * Will return last item of passed list.
 *
 * Input : t_upslst_item *, pointer to a list
 * Output: none
 * Return: t_upslst_item *, pointer to last item of list
 */
t_upslst_item *upslst_last( t_upslst_item *list_ptr )
{
  t_upslst_item *l_ptr = NULL;
  
  if ( !list_ptr ) return NULL;

  for ( l_ptr = list_ptr; l_ptr->next; l_ptr = l_ptr->next ) {}

  return l_ptr;
}

/*
 * Definition of private functions.
 */









