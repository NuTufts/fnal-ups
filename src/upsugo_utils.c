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

	int	errflg = 0;

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
struct ups_command * upsugo_next(int ups_argc,char *ups_argv[],char *validopts)
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
   struct ups_command * uc;
   uc=(struct ups_command *)malloc( sizeof(struct ups_command));
   uc->ugo_product = 0;
   uc->ugo_version = 0;
/* Chain flags, still setting them in addition to chain list 
** to be consistant and possibly could be used				*/

	uc->ugo_a = 0;	/* All include				*/
	uc->ugo_A = 0;	/* Authorized Host(s) 			*/
/*	uc->ugo_b = 0;	UNDEFINED				*/
/*	uc->ugo_B = 0;	/* CODE INCOMPLETE			*/
	uc->ugo_c = 0;	/* current specified			*/
	uc->ugo_C = 0;	/* Don't do Configure			*/
	uc->ugo_d = 0;	/* development chain			*/
	uc->ugo_D = 0;	/* list all versions with archive file	*/
	uc->ugo_e = 0;	/* Define ups_extended			*/
	uc->ugo_E = 0;	/* Run Editor 				*/
	uc->ugo_f = 0;	/* Flavor(s) specified			*/
	uc->ugo_F = 0;	/* Return list of files not in product	*/
/*	uc->ugo_G = 0;	UNDEFINED				*/
	uc->ugo_g = 0;	/* Did they request a "special" chain?	*/
	uc->ugo_h = 0;	/* Host(s) specified			*/
/*	uc->ugo_H = 0;	UNDEFINED				*/
	uc->ugo_j = 0;	/* applies to top level product		*/
/*	uc->ugo_J = 0;	UNDEFINED				*/
	uc->ugo_k = 0;	/* Don't do unsetup first		*/
	uc->ugo_K = 0;	/* Keywords				*/
	uc->ugo_l = 0;	/* long (listing)			*/
/*	uc->ugo_L = 0;	UNDEFINED				*/
	uc->ugo_n = 0;	/* new chain				*/
	uc->ugo_N = 0;	/* And N file                           */
	uc->ugo_m = 0;	/* Table file directory			*/
	uc->ugo_M = 0;	/* Table file name			*/
	uc->ugo_o = 0;	/* old chain				*/
	uc->ugo_O = 0;	/* set UPS_OPTIONS to value             */
/*	uc->ugo_p = 0;	/* CODE INCOMPLETE			*/
	uc->ugo_P = 0;	/* override product name                */
/*	uc->ugo_q = 0;	/* CODE INCOMPLETE			*/
/*	uc->ugo_Q = 0;	UNDEFINED				*/
	uc->ugo_r = 0;	/* set PROD_DIR to value                */
/*	uc->ugo_R = 0;	UNDEFINED				*/
	uc->ugo_S = 0;	/* Syntax Checking			*/
/*	uc->ugo_s = 0;	UNDEFINED				*/
	uc->ugo_t = 0;	/* test chain				*/
	uc->ugo_T = 0;	/* CODE INCOMPLETE			*/
	uc->ugo_u = 0;	/* uncompile first			*/
