/***********************************************************************
 *
 * FILE:
 *       upsugo.h
 * 
 * DESCRIPTION: 
 *       Prototypes for upsugo
 *
 * AUTHORS:
 *       Eileen Berman
 *       David Fagan
 *       Lars Rasmussen
 *
 *       Fermilab Computing Division
 *       Batavia, Il 60510, U.S.A.                                              
 *
 * MODIFICATIONS:
 *       06 Oct 1997, DjF First 
 *
 ***********************************************************************/

#ifndef _UPSUGO_H_
#define _UPSUGO_H_

/*
 * Standard include files, if needed for .h file
 */
#include <stdio.h>

/* ups specific include files, if needed for .h file */
#include "upslst.h"

/*
 * Public typdef's
 */

/* Inputs from the command line */
typedef struct ups_command
{
    char    *ugo_product;     
    char    *ugo_version;     
    int     ugo_a;           /* All include                          */
    int     ugo_A;           /* Authorized Host(s)                   */
    t_upslst_item *ugo_auth;
    int     ugo_b;           /* Compile_file name                    */
    char    *ugo_compile_file;
    int     ugo_B;           /* CODE INCOMPLETE                      */
    int     ugo_c;           /* current specified                    */
    int     ugo_C;           /* Don't do Configure                   */
    int     ugo_d;           /* development chain                    */
    int     ugo_D;           /* origin file                          */
    char    *ugo_origin;
    int     ugo_e;           /* Define ups_extended                  */
    int     ugo_E;           /* Run Editor                           */
    int     ugo_f;           /* Flavor(s) specified                  */
    t_upslst_item *ugo_flavor;
    int     ugo_F;           /* Return list of files not in product  */
/*  int     ugo_G;           UPD-RESERVE */
    int     ugo_g;           /* Did they request a "special" chain?  */
    int     ugo_h;           /* Host(s) specified                    */
    t_upslst_item *ugo_host;
    int     ugo_H;           /* Gen flavor match list from -f no OS  */
    t_upslst_item *ugo_osname; /* user specified os name             */
/*  int     ugo_i;           UPD-RESERVE */
/*  int     ugo_I;           UPD-RESERVE */
    int     ugo_j;           /* applies to top level product         */
/*  int     ugo_J;           UPD-RESERVE */
    int     ugo_k;           /* Don't do unsetup first               */
    int     ugo_K;           /* Keywords                             */
    t_upslst_item *ugo_key;
    int     ugo_l;           /* long (listing)                       */
    int     ugo_L;           /* statistics add in verson on declare  */
    int     ugo_m;           /* Table file                           */
    char    *ugo_tablefiledir;  
    int     ugo_M;           /* Table file directory                 */
    char    *ugo_tablefile; 
    int     ugo_n;           /* new chain                            */
    int     ugo_N;           /* and N file                           */
    char    *ugo_anyfile; 
    int     ugo_o;           /* old chain                            */
    int     ugo_O;           /* set UPS_OPTIONS to value             */
    char    *ugo_options; 
    int     ugo_p;           /* Product Description                  */
    char    *ugo_description; 
    int     ugo_P;           /* Output Actions Only                  */
/*    char    *ugo_override;  old override product name */
    int     ugo_q;           /* CODE INCOMPLETE                      */
    t_upslst_item *ugo_qualifiers;
/*  int     ugo_Q;           UPD-RESERVE */
    int     ugo_r;           /* set product dir to value             */
    char    *ugo_productdir; 
/*  int     ugo_R;           UPD-RESERVE */
    int     ugo_s;           /* simulate                             */
    int     ugo_S;           /* Syntax Checking                      */
    int     ugo_t;           /* test chain                           */
    int     ugo_T;           /* archive file name                    */
    char    *ugo_archivefile; 
    int     ugo_u;           /* compile file dir                     */
    char    *ugo_compile_dir;
    int     ugo_U;           /* ups directory location               */
    char    *ugo_upsdir; 
    int     ugo_v;           /* verbose                              */
    int     ugo_V;           /* Don't delete temp file(s)            */
    int     ugo_w;           /* stop first then start                */
    int     ugo_W;           /* use environment variables            */
    int     ugo_x;           /* CODE INCOMPLETE                      */
    int     ugo_X;           /* execute instead of echo??            */
    int     ugo_y;           /* delete home dir, no query            */
    int     ugo_Y;           /* delete home dir, query               */
    int     ugo_z;           /* Database(s) were specified           */
    t_upslst_item *ugo_db;
    int     ugo_Z;           /* Time this command                    */
/* these are associated with n,o,d,c,t,and g chains                  */
    t_upslst_item *ugo_chain;
    int     ugo_help;        /* Help flag                            */
    int     ugo_shell;       /* shell variant that the user has      */
    int     ugo_number;      /* -0,-1,-2,etc value offset by +1      */
} t_upsugo_command;

/* 
 * Public variables
 */
enum e_shell_type {
  e_INVALID_SHELL = -1,
  e_BOURNE = 0,
  e_CSHELL
};

#define SETUPENV "SETUP_"

/*
 * Declaration of public functions.
 */

t_upsugo_command     *upsugo_next(const int ups_argc,
			       char *ups_argv[],
			       char * const validopts);

t_upsugo_command     *upsugo_env(char * const product,
			       char * const validopts);
t_upsugo_command     *upsugo_bldcmd(char * const cmdstr,
			       char * const validopts);
int         upsugo_bldfvr(struct ups_command * const uc);
int         upsugo_free(struct ups_command * const uc);
void        upsugo_prtlst( t_upslst_item * const list_ptr, char * const title ,
		           const unsigned int prnt_ptr);
int         upsugo_dump (struct ups_command * const uc, 
			 const unsigned int prnt_ptr);
#endif /* _UPSUGO_H_ */

