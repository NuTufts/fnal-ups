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

#ifndef _UPS_LIST_H_
#define _UPS_LIST_H_

/* standard include files, if needed for .h file */

/* ups specific include files, if needed for .h file */

/*
 * Constans.
 */


/*
 * Types.
 */
typedef struct ups_list_item
{
  struct ups_list_item    *prev;
  void                    *data;
  struct ups_list_item    *next;
} t_ups_list_item;

/*
 * Declaration of public functions.
 */
t_ups_list_item *ups_list_new( void *data_ptr );
t_ups_list_item *ups_list_free( t_ups_list_item *list_ptr, char copt );
t_ups_list_item *ups_list_insert( t_ups_list_item *list_ptr, void *data_ptr );
t_ups_list_item *ups_list_add( t_ups_list_item *list_ptr, void *data_ptr );
t_ups_list_item *ups_list_delete( t_ups_list_item *list_ptr, void *data_ptr, char copt );

/*
 * Declaration of private globals.
 */

/*
 * Declarations of public variables.
 */

#endif /* _UPS_LIST_H_ */






