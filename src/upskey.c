/***********************************************************************
 *
 * FILE:
 *       upskey.c
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
/* standard include files */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* ups specific include files */
#include "upstyp.h"
#include "upsmem.h"
#include "upsutl.h"
#include "upskey.h"

/*
 * Definition of public variables
 */

/*
 * Declaration of private functions
 */

/*
 * Definition of global variables
 */
#define NO INVALID_INDEX

/*
 * g_key_map.
 *
 * It goes like: 'enum key', 'string key', 'index to product structure',
 *               'index to instance structure', 'index into config structure',
 *               'flag'.
 * where flag:
 * <byte>: <description>
 *   0   : key can be in a version file
 *   1   : key can be in a table file
 *   2   : key can be in a chain file
 *   3   : key can be in a config file
 *   4   : spare
 *   5   : spare
 *   6   : spare
 *   7   : spare
 */
static t_upskey_map g_key_map[] =
{
  {  0, "FILE",             0,    NO,   NO, 0x00001111 },
  {  1, "PRODUCT",          1,     0,   NO, 0x00000111 },
  {  2, "VERSION",          2,     1,   NO, 0x00000111 }, 
  {  3, "CHAIN",            3,     4,   NO, 0x00000110 },
  {  4, "UPS_DB_VERSION",   4,    NO,    0, 0x00001111 },
  {  5, "FLAVOR",           NO,    2,   NO, 0x00000111 },
  {  6, "QUALIFIERS",       NO,    3,   NO, 0x00000111 },
  {  7, "DECLARER",         NO,    5,   NO, 0x00000101 },
  {  8, "DECLARED",         NO,    6,   NO, 0x00000101 },
  {  9, "MODIFIER",         NO,    7,   NO, 0x00000001 },
  { 10, "MODIFIED",         NO,    8,   NO, 0x00000001 },
  { 11, "ORIGIN",           NO,    9,   NO, 0x00000001 },
  { 12, "PROD_DIR",         NO,   10,   NO, 0x00000001 },
  { 13, "UPS_DIR",          NO,   11,   NO, 0x00000001 },
  { 14, "TABLE_DIR",        NO,   12,   NO, 0x00000001 },
  { 15, "TABLE_FILE",       NO,   13,   NO, 0x00000001 },
  { 16, "ARCHIVE_FILE",     NO,   14,   NO, 0x00000001 },
  { 17, "AUTHORIZED_NODES", NO,   15,    2, 0x00001001 },
  { 18, "DESCRIPTION",      NO,   16,   NO, 0x00000010 },
  { 19, "STATISTICS",       NO,   17,    3, 0x00001001 },
  { 20, "COMPILE_DIR",      NO,   18,   NO, 0x00000001 },
  { 21, "COMPILE_FILE",     NO,   19,   NO, 0x00000001 },

  { 22, "DB_DIR",           NO,   20,   NO, 0x00000000 },
  { 23, "ACTION",           NO,   21,   NO, 0x00000010 },
  { 24, "USER",             NO,   22,   NO, 0x00000011 },

  { 25, "PROD_DIR_PREFIX",  NO,   NO,    1, 0x00001000 },
  { 26, "MAN_PATH",         NO,   NO,    4, 0x00001000 },
  { 27, "CATMAN_PATH",      NO,   NO,    5, 0x00001000 },
  { 28, "INFO_PATH",        NO,   NO,    6, 0x00001000 },
  { 29, "HTML_PATH",        NO,   NO,    7, 0x00001000 },
  { 30, "NEWS_PATH",        NO,   NO,    8, 0x00001000 },
  { 31, "UPD_USERCODE",     NO,   NO,    9, 0x00001000 },
  

  { 32, "GROUP:",           NO,   NO,   NO, 0x00000010 },
  { 33, "COMMON:",          NO,   NO,   NO, 0x00000010 },
  { 34, "END:",             NO,   NO,   NO, 0x00000010 },

  { 35,0,0,0,0 },
};

/*
 * Definition of public functions
 */

/*-----------------------------------------------------------------------
 * upskey_get_map
 *
 * Will return a map (t_upskey_map) for passed key.
 *
 * Input : char *, string of key.
 * Output: none.
 * Return: t_upskey_map *, corresponding map, or 0.
 */
t_upskey_map *upskey_get_map( const char * const str )
{
  t_upskey_map *keys;

  /* for now, just a linear search */
  
  for ( keys = g_key_map; keys->key; keys++ ) {
    if ( !upsutl_stricmp( keys->key, str ) )
      return keys;
  }

  return 0;
}

/*-----------------------------------------------------------------------
 * upskey_inst_getval
 *
 * Will return an instance value corresponding to passed key
 *
 * Input : t_upstyp_instance *, an instance.
 *         char *, string of key.
 * Output: none.
 * Return: char *, an instance value, or 0.
 */
char *upskey_inst_getval( t_upstyp_instance * const inst, const char * const skey )
{
  t_upskey_map *key = upskey_get_map( skey );
  if ( key && key->i_index != NO )
    return UPSKEY_INST2ARR( inst )[key->i_index];
  else
    return 0;  
}

/*-----------------------------------------------------------------------
 * upskey_inst_setval
 *
 * Will set an instance value corresponding to passed key.
 *
 * Input : t_upstyp_instance *, an instance.
 *         char *, string of key.
 *         char *, value.
 * Output: none.
 * Return: char *, return value, or 0.
 */
