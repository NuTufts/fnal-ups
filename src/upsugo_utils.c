/****************************** Copyright Notice ******************************
/*									
 *                                                                            *
 * Copyright (c) 1991 Universities Research Association, Inc.                 *
 *               All Rights Reserved.                                         *
 *                                                                            *
 ******************************************************************************/
/* ==========================================================================
**                                                                           
** DESCRIPTION
**
**	ups ugo utils (ups get opts) 
**           upsugo_getarg
**                                                                           
** DEVELOPERS                                                                
**
**       Eileen Berman
**       David Fagan
**       Lars Rasmussen
**                                                                           
**      Batavia, Il 60510, U.S.A.                                            
**                                                                           
** ENTRY POINTS                 SCOPE                                        
**      +++                     +++                                          
**                                                                           
** MODIFICATIONS                                                             
** 	Jul 24 1997, DjF, First
**
** HEADER STATEMENTS                                                         
*/                                                                           

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
/*
#include <sys/stat.h>
*/
#include <sys/utsname.h>

#include "../inc/ups_types.h"

#ifdef UPS_ID
	char	UPS_UGO_ID[] = "@(#)upsugo_utils.c	1.00";
#endif

#define FREE( X )	{			\
			free( X );		\
			X = 0;		\
			}

	char	*ugo_product,
		*ugo_version;

/* Chain flags, still setting them in addition to chain list 
** to be consistant and possibly could be used				*/

	int	ugo_a = 0;	/* All include				*/
	int	ugo_A = 0;	/* Authorized Host(s) 			*/
/*	int	ugo_b = 0;	UNDEFINED				*/
/*	int	ugo_B = 0;	/* CODE INCOMPLETE			*/
	int	ugo_c = 0;	/* current specified			*/
	int	ugo_C = 0;	/* Don't do Configure			*/
	int	ugo_d = 0;	/* development chain			*/
	int	ugo_D = 0;	/* list all versions with archive file	*/
	int	ugo_e = 0;	/* Define ups_extended			*/
	int	ugo_E = 0;	/* Run Editor 				*/
	int	ugo_f = 0;	/* Flavor(s) specified			*/
	int	ugo_F = 0;	/* Return list of files not in product	*/
/*	int	ugo_G = 0;	UNDEFINED				*/
	int	ugo_g = 0;	/* Did they request a "special" chain?	*/
	int	ugo_h = 0;	/* Host(s) specified			*/
/*	int	ugo_H = 0;	UNDEFINED				*/
	int	ugo_j = 0;	/* applies to top level product		*/
/*	int	ugo_J = 0;	UNDEFINED				*/
	int	ugo_k = 0;	/* Don't do unsetup first		*/
	int	ugo_K = 0;	/* Keywords				*/
	int	ugo_l = 0;	/* long (listing)			*/
/*	int	ugo_L = 0;	UNDEFINED				*/
	int	ugo_n = 0;	/* new chain				*/
/*	int	ugo_N = 0;	/* CODE INCOMPLETE			*/
	int	ugo_o = 0;	/* old chain				*/
/*	int	ugo_O = 0;	/* CODE INCOMPLETE			*/
/*	int	ugo_p = 0;	/* CODE INCOMPLETE			*/
/*	int	ugo_P = 0;	/* CODE INCOMPLETE			*/
/*	int	ugo_q = 0;	/* CODE INCOMPLETE			*/
/*	int	ugo_Q = 0;	UNDEFINED				*/
/*	int	ugo_r = 0;	/* CODE INCOMPLETE			*/
/*	int	ugo_R = 0;	UNDEFINED				*/
	int	ugo_S = 0;	/* Syntax Checking			*/
/*	int	ugo_s = 0;	UNDEFINED				*/
	int	ugo_t = 0;	/* test chain				*/
/*	int	ugo_T = 0;	/* CODE INCOMPLETE			*/
	int	ugo_u = 0;	/* uncompile first			*/
