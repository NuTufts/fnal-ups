/*                                                                           
**========================== Copyright Notice =============================**
**                                                                         **
**       Copyright (c) 1990 Universities Research Association, Inc.        **
**                      All Rights Reserved.                               **
**                                                                         **
**=========================================================================**/
/*                                                                           
** DESCRIPTION                                                               
**      +++                                                                  
**                                                                           
** DEVELOPERS                                                                
**      +++                                                                  
**                                                                           
**      Batavia, Il 60510, U.S.A.                                            
**                                                                           
** ENTRY POINTS                 SCOPE                                        
**      +++                     +++                                          
**                                                                           
** MODIFICATIONS                                                             
**         Date       Initials  Description                                  
**      +++             +++     +++                                          
**                                                                           
** HEADER STATEMENTS                                                         
*/                                                                           

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

#include "../inc/ups_types.h"
 
void print_list( t_upslst_item *list_ptr );
/* ==========================================================================
**                                                                           
** ROUTINE: main
**                                                                           
** DESCRIPTION                                                               
**
** VALUES RETURNED                                                           
**      +++                                                                  
**                                                                           
** ==========================================================================
*/                                                                           
int main (argc,argv)
	int	argc;
	char	*argv[];
{

	int 	status = 0;
        struct upslst_item * pointer;
	struct ups_command * uc;

/*
	signal(SIGSEGV, ups_signal_handler);
*/

	uc = upsugo_next(argc,argv,"AacCdfghKtmMNoOPrTuU");
	fprintf(stdout,"product: %s\n",uc->ugo_product);
	fprintf(stdout,"version: %s\n",uc->ugo_version);
	if (uc->ugo_C) {
	fprintf(stdout,"Don't do Configure\n");
	}
	if (uc->ugo_M) {
	fprintf(stdout,"Table file dir: %s\n",uc->ugo_tablefiledir);
	}
	if (uc->ugo_m) {
	fprintf(stdout,"Table file: %s\n",uc->ugo_tablefile);
	}
	if (uc->ugo_N) {
	fprintf(stdout,"Any \"N\" file: %s\n",uc->ugo_anyfile);
	}
	if (uc->ugo_O) {
	fprintf(stdout,"set UPS_OPTIONS =: %s\n",uc->ugo_options);
	}
	if (uc->ugo_P) {
	fprintf(stdout,"Override Product name=: %s\n",uc->ugo_override);
	}
	if (uc->ugo_r) {
	fprintf(stdout,"set PROD_DIR =: %s\n",uc->ugo_productdir);
	}
	if (uc->ugo_T) {
	fprintf(stdout,"set archive file=: %s\n",uc->ugo_archivefile);
	}
	if (uc->ugo_u) {
	fprintf(stdout,"Uncompile first\n");
	}
	if (uc->ugo_U) {
	fprintf(stdout,"set UPS_DIR =: %s\n",uc->ugo_upsdir);
	}
	if (uc->ugo_flavor) {
	    fprintf(stdout,"Flavors:\n");
            print_list( uc->ugo_flavor );
	    fprintf(stdout,"\n");
	}
	if (uc->ugo_host) {
	    fprintf(stdout,"host\n");
            print_list( uc->ugo_host );
	    fprintf(stdout,"\n");
	}
	if (uc->ugo_auth) {
	    fprintf(stdout,"auth\n");
            print_list( uc->ugo_auth );
	    fprintf(stdout,"\n");
	}
	if (uc->ugo_key) {
	    fprintf(stdout,"key\n");
            print_list( uc->ugo_key );
	    fprintf(stdout,"\n");
	}
	if (uc->ugo_chain) {
	    fprintf(stdout,"chain\n");
            print_list( uc->ugo_chain );
	    fprintf(stdout,"\n");
	}
/*
	if (status != UPS_SUCCESS) fprintf(stderr," %s \n", UPS_ERRTXT[status]);
*/
	exit(status);

}

void print_list( t_upslst_item *list_ptr )
{
  t_upslst_item *l_ptr;
  int count = 0;

  /*
   * Note use of upslst_first(), to be sure to start from first item
   */
  
  for ( l_ptr = upslst_first( list_ptr ); l_ptr; l_ptr = l_ptr->next, count++ ) {
    printf( "%03d: p=%08x, i=%08x, n=%08x, data=%s\n",
	    count, (int)l_ptr->prev, (int)l_ptr,
	    (int)l_ptr->next, (int)l_ptr->data );    
  }
}