char *upskey_inst_setval( t_upstyp_instance * const inst,
			  const char * const skey, const char * const sval )
{
  t_upskey_map *key = upskey_get_map( skey );
  if ( key && key->i_index != NO ) {
    char *new_val = (char *)upsmem_malloc( (int)strlen( sval ) + 1 );
    strcpy( new_val, sval );
    return ( UPSKEY_INST2ARR( inst )[key->i_index] = new_val );
  }
  else
    return 0;  
}

/*-----------------------------------------------------------------------
 * upskey_inst_getaction
 *
 * Will return from an instance, the action with the passed name
 *
 * Input : t_upstyp_instance *, an instance.
 *         char *, action name
 * Output: none.
 * Return: t_upstyp_action *, a pointer to an action or 0.
 */
t_upstyp_action *upskey_inst_getaction( t_upstyp_instance * const inst,
				     const char * const action_name )
{
  t_upstyp_action *act_ptr = 0;
  t_upslst_item *act_list = 0;

  if ( !action_name || !inst || !inst->action_list )
    return 0;

  act_list = upslst_first( inst->action_list );
  for ( ; act_list; act_list = act_list->next ) {
    t_upstyp_action *act = (t_upstyp_action *)act_list->data;
    if ( !upsutl_stricmp( action_name, act->action ) ) {
      act_ptr = act;
      break;
    }
  }

  return act_ptr;
}

/*-----------------------------------------------------------------------
 * upskey_inst_getuserval
 *
 * Will return from an instance, the value defined by the passed user key.
 * If user key is defined but have no value, it will return a pointer
 * to a static zero length string.
 * If user key is not defined, it will return 0.
 *
 * Input : t_upstyp_instance *, an instance.
 *         char *, action name
 * Output: none.
 * Return: char *, a pointer to a string or 0.
 */
char *upskey_inst_getuserval( t_upstyp_instance * const inst,
			      const char * const skey )
{
  static char *sany = "";
  t_upslst_item *usr_l = 0;
  char *sp_d = 0;
  char *sp_e = 0;
  size_t len = 0;

  if ( !inst || !inst->user_list || !skey ) 
    return 0;

  len = strlen( skey );
  usr_l = upslst_first( inst->user_list );
  for ( ; usr_l; usr_l = usr_l->next ) {
    sp_d = (char *)usr_l->data;
    if ( (sp_e = strchr( sp_d, '=' )) ) {
      if ( (len == (size_t )(sp_e - sp_d)) && 
	   !upsutl_strincmp( sp_d, skey, len ) )
	return ++sp_e;
    }
    else if ( (len == strlen( sp_d )) && 
	      !upsutl_strincmp( sp_d, skey, len ) ) {
      return sany;	      
    }
  }

  return 0;
}

void upskey_inst_print( const t_upstyp_instance * const inst )
{
  t_upskey_map *keys;
  int ix;
  
  if ( !inst ) return;
  
  for ( keys = g_key_map; keys->key; keys++ ) {
    if ( (ix = keys->i_index) != NO && UPSKEY_INST2ARR( inst )[ix] )
      printf( "%s = %s\n", keys->key, UPSKEY_INST2ARR( inst )[ix] );    
  }
}

/*-----------------------------------------------------------------------
 * upskey_prod_getval
 *
 * Will return a product value corresponding to passed key
 *
 * Input : t_upstyp_product *, a product.
 *         char *, string of key.
 * Output: none.
 * Return: char *, a product value, or 0.
 */
char *upskey_prod_getval( t_upstyp_product * const prod, const char * const skey )
{
  t_upskey_map *key = upskey_get_map( skey );
  if ( key && key->p_index != NO )
    return UPSKEY_PROD2ARR( prod )[key->p_index];
  else
    return 0;  
}

/*-----------------------------------------------------------------------
 * upskey_prod_setval
 *
 * Will set a product value corresponding to passed key.
 *
 * Input : t_upstyp_product *, a product.
 *         char *, string of key.
 *         char *, value.
 * Output: none.
 * Return: char *, return value, or 0.
 */
char *upskey_prod_setval( t_upstyp_product * const prod,
			  const char * const skey, const char * const sval )
{
  t_upskey_map *key = upskey_get_map( skey );
  if ( key && key->p_index != NO ) {
    char *new_val = (char *)upsmem_malloc( (int)strlen( sval ) + 1 );
    strcpy( new_val, sval );
    return ( UPSKEY_PROD2ARR( prod )[key->p_index] = new_val );
  }
  else
    return 0;  
}

void upskey_prod_print( const t_upstyp_product * const prod )
{
  t_upskey_map *keys;
  int ix;
   
  if ( !prod ) return;
  
  for ( (keys = g_key_map); keys->key; keys++ ) {
    if ( (ix = keys->p_index) != NO && UPSKEY_PROD2ARR( prod )[ix] ) 
      printf( "%s = %s\n", keys->key, UPSKEY_PROD2ARR( prod )[ix] );
  }
}

void upskey_conf_print( const t_upstyp_config * const conf )
{
  t_upskey_map *keys;
  int ix;
  
  if ( !conf ) return;
  
  for ( keys = g_key_map; keys->key; keys++ ) {
    if ( (ix = keys->c_index) != NO && UPSKEY_CONF2ARR( conf )[ix] )
      printf( "%s = %s\n", keys->key, UPSKEY_CONF2ARR( conf )[ix] );    
  }
}

/*
 * Definition of private functions
 */
