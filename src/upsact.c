/************************************************************************
 *
 * FILE:
 *       upsact.c
 * 
 * DESCRIPTION: 
 *       This file contains routines to manage ups action lines.
 *       *** It need to be reentrant ***
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
 *       28-Aug-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ups specific include files */
#include "upsact.h"
#include "upserr.h"
#include "upslst.h"
#include "upsutl.h"
#include "upsmem.h"
#include "upskey.h"
#include "upsugo.h"
#include "upsget.h"
#include "ups_main.h"

/*
 * Definition of public variables.
 */

/*
 * Private constants
 */

#define DQUOTE '"'
#define COMMA ','
#define OPEN_PAREN '('
#define CLOSE_PAREN ')'
#define WSPACE " \t\n\r\f\""

/*
 * Private types
 */


/* this one is the type of a action command handler */
typedef int (*tpf_cmd)( const t_upsact_cmd * const cmd_ptr, const char copt );

/* this one is the type for a single action command */
typedef struct s_cmd_map {
  char *cmd;
  int  icmd;
  tpf_cmd func;
  int  min_params;
  int  max_params;
} t_cmd_map;

/*
 * Declaration of private functions.
 */

int parse_params( const char * a_params,
		   char *argv[] );
t_upslst_item *next_cmd( t_upstyp_action *action,
			 t_upslst_item *dep_list,
			 t_upsact_item *p_cur, 
			 const char * const act_name );
t_upstyp_action *get_act( t_upsugo_command *ugo_cmd,
			  t_upstyp_matched_product *mat_prod,
			  char *act_name );
t_upsact_item *find_product( t_upslst_item *dep_list,
			     const char *const prod_name );

/* functions to handle specific action commands */

void f_copyhtml( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc, const char ** const a_argv);
void f_copyinfo( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc, const char ** const a_argv);
void f_copyman( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc, const char ** const a_argv);
void f_uncopyman( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc, const char ** const a_argv);
void f_copynews( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc, const char ** const a_argv);
void f_envappend( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc, const char ** const a_argv);
void f_envprepend( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc, const char ** const a_argv);
void f_envremove( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc, const char ** const a_argv);
void f_envset( const t_upstyp_matched_instance * const a_inst,
	       const t_upstyp_db * const a_db_info,
	       const t_upsugo_command * const a_command_line,
	       const FILE * const a_stream,
	       const struct s_cmd_map * const a_cmd_map,
	       const int a_argc, const char ** const a_argv);
void f_envunset( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc, const char ** const a_argv);
void f_exeaccess( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc, const char ** const a_argv);
void f_execute( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc, const char ** const a_argv);
void f_filetest( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc, const char ** const a_argv);
void f_pathappend( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc, const char ** const a_argv);
void f_pathprepend( const t_upstyp_matched_instance * const a_inst,
		    const t_upstyp_db * const a_db_info,
		    const t_upsugo_command * const a_command_line,
		    const FILE * const a_stream,
		    const struct s_cmd_map * const a_cmd_map,
		    const int a_argc, const char ** const a_argv);
void f_pathremove( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc, const char ** const a_argv);
void f_pathset( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc, const char ** const a_argv);
void f_sourcerequired( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc, const char ** const a_argv);
void f_sourceoptional( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc, const char ** const a_argv);
void f_sourcereqcheck( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc, const char ** const a_argv);
void f_sourceoptcheck( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc, const char ** const a_argv);
void f_dodefaults( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc, const char ** const a_argv);

#define CHECK_NUM_PARAM(action) \
    if ((a_argc < a_cmd_map->min_params) ||                     \
        (a_argc > a_cmd_map->max_params)) {                     \
      upserr_vplace();                                          \
      upserr_add(UPS_INVALID_ACTION_PARAMS, UPS_FATAL,          \
                 action, a_cmd_map->min_params,                 \
                 a_cmd_map->max_params, a_argc);                \
    }

#define GET_DELIMITER() \
    if (a_argc == a_cmd_map->max_params) {                        \
      /* remember arrays start at 0, so subtract one here */      \
      delimiter = (char *)&a_argv[a_cmd_map->max_params-1];       \
    } else {                                                      \
      /* use the default, nothing was entered */                  \
      delimiter = &g_default_delimiter[0];                        \
    }