/*	uc->ugo_U = 0;	/* CODE INCOMPLETE			*/
	uc->ugo_v = 0;	/* verbose				*/
	uc->ugo_V = 0;	/* Don't delete temp file(s)		*/
	uc->ugo_w = 0;	/* stop first then start		*/
	uc->ugo_W = 0;	/* use environment variables		*/
	uc->ugo_x = 0;	/* CODE INCOMPLETE			*/
	uc->ugo_X = 0;	/* execute instead of echo??		*/
	uc->ugo_y = 0;	/* delete home dir, no query		*/
	uc->ugo_Y = 0;	/* delete home dir, query		*/
	uc->ugo_z = 0;	/* Database(s) were specified		*/
	uc->ugo_Z = 0;	/* Time this command			*/

   uc->ugo_chain_first = 0;
   uc->ugo_chain_last = 0;
   uc->ugo_flavor_first = 0;
   uc->ugo_flavor_last = 0;
   uc->ugo_key_first = 0;
   uc->ugo_key_last = 0;
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
              uc->ugo_a = 1;
              break;
         case 'C':
              uc->ugo_C = 1;
              break;
         case 'D':
              uc->ugo_D = 1;
              break;
         case 'e':
              uc->ugo_e = 1;
              break;
         case 'E':
              uc->ugo_E = 1;
              break;
         case 'F':
              uc->ugo_F = 1;
              break;
         case 'j':
              uc->ugo_j = 1;
              break;
         case 'k':
              uc->ugo_k = 1;
              break;
         case 'l':
              uc->ugo_l = 1;
              break;
         case 'S':
              uc->ugo_S = 1;
              break;
         case 'u':
              uc->ugo_u = 1;
              break;
         case 'v':
              uc->ugo_v = 1;
              break;
         case 'V':
              uc->ugo_V = 1;
              break;
         case 'w':
              uc->ugo_w = 1;
              break;
         case 'W':
              uc->ugo_W = 1;
              break;
         case 'x':		/* This command is incomplete */
              uc->ugo_x = 1;
              break;
         case 'y':
              uc->ugo_y = 1;
              break;
         case 'Y':
              uc->ugo_Y = 1;
              break;
         case 'Z':
              uc->ugo_Z = 1;
              break;
         case 'c':
              uc->ugo_c = 1;
              ugo_make_next("current",&uc->ugo_chain_last,&uc->ugo_chain_first);
              break;
         case 'n':
              uc->ugo_n = 1;
              ugo_make_next("new",&uc->ugo_chain_last,&uc->ugo_chain_first);
              break;
         case 'd':
              uc->ugo_d = 1;
              ugo_make_next("development",&uc->ugo_chain_last,&uc->ugo_chain_first);
         case 't':
              uc->ugo_t = 1;
              ugo_make_next("test",&uc->ugo_chain_last,&uc->ugo_chain_first);
              break;
         case 'o':
              uc->ugo_o = 1;
              ugo_make_next("old",&uc->ugo_chain_last,&uc->ugo_chain_first);
              break;
         case 'g':
              uc->ugo_g = 1;
              if ( *argbuf ) 
              {  ugo_make_next(*argbuf,&uc->ugo_chain_last,&uc->ugo_chain_first);
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
                ugo_make_next(arg_str,&uc->ugo_chain_last,&uc->ugo_chain_first);
                break;
              }
              errflg = 1;
              break;
         case 'f':
              uc->ugo_f = 1;
              if ( *argbuf ) 
              {  ugo_make_next(*argbuf,&uc->ugo_flavor_last,&uc->ugo_flavor_first);
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
                ugo_make_next(arg_str,&uc->ugo_flavor_last,&uc->ugo_flavor_first);
                break;
              }
              errflg = 1;
              break;
         case 'h':
              uc->ugo_h = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf;
                  *argbuf=loc+1;
                  *loc = 0;
                  ugo_make_next(addr,&uc->ugo_host_last,&uc->ugo_host_first);
               }
               ugo_make_next(*argbuf,&uc->ugo_host_last,&uc->ugo_host_first);
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
                    ugo_make_next(addr,&uc->ugo_host_last,&uc->ugo_host_first);
                 }
                 ugo_make_next(arg_str,&uc->ugo_host_last,&uc->ugo_host_first);
                 break;
               }
                 errflg = 1;
                 break;
         case 'K':
              uc->ugo_K = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf;
                  *argbuf=loc+1;
                  *loc = 0;
                  ugo_make_next(addr,&uc->ugo_key_last,&uc->ugo_key_first);
               }
               ugo_make_next(*argbuf,&uc->ugo_key_last,&uc->ugo_key_first);
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
                    ugo_make_next(addr,&uc->ugo_key_last,&uc->ugo_key_first);
                 }
                 ugo_make_next(arg_str,&uc->ugo_key_last,&uc->ugo_key_first);
                 break;
               }
                 errflg = 1;
                 break;
         case 'm':
              uc->ugo_m = 1;
              if ( *argbuf ) 
              {  uc->ugo_tablefiledir = *argbuf;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
		uc->ugo_tablefiledir = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'M':
              uc->ugo_M = 1;
              if ( *argbuf ) 
              {  uc->ugo_tablefile = *argbuf;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
		uc->ugo_tablefile = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'N':
              uc->ugo_N = 1;
              if ( *argbuf ) 
              {  uc->ugo_anyfile = *argbuf;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
		uc->ugo_anyfile = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'O':
              uc->ugo_O = 1;
              if ( *argbuf ) 
              {  uc->ugo_options = *argbuf;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
		uc->ugo_options = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'P':
              uc->ugo_P = 1;
              if ( *argbuf ) 
              {  uc->ugo_override = *argbuf;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
		uc->ugo_override = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'r':
              uc->ugo_r = 1;
              if ( *argbuf ) 
              {  uc->ugo_productdir = *argbuf;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
		uc->ugo_productdir = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'T':
              uc->ugo_T = 1;
              if ( *argbuf ) 
              {  uc->ugo_archivefile = *argbuf;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
		uc->ugo_archivefile = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'U':
              uc->ugo_U = 1;
              if ( *argbuf ) 
              {  uc->ugo_upsdir = *argbuf;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { errflg = 1;
                  break;
                }
		uc->ugo_upsdir = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'A':
              uc->ugo_A = 1;
              if ( *argbuf ) 
              {  while((loc=strchr(*argbuf,','))!=0) {
                    addr=*argbuf;
                    *argbuf=loc+1;
                    *loc = 0;
                    ugo_make_next(addr,&uc->ugo_auth_last,&uc->ugo_auth_first);
                 }
                 ugo_make_next(*argbuf,&uc->ugo_auth_last,&uc->ugo_auth_first);
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
                   ugo_make_next(addr,&uc->ugo_auth_last,&uc->ugo_auth_first);
                }
                ugo_make_next(arg_str,&uc->ugo_auth_last,&uc->ugo_auth_first);
                break;
              }
              errflg = 1;
              break;
         case 'z':
              uc->ugo_z = 1;
              if ( *argbuf ) 
              {  while((loc=strchr(*argbuf,','))!=0) {
                    addr=*argbuf;
                    *argbuf=loc+1;
                    *loc = 0;
                    ugo_make_next(addr,&uc->ugo_db_last,&uc->ugo_db_first);
                 }
                 ugo_make_next(*argbuf,&uc->ugo_db_last,&uc->ugo_db_first);
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
                   ugo_make_next(addr,&uc->ugo_db_last,&uc->ugo_db_first);
                }
                ugo_make_next(arg_str,&uc->ugo_db_last,&uc->ugo_db_first);
                break;
              }
              errflg = 1;
              break;
         default:
            errflg = 1;
       }
     } else {
       uc->ugo_product = arg_str;
       if((arg_str = upsugo_getarg(ups_argc, ups_argv, argbuf)) != 0)
       if(*arg_str == '-')   errflg = 1;
       uc->ugo_version = arg_str;
     }
   }
   if ((errflg ))
   {   fprintf(stderr, "Valid options are %s\n",validopts); }
/*
   return errflg;
*/
   return uc;
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


