/************************************************************************
 *
 * FILE:
 *       ups_types.c
 * 
 * DESCRIPTION: 
 *       Creation and destuction of common types for ups.
 *       Specially for types there is using upsmem for reference
 *       counting.
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
 *       29-jul-1997, LR, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdlib.h>
#include <string.h>                       /* for memset */

/* ups specific include files */
#include "../inc/ups_types.h"
#include "../inc/ups_memory.h"
#include "../inc/ups_list.h"

/*
 * Definition of public variables
 */

/*
 * Declaration of private functions
 */
  
/*
 * Definition of public functions
 */

/*-----------------------------------------------------------------------
 * ups_new_product
 *
 * Will create an empty t_ups_product structure.
 *
 * Input : none
 * Output: none
 * Return: t_ups_product *, a pointer to a product structure
 */
t_ups_product *ups_new_product( void )
{  
  t_ups_product *prod_ptr =
    (t_ups_product *)upsmem_malloc( sizeof( t_ups_product ) );
  
  memset( prod_ptr, 0, sizeof( t_ups_product ) );
  return prod_ptr;
}

/*-----------------------------------------------------------------------
 * ups_free_product
 *
 * Will free a product.
 *
 * Input : t_ups_product *, a pointer to a product structure.
 * Output: none
 * Return: int, 0 on failer else 1.
 */
int ups_free_product( t_ups_product *prod_ptr )
{
  t_upslst_item *l_ptr;

  if ( !prod_ptr ) return 0;

  if ( prod_ptr->file ) { free( prod_ptr->file ); }
  if ( prod_ptr->product ) { free( prod_ptr->product); }
  if ( prod_ptr->chaver ) { free( prod_ptr->chaver ); }
  
  for ( l_ptr = upslst_first( prod_ptr->instance_list ); l_ptr; l_ptr->next ) {
    ups_free_action( l_ptr->data );
  }
  upslst_free( prod_ptr->instance_list, ' ' );

  upsmem_free( prod_ptr );
  
  return 1;
}

/*-----------------------------------------------------------------------
 * ups_new_instance
 *
 * Will create an empty t_ups_instance structure.
 *
 * Input : none
 * Output: none
 * Return: t_ups_instance *, a pointer to an instance structure
 */
t_ups_instance *ups_new_instance( void )
{
  t_ups_instance *inst_ptr =
    (t_ups_instance *)upsmem_malloc( sizeof( t_ups_instance ) );
  
  memset( inst_ptr, 0, sizeof( t_ups_instance ) );
  return inst_ptr;
}

/*-----------------------------------------------------------------------
 * ups_free_instance
 *
 * Will free an instance.
 *
 * Input : t_ups_instance *, a pointer to an instance structure.
 * Output: none
 * Return: int, 0 on failer else 1.
 */
int ups_free_instance( t_ups_instance *inst_ptr )
{
  t_upslst_item *l_ptr;

  if ( !inst_ptr ) return 0;

  if ( inst_ptr->product ) { free( inst_ptr->product); }
  if ( inst_ptr->version ) { free( inst_ptr->version ); }
  if ( inst_ptr->flavor ) { free( inst_ptr->flavor ); }
  if ( inst_ptr->qualifiers ) { free( inst_ptr->qualifiers ); }
  
  if ( inst_ptr->chain ) { free( inst_ptr->chain ); }
  if ( inst_ptr->chain_declarer ) { free( inst_ptr->chain_declarer ); }
  if ( inst_ptr->chain_declared ) { free( inst_ptr->chain_declared ); }

  if ( inst_ptr->declarer ) { free( inst_ptr->declarer ); }
  if ( inst_ptr->declared ) { free( inst_ptr->declared ); }
  if ( inst_ptr->prod_dir ) { free( inst_ptr->prod_dir ); }
  if ( inst_ptr->ups_dir ) { free( inst_ptr->ups_dir ); }
  if ( inst_ptr->table_dir ) { free( inst_ptr->table_dir ); }
  if ( inst_ptr->archive_file ) { free( inst_ptr->archive_file ); }
  if ( inst_ptr->authorized_nodes ) { free( inst_ptr->authorized_nodes ); }
  if ( inst_ptr->description ) { free( inst_ptr->description ); }
  
  if ( inst_ptr->unknown_list ) upslst_free( inst_ptr->unknown_list, 'd' );
  
  for ( l_ptr = upslst_first( inst_ptr->action_list ); l_ptr; l_ptr->next ) {
    ups_free_action( l_ptr->data );
  }
  upslst_free( inst_ptr->action_list, ' ' );

  upsmem_free ( inst_ptr );

  return 1;
}

/*-----------------------------------------------------------------------
 * ups_new_action
 *
 * Will create an empty t_ups_action structure
 *
 * Input : none
 * Output: none
 * Return: t_ups_action *, a pointer to an actions instance structure
 */
t_ups_action *ups_new_action( void )
{
  t_ups_action *act_ptr =
    (t_ups_action *)upsmem_malloc( sizeof( t_ups_action ) );
  
  memset( act_ptr, 0, sizeof( t_ups_action ) );
  return act_ptr;
}

/*-----------------------------------------------------------------------
 * ups_free_action
 *
 * Will free an action.
 *
 * Input : t_ups_action *, a pointer to an action structure.
 * Output: none
 * Return: int, 0 on failer else 1.
 */
int ups_free_action( t_ups_action *act_ptr )
{
  if ( !act_ptr ) return 0;

  if ( act_ptr->action ) { free( act_ptr->action ); }
  if ( act_ptr->command_list ) { upslst_free( act_ptr->command_list, 'd' ); }

  upsmem_free( act_ptr );

  return 1;
}