#define GET_ERR_MESSAGE(msg_ptr) \
    if (msg_ptr) {                                 \
      err_message = msg_ptr;                       \
    } else {                                       \
      err_message = "";                            \
    }

/*
 * Definition of global variables.
 */

static char g_default_delimiter[2] = ":";

/* This is a list of the actions that can exist that are not UPS commands.
   The list of Ups commands can also be actions but they are defined in an
   enum in ups_main.h */
enum {
  e_invalid_action = -1,
  e_current = e_unk+1,
  e_development,
  e_new,
  e_old,
  e_test,
  e_chain,
  e_uncurrent,
  e_undevelopment,
  e_unnew,
  e_unold,
  e_untest,
  e_unchain
};

enum {
  e_invalid_cmd = -1,
  e_setupoptional = 0,
  e_setuprequired,
  e_unsetupoptional,
  e_unsetuprequired,
  e_envappend,
  e_envremove,
  e_envprepend,
  e_envset,
  e_envunset,
  e_pathappend,
  e_pathremove,
  e_pathprepend,
  e_pathset,
  e_sourcerequired,
  e_sourceoptional,
  e_sourcereqcheck,
  e_sourceoptcheck,
  e_exeaccess,
  e_execute,
  e_filetest,
  e_copyhtml,
  e_copyinfo,
  e_copyman,
  e_uncopyman,
  e_copynews,
  e_dodefaults,
  e_nodefaults,
  e_nosetupenv,
  e_noproddir,
  e_forkactions,
  e_sourceactions
};

/* These action commands are listed in order of use.  Hopefully the more
 * used actions are at the front of the list. Also the ones most used by
 * setup and unsetup are at the front of the array.  The actions in this
 * array MUST appear in the same order in the following enumeration
 */

static t_cmd_map g_cmd_maps[] = {
  { "setupoptional", e_setupoptional, NULL, 0, 0 },
  { "setuprequired", e_setuprequired, NULL, 0, 0 },
  { "unsetupoptional", e_unsetupoptional, NULL, 0, 0 },
  { "unsetuprequired", e_unsetuprequired, NULL, 0, 0 },
  { "envappend", e_envappend, f_envappend, 2, 3 },
  { "envremove", e_envremove, f_envremove, 2, 3 },
  { "envprepend", e_envprepend, f_envprepend, 2, 3 },
  { "envset", e_envset, f_envset, 2, 2 },
  { "envunset", e_envunset, f_envunset, 1, 1 },
  { "pathappend", e_pathappend, f_pathappend, 2, 3 },
  { "pathremove", e_pathremove, f_pathremove, 2, 3 },
  { "pathprepend", e_pathprepend, f_pathprepend, 2, 3 },
  { "pathset", e_pathset, f_pathset, 2, 2 },
  { "sourcerequired", e_sourcerequired, f_sourcerequired, 1, 1 },
  { "sourceoptional", e_sourceoptional, f_sourceoptional, 1, 1 },
  { "sourcereqcheck", e_sourcereqcheck, f_sourcereqcheck, 1, 1 },
  { "sourceoptcheck", e_sourceoptcheck, f_sourceoptcheck, 1, 1 },
  { "exeaccess", e_exeaccess, f_exeaccess, 1, 1 },
  { "execute", e_execute, f_execute, 1, 2 },
  { "filetest", e_filetest, f_filetest, 2, 3 },
  { "copyhtml", e_copyhtml, f_copyhtml, 1, 1 },
  { "copyinfo", e_copyinfo, f_copyinfo, 1, 1 },
  { "copyman", e_copyman, f_copyman, 0, 1 },
  { "uncopyman", e_uncopyman, f_uncopyman, 0, 1 },
  { "copynews", e_copynews, f_copynews, 1, 1 },
  { "dodefaults", e_dodefaults, f_dodefaults, 0, 0 },
  { "nodefaults", e_nodefaults, NULL, 0, 0 },
  { "nosetupenv", e_nosetupenv, NULL, 0, 0 },
  { "noproddir", e_noproddir, NULL, 0, 0 },
  { "forkactions", e_forkactions, NULL, 0, 0 },
  { "sourceactions", e_sourceactions, NULL, 0, 0 },
  { 0,0,0,0,0 }
};

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upsact_get_cmd
 *
 * Input : t_upsugo_command *,
 *         t_upstyp_matched_product *,
 *         char *,
 * Output: none
 * Return: t_upsact_tree *,
 */
