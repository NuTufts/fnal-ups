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
 * ups_list_new
 *
 * Will create a new ups list and add first item.
 *
 * Input : void *, a list item
 * Output: none
 * Return: t_ups_list_item *, pointer to top of list or NULL
 */
t_ups_list_item *ups_list_new( void *data_ptr )
{
  t_ups_list_item *l_top = NULL;

  l_top = (t_ups_list_item *)malloc( sizeof( t_ups_list_item ) );

  if ( !l_top ) return NULL;
  
  l_top->data = data_ptr;
  l_top->prev = NULL;
  l_top->next = NULL;

  return( l_top );
}	

/*-----------------------------------------------------------------------
 * ups_list_free
 *
 * Will free up all memory for ups list.
 * If option 'd' is passed the data elements are also freed.
 *
 * Input : t_ups_list_item *, pointer to a list.
 *         char, copt = 'd', will also delete data elements.
 * Output: none
 * Return: t_ups_list_item *, NULL
 */
t_ups_list_item *ups_list_free( t_ups_list_item *list_ptr, char copt )
{
  t_ups_list_item *l_ptr = NULL;
  t_ups_list_item *l_tmp = NULL;
  t_ups_list_item *l_top = ups_list_top( list_ptr );

  if ( !l_top ) return NULL;

  l_ptr = l_top;
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
 * ups_list_insert
 *
 * Will add an item to the top of the list.
 *
 * Input : t_ups_list_item *, pointer to a list.
 * Output: none
 * Return: t_ups_list_item *, pointer to top of list or NULL
 */
t_ups_list_item *ups_list_insert( t_ups_list_item *list_ptr, void *data_ptr )
{
  t_ups_list_item *l_top = NULL;
  t_ups_list_item *l_new = NULL;

  l_top = ups_list_top( list_ptr );

  if ( !l_top ) return NULL;
  
  l_new = (t_ups_list_item *)malloc( sizeof( t_ups_list_item ) );

  if ( !l_new ) return NULL;

  l_new->data = data_ptr;
  l_top->prev = l_new;
  l_new->next = l_top;
  l_new->prev = NULL;

  return l_new;
}

/*-----------------------------------------------------------------------
 * ups_list_add
 *
 * Will add an item to the end of the list.
 *
 * Input : t_ups_list_item *, pointer to a list.
 * Output: none
 * Return: t_ups_list_item *, pointer to top the list or NULL
 */
t_ups_list_item *ups_list_add( t_ups_list_item *list_ptr, void *data_ptr )
{
  t_ups_list_item *l_bot = NULL;
  t_ups_list_item *l_new = NULL;

  l_bot = ups_list_bot( list_ptr );

  if ( !l_bot ) return NULL;
  
  l_new = (t_ups_list_item *)malloc( sizeof( t_ups_list_item ) );

  if ( !l_new ) return NULL;

  l_new->data = data_ptr;
  l_bot->next = l_new;
  l_new->prev = l_bot;
  l_new->next = NULL;

  return ups_list_top( l_new );
}

/*-----------------------------------------------------------------------
 * ups_list_delete
 *
 * Will delete item from list.
 * If option 'd' is passed the data elements are also freed.
 *
 * Input : t_ups_list_item *, pointer to a list
 *         void *, data element of item to be deletet.
 *         char, copt = 'd', will also delete data elements.
 * Output: none
 * Return: t_ups_list_item *, pointer to top of list
 */
t_ups_list_item *ups_list_delete( t_ups_list_item *list_ptr, void *data_ptr, char copt )
{
  t_ups_list_item *l_ptr = NULL;
  t_ups_list_item *l_prev = NULL;
  t_ups_list_item *l_top = NULL;

  l_top = ups_list_top( list_ptr );
  
  for ( l_ptr = l_top; l_ptr; l_ptr = l_ptr->next ) {
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
      if ( l_ptr == l_top ) {
	l_top = l_ptr->next;
      }
      free( l_ptr );
      return l_top;
    }
  }

  /* item was not found */
  return NULL;
}

/*-----------------------------------------------------------------------
 * ups_list_top
 *
 * Will return top item of passed list.
 *
 * Input : t_ups_list_item *, pointer to a list
 * Output: none
 * Return: t_ups_list_item *, pointer to top of list
 */
t_ups_list_item *ups_list_top( t_ups_list_item *list_ptr )
{
  t_ups_list_item *l_ptr = NULL;
  
  if ( !list_ptr ) return NULL;

  for ( l_ptr = list_ptr; l_ptr->prev; l_ptr = l_ptr->prev ) {}

  return l_ptr;
}

/*-----------------------------------------------------------------------
 * ups_list_bot
 *
 * Will return bottom item of passed list.
 *
 * Input : t_ups_list_item *, pointer to a list
 * Output: none
 * Return: t_ups_list_item *, pointer to end of list
 */
t_ups_list_item *ups_list_bot( t_ups_list_item *list_ptr )
{
  t_ups_list_item *l_ptr = NULL;
  
  if ( !list_ptr ) return NULL;

  for ( l_ptr = list_ptr; l_ptr->next; l_ptr = l_ptr->next ) {}

  return l_ptr;
}

/*
 * Definition of private functions.
 */

