#include <stdio.h>
#include "ups_files.h"

int  main( int argc, char* argv[] )
{
  int i = 0;
  FILE *fh = 0;
  t_ups_product *prod_ptr = NULL;

  if ( argc <= 1 ) {
    printf( "Usage: test_ups_files file_name\n" );
    exit( 1 );
  }

  for ( i=1; i<argc; i++ ) {
    
    fh = fopen ( argv[i], "r" );
    if ( ! fh ) { printf( "Error opening file %s\n", argv[i] ); exit( 1 ); }
    prod_ptr = upsfil_read_file( fh );  
    fclose( fh );
    
    g_print_product( prod_ptr );
  }
  

  return 0;
}