t_upslst_item *upsact_get_cmd( t_upsugo_command *ugo_cmd,
			       t_upstyp_matched_product *mat_prod,
			       const char * const act_name )
{
  static char * valid_switch = "AacCdfghKtmMNoOPqrTuUv";
  
  t_upsact_item *new_cur;
  t_upstyp_matched_instance *mat_inst;
  t_upslst_item *l_item;
  t_upstyp_action *p_act;
  t_upslst_item *dep_list = 0;


  t_upslst_item *l_mproduct;

  if ( !ugo_cmd || !act_name )
    return 0;

  if ( !mat_prod ) {
    l_mproduct = upsmat_instance( ugo_cmd, 1 );
    if ( !l_mproduct || !l_mproduct->data )
      return 0;
    mat_prod = (t_upstyp_matched_product *)l_mproduct->data;
  }

  new_cur = (t_upsact_item *)malloc( sizeof( t_upsact_item ) );
  new_cur->indent = 0;
  new_cur->ugo = ugo_cmd;
  new_cur->mat = mat_prod;
  new_cur->cmd = 0;
  p_act = get_act( ugo_cmd, mat_prod, act_name );

  /* get setup for top level product */

  dep_list = next_cmd( p_act, dep_list, new_cur, act_name );

  return upslst_first( dep_list );
}

/*-----------------------------------------------------------------------
 * upsact_parse_cmd
 *
 * It will translate a single action command line.
 * The returned t_upsact_cmd structure is valid untill next call to
 * upsact_parse_cmd.
 *
 * Input : action line.
 * Output: none
 * Return: t_upsact_cmd *, a translated command line
 */
t_upsact_cmd *upsact_parse_cmd( const char * const cmd_str )
{
  static char trim_chars[] = " \t\n\r\f)";
  t_upsact_cmd *pcmd;
  char *act_s = (char *)cmd_str, *act_e = NULL;
  int icmd = e_invalid_action;
  int i;
  int len;

  /* get rid of leading spaces */
  
  while ( act_s && *act_s && isspace( *(act_s) ) ){ ++act_s; }

  if ( !act_s || !*act_s )
    return 0;

  /* create a new command structure */
  
  pcmd = (t_upsact_cmd *)malloc( sizeof( t_upsact_cmd ) );
  pcmd->pmem = (char *)malloc( strlen( act_s ) + 1 );  
  
  strcpy( pcmd->pmem, act_s );
  act_s = pcmd->pmem;

  if ( (act_e = strchr( act_s, OPEN_PAREN )) != NULL ) {
    len = act_e - act_s;

    /* look for action in the supported action array */
    
    for ( i = 0; g_cmd_maps[i].cmd; ++i ) {
      if ( !upsutl_strincmp( act_s, g_cmd_maps[i].cmd, (size_t)len ) ) {
	
	/* we found a match. create a pointer to a string with these parameters.
	   note - it does not include an open parenthesis */
	act_s = act_e + 1;
		 
	/* trim off whitespace & the ending ")" */	  
	upsutl_str_remove_edges( act_s, trim_chars );

        /* save the location in the array */
	icmd = i;
	
	break;
      }
    }
  }

  /* fill p_cmd, split parameter string into a list of arguments */
  
  if ( icmd != e_invalid_action ) {
    pcmd->icmd = icmd;    
    pcmd->argc = parse_params( act_s, pcmd->argv );
    return pcmd;
  }
  else {
    free( pcmd->pmem );
    free( pcmd );
    return 0;
  }
}

void upsact_cleanup( t_upslst_item *dep_list )
{
  /* here you should cleanup dep_list */
}

