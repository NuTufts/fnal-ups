#include <stdio.h>
#include <stdlib.h>
#include "ups_files.h"

int  main( const int argc, char * const argv[] )
{
  int i = 0;
  t_ups_product *prod_ptr = NULL;

  if ( argc <= 1 ) {
    printf( "Usage: test_ups_files file_name\n" );
    exit( 1 );
  }

  for ( i=1; i<argc; i++ ) {
    
    prod_ptr = upsfil_read_file( argv[i] );  

    if ( prod_ptr ) {
      g_print_product( prod_ptr );
      ups_free_product( prod_ptr );
    }
  }
  

  return 0;
}


