/***********************************************************************
 *
 * FILE:
 *       upstyp.h
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
 *       08-Sep-1997, LR, Added constant INVALID_INDEX.
 *
 ***********************************************************************/

#ifndef _UPSTYP_H_
#define _UPSTYP_H_

/*
 * Standard include files, if needed for .h file
 */

/*
 * ups specific include files, if needed for .h file
 */
#include "upslst.h"

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

/* a db config file */
typedef struct ups_config {
  char             *ups_db_version;
  char             *prod_dir_prefix;
  char             *authorized_nodes;
  char             *statistics;
  char             *man_path;
  char             *info_path;
  char             *html_path;
} t_ups_config;

/* a product instance */
typedef struct ups_instance
{
  char             *product;
  char             *version;
  char             *flavor;
  char             *qualifiers;
  
  char             *chain;
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

/* any ups file */
typedef struct ups_product
{
  char             *file;
  char             *product;
  char             *version;
  char             *chain;
  char             *ups_db_version;
  
  t_upslst_item    *instance_list;
  t_upslst_item    *comment_list;
  t_ups_config     *config;
} t_ups_product;

/* 
 * Public variables
 */
#define ANY_MATCH "*"
#define MAX_LINE_LEN 1024
#define INVALID_INDEX -1

/*
 * Declaration of public functions.
 */
t_ups_product     *ups_new_product( void );
int               ups_free_product( t_ups_product * const prod_ptr );
t_ups_instance    *ups_new_instance( void );
int               ups_free_instance( t_ups_instance * const inst_ptr );
t_ups_action      *ups_new_action( void );
int               ups_free_action( t_ups_action * const act_ptr );
t_ups_config      *ups_new_config( void );
int               ups_free_config( t_ups_config * const conf_ptr );
t_ups_match_product *ups_new_mp(const char * const a_db,
				t_upslst_item * const a_chain_list,
				t_upslst_item * const a_vers_list,
				t_upslst_item * const a_table_list);
t_ups_match_product *ups_free_mp(t_ups_match_product * const a_mproduct);


t_ups_command     *upsugo_next(const int ups_argc,
			       char *ups_argv[],
			       char * const validopts);

t_ups_command     *upsugo_env(char * const product,
			       char * const validopts);
int         upsugo_free(struct ups_command * const uc);
void        upsugo_prtlst( t_upslst_item * const list_ptr, char * const title );
int         upsugo_dump (struct ups_command * const uc);
int         upshlp_command(const char * const what);
#endif /* _UPSTYP_H_ */