void upsact_print_item( const t_upsact_item *const p_cur )
{
  t_upstyp_matched_instance *mat_inst;
  t_upstyp_instance *inst;
  int i;
  
  if ( !p_cur )
    return;
  
  mat_inst = (t_upstyp_matched_instance *)(upslst_first( p_cur->mat->minst_list ))->data;
  if ( mat_inst->chain )
    inst = mat_inst->chain;
  else if ( mat_inst->version )
    inst = mat_inst->version;
  else if ( mat_inst->table )
    inst = mat_inst->table;
  else {
    printf( "%s: No matched instance found\n", p_cur->mat->product );
    return;
  }

  for ( i=0; i<p_cur->indent; i++ ) { printf( "\t" ); }
  printf( "%d:( %s, %s, %s, \"%s\" ):", p_cur->indent, inst->product,
	  inst->flavor, inst->version, inst->qualifiers );
  upsact_print_cmd( p_cur->cmd );  
}

void upsact_print_cmd( const t_upsact_cmd * const cmd_cur )
{
  int i;
  int icmd;
  
  if ( !cmd_cur )
    return;

  icmd = cmd_cur->icmd;
  
  printf( "%s(", g_cmd_maps[icmd].cmd );
  for ( i = 0; i < cmd_cur->argc; i++ ) {
    if ( i == cmd_cur->argc - 1 ) 
      printf( " %s ", cmd_cur->argv[i] );
    else
      printf( " %s,", cmd_cur->argv[i] );
  }
  printf( ")\n" ); 
}

/*
 * Private functions
 */

/*-----------------------------------------------------------------------
 * get_act
 *
 * Input : t_upsugo_command *,
 * Output: none
 * Return: t_upsact_action *,
 */
t_upstyp_action *get_act( t_upsugo_command *ugo_cmd,
  	                         t_upstyp_matched_product *mat_prod,
			         char *act_name )
{
  t_upstyp_matched_instance *mat_inst;
  t_upsugo_command *u_cmd;

  if ( !ugo_cmd || !act_name )
    return 0;

  if ( !mat_prod ) {
    t_upslst_item *l_mproduct = upsmat_instance( ugo_cmd, 1 );
    if ( !l_mproduct || !l_mproduct->data )
      return 0;
    mat_prod = (t_upstyp_matched_product *)l_mproduct->data;
  }

  mat_inst = (t_upstyp_matched_instance *)(upslst_first( mat_prod->minst_list ))->data;
  if ( mat_inst->table ) 
    return upskey_inst_getaction( mat_inst->table, act_name );
  else
    return 0;
}

/*-----------------------------------------------------------------------
 * next_cmd
 *
 * Input : t_upsact_tree *,
 * Output: none
 * Return: t_upsact_cmd *,
 */
t_upslst_item *next_cmd( t_upstyp_action *action,
			 t_upslst_item *dep_list,
			 t_upsact_item *p_cur,
			 const char * const act_name )
{
  static char * valid_switch = "AacCdfghKtmMNoOPqrTuUv";
  t_upslst_item *l_cmd = action->command_list;
  t_upsact_cmd *p_cmd = 0;

  for ( ; l_cmd; l_cmd = l_cmd->next ) {
    p_cmd = upsact_parse_cmd( (char *)l_cmd->data );
    if ( p_cmd ) {
      if ( p_cmd->icmd > e_unsetuprequired ) {
	t_upsact_item *new_cur = (t_upsact_item *)malloc( sizeof( t_upsact_item ) );
	new_cur->indent = p_cur->indent;
	new_cur->ugo = p_cur->ugo;
	new_cur->mat = p_cur->mat;
	new_cur->cmd = p_cmd;
	dep_list = upslst_add( dep_list, new_cur );
	continue;
      }
      else if ( p_cmd->icmd >= 0 ) { /* here we go again */
	t_upstyp_action *new_act = 0;
	t_upstyp_matched_product *new_mat = 0;
	t_upslst_item *l_mproduct = 0;
	t_upsact_item *new_cur = 0;
	t_upsugo_command *new_ugo = upsugo_bldcmd( p_cmd->argv[0], valid_switch );
	if ( !new_ugo ) {
	  printf( "???? no new ugo\n" );
	  continue;
	}

	/* check if product is already at first level */

	/* check if product is already in list */
	
	if ( find_product( dep_list, new_ugo->ugo_product ) ) {
	  /* ??? free stuff */
	  continue;
	}
	
	l_mproduct = upsmat_instance( new_ugo, 1 );
	if ( !l_mproduct || !l_mproduct->data ) {
	  printf( "???? no product\n" );
	  continue;
	}
	new_mat = (t_upstyp_matched_product *)l_mproduct->data;
	new_act = get_act( new_ugo, new_mat, act_name );
	if ( !new_act ) {
	  printf( "???? no new act\n" );
	  continue;
	}
	new_cur = (t_upsact_item *)malloc( sizeof( t_upsact_item ) );
	new_cur->indent = p_cur->indent + 1;
	new_cur->ugo = new_ugo;
	new_cur->mat = new_mat;
	new_cur->cmd = 0;
	dep_list = next_cmd( new_act, dep_list, new_cur, act_name );
	continue;
      }
    }
  }
  
  return dep_list;
}

