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
#include "ups_error.h"

/*
 * Definition of public variables
 */

/*
 * Declaration of private functions
 */
static int all_gone( const void * const ptr );

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

  if ( prod_ptr ) {
    memset( prod_ptr, 0, sizeof( t_ups_product ) );
  }
  else {
    upserr_vplace();
    upserr_add( UPS_NO_MEMORY, UPS_FATAL, sizeof( t_ups_product ) );
  }
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
int ups_free_product( t_ups_product * const prod_ptr )
{
  t_upslst_item *l_ptr;
  t_ups_instance *inst_ptr;

  if ( !prod_ptr ) return 0;

  if ( all_gone( prod_ptr ) ) {
    if ( prod_ptr->file ) { upsmem_free( prod_ptr->file ); }
    if ( prod_ptr->product ) { upsmem_free( prod_ptr->product); }
    if ( prod_ptr->chaver ) { upsmem_free( prod_ptr->chaver ); }
    if ( prod_ptr->ups_db_version ) { upsmem_free( prod_ptr->ups_db_version ); }

    /* get rid of instances */
    
    l_ptr = upslst_first( prod_ptr->instance_list );
    while( l_ptr ) {
      inst_ptr = l_ptr->data;
      l_ptr = upslst_delete( l_ptr, inst_ptr, ' ' );
      ups_free_instance( inst_ptr );
    }

    /* get rid of comments */

    upslst_free( prod_ptr->comment_list, 'd' );

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
  
  if ( inst_ptr ) {
    memset( inst_ptr, 0, sizeof( t_ups_instance ) );
  }
  else {
    upserr_vplace();
    upserr_add( UPS_NO_MEMORY, UPS_FATAL, sizeof( t_ups_instance ) );
  }
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
int ups_free_instance( t_ups_instance * const inst_ptr )
{
  t_upslst_item *l_ptr;
  void *act_ptr;

  if ( !inst_ptr ) return 0;

  if ( all_gone( inst_ptr ) ) {
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
    if ( inst_ptr->table_file ) { upsmem_free( inst_ptr->table_file ); }
    if ( inst_ptr->archive_file ) { upsmem_free( inst_ptr->archive_file ); }
    if ( inst_ptr->authorized_nodes ) { upsmem_free( inst_ptr->authorized_nodes ); }
    if ( inst_ptr->description ) { upsmem_free( inst_ptr->description ); }
    
    if ( inst_ptr->statistics ) { upsmem_free( inst_ptr->statistics ); }
    
    if ( inst_ptr->db_dir ) { upsmem_free( inst_ptr->db_dir ); }
  
    if ( inst_ptr->unknown_list ) { upslst_free( inst_ptr->unknown_list, 'd' ); }
    
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
  
  if ( act_ptr ) {
    memset( act_ptr, 0, sizeof( t_ups_action ) );
  }
  else {
    upserr_vplace();
    upserr_add( UPS_NO_MEMORY, UPS_FATAL, sizeof( t_ups_action ) );
  }
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
int ups_free_action( t_ups_action * const act_ptr )
{
  if ( !act_ptr ) return 0;

  if ( all_gone( act_ptr ) ) {
    if ( act_ptr->action ) { upsmem_free( act_ptr->action ); }
    if ( act_ptr->command_list ) { upslst_free( act_ptr->command_list, 'd' ); }

    upsmem_free( act_ptr );
  }

  return 1;
}

/*-----------------------------------------------------------------------
 * ups_new_mp
 *
 * Return an initialized matched product structure.
 *
 * Input : db name, list of chain instances, list of version intances,
 *         list of table intsances, list of matched flavors, 
 *         list of matched qualifiers
 * Output: none
 * Return: pointer to matched product structure, NULL if error.
 */
t_ups_match_product *ups_new_mp(const char * const a_db,
				t_upslst_item * const a_chain_list,
				t_upslst_item * const a_vers_list,
				t_upslst_item * const a_table_list)
{
  t_ups_match_product *mproduct;
  
  mproduct = (t_ups_match_product *)upsmem_malloc(sizeof(t_ups_match_product));
  if (mproduct != NULL) {
    upsmem_inc_refctr(a_db);      /* don't free db till we no longer need it */
    mproduct->db = (char *)a_db;
    mproduct->chain_list = a_chain_list;
    mproduct->version_list = a_vers_list;
    mproduct->table_list = a_table_list;
  } else {
    upserr_vplace();
    upserr_add(UPS_NO_MEMORY, UPS_FATAL, sizeof(t_ups_match_product));
  }

  return mproduct;
}

/*-----------------------------------------------------------------------
 * ups_free_mp
 *
 * Free a matched product structure.
 *
 * Input : pointer to matched product structure
 * Output: none
 * Return: NULL
 */
t_ups_match_product *ups_free_mp(t_ups_match_product *a_mproduct)
{
  if (! a_mproduct) {
    /* we incremented the ref counter in the ups_new_mp function, doing a
       free here will decrement it or free it */
    if (all_gone(a_mproduct)) {
      upsmem_free(a_mproduct->db);
    }
    upsmem_free((void *)a_mproduct);
  }

  return NULL;
}

/*
 * Definition of private functions
 */

int all_gone( const void * const ptr )
{
  return ( upsmem_get_refctr( ptr ) <= 0 );
}