/*	int	ugo_U = 0;	/* CODE INCOMPLETE			*/
	int	ugo_v = 0;	/* verbose				*/
	int	ugo_V = 0;	/* Don't delete temp file(s)		*/
	int	ugo_w = 0;	/* stop first then start		*/
	int	ugo_W = 0;	/* use environment variables		*/
	int	ugo_x = 0;	/* CODE INCOMPLETE			*/
	int	ugo_X = 0;	/* execute instead of echo??		*/
	int	ugo_y = 0;	/* delete home dir, no query		*/
	int	ugo_Y = 0;	/* delete home dir, query		*/
	int	ugo_z = 0;	/* Database(s) were specified		*/
	int	ugo_Z = 0;	/* Time this command			*/

	int	errflg = 0;

struct ups_list_item * ugo_chain_first;
struct ups_list_item * ugo_chain_last;
struct ups_list_item * ugo_flavor_first;
struct ups_list_item * ugo_flavor_last;
struct ups_list_item * ugo_host_first;
struct ups_list_item * ugo_host_last;
struct ups_list_item * ugo_auth_first;
struct ups_list_item * ugo_auth_last;
struct ups_list_item * ugo_db_first;
struct ups_list_item * ugo_db_last;
struct ups_list_item * ugo_key_first;
struct ups_list_item * ugo_key_last;

char    *       upsugo_getarg          (int, char **, char **);
struct	ups_list_item	*	ugo_make_next	
	(char *, struct ups_list_item **, struct ups_list_item ** );

