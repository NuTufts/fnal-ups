/************************************************************************
 *
 * FILE:
 *       ups_types.h
 * 
 * DESCRIPTION: 
 *       Common types for ups
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
 *       22-jul-1997, LR, first, very preliminary
 *       25-jul-1997, DjF, added command line inputs
 *       29-jul-1997, LR, New 'unified' t_ups_instance structure.
 *                        Added function declarations for creating
 *                        and destroying common types.
 *       30-jul-1997, LR, Added 'char *db_dir' to instance structure.
 *
 ***********************************************************************/

#ifndef _UPS_TYPES_H_
#define _UPS_TYPES_H_

/*
 * Standard include files, if needed for .h file
 */

/*
 * ups specific include files, if needed for .h file
 */
#include "ups_list.h"

/*
 * Public typdef's
 */

/* a ups file, chaver will be chain name if file == CHAIN, else version */
typedef struct ups_product
{
  char             *file;
  char             *product;
  char             *chaver;
  
  t_upslst_item    *instance_list;
} t_ups_product;

/* a product instance */
typedef struct ups_instance
{
  char             *product;
  char             *version;
  char             *flavor;
  char             *qualifiers;
  
  char             *chain;
  char             *chain_declarer;
  char             *chain_declared;
  
  char             *declarer;
  char             *declared;
  char             *prod_dir;
  char             *ups_dir;
  char             *table_dir;
  char             *table_file;
  char             *archive_file;
  char             *authorized_nodes;
  char             *description;  
  t_upslst_item    *unknown_list;

  t_upslst_item    *action_list;

  char             *db_dir;
  
} t_ups_instance;

/* an action */
typedef struct ups_action
{
  char             *action;
  t_upslst_item    *command_list;
} t_ups_action;

/* Inputs from the command line */
typedef struct ups_command
{
    char    *ugo_product;     
    char    *ugo_version;     
    int     ugo_a;           /* All include                          */
    int     ugo_A;           /* Authorized Host(s)                   */
    t_upslst_item *ugo_auth_first;
    t_upslst_item *ugo_auth_last;
/*  int     ugo_b;           UNDEFINED                               */
    int     ugo_B;           /* CODE INCOMPLETE                      */
    int     ugo_c;           /* current specified                    */
    int     ugo_C;           /* Don't do Configure                   */
    int     ugo_d;           /* development chain                    */
    int     ugo_D;           /* list all versions with archive file  */
    int     ugo_e;           /* Define ups_extended                  */
    int     ugo_E;           /* Run Editor                           */
    int     ugo_f;           /* Flavor(s) specified                  */
    t_upslst_item *ugo_flavor_first;
    t_upslst_item *ugo_flavor_last;
    int     ugo_F;           /* Return list of files not in product  */
/*  int     ugo_G;           UNDEFINED                               */
    int     ugo_g;           /* Did they request a "special" chain?  */
    int     ugo_h;           /* Host(s) specified                    */
    t_upslst_item *ugo_host_first;
    t_upslst_item *ugo_host_last;
/*  int     ugo_H;           UNDEFINED                               */
    int     ugo_j;           /* applies to top level product         */
/*  int     ugo_J;           UNDEFINED                               */
    int     ugo_k;           /* Don't do unsetup first               */
    int     ugo_K;           /* Keywords                             */
    t_upslst_item *ugo_key_first;
    t_upslst_item *ugo_key_last;
    int     ugo_l;           /* long (listing)                       */
/*  int     ugo_L;           UNDEFINED                               */
    int     ugo_m;           /* Table file directory                 */
    char    *ugo_tablefiledir;  
    int     ugo_M;           /* Table file                           */
    char    *ugo_tablefile; 
    int     ugo_n;           /* new chain                            */
    int     ugo_N;           /* and N file                           */
    char    *ugo_anyfile; 
    int     ugo_o;           /* old chain                            */
    int     ugo_O;           /* set UPS_OPTIONS to value             */
    char    *ugo_options; 
    int     ugo_p;           /* Product Description                  */
    int     ugo_P;           /* override product name                */
    char    *ugo_override; 
    int     ugo_q;           /* CODE INCOMPLETE                      */
/*  int     ugo_Q;           UNDEFINED                               */
    int     ugo_r;           /* set product dir to value             */
    char    *ugo_productdir; 
/*  int     ugo_R;           UNDEFINED                               */
    int     ugo_S;           /* Syntax Checking                      */
/*  int     ugo_s;           UNDEFINED                               */
    int     ugo_t;           /* test chain                           */
    int     ugo_T;           /* archive file name                    */
    char    *ugo_archivefile; 
    int     ugo_u;           /* uncompile first                      */
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
    t_upslst_item *ugo_db_first;
    t_upslst_item *ugo_db_last;
    int     ugo_Z;           /* Time this command                    */
/* these are associated with n,o,d,c,t,and g chains                  */
    t_upslst_item *ugo_chain_first;
    t_upslst_item *ugo_chain_last;
} t_ups_command;

/*
 * Declaration of public functions.
 */
t_ups_product     *ups_new_product( void );
int               ups_free_product( t_ups_product *prod_ptr );
t_ups_instance    *ups_new_instance( void );
int               ups_free_instance( t_ups_instance *inst_ptr );
t_ups_action      *ups_new_action( void );
int               ups_free_action( t_ups_action *act_ptr );

t_ups_command     *upsugo_next(int ups_argc,char *ups_argv[],char *validopts);

#endif /* _UPS_TYPES_H_ */
