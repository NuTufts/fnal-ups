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

#include "ups.h"
 
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

	struct ups_command * uc;

while ((uc = upsugo_next(argc,argv,"AacCdfghKltmMNoOPqrTuUv")) != 0 )
      { ups_list(uc); 
        upserr_output();
       }
	exit(0);
}
