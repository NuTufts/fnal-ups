#include <stdio.h>

#include "../inc/ups_list.h"

void print_list( t_ups_list_item *list_ptr );

void main( void )
{
  t_ups_list_item *l_ptr;
  
  /*
   * Note:
   *
   * Calls to ups_list_add, will add items as last item in the list
   * and return a pointer to last item.
   *
   * Calls to ups_list_insert, will add items as first item in the list
   * and return a pointer to first item.
   *
   * In that way successive calls using previous return will be fast.  
   *
   */
  
  printf( "\nCreating a list:\n" );
  l_ptr = ups_list_new( 0 );
  print_list( l_ptr );
  
  printf( "\nAdding as last 1\n" );
  l_ptr = ups_list_add( l_ptr, (void*)1 );
  print_list( l_ptr );
  
  printf( "\nAdding as last 2\n" );
  l_ptr = ups_list_add( l_ptr, (void*)2 );
  print_list( l_ptr );
  
  printf( "\nInserting as first !!! 3\n" );
  l_ptr = ups_list_insert( l_ptr, (void*)3 );
  print_list( l_ptr );
  
  printf( "\nAdding as last 4\n" );
  l_ptr = ups_list_add( l_ptr, (void*)4 );
  print_list( l_ptr );

  /*
  printf( "Deleting the list:\n" );
  l_ptr = ups_list_free( l_ptr, ' ' );
  print_list( l_ptr );
  */
  
  printf( "\nDeleting 1\n" );
  l_ptr = ups_list_delete( l_ptr, (void*)1, ' ' );
  print_list( l_ptr );
  
  printf( "\nDeleting 2\n" );
  l_ptr = ups_list_delete( l_ptr, (void*)2, ' ' );
  print_list( l_ptr );
  
  printf( "\nDeleting 3\n" );
  l_ptr = ups_list_delete( l_ptr, (void*)3, ' ' );
  print_list( l_ptr );
  
  printf( "\nDeleting 0\n" );
  l_ptr = ups_list_delete( l_ptr, (void*)0, ' ' );
  print_list( l_ptr );
  
  printf( "\nDeleting 4\n" );
  l_ptr = ups_list_delete( l_ptr, (void*)4, ' ' );
  print_list( l_ptr );
}

void print_list( t_ups_list_item *list_ptr )
{
  t_ups_list_item *l_ptr;
  int count = 0;

  /*
   * Note use of ups_list_first(), to be sure to start from first item
   */
  
  for ( l_ptr = ups_list_first( list_ptr ); l_ptr; l_ptr = l_ptr->next, count++ ) {
    printf( "%03d: p=%08x, i=%08x, n=%08x, data=%08x\n",
	    count, (int)l_ptr->prev, (int)l_ptr,
	    (int)l_ptr->next, (int)l_ptr->data );    
  }
}



