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
	if (uc->ugo_key_first ) {
	    fprintf(stdout,"Keys:\n");
	}
	for(pointer=uc->ugo_key_first; 
	    pointer != 0;
	    pointer = pointer->next) {
	    fprintf(stdout,"%s\n",pointer->data);
	}
	if (uc->ugo_key_first ) {
	    fprintf(stdout,"\n");
	}
/*
	if (status != UPS_SUCCESS) fprintf(stderr," %s \n", UPS_ERRTXT[status]);
*/
	exit(status);

}
