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
        struct ups_list_item * pointer;
	struct ups_command * uc;

/*
	signal(SIGSEGV, ups_signal_handler);
*/

	uc = upsugo_next(argc,argv,"Aachtodgf");
	fprintf(stdout,"product %s\n",uc->ugo_product);
	fprintf(stdout,"version %s\n",uc->ugo_version);
	if (uc->ugo_chain_first ) {
	    fprintf(stdout,"Chains:\n");
	}
	for(pointer=uc->ugo_chain_first; 
	    pointer != 0;
	    pointer = pointer->next) {
	    fprintf(stdout,"%s\n",pointer->data);
	}
	if (uc->ugo_chain_first ) {
	    fprintf(stdout,"\n");
	}
	if (uc->ugo_flavor_first ) {
	    fprintf(stdout,"Flavors:\n");
	}
	for(pointer=uc->ugo_flavor_first; 
	    pointer != 0;
	    pointer = pointer->next) {
	    fprintf(stdout,"%s\n",pointer->data);
	}
	if (uc->ugo_flavor_first ) {
	    fprintf(stdout,"\n");
	}
	if (uc->ugo_host_first ) {
	    fprintf(stdout,"host\n");
	}
	for(pointer=uc->ugo_host_first; 
	    pointer != 0;
	    pointer = pointer->next) {
	    fprintf(stdout,"%s\n",pointer->data);
	}
	if (uc->ugo_host_first ) {
	    fprintf(stdout,"\n");
	}
	if (uc->ugo_auth_first ) {
	    fprintf(stdout,"authnodes\n");
	}
	for(pointer=uc->ugo_auth_first; 
	    pointer != 0;
	    pointer = pointer->next) {
	    fprintf(stdout,"%s\n",pointer->data);
	}
	if (uc->ugo_auth_first ) {
	    fprintf(stdout,"\n");
	}
/*
	if (status != UPS_SUCCESS) fprintf(stderr," %s \n", UPS_ERRTXT[status]);
*/
	exit(status);

}
