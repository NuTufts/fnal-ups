/***********************************************************************
 *
 * FILE:
 *       ups_keys.h
 * 
 * DESCRIPTION: 
 *       Translations for ups keys
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
 *       25-aug-1997, LR, first
 *
 ***********************************************************************/

#ifndef _UPS_KEYS_H_
#define _UPS_KEYS_H_

/*
 * Standard include files, if needed for .h file
 */

/*
 * ups specific include files, if needed for .h file
 */
#include "ups_types.h"

/*
 * Constants.
 */

/* enum of known keys (changes here should be reflected in g_ups_keys) */
enum e_upskey_key {
  e_key_file = 0,
  e_key_product,
  e_key_version,
  e_key_chain,
  e_key_ups_db_version,

  e_key_flavor,
  e_key_qualifiers,
  e_key_declarer,
  e_key_declared,
  e_key_prod_dir,
  e_key_ups_dir,
  e_key_table_dir,
  e_key_table_file,
  e_key_archive_file,
  e_key_authorized_nodes,
  e_key_description,
  e_key_statistics,

  e_key_db_dir,
  e_key_action,
  e_key_unknown,
  
  e_key_group,
  e_key_common,
  e_key_end,

  e_key_count
};

/*
 * Types.
 */

typedef struct upskey_map {
  int           ikey;    /* enum of key (corresponding to e_upskey_key) */
  char          *key;    /* string of key */
  int           p_index; /* index into product structure (t_ups_product) */
  int           i_index; /* index into instance structure (t_ups_instance) */
  unsigned int  flag;    /* flags, see ups_keys.c */
} t_upskey_map;

typedef union ups_product_u {
  t_ups_product  prod;
  char           *arr[e_key_count];
} t_ups_product_u;

typedef union ups_instance_u {
  t_ups_instance inst;
  char           *arr[e_key_count];
} t_ups_instance_u;

/* some convenient macros */

#define UPSKEY_PROD2ARR( prod ) (((t_ups_product_u *)prod)->arr)
#define UPSKEY_INST2ARR( inst ) (((t_ups_instance_u *)inst)->arr)

#define UPSKEY_ISIN_VERSION( flag ) (flag&0x0001 ? 1 : 0)
#define UPSKEY_ISIN_TABLE( flag )   (flag&0x0010 ? 1 : 0)
#define UPSKEY_ISIN_CHAIN( flag )   (flag&0x0100 ? 1 : 0)
#define UPSKEY_ISIN_CONFIG( flag )  (flag&0x1000 ? 1 : 0)

/*
 * Declaration of public functions.
 */
t_upskey_map *upskey_get_map( const char * const skey );
char         *upskey_inst_getval( t_ups_instance * const inst,
				  const char * const skey );
char         *upskey_inst_setval( t_ups_instance * const inst,
				  const char * const skey,
				  const char * const sval );
void         upskey_inst_print( const t_ups_instance * const prod );
char         *upskey_prod_getval( t_ups_product * const prod,
				  const char * const skey );
char         *upskey_prod_setval( t_ups_product * const prod,
				  const char * const skey,
				  const char * const sval );
void         upskey_prod_print( const t_ups_product * const prod );
     
/*
 * Declarations of public variables.
 */

#endif /* _UPS_KEYS_H_ */
