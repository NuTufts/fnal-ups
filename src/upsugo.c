/****************************** Copyright Notice ******************************
 *                                                                            *
 * Copyright (c) 1991 Universities Research Association, Inc.                 *
 *               All Rights Reserved.                                         *
 *                                                                            *
 ******************************************************************************/
/*static char           *str_create( char * const str ); */
/* char    *       upsugo_getarg          (int , char *, char **); */
/* ==========================================================================
**                                                                           
** DESCRIPTION
**
**	ups ugo (ups get opts) 
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

#include "ups_types.h"
#include "ups_error.h"
#include "ups_memory.h"

#ifdef UPS_ID
	char	UPS_UGO_ID[] = "@(#)upsugo.c	1.00";
#endif
#define FREE( X )	{			\
			free( X );		\
			X = 0;		\
			}

   int	errflg = 0;
  t_upslst_item *ugo_commands = 0;
    int argindx;
 
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

char * upsugo_getarg(argc,argv,argbuf)
int argc;
char *  argv[];
char ** argbuf;
{

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
		buff = (char *) malloc((int)strlen(argv[argindx]) +1);
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

/*
 * Utils
 */

char *str_create( char * const str )
{
  char *new_str = NULL;
    
  if ( str ) {
/*    new_str = (char *)upsmem_malloc( (int) strlen( str ) + 1 ); */
    new_str = (char *)upsmem_malloc( strlen( str ) + 1 );
    strcpy( new_str, str );
  }
  
  return new_str;
}

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
t_ups_command *upsugo_next(int ups_argc,char *ups_argv[],char *validopts)
{
   char   *arg_str;

   char   * addr;
   char   * loc;
   int add_ver=0;
   int my_argc=0;
   
   char   **savbuf;
   char   **argbuf;		/* String to hold residual argv stuff*/
				/* returned by upsugo_getarg */
				/* if contents are used is reset to */
				/* to 0 before recalling getarg */
/* Initialize those pesky variables
    -------------------------------- */
   struct ups_command * uc;
   struct ups_command * luc;
   t_upslst_item *my_qualifiers=0;
   uc=(struct ups_command *)upsmem_malloc( sizeof(struct ups_command));
   my_qualifiers=0;
   uc->ugo_product = 0;
   uc->ugo_version = 0;
   uc->ugo_key = 0;
   uc->ugo_qualifiers = 0;
   uc->ugo_auth = 0;
   uc->ugo_db = 0;
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
	uc->ugo_q = 0;	/* CODE INCOMPLETE			*/
/*	uc->ugo_Q = 0;	UNDEFINED				*/
	uc->ugo_r = 0;	/* set PROD_DIR to value                */
/*	uc->ugo_R = 0;	UNDEFINED				*/
	uc->ugo_S = 0;	/* Syntax Checking			*/
/*	uc->ugo_s = 0;	UNDEFINED				*/
	uc->ugo_t = 0;	/* test chain				*/
	uc->ugo_T = 0;	/* CODE INCOMPLETE			*/
	uc->ugo_u = 0;	/* uncompile first			*/
	uc->ugo_U = 0;	/* CODE INCOMPLETE			*/
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

	uc->ugo_flavor = 0; 
	uc->ugo_host = 0; 
	uc->ugo_auth = 0; 
	uc->ugo_db = 0; 
	uc->ugo_chain = 0; 
  if ( ugo_commands ) { /* subsequent call */ 
     /* dealloc your brain out */ 
     if ( ugo_commands=ugo_commands->next ) {
        return (t_ups_command *)ugo_commands->data; 
     } else {
        return 0;
     }
  } else { 

   argbuf = (char **)upsmem_malloc(sizeof(char *)+1);
   *argbuf = 0;

   while ((arg_str= upsugo_getarg(ups_argc, ups_argv, argbuf)) != 0)
   { my_argc=+1; 
     if(*arg_str == '-')      /* is it an option */
     { if (!strchr(validopts,*(arg_str+1))) { 
          upserr_add(UPS_INVALID_ARGUMENT, UPS_FATAL, arg_str+1);
/*          errflg=1; */
       }
       add_ver=0;
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
              addr=str_create("current");
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 'n':
              uc->ugo_n = 1;
              addr=str_create("new");
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 'd':
              uc->ugo_d = 1;
              addr=str_create("development");
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 't':
              uc->ugo_t = 1;
              addr=str_create("test");
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 'o':
              uc->ugo_o = 1;
              addr=str_create("old");
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 'g':
              uc->ugo_g = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=str_create(addr);
                  uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
                }
                addr=str_create(*argbuf);
                uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "g" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=str_create(addr);
		    uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
                 }
                    addr=str_create(addr);
                 uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
                 break;
               }
               errflg = 1;
               break;
         case 'f':
              uc->ugo_f = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=str_create(addr);
                  uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
                }
                addr=str_create(*argbuf);
                uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "f" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=str_create(addr);
		    uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
                 }
                    addr=str_create(arg_str);
                 uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
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
                  *loc = 0;		/* replace "," terminate the string */
                  addr=str_create(addr);
                  uc->ugo_host = upslst_add(uc->ugo_host,addr);
                }
                addr=str_create(*argbuf);
                uc->ugo_host = upslst_add(uc->ugo_host,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "h" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=str_create(addr);
		    uc->ugo_host = upslst_add(uc->ugo_host,addr);
                 }
                 addr=str_create(arg_str);
                 uc->ugo_host = upslst_add(uc->ugo_host,addr);
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
                  *loc = 0;		/* replace "," terminate the string */
                  addr=str_create(addr);
                  uc->ugo_key = upslst_add(uc->ugo_key,addr);
                }
                addr=str_create(*argbuf);
                uc->ugo_key = upslst_add(uc->ugo_key,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "K" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=str_create(addr);
		    uc->ugo_key = upslst_add(uc->ugo_key,addr);
                 }
                    addr=str_create(arg_str);
                 uc->ugo_key = upslst_add(uc->ugo_key,addr);
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
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "m" );
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
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "M" );
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
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "N" );
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
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "O" );
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
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "P" );
                  break;
                }
		uc->ugo_override = arg_str;
                break;
              }
              errflg = 1;
              break;
         case 'q':
              uc->ugo_q = 1;
              if ( *argbuf ) 
              { addr=str_create(*argbuf);
                my_qualifiers = upslst_add(my_qualifiers,addr);
/* return something for now */
                uc->ugo_qualifiers = upslst_add(uc->ugo_qualifiers,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "q" );
                   break;
                 }
                    addr=str_create(arg_str);
                 my_qualifiers = upslst_add(my_qualifiers,addr);
