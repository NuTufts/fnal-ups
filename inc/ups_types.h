/************************************************************************
 *
 * FILE:
 *       ups_types.h
 * 
 * DESCRIPTION: 
 *       Common types for ups
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
 *       22-jul-1997, LR, first, very preliminary
 *
 ***********************************************************************/

#ifndef _UPS_TYPES_H_
#define _UPS_TYPES_H_

/*-----------------------------------------------------------------------
 * Standard include files, if needed for .h file
 */

/*-----------------------------------------------------------------------
 * ups specific include files, if needed for .h file
 */

/*-----------------------------------------------------------------------
 * Public typdef's
 */

/*
 * a list, still need a 'nice' interface (add, delete, clear, etc).
 * which probaly will justify a 'ups_list.c' and 'ups_list.h'.
 */
typedef struct ups_list_item
{
  struct ups_list_item    *prev;
  void                    *data;
  struct ups_list_item    *next;
} t_ups_list_item;

/* a 'full' product, not sure if that need to be public */
typedef struct ups_product
{
  char             *name;
  t_ups_list_item  *instance_list;
  t_ups_list_item  *chain_list;
  t_ups_list_item  *action_list;
} t_ups_product;

/* a desciptor for an ups file, chaver can be product version or chain name */
typedef struct ups_file_desc
{
  char             *file;
  char             *product;
  char             *chaver;
} t_ups_file_desc;

/* a product instance */
typedef struct ups_instance
{
  char             *product;
  char             *version;
  char             *flavor;
  char             *qualifiers;
  
  char             *declarer;
  char             *declared;
  char             *prod_dir;
  char             *ups_dir;
  char             *table_dir;
  char             *table_file;
  char             *archive_file;
  char             *authorized_nodes;
  char             *description;
  t_ups_list_item  *unknown;
} t_ups_instance;

/* a chain */
typedef struct ups_chain
{
  char             *chain;
  char             *prod_name;
  char             *prod_version;
  char             *prod_qualifiers;
  char             *declarer;
  char             *declared;

  t_ups_instance   *instance;
} t_ups_chain;

/* an action */
typedef struct ups_action
{
  char             *action;
  char             *prod_name;
  char             *prod_version;
  char             *prod_qualifiers;
    
  t_ups_list_item  *action_list;

  t_ups_instance   *instance;
} t_ups_action;

#endif /* _UPS_TYPES_H_ */