t_upsact_item *find_product( t_upslst_item* dep_list,
			     const char *const prod_name )
{
  t_upslst_item *l_ptr = upslst_first( dep_list );

  for ( ; l_ptr; l_ptr = l_ptr->next ) {
    t_upsact_item *p_item = (t_upsact_item *)l_ptr->data;
    if ( upsutl_stricmp( prod_name, p_item->ugo->ugo_product ) == 0 )
      return p_item;
  }

  return 0;    
}

/*-----------------------------------------------------------------------
 * parse_params
 *
 * Given an action's parameters split them up into separate params and
 * return a linked list of the separate parameters.  The parameters are
 * separated by commas, but ignore commas within quotes.
 *
 * The routine writes into passed parameter string.
 *
 * called by upsact_parse.
 *
 * Input : parameter string
 *         pointer to array of arguments
 * Output: pointer to array of arguments
 * Return: number of arguments found
 */
int parse_params( const char * a_params, char **argv )
{
  char *ptr = (char *)a_params, *saved_ptr = NULL;
  char *new_ptr;
  int count = 0;

  while ( ptr && *ptr ) {

    if ( count >= UPS_MAX_ARGC ) {
      upserr_vplace();
      upserr_add( UPS_TOO_MANY_ACTION_ARG, UPS_FATAL, a_params );
      return 0;
    }
    
    switch ( *ptr ) {
      
    case DQUOTE:    /* found a double quote */
      
      /* this may be the beginning of the line, saved_ptr is not set yet
	 so do it now. */
      
      if ( !saved_ptr )
	saved_ptr = ptr;           /* the beginning of a new parameter */
      
      /* found a double quote, skip to next double quote */
      
      if ( (new_ptr = strchr( ++ptr, (int)DQUOTE) ) == NULL ) {
	
	/* did not find another quote  - take the end of the line as end of
	   string and end of param list */
	
	saved_ptr = ptr;         /* no longer valid, we got the param */
	ptr = NULL;               /* all done */
      }
      else {
	
	/* point string just past double quote */
	
	ptr = ++new_ptr;
      }
      break;
      
    case COMMA:     /* found a comma */       

      if ( saved_ptr ) {
	
	/* we have a param, create a new pointer to the string */
        /* and add it to the list */
	
	*ptr = '\0';
	upsutl_str_remove_edges( saved_ptr, WSPACE );
	argv[count++] = saved_ptr;
	
      }
      ++ptr;                       /* go past the comma */
      saved_ptr = ptr;             /* start of param */
      break;
      
    default:       /* go to next char */
      
      if ( !saved_ptr ) {
	saved_ptr = ptr;           /* the beginning of a new parameter */
      }
      ++ptr;                       /* go to the next character */
    }
  }

  if ( saved_ptr ) {
    
    /* Get the last one too */
    upsutl_str_remove_edges( saved_ptr, WSPACE );
    argv[count++] = saved_ptr;
  }
  
  return count;
}


/* Action handling - the following routines are the ones that output shell
 *   specific code for each action supported by UPS
 */



