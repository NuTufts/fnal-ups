#include <stdio.h>

#include "../inc/ups_list.h"

void print_list( t_ups_list_item *list_ptr );

void main( void )
{
  t_ups_list_item *l_top;
  
  printf( "\nCreating a list:\n" );
  l_top = ups_list_new( 0 );
  print_list( l_top );
  
  printf( "\nInsering 1\n" );
  l_top = ups_list_insert( l_top, (void*)1 );
  print_list( l_top );
  
  printf( "\nInsering 2\n" );
  l_top = ups_list_insert( l_top, (void*)2 );
  print_list( l_top );
  
  printf( "\nInsering 3\n" );
  l_top = ups_list_insert( l_top, (void*)3 );
  print_list( l_top );
  
  printf( "\nAdding 4\n" );
  l_top = ups_list_add( l_top, (void*)4 );
  print_list( l_top );

  /*
  printf( "Deleting the list:\n" );
  l_top = ups_list_free( l_top, ' ' );
  print_list( l_top );
  */
  
  printf( "\nDeleting 1\n" );
  l_top = ups_list_delete( l_top, (void*)1, ' ' );
  print_list( l_top );
  
  printf( "\nDeleting 2\n" );
  l_top = ups_list_delete( l_top, (void*)2, ' ' );
  print_list( l_top );
  
  printf( "\nDeleting 3\n" );
  l_top = ups_list_delete( l_top, (void*)3, ' ' );
  print_list( l_top );
  
  printf( "\nDeleting 0\n" );
  l_top = ups_list_delete( l_top, (void*)0, ' ' );
  print_list( l_top );
  
  printf( "\nDeleting 4\n" );
  l_top = ups_list_delete( l_top, (void*)4, ' ' );
  print_list( l_top );
}

void print_list( t_ups_list_item *list_ptr )
{
  t_ups_list_item *l_ptr;
  int count = 0;
  
  for ( l_ptr = list_ptr; l_ptr; l_ptr = l_ptr->next, count++ ) {
    printf( "%03d: p=%08x, i=%08x, n=%08x, data=%08x\n",
	    count, (int)l_ptr->prev, (int)l_ptr,
	    (int)l_ptr->next, (int)l_ptr->data );    
  }
}