/* return something for now */
                 uc->ugo_qualifiers = upslst_add(uc->ugo_qualifiers,addr);
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
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "r" );
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
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "T" );
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
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "U" );
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
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=str_create(addr);
                  uc->ugo_auth = upslst_add(uc->ugo_auth,addr);
                }
                addr=str_create(*argbuf);
                uc->ugo_auth = upslst_add(uc->ugo_auth,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "A" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=str_create(addr);
		    uc->ugo_auth = upslst_add(uc->ugo_auth,addr);
                 }
                    addr=str_create(arg_str);
                 uc->ugo_auth = upslst_add(uc->ugo_auth,addr);
                 break;
               }
               errflg = 1;
               break;
         case 'z':
              uc->ugo_z = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=str_create(addr);
                  uc->ugo_db = upslst_add(uc->ugo_db,addr);
                }
                addr=str_create(*argbuf);
                uc->ugo_db = upslst_add(uc->ugo_db,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "z" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=str_create(addr);
		    uc->ugo_db = upslst_add(uc->ugo_db,addr);
                 }
                    addr=str_create(arg_str);
                 uc->ugo_db = upslst_add(uc->ugo_db,addr);
                 break;
               }
               errflg = 1;
               break;
         default:
            errflg = 1;
       }
     } else {
       if ( strchr(arg_str,',') == 0 )
       { addr=str_create(arg_str);
         if (add_ver) 
         { luc->ugo_version=addr;
           add_ver=0;
         } else { 
           uc->ugo_product = addr;
           luc=uc;
           add_ver=1;
           ugo_commands = upslst_add(ugo_commands,uc);
           uc=(struct ups_command *)upsmem_malloc( sizeof(struct ups_command));
         } 
       } else { 
/* was it just a , or a ,something? */
         if(strlen(arg_str)==1 || *arg_str==',' )  /* just a comma all alone */
         { add_ver=0;                              /* just in case... */
           if ( strlen(arg_str)!=1 )               /* a ,something not just , */
           { ups_argv[argindx]=arg_str+1;
             argindx=argindx-1;
             my_argc=my_argc-1;
           }
         } else { 
           loc=strchr(arg_str,',');
           if(loc==arg_str)
           { addr=str_create(arg_str+1);
           } else {
             *loc=0;
             addr=str_create(arg_str);
            *loc=' '; 
           }
           if (add_ver) 
           { luc->ugo_version=addr;
             add_ver=0;
           } else { 
             uc->ugo_product = addr;
             ugo_commands = upslst_add(ugo_commands,uc);
             uc=(struct ups_command *)upsmem_malloc( sizeof(struct ups_command));
             luc=uc;
           }
/* prod/version, (space) */
           if (strlen(arg_str) != (strlen(addr)+1))
           { ups_argv[argindx]=loc+1;
             argindx=argindx-1;
             my_argc=my_argc-1;
           }
         }
       }
     }
   }
/*    if ((errflg ))
   {   fprintf(stderr, "Valid options are %s\n",validopts); } */
   ugo_commands=upslst_first(ugo_commands);
   return (t_ups_command *)ugo_commands->data; 
   } /* not subsequent call ... */
}