/* ==========================================================================
**                                                                           
** ROUTINE: upsugo_next
**                                                                           
** DESCRIPTION                                                               
**
**                                                                           
** VALUES RETURNED                                                           
**      +++                                                                  
**                                                                           
** ==========================================================================
*/                                                                           
int upsugo_next(int ups_argc,char *ups_argv[],char *validopts)
/*   int   ups_argc;
   char   *ups_argv[];
   char   *validopts;
*/
{

   char   *arg_str;

   char   * addr;
   char   * loc;
   

   int         argnum = 0;

   char   **argbuf;		/* String to hold residual argv stuff*/
				/* returned by upsugo_getarg */
				/* if contents are used is reset to */
				/* to 0 before recalling getarg */
/* struct stat statbuf; */


/* Initialize those pesky variables
    -------------------------------- */
   ugo_product = 0;
   ugo_version = 0;
   ugo_chain_first = 0;
   ugo_flavor_first = 0;
   ugo_chain_last = 0;
   ugo_flavor_last = 0;
   argbuf = (char **)malloc(sizeof(char *)+1);
   *argbuf = 0;

   while ((arg_str= upsugo_getarg(ups_argc, ups_argv, argbuf)) != 0)
   { if(*arg_str == '-')      /* is it an option */
     { if (!strchr(validopts,*(arg_str+1))) { 
          fprintf(stderr,"invalid option %s specified\n",arg_str+1); 
          errflg=1;
       }
       switch(*(arg_str+1))      /* which flag was specified */
       { case 'a':
              ugo_a = 1;
              break;
         case 'C':
              ugo_C = 1;
              break;
         case 'D':
              ugo_D = 1;
              break;
         case 'e':
              ugo_e = 1;
              break;
         case 'E':
              ugo_E = 1;
              break;
         case 'F':
              ugo_F = 1;
              break;
         case 'j':
              ugo_j = 1;
              break;
         case 'k':
              ugo_k = 1;
              break;
         case 'l':
              ugo_l = 1;
              break;
         case 'S':
              ugo_S = 1;
              break;
         case 'u':
              ugo_u = 1;
              break;
         case 'v':
              ugo_v = 1;
              break;
         case 'V':
              ugo_V = 1;
              break;
         case 'w':
              ugo_w = 1;
              break;
         case 'W':
              ugo_W = 1;
              break;
         case 'x':		/* This command is incomplete */
              ugo_x = 1;
              break;
         case 'y':
              ugo_y = 1;
              break;
         case 'Y':
              ugo_Y = 1;
              break;
         case 'Z':
              ugo_Z = 1;
              break;
         case 'c':
              ugo_c = 1;
              ugo_make_next("current",&ugo_chain_last,&ugo_chain_first);
              break;
         case 'n':
              ugo_n = 1;
              ugo_make_next("new",&ugo_chain_last,&ugo_chain_first);
              break;
         case 'd':
              ugo_d = 1;
              ugo_make_next("development",&ugo_chain_last,&ugo_chain_first);
         case 't':
              ugo_t = 1;
              ugo_make_next("test",&ugo_chain_last,&ugo_chain_first);
              break;
         case 'o':
              ugo_o = 1;
              ugo_make_next("old",&ugo_chain_last,&ugo_chain_first);
              break;
         case 'g':
              ugo_g = 1;
              if ( *argbuf ) 
              {  ugo_make_next(*argbuf,&ugo_chain_last,&ugo_chain_first);
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
                ugo_make_next(arg_str,&ugo_chain_last,&ugo_chain_first);
                break;
              }
              errflg = 1;
              break;
         case 'f':
              ugo_f = 1;
              if ( *argbuf ) 
              {  ugo_make_next(*argbuf,&ugo_flavor_last,&ugo_flavor_first);
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
                ugo_make_next(arg_str,&ugo_flavor_last,&ugo_flavor_first);
                break;
              }
              errflg = 1;
              break;
         case 'h':
              ugo_h = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf;
                  *argbuf=loc+1;
                  *loc = 0;
                  ugo_make_next(addr,&ugo_host_last,&ugo_host_first);
               }
               ugo_make_next(*argbuf,&ugo_host_last,&ugo_host_first);
               *argbuf = 0;
               break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { errflg = 1;
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    ugo_make_next(addr,&ugo_host_last,&ugo_host_first);
                 }
                 ugo_make_next(arg_str,&ugo_host_last,&ugo_host_first);
                 break;
               }
                 errflg = 1;
                 break;
         case 'A':
              ugo_A = 1;
              if ( *argbuf ) 
              {  while((loc=strchr(*argbuf,','))!=0) {
                    addr=*argbuf;
                    *argbuf=loc+1;
                    *loc = 0;
                    ugo_make_next(addr,&ugo_auth_last,&ugo_auth_first);
                 }
                 ugo_make_next(*argbuf,&ugo_auth_last,&ugo_auth_first);
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
                while((loc=strchr(arg_str,','))!=0) {
                   addr=arg_str;
                   arg_str=loc+1;
                   *loc = 0;
                   ugo_make_next(addr,&ugo_auth_last,&ugo_auth_first);
                }
                ugo_make_next(arg_str,&ugo_auth_last,&ugo_auth_first);
                break;
              }
              errflg = 1;
              break;
         case 'K':
              ugo_K = 1;
              if ( *argbuf ) 
              {  while((loc=strchr(*argbuf,','))!=0) {
                    addr=*argbuf;
                    *argbuf=loc+1;
                    *loc = 0;
                    ugo_make_next(addr,&ugo_key_last,&ugo_key_first);
                 }
                 ugo_make_next(*argbuf,&ugo_key_last,&ugo_key_first);
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
                while((loc=strchr(arg_str,','))!=0) {
                   addr=arg_str;
                   arg_str=loc+1;
                   *loc = 0;
                   ugo_make_next(addr,&ugo_key_last,&ugo_key_first);
                }
                ugo_make_next(arg_str,&ugo_key_last,&ugo_key_first);
                break;
              }
              errflg = 1;
              break;
         case 'z':
              ugo_z = 1;
              if ( *argbuf ) 
              {  while((loc=strchr(*argbuf,','))!=0) {
                    addr=*argbuf;
                    *argbuf=loc+1;
                    *loc = 0;
                    ugo_make_next(addr,&ugo_db_last,&ugo_db_first);
                 }
                 ugo_make_next(*argbuf,&ugo_db_last,&ugo_db_first);
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
                while((loc=strchr(arg_str,','))!=0) {
                   addr=arg_str;
                   arg_str=loc+1;
                   *loc = 0;
                   ugo_make_next(addr,&ugo_db_last,&ugo_db_first);
                }
                ugo_make_next(arg_str,&ugo_db_last,&ugo_db_first);
                break;
              }
              errflg = 1;
              break;
         default:
            errflg = 1;
       }
     } else {
       ugo_product = arg_str;
       if((arg_str = upsugo_getarg(ups_argc, ups_argv, argbuf)) != 0)
       if(*arg_str == '-')   errflg = 1;
       ugo_version = arg_str;
     }
   }
   if ((errflg ))
   {   fprintf(stderr, "Valid options are %s\n",validopts); }
   return errflg;
}
/* ===========================================================================
** ROUTINE	upsugo_getarg( int argc, char * argv[])
**
** DESCRIPTION
**	Ups_ugo_getarg replaces getopt. It will always return
**	the next argument/option from the argc and argv pointers.
**	It does not modify either of its arguments.
**
** Returns
**	Ups_ugo_getarg returns either of the following:
**
**	The pointer to the next argument, or
**	0 if no more arguments exist.
** ==========================================================================
*/

