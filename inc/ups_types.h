/***********************************************************************
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
 *       22-Jul-1997, LR, first, very preliminary
 *       25-Jul-1997, DjF, added command line inputs
 *       29-Jul-1997, LR, New 'unified' t_ups_instance structure.
 *                        Added function declarations for creating
 *                        and destroying common types.
 *       30-Jul-1997, LR, Added 'char *db_dir' to instance structure.
 *       13-Aug-1997, LR, Added MAX_LINE_LENGTH.
 *       28-Aug-1997, DjF, added upsugo_env - Build ups_command structure
 *                         from the product environment variable given
 *                         product name.
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

/* a matched ups product. a list of all the instances that could pertain to
   the product/ */
typedef struct ups_match_product
{
  char             *db;
  
  t_upslst_item    *chain_list;
  t_upslst_item    *version_list;
  t_upslst_item    *table_list;
} t_ups_match_product;

/* a ups file, chaver will be chain name if file == CHAIN, else version */
typedef struct ups_product
{
  char             *file;
  char             *product;
  char             *chaver;
  char             *ups_db_version;
  
  t_upslst_item    *instance_list;
  t_upslst_item    *comment_list;
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
  char             *statistics;
  
  char             *db_dir;  
  
  t_upslst_item    *unknown_list;

  t_upslst_item    *action_list;

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
    t_upslst_item *ugo_auth;
/*  int     ugo_b;           UNDEFINED                               */
    int     ugo_B;           /* CODE INCOMPLETE                      */
    int     ugo_c;           /* current specified                    */
    int     ugo_C;           /* Don't do Configure                   */
    int     ugo_d;           /* development chain                    */
    int     ugo_D;           /* list all versions with archive file  */
    int     ugo_e;           /* Define ups_extended                  */
    int     ugo_E;           /* Run Editor                           */
    int     ugo_f;           /* Flavor(s) specified                  */
    t_upslst_item *ugo_flavor;
    int     ugo_F;           /* Return list of files not in product  */
/*  int     ugo_G;           UNDEFINED                               */
    int     ugo_g;           /* Did they request a "special" chain?  */
    int     ugo_h;           /* Host(s) specified                    */
    t_upslst_item *ugo_host;
/*  int     ugo_H;           UNDEFINED                               */
    int     ugo_j;           /* applies to top level product         */
/*  int     ugo_J;           UNDEFINED                               */
    int     ugo_k;           /* Don't do unsetup first               */
    int     ugo_K;           /* Keywords                             */
    t_upslst_item *ugo_key;
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
    char    *ugo_description; 
    int     ugo_P;           /* override product name                */
    char    *ugo_override; 
    int     ugo_q;           /* CODE INCOMPLETE                      */
    t_upslst_item *ugo_qualifiers;
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
    t_upslst_item *ugo_db;
    int     ugo_Z;           /* Time this command                    */
/* these are associated with n,o,d,c,t,and g chains                  */
    t_upslst_item *ugo_chain;
} t_ups_command;

/* 
 * Public variables
 */
#define ANY_MATCH "*"
#define MAX_LINE_LEN 1024


/*
 * Declaration of public functions.
 */
t_ups_product     *ups_new_product( void );
int               ups_free_product( t_ups_product * const prod_ptr );
t_ups_instance    *ups_new_instance( void );
int               ups_free_instance( t_ups_instance * const inst_ptr );
t_ups_action      *ups_new_action( void );
int               ups_free_action( t_ups_action * const act_ptr );
t_ups_match_product *ups_new_mp(const char * const a_db,
				t_upslst_item * const a_chain_list,
				t_upslst_item * const a_vers_list,
				t_upslst_item * const a_table_list);
t_ups_match_product *ups_free_mp(t_ups_match_product *a_mproduct);


t_ups_command     *upsugo_next(const int ups_argc,
			       char *ups_argv[],
			       char * const validopts);

t_ups_command     *upsugo_env(char * const product,
			       char * const validopts);

#endif /* _UPS_TYPES_H_ */

