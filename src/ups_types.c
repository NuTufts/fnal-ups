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
 *       30-jul-1997, LR, Using ups_memory (reference counting).
 *
 ***********************************************************************/

/* standard include files */
#include <stdlib.h>
#include <string.h> /* for memset */

/* ups specific include files */
#include "ups_types.h"
#include "ups_memory.h"
#include "ups_list.h"

/*
 * Definition of public variables
 */

/*
 * Declaration of private functions
 */
static int all_gone( void *ptr );

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
  void *inst_ptr;

  if ( !prod_ptr ) return 0;

  if ( all_gone( prod_ptr ) ) {
    printf( "Freeing Product\n" );
    if ( prod_ptr->file ) { upsmem_free( prod_ptr->file ); }
    if ( prod_ptr->product ) { upsmem_free( prod_ptr->product); }
    if ( prod_ptr->chaver ) { upsmem_free( prod_ptr->chaver ); }
    
    l_ptr = upslst_first( prod_ptr->instance_list );
    while( l_ptr ) {
      inst_ptr = l_ptr->data;
      l_ptr = upslst_delete( l_ptr, inst_ptr, ' ' );
      ups_free_instance( inst_ptr );
    }

    upsmem_free( prod_ptr );
  }
  
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
  void *act_ptr;

  if ( !inst_ptr ) return 0;

  if ( all_gone( inst_ptr ) ) {
    printf( "Freeing Instance\n" );
    if ( inst_ptr->product ) { upsmem_free( inst_ptr->product); }
    if ( inst_ptr->version ) { upsmem_free( inst_ptr->version ); }
    if ( inst_ptr->flavor ) { upsmem_free( inst_ptr->flavor ); }
    if ( inst_ptr->qualifiers ) { upsmem_free( inst_ptr->qualifiers ); }
  
    if ( inst_ptr->chain ) { upsmem_free( inst_ptr->chain ); }
    if ( inst_ptr->chain_declarer ) { upsmem_free( inst_ptr->chain_declarer ); }
    if ( inst_ptr->chain_declared ) { upsmem_free( inst_ptr->chain_declared ); }

    if ( inst_ptr->declarer ) { upsmem_free( inst_ptr->declarer ); }
    if ( inst_ptr->declared ) { upsmem_free( inst_ptr->declared ); }
    if ( inst_ptr->prod_dir ) { upsmem_free( inst_ptr->prod_dir ); }
    if ( inst_ptr->ups_dir ) { upsmem_free( inst_ptr->ups_dir ); }
    if ( inst_ptr->table_dir ) { upsmem_free( inst_ptr->table_dir ); }
    if ( inst_ptr->archive_file ) { upsmem_free( inst_ptr->archive_file ); }
    if ( inst_ptr->authorized_nodes ) { upsmem_free( inst_ptr->authorized_nodes ); }
    if ( inst_ptr->description ) { upsmem_free( inst_ptr->description ); }
  
    if ( inst_ptr->unknown_list ) upslst_free( inst_ptr->unknown_list, 'd' );
    
    l_ptr = upslst_first( inst_ptr->action_list );
    while( l_ptr ) {
      act_ptr = l_ptr->data;
      l_ptr = upslst_delete( l_ptr, act_ptr, ' ' );
      ups_free_action( act_ptr );
    }

    upsmem_free ( inst_ptr );
  }

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

  if ( all_gone( act_ptr ) ) {
    printf( "Freeing Action\n" );
    if ( act_ptr->action ) { upsmem_free( act_ptr->action ); }
    if ( act_ptr->command_list ) { upslst_free( act_ptr->command_list, 'd' ); }

    upsmem_free( act_ptr );
  }

  return 1;
}

/*
 * Definition of private functions
 */

int all_gone( void *ptr )
{
  return ( upsmem_get_refctr( ptr ) <= 0 );
}