char * upsugo_getarg( argc, argv, argbuf )

int argc;
char *  argv[];
char ** argbuf;

{

    static int 	argindx;
    static int	arg_end;
    static char **   argpt = 0;
    static char * buff = 0;
    char * c;
    char d[3];

    if( argc < 2 )
	{
	argpt = 0;
	argindx = 0;
	return  0;
	}

    if( argv == 0 )
	{
	argpt = 0;
	argindx = 0;
	return 0;
	}


    if( argv != argpt)
	{
	if( buff )
	    FREE(buff);
	argpt = argv;
	argindx = 0;
	arg_end = 0;
	}

    if( (*argbuf == 0) && (buff != 0))
	FREE(buff);

    if( argv[argindx] == 0 )
	{
	++arg_end;
	return  0;
	}

    if(*argbuf == 0)
	{

	if( ++argindx < argc )
	    {
	    if((*argv[argindx] == '-') && (strlen(argv[argindx]) >2))
		{
		buff = (char *) malloc(strlen(argv[argindx] +1));
		strcpy(buff, argv[argindx]);
		*argbuf = buff + 2;
		}
	    return argv[argindx];
	    }

	return  0;
    	}	
/* else if argbuf != 0
   ---------------------- */

	d[0] = '-';
	d[1] = **argbuf;
	d[2] = '\0';
	c = *argbuf;
	*argbuf = c+1;
	if(**argbuf == '\0')
	    {
	    FREE(buff);
	    *argbuf = 0;
	    }
	return &d[0];

}
/* =========================================================================
** Dis-allow wildcarding
** ========================================================================
if((strchr(ugo_product,'*')!=0)||(strchr(ugo_version,'*')!= 0))
	{
          fprintf( stderr, "ups: Error - Wildcarding is not allowed !!\n");
        }
*/

/*                                                     */
/* Caution - this routine constructs an inverted list. */
/*                                                     */

struct ups_list_item * ugo_make_next(data, data_addr, first_addr)

struct ups_list_item ** data_addr;
struct ups_list_item ** first_addr;
char * data;

{	
	struct ups_list_item * hold_element;
	struct ups_list_item * internal_pointer;

	hold_element = *data_addr;

/* Allocate new first element structure
--------------------------------------- */

	internal_pointer=(struct ups_list_item *)malloc(
					sizeof(struct ups_list_item));


/* If there was an element when we entered, set its next pointer to 
   the just-allocated element.
------------------------------------------------------------------ */

	if(hold_element != 0) {
		hold_element->next = internal_pointer;
	} else { 
		*first_addr = internal_pointer;
	}


/* Set back pointer.
   ----------------   */

	internal_pointer->prev = hold_element;

/* Set next pointer.
   -----------------  */

	internal_pointer->next = 0;

/* Allocate data slot.
   ---------------------  */
	internal_pointer->data = (char *)malloc(strlen(data) + 1);
/* Move data in to data slot.
   -------------------------- */
	strcpy(internal_pointer->data, data);

/* Reset first data pointer and return.
   ------------------------------------ */

	*data_addr = internal_pointer;
	return internal_pointer;
}	


