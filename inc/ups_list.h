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
typedef struct upslst_item
{
  struct upslst_item    *prev;
  void                  *data;
  struct upslst_item    *next;
} t_upslst_item;

/*
 * Declaration of public functions.
 */
t_upslst_item *upslst_new( void *data_ptr );
t_upslst_item *upslst_free( t_upslst_item *list_ptr, char copt );
t_upslst_item *upslst_insert( t_upslst_item *list_ptr, void *data_ptr );
t_upslst_item *upslst_add( t_upslst_item *list_ptr, void *data_ptr );
t_upslst_item *upslst_delete( t_upslst_item *list_ptr, void *data_ptr, char copt );

t_upslst_item *upslst_insert_list( t_upslst_item *list_ptr, t_upslst_item *list_new );
t_upslst_item *upslst_add_list( t_upslst_item *list_ptr, t_upslst_item *list_new );

t_upslst_item *upslst_first( t_upslst_item *list_ptr );
t_upslst_item *upslst_last( t_upslst_item *list_ptr );

/*
 * Declaration of private globals.
 */

/*
 * Declarations of public variables.
 */

#endif /* _UPS_LIST_H_ */






