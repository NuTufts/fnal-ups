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

#include "ups_types.h"
 
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
	struct ups_command * uc;

/*
	signal(SIGSEGV, ups_signal_handler);
*/
        uc=upsugo_env("TEST","AacCdfghKtmMNoOPqrTuU");
        if (uc) 
	{ upsugo_dump(uc);
          upsugo_free(uc);
        }
        uc=0;
while ((uc = upsugo_next(argc,argv,"AacCdfghKtmMNoOPqrTuU")) != 0 )
      { upsugo_dump(uc); }
/*
	if (status != UPS_SUCCESS) fprintf(stderr," %s \n", UPS_ERRTXT[status]);
*/
	exit(status);

}