void f_copyhtml( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv)
{
  CHECK_NUM_PARAM("copyHtml");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->html_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "html");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n", 
		    a_argv[0], a_db_info->config->html_path) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_copyinfo( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv)
{
  CHECK_NUM_PARAM("copyInfo");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->info_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "info");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n", 
		    a_argv[0], a_db_info->config->info_path) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_copyman( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc,
		const char ** const a_argv)
{
  char *buf = NULL;

  CHECK_NUM_PARAM("copyMan");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->man_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "man");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (a_argc == 1) {
	  /* the user specified a source in the action */
	  buf = (char *)a_argv[0];
	} else {
	  /* we have to construct a source */
	  buf = upsutl_find_manpages(a_inst, a_db_info);
	}
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n", 
		    buf, a_db_info->config->man_path) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",strerror(errno));
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_uncopyman( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc,
		  const char ** const a_argv)
{
  char *buf = NULL;
  t_upslst_item *man_item, *man_list;

  CHECK_NUM_PARAM("uncopyMan");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->man_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "man");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (a_argc == 1) {
	  /* the user specified a source in the action (gotten from current
	     action */
	  buf = (char *)a_argv[0];
	} else {
	  /* we have to construct a source */
	  buf = upsutl_find_manpages(a_inst, a_db_info);
	}

	/* Get a list of all the files in the specified directory */
	man_list = upsutl_get_files(buf, ANY_MATCH);

	for (man_item = man_list ; man_item ; man_item = man_item->next) {
	  if (fprintf((FILE *)a_stream, "rm %s/%s\n", 
		      a_db_info->config->man_path, (char *)man_item->data)
	      < 0) {
	    upserr_vplace();
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
		       strerror(errno));
	    break;
	  }
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_copynews( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv)
{
  CHECK_NUM_PARAM("copyNews");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->news_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "news");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n", 
		    a_argv[0], a_db_info->config->news_path) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_envappend( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc,
		  const char ** const a_argv)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("envAppend");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=\${%s-}%s%s;export %s\n", a_argv[0],
		  a_argv[0], delimiter, a_argv[1], a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s \${%s}%s%s\n", a_argv[0],
		  a_argv[0], delimiter, a_argv[1]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_envprepend( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc,
		   const char ** const a_argv)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("envPrepend");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s%s\${%s-};export %s\n", a_argv[0],
		  a_argv[1], delimiter, a_argv[0], a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s %s%s\${%s}\n", a_argv[0],
		  a_argv[1], delimiter, a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_envremove( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc,
		  const char ** const a_argv)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("envRemove");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream,
		  "upstmp=`dropit.pl %s %s %s`;\nif [ $? -eq 0 ]; then %s=$upstmp; fi\nunset upstmp;\n",
		  a_argv[0], a_argv[1], delimiter, a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream,
		  "setenv upstmp \"`dropit.pl %s %s %s`\"\nif ($status == 0) setenv %s $upstmp\nunsetenv upstmp\n",
		  a_argv[0], a_argv[0], delimiter, a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_envset( const t_upstyp_matched_instance * const a_inst,
	       const t_upstyp_db * const a_db_info,
	       const t_upsugo_command * const a_command_line,
	       const FILE * const a_stream,
	       const struct s_cmd_map * const a_cmd_map,
	       const int a_argc,
	       const char ** const a_argv)
{
  CHECK_NUM_PARAM("envSet");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s;export %s\n", a_argv[0], a_argv[1],
		  a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s %s\n", a_argv[0], a_argv[1])
	  < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_envunset( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv)
{
  CHECK_NUM_PARAM("envUnset");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "unset %s\n", a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "unsetenv %s\n", a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_exeaccess( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc,
		  const char ** const a_argv)
{
  CHECK_NUM_PARAM("exeAccess");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream,
		  "hash %s;\nif [ $? -eq 1 ]; then return 1; fi\n",
		  a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "whereis %s\nif ($status == 1) return 1\n",
		  a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_execute( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc,
		const char ** const a_argv)
{
  CHECK_NUM_PARAM("execute");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=`%s`;export %s\n", a_argv[1],
		  a_argv[0], a_argv[1]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s \"`%s`\"\n", a_argv[1],
		  a_argv[0])< 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_filetest( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv)
{
  char *err_message;
  
  CHECK_NUM_PARAM("fileTest");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct error message */
    GET_ERR_MESSAGE((char *)a_argv[a_cmd_map->max_params-1]);

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream,
		  "if [ ! %s %s ]; then\necho %s;\nreturn 1;\nfi;\n",
		  a_argv[1], a_argv[0], err_message) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "if ( ! %s %s ) return 1\n", 
		  a_argv[1], a_argv[0], err_message) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_pathappend( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc,
		   const char ** const a_argv)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("pathAppend");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=${%s-}%s%s;export %s\n", a_argv[0],
		  a_argv[0], delimiter, a_argv[1], a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "set %s=($%s %s)\nrehash\n", a_argv[0],
		  a_argv[0], a_argv[1]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_pathprepend( const t_upstyp_matched_instance * const a_inst,
		    const t_upstyp_db * const a_db_info,
		    const t_upsugo_command * const a_command_line,
		    const FILE * const a_stream,
		    const struct s_cmd_map * const a_cmd_map,
		    const int a_argc,
		    const char ** const a_argv)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("pathPrepend");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s%s\${%s-};export %s\n", a_argv[0],
		  a_argv[1], delimiter, a_argv[0], a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "set %s=(%s $%s)\nrehash\n", a_argv[0],
		  a_argv[1], a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_pathremove( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc,
		   const char ** const a_argv)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("pathRemove");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream,
		  "upstmp=`dropit.pl %s %s %s`;\nif [ $? -eq 0 ]; then %s=$upstmp; fi\nunset upstmp;\n",
		  a_argv[0], a_argv[1], delimiter, a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream,
		  "setenv upstmp \"`dropit.pl %s %s %s`\"\nif ($status == 0) set %s=$upstmp\nrehash\nunsetenv upstmp\n",
		  a_argv[0], a_argv[0], delimiter, a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_pathset( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc,
		const char ** const a_argv)
{
  CHECK_NUM_PARAM("pathSet");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s;export %s\n", a_argv[0], a_argv[1],
		  a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "set %s=(%s)\nrehash\n", a_argv[0],
		  a_argv[1]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_sourcerequired( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc,
		       const char ** const a_argv)
{
  CHECK_NUM_PARAM("sourceRequired");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* define all of the UPS local variables that the user may need. */
    upsget_allout(a_stream, a_db_info, a_inst, a_command_line);
    if (UPS_ERROR == UPS_SUCCESS) {
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
	if (fprintf((FILE *)a_stream, ". %s;\n", a_argv[0]) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "source %s\n", a_argv[0]) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
    } else {
      upserr_vplace();
      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_sourceoptional( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc,
		       const char ** const a_argv)
{
  CHECK_NUM_PARAM("sourceOptional");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "if [ -s %s ]; then\n", a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      } else {
	if (fprintf((FILE *)a_stream,
		    "if [ ! -r %s -o ! -x %s]; then\n  echo File to be optionally sourced (%s) is not readable or not executable;\nelse\n",
		    a_argv[0], a_argv[0], a_argv[0]) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	} else {
	  /* define all of the UPS local variables that the user may need. */
	  upsget_allout(a_stream, a_db_info, a_inst, a_command_line);
	  if (UPS_ERROR == UPS_SUCCESS) {
	    if (fprintf((FILE *)a_stream, "  . %s;\n;\nfi;\nfi;\n", 
			a_argv[0]) < 0) {
	      upserr_vplace();
	      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
			 strerror(errno));
	    }
	  } else {
	    upserr_vplace();
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
		       strerror(errno));
	  }
	}
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "if (-e %s) then\n\n", a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      } else {
	if (fprintf((FILE *)a_stream,
		    "if (! -r %s || ! -x %s) then\n  echo File to be optionally sourced (%s) is not readable or not executable\nelse\n", 
		  a_argv[0], a_argv[0], a_argv[0]) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	} else {
	  /* define all of the UPS local variables that the user may need. */
	  upsget_allout(a_stream, a_db_info, a_inst, a_command_line);
	  if (UPS_ERROR == UPS_SUCCESS) {
	    if (fprintf((FILE *)a_stream, "  source %s\n\nendif\nendif\n", 
			a_argv[0]) < 0) {
	      upserr_vplace();
	      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
			 strerror(errno));
	    }
	  } else {
	    upserr_vplace();
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
		       strerror(errno));
	  }
	}
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_sourcereqcheck( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc,
		       const char ** const a_argv)
{
  CHECK_NUM_PARAM("sourceReqCheck");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
    /* define all of the UPS local variables that the user may need. */
    upsget_allout(a_stream, a_db_info, a_inst, a_command_line);
    if (UPS_ERROR == UPS_SUCCESS) {
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
	if (fprintf((FILE *)a_stream,
		    ". %s;\nif [ $? -eq 1 ]; then return 1; fi;\n",
		    a_argv[0]) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      case e_CSHELL:
	if (fprintf((FILE *)a_stream,
		    "source %s\nif ($status == 1) return 1/n",
		    a_argv[0]) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
    } else {
      upserr_vplace();
      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_sourceoptcheck( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc,
		       const char ** const a_argv)
{
  CHECK_NUM_PARAM("sourceOptCheck");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "if [ -s %s ]; then\n", a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      } else {
	if (fprintf((FILE *)a_stream,
		    "if [ ! -r %s -o ! -x %s]; then\n  echo File to be optionally sourced (%s) is not readable or not executable;\nelse\n",
		    a_argv[0], a_argv[0], a_argv[0]) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	} else {
	  /* define all of the UPS local variables that the user may need. */
	  upsget_allout(a_stream, a_db_info, a_inst, a_command_line);
	  if (UPS_ERROR == UPS_SUCCESS) {
	    if (fprintf((FILE *)a_stream,
			"  . %s;\n  if [ $? -eq 1 ]; then return 1; fi;\nfi;\nfi;\n", 
			a_argv[0]) < 0) {
	      upserr_vplace();
	      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
			 strerror(errno));
	    }
	  } else {
	    upserr_vplace();
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
		       strerror(errno));
	  }
	}
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "if (-e %s) then\n\n", a_argv[0]) < 0) {
	upserr_vplace();
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      } else {
	if (fprintf((FILE *)a_stream,
		    "if (! -r %s || ! -x %s) then\n  echo File to be optionally sourced (%s) is not readable or not executable\nelse\n", 
		  a_argv[0], a_argv[0], a_argv[0]) < 0) {
	  upserr_vplace();
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	} else {
	  /* define all of the UPS local variables that the user may need. */
	  upsget_allout(a_stream, a_db_info, a_inst, a_command_line);
	  if (UPS_ERROR == UPS_SUCCESS) {
	    if (fprintf((FILE *)a_stream,
			"  source %s\n  if ($status == 1) return 1/nendif\nendif\nendif\n", 
			a_argv[0]) < 0) {
	      upserr_vplace();
	      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
			 strerror(errno));
	    }
	  } else {
	    upserr_vplace();
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf",
		       strerror(errno));
	  }
	}
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_dodefaults( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc,
		   const char ** const a_argv)
{
  int dummy = 0;

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( dummy /* place command enum here */ ) {
    case e_setup:	/* Define <PROD>_DIR and SETUP_<PROD> */
      f_pathset(a_inst, a_db_info, a_command_line, a_stream, a_cmd_map, a_argc,
		a_argv);
      f_envset(a_inst, a_db_info, a_command_line, a_stream, a_cmd_map, a_argc,
	       a_argv);
      break;
    case e_chain:	/* None */
      break;
    case e_configure:	/* None */
      break;
    case e_copy:    	/* None */
      break;
    case e_create:	/* None */
      break;
    case e_current:     /* Copy man pages to man page area in dbconfig file */
      f_copyman(a_inst, a_db_info, a_command_line, a_stream, a_cmd_map, a_argc, a_argv);
      break;
    case e_declare:	/* None */
      break;
    case e_depend:	/* None */
      break;
    case e_development:	/* None */
      break;
    case e_exist:	/* None */
      break;
    case e_get: 	/* None */
      break;
    case e_list:	/* None */
      break;
    case e_modify:	/* None */
      break;
    case e_new :	/* None */
      break;
    case e_old:   	/* None */
      break;
    case e_start:	/* None */
      break;
    case e_stop:	/* None */
      break;
    case e_tailor:	/* None */
      break;
    case e_test:	/* None */
      break;
    case e_unchain:	/* None */
      break;
    case e_unconfigure:	/* None */
      break;
    case e_uncurrent:   /* Remove the man pages from the man page area */
      f_uncopyman(a_inst, a_db_info, a_command_line, a_stream, a_cmd_map,
		  a_argc, a_argv);
      break;
    case e_undeclare:	/* None */
      break;
    case e_undevelopment:	/* None */
      break;
    case e_unk:	/* None */
      break;
    case e_unnew:	/* None */
      break;
    case e_unold:	/* None */
      break;
    case e_unsetup:
      break;
    case e_untest:	/* None */
      break;
    case e_validate:	/* None */
      break;
    default:
      break;
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

