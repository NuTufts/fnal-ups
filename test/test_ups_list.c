#include <stdio.h>
#include <stdlib.h>

#include "ups_list.h"

void print_list( t_upslst_item *list_ptr );

void main( void )
{
  t_upslst_item *l_ptr;
  char *data_ptr = 0;
  
  /*
   * Note:
   *
   * Calls to upslst_add/insert, with a passed list pointer equal null,
   * will create a new list, add the passed item and return a pointer
   * to the created list.
   *
   * Calls to upslst_free, with option 'd', will also delete the data
   * item.
   *
   * Calls to upslst_add, will add items as last item in the list
   * and return a pointer to last item.
   * Calls to upslst_insert, will add items as first item in the list
   * and return a pointer to first item.
   * In that way successive calls using previous return will be fast.  
   *
   */
  
  printf( "\nCreating and adding as last 1\n" );
  l_ptr = upslst_add( 0, (void*)1 );
  print_list( l_ptr );
  
  printf( "\nAdding as last 2\n" );
  l_ptr = upslst_add( l_ptr, (void*)2 );
  print_list( l_ptr );
  
  data_ptr = (char *)malloc( 1024 );
  printf( "\nInserting as first !!! %0x\n", (int)data_ptr );
  l_ptr = upslst_insert( l_ptr, data_ptr );
  print_list( l_ptr );
  
  printf( "\nAdding as last 3\n" );
  l_ptr = upslst_add( l_ptr, (void*)3 );
  print_list( l_ptr );

  printf( "\nAdding as last 4\n" );
  l_ptr = upslst_add( l_ptr, (void*)4 );
  print_list( l_ptr );

  /*
  printf( "Deleting the list:\n" );
  free ( data_ptr );
  l_ptr = upslst_free( l_ptr, ' ' );
  print_list( l_ptr );
  */
  
  printf( "\nDeleting 1\n" );
  l_ptr = upslst_delete( l_ptr, (void*)1, ' ' );
  print_list( l_ptr );
  
  printf( "\nDeleting 2\n" );
  l_ptr = upslst_delete( l_ptr, (void*)2, ' ' );
  print_list( l_ptr );
  
  printf( "\nDeleting 4\n" );
  l_ptr = upslst_delete( l_ptr, (void*)4, ' ' );
  print_list( l_ptr );
  
  printf( "\nDeleting 3\n" );
  l_ptr = upslst_delete( l_ptr, (void*)3, ' ' );
  print_list( l_ptr );
  
  printf( "\nDeleting %0x with option 'd'\n", (int)data_ptr );
  l_ptr = upslst_delete( l_ptr, (void*)data_ptr, 'd' );
  print_list( l_ptr );
  
}

void print_list( t_upslst_item *list_ptr )
{
  t_upslst_item *l_ptr;
  int count = 0;

  /*
   * Note use of upslst_first(), to be sure to start from first item
   */
  
  for ( l_ptr = upslst_first( list_ptr ); l_ptr; l_ptr = l_ptr->next, count++ ) {
    printf( "%03d: p=%08x, i=%08x, n=%08x, data=%08x\n",
	    count, (int)l_ptr->prev, (int)l_ptr,
	    (int)l_ptr->next, (int)l_ptr->data );    
  }
}



