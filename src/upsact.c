/************************************************************************
 *
 * FILE:
 *       upsact.c
 * 
 * DESCRIPTION: 
 *       This file contains routines to manage ups action lines.
 *       *** It need to be reentrant ***
 *
 *       Steps in order to add a new action -
 *           1. add a f_<action> prototype at the top of the file
 *           2. add an e_<action> to the enum below
 *           3. add a line to the g_cmd_maps structure
 *           4. add the code for the f_<action> function
 *           5. check if you have to add anything to upsact_check_files
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
#include <errno.h>

/* ups specific include files */
#include "upsact.h"
#include "upserr.h"
#include "upsver.h"
#include "upslst.h"
#include "upsutl.h"
#include "upsmem.h"
#include "upskey.h"
#include "upsugo.h"
#include "upsget.h"

/*
 * Definition of public variables.
 */
int g_LOCAL_VARS_DEF = 0;
int g_keep_temp_file = 0;
char *g_temp_file_name = NULL;
int g_COMPILE_FLAG = 0;

/*
 * Private constants
 */

#define DQUOTE '"'
#define COMMA ','
#define OPEN_PAREN '('
#define CLOSE_PAREN ')'
#define SLASH "/"
#define WSPACE " \t\n\r\f\""
#define EXIT "EXIT"
#define CONTINUE "CONTINUE"
#define UPS_ENV "UPS_ENV"
#define NO_UPS_ENV "NO_UPS_ENV"
#define DO_CHECK 1
#define NO_CHECK 0
#define NO_EXIT 0
#define DO_EXIT 1
#define DO_NO_UPS_ENV 1
#define DO_UPS_ENV 0
#define DATE_FLAG "DATE"
#define OLD_FLAG "OLD"


/*
 * Private types
 */

/*
 * Declaration of private functions.
 */

int parse_params( const char * const a_params,
		   char *argv[] );

t_upslst_item *next_cmd( t_upslst_item * const top_list,
			 t_upslst_item *dep_list,
			 t_upsact_item * const p_cur,
			 const char * const act_name );
t_upslst_item *get_top_prod( t_upsact_item *const p_cur, 
			     const char *const act_name );
t_upstyp_action *get_act( const t_upsugo_command * const ugo_cmd,
			  t_upstyp_matched_product * mat_prod,
			  const char * const act_name );
t_upsact_item *find_product_str( t_upslst_item *const dep_list,
			     const char *const prod_name );
t_upsact_item *find_product_ptr( t_upslst_item* const dep_list,
				 const t_upsact_item* const act_item );
t_upsact_item *new_act_item( t_upsugo_command * const ugo_cmd,
			     t_upstyp_matched_product *mat_prod,
			     const int level,
			     const char * const act_name );
t_upstyp_action *new_default_action( t_upsact_item *const p_cur, 
				     const char * const act_name, 
				     const int iact );
t_upslst_item *reverse_command_list( t_upsact_item *const p_cur,
				     t_upslst_item *const cmd_list );
int actname2enum( const char * const act_name );
char *actitem2inststr( const t_upsact_item *const p_cur );
int dbl2dbs( char * const db_name, t_upslst_item * const l_db );
t_upsugo_command *get_SETUP_prod( t_upsact_cmd * const p_cmd, const int i_act );
int lst_cmp_str( t_upslst_item * const l1, t_upslst_item * const l2 );
void trim_delimiter( char * str );

/* functions to handle specific action commands */

static void f_copyhtml( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd);
static void f_copyinfo( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd);
static void f_copyman( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const t_upsact_cmd * const a_cmd);
static void f_uncopyman( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const t_upsact_cmd * const a_cmd);
static void f_copynews( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd);
static void f_envappend( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const t_upsact_cmd * const a_cmd);
static void f_envprepend( const t_upstyp_matched_instance * const a_inst,
			  const t_upstyp_db * const a_db_info,
			  const t_upsugo_command * const a_command_line,
			  const FILE * const a_stream,
			  const t_upsact_cmd * const a_cmd);
static void f_envremove( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const t_upsact_cmd * const a_cmd);
static void f_envset( const t_upstyp_matched_instance * const a_inst,
		      const t_upstyp_db * const a_db_info,
		      const t_upsugo_command * const a_command_line,
		      const FILE * const a_stream,
		      const t_upsact_cmd * const a_cmd);
static void f_envunset( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd);
static void f_exeaccess( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const t_upsact_cmd * const a_cmd);
static void f_execute( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const t_upsact_cmd * const a_cmd);
static void f_filetest( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd);
static void f_makedir( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const t_upsact_cmd * const a_cmd);
static void f_pathappend( const t_upstyp_matched_instance * const a_inst,
			  const t_upstyp_db * const a_db_info,
			  const t_upsugo_command * const a_command_line,
			  const FILE * const a_stream,
			  const t_upsact_cmd * const a_cmd);
static void f_pathprepend( const t_upstyp_matched_instance * const a_inst,
			   const t_upstyp_db * const a_db_info,
			   const t_upsugo_command * const a_command_line,
			   const FILE * const a_stream,
			   const t_upsact_cmd * const a_cmd);
static void f_pathremove( const t_upstyp_matched_instance * const a_inst,
			  const t_upstyp_db * const a_db_info,
			  const t_upsugo_command * const a_command_line,
			  const FILE * const a_stream,
			  const t_upsact_cmd * const a_cmd);
static void f_pathset( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const t_upsact_cmd * const a_cmd);
static void f_sourcerequired( const t_upstyp_matched_instance * const a_inst,
			      const t_upstyp_db * const a_db_info,
			      const t_upsugo_command * const a_command_line,
			      const FILE * const a_stream,
			      const t_upsact_cmd * const a_cmd);
static void f_sourceoptional( const t_upstyp_matched_instance * const a_inst,
			      const t_upstyp_db * const a_db_info,
			      const t_upsugo_command * const a_command_line,
			      const FILE * const a_stream,
			      const t_upsact_cmd * const a_cmd);
static void f_sourcereqcheck( const t_upstyp_matched_instance * const a_inst,
			      const t_upstyp_db * const a_db_info,
			      const t_upsugo_command * const a_command_line,
			      const FILE * const a_stream,
			      const t_upsact_cmd * const a_cmd);
static void f_sourceoptcheck( const t_upstyp_matched_instance * const a_inst,
			      const t_upstyp_db * const a_db_info,
			      const t_upsugo_command * const a_command_line,
			      const FILE * const a_stream,
			      const t_upsact_cmd * const a_cmd);
static void f_sourcecompilereq( const t_upstyp_matched_instance * const a_inst,
				const t_upstyp_db * const a_db_info,
				const t_upsugo_command * const a_command_line,
				const FILE * const a_stream,
				const t_upsact_cmd * const a_cmd);
static void f_sourcecompileopt( const t_upstyp_matched_instance * const a_inst,
				const t_upstyp_db * const a_db_info,
				const t_upsugo_command * const a_command_line,
				const FILE * const a_stream,
				const t_upsact_cmd * const a_cmd);
static void f_writecompilescript(
			       const t_upstyp_matched_instance * const a_inst,
			       const t_upstyp_db * const a_db_info,
			       const t_upsugo_command * const a_command_line,
			       const FILE * const a_stream,
			       const t_upsact_cmd * const a_cmd);
static void f_dodefaults( const t_upstyp_matched_instance * const a_inst,
			  const t_upstyp_db * const a_db_info,
			  const t_upsugo_command * const a_command_line,
			  const FILE * const a_stream,
			  const t_upsact_cmd * const a_cmd);

#define CHECK_NUM_PARAM(action) \
    if ((a_cmd->argc < g_cmd_maps[a_cmd->icmd].min_params) ||   \
        (a_cmd->argc > g_cmd_maps[a_cmd->icmd].max_params)) {   \
      upserr_vplace();                                          \
      upserr_add(UPS_INVALID_ACTION_PARAMS, UPS_FATAL,          \
                 action, g_cmd_maps[a_cmd->icmd].min_params,    \
                 g_cmd_maps[a_cmd->icmd].max_params,            \
		 a_cmd->argc);                                  \
    }

#define GET_DELIMITER() \
    if (a_cmd->argc == g_cmd_maps[a_cmd->icmd].max_params) {            \
      /* remember arrays start at 0, so subtract one here */            \
      delimiter =                                                       \
	(a_cmd->argv[g_cmd_maps[a_cmd->icmd].max_params-1]);            \
      /* trim delimiter for quotes */                                   \
      trim_delimiter( delimiter );                                      \
    } else {                                                            \
      /* use the default, nothing was entered */                        \
      delimiter = g_default_delimiter;                                  \
    }

#define GET_ERR_MESSAGE(msg_ptr) \
    if (msg_ptr) {                                 \
      err_message = msg_ptr;                       \
    } else {                                       \
      err_message = "";                            \
    }

#define FPRINTF_ERROR() \
    upserr_vplace(); \
    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));

#define GET_FLAGS() \
    if (a_cmd->argc > g_cmd_maps[a_cmd->icmd].min_params) {                \
      int i;                                                               \
      /* we have more than the minimum number of params, must be flags */  \
      for (i = 1 ; i < a_cmd->argc ; ++i) {                                \
	if (! strcmp(a_cmd->argv[i], EXIT)) {                              \
	  exit_flag = DO_EXIT;                                             \
	  continue;                                                        \
	}                                                                  \
	if (! strcmp(a_cmd->argv[i], NO_UPS_ENV)) {                        \
	  no_ups_env_flag = DO_NO_UPS_ENV;                                 \
	}                                                                  \
      }                                                                    \
    }

#define SH_OUTPUT_LAST_PART_OPT() \
   if (fprintf((FILE *)a_stream, "fi\n#\n") < 0) {  \
     FPRINTF_ERROR();                               \
   }

#define CSH_OUTPUT_LAST_PART_OPT() \
   if (fprintf((FILE *)a_stream, "endif\n#\n") < 0) {  \
     FPRINTF_ERROR();                                  \
   }

#define DO_SYSTEM_MOVE(move_flag)  \
   if (system(buff) != 0) {                                               \
     /* error from system call */                                         \
     upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "system", strerror(errno));  \
   } else {                                                               \
     move_flag = 1;                                                       \
   }

#define SET_PARSE_ERROR( str ) \
    upserr_vplace(); \
    upserr_add(UPS_ACTION_PARSE, UPS_FATAL, str );

#define P_VERB_s( iver, str ) \
  if( UPS_VERBOSE ) upsver_mes( iver, "UPSACT: %s\n", \
				str )

#define P_VERB_s_s( iver, str1, str2 ) \
  if( UPS_VERBOSE ) upsver_mes( iver, "UPSACT: %s %s\n", \
				str1, str2 )
  
#define P_VERB_s_s_i( iver, str1, str2, inum ) \
  if( UPS_VERBOSE ) upsver_mes( iver, "UPSACT: %s %s %d\n", \
				str1, str2, inum )
     

/*
 * Definition of global variables.
 */

static char *g_default_delimiter = ":";
static int g_ups_cmd = e_invalid_action;

/* 
 * Note: This array should in princip be in ups_main.c, but since
 * ups_main is not part of the ups library, it's here.
 *
 * The enum is defined in ups_main.h
 * where flag:
 * <byte>: <description>
 *   0   : specify if that action has a default set of commands
 */

t_cmd_info g_cmd_info[] = {
  {e_current,       "current", 0, 0x00000001, e_invalid_action},
  {e_development,   "development", 0, 0x00000000, e_invalid_action},
  {e_new,           "new", 0, 0x00000000, e_invalid_action},
  {e_old,           "old", 0, 0x00000000, e_invalid_action},
  {e_test,          "test", 0, 0x00000000, e_invalid_action},
  {e_chain,         "chain", 0, 0x00000000, e_invalid_action},
  {e_uncurrent,     "uncurrent", 0, 0x00000001, e_invalid_action},
  {e_undevelopment, "undevelopment", 0, 0x00000000, e_invalid_action},
  {e_unnew,         "unnew", 0, 0x00000000, e_invalid_action},
  {e_unold,         "unold", 0, 0x00000000, e_invalid_action},
  {e_untest,        "untest", 0, 0x00000000, e_invalid_action},
  {e_unchain,       "unchain", 0, 0x00000000, e_invalid_action},
  {e_setup,       "setup",       "?B:cde:f:g:H:jkm:M:noO:q:r:tU:vVz:Z", 0x00000001, e_invalid_action},
  {e_unsetup,     "unsetup",     "?cde:f:g:H:jm:M:noO:q:tU:vVz:Z", 0x00000001, e_setup},
  {e_list,        "list",        "a?cdf:g:h:H:K:lm:M:noq:r:tU:vVz:Z0123", 0x00000000, e_invalid_action},
  {e_configure,   "configure",   "?cdf:g:H:m:M:noO:q:r:tU:vVz:Z", 0x00000000, e_invalid_action},
  {e_copy,        "copy",        "?A:cCdf:g:H:m:M:noO:p:q:r:tT:U:vVWXz:Z", 0x00000000, e_invalid_action},
  {e_declare,     "declare",     "?A:b:cCdD:f:g:H:m:M:noO:p:q:r:tT:u:U:vVz:Z0123", 0x00000000, e_invalid_action},
  {e_depend,      "depend",      "?cdotg:f:H:K:m:M:q:r:U:vVz:Z", 0x00000000, e_invalid_action},
  {e_exist,       "exist",       "?B:cde:f:g:H:jkm:M:oO:q:r:tU:vVz:Z", 0x00000000, e_invalid_action},
  {e_modify,      "modify",      "?A:Ef:H:m:M:op:q:r:T:U:vVx:z:Z", 0x00000000, e_invalid_action},
  {e_start,       "start",       "?cdf:g:H:m:M:noO:q:r:tU:vVwz:Z", 0x00000000, e_invalid_action},
  {e_stop,        "stop",        "?cdf:g:H:m:M:noO:q:r:tU:vVz:Z", 0x00000000, e_invalid_action},
  {e_tailor,      "tailor",      "?cdf:g:h:H:K:m:M:noO:q:r:tU:vVz:Z", 0x00000000, e_invalid_action},
  {e_unconfigure, "unconfigure", "?cdf:g:H:m:M:noO:q:r:tU:vVz:Z", 0x00000000, e_invalid_action},
  {e_undeclare,   "undeclare",   "?cdf:g:H:m:M:noO:q:r:tU:vVyYz:Z0123", 0x00000000, e_invalid_action},
  {e_create,      "create",      "?f:H:m:M:p:q:vZ", 0x00000000, e_invalid_action},
  {e_get,         "get",         "?cdf:Fg:H:m:M:noq:r:tU:vVz:Z", 0x00000000, e_invalid_action},
  {e_validate,    "validate",    "?cdf:g:h:H:lm:M:nNoq:r:StU:vVz:Z", 0x00000000, e_invalid_action},
  {e_flavor,      "flavor",      "?f:H:l0123", 0x00000000, e_invalid_action},
  {e_help,        "help",
            "a?A:b:B:cCdD:eEf:Fg:h:H:jkK:lm:M:nNoO:p:q:r:StT:u:U:vVwW:x:XyYz:Z", 0x00000000, e_invalid_action},
  /* the following one must always be at the end and contains all options */
  {e_unk,         NULL,
            "a?A:b:B:cCdD:eEf:Fg:h:H:jkK:lm:M:nNoO:p:q:r:StT:u:U:vVwW:x:XyYz:Z", 0x00000000, e_invalid_action}
};

/* These action commands are listed in order of use.  Hopefully the more
 * used actions are at the front of the list. Also the ones most used by
 * setup and unsetup are at the front of the array.  The actions in this
 * array MUST appear in the same order as in the above enumeration. NOTE:
 * the 4th and 5th parameters are the minimum and maximum parameters expected
 * by the action.
 */
static t_cmd_map g_cmd_maps[] = {
  { "setupoptional", e_setupoptional, NULL, 0, 0, e_unsetupoptional },
  { "setuprequired", e_setuprequired, NULL, 0, 0, e_unsetuprequired },
  { "unsetupoptional", e_unsetupoptional, NULL, 0, 0, e_setupoptional },
  { "unsetuprequired", e_unsetuprequired, NULL, 0, 0, e_setuprequired },
  { "sourcecompilereq", e_sourcecompilereq, f_sourcecompilereq, 1, 1, e_invalid_cmd },
  { "sourcecompileopt", e_sourcecompileopt, f_sourcecompileopt, 1, 1, e_invalid_cmd },
  { "envappend", e_envappend, f_envappend, 2, 3, e_envremove },
  { "envremove", e_envremove, f_envremove, 2, 3, e_invalid_cmd },
  { "envprepend", e_envprepend, f_envprepend, 2, 3, e_envremove },
  { "envset", e_envset, f_envset, 2, 2, e_envunset },
  { "envunset", e_envunset, f_envunset, 1, 1, e_invalid_cmd },
  { "pathappend", e_pathappend, f_pathappend, 2, 3, e_pathremove },
  { "pathremove", e_pathremove, f_pathremove, 2, 3, e_invalid_cmd },
  { "pathprepend", e_pathprepend, f_pathprepend, 2, 3, e_pathremove },
  { "pathset", e_pathset, f_pathset, 2, 2, e_envunset },
  { "sourcerequired", e_sourcerequired, f_sourcerequired, 1, 3, e_invalid_cmd },
  { "sourceoptional", e_sourceoptional, f_sourceoptional, 1, 3, e_invalid_cmd },
  { "sourcereqcheck", e_sourcereqcheck, f_sourcereqcheck, 1, 3, e_invalid_cmd },
  { "sourceoptcheck", e_sourceoptcheck, f_sourceoptcheck, 1, 3, e_invalid_cmd },
  { "exeaccess", e_exeaccess, f_exeaccess, 1, 1, e_invalid_cmd },
  { "execute", e_execute, f_execute, 1, 2, e_invalid_cmd },
  { "filetest", e_filetest, f_filetest, 2, 3, e_invalid_cmd },
  { "makedir", e_makedir, f_makedir, 1, 1, e_invalid_cmd },
  { "copyhtml", e_copyhtml, f_copyhtml, 1, 1, e_invalid_cmd },
  { "copyinfo", e_copyinfo, f_copyinfo, 1, 1, e_invalid_cmd },
  { "copyman", e_copyman, f_copyman, 0, 1, e_uncopyman },
  { "uncopyman", e_uncopyman, f_uncopyman, 0, 1, e_copyman},
  { "copynews", e_copynews, f_copynews, 1, 1, e_invalid_cmd },
  { "writecompilescript", e_writecompilescript, f_writecompilescript, 2, 3, e_invalid_cmd },
  { "dodefaults", e_dodefaults, f_dodefaults, 0, 0, e_dodefaults },
  { "nodefaults", e_nodefaults, NULL, 0, 0, e_nodefaults },
  { "nosetupenv", e_nosetupenv, NULL, 0, 0, e_invalid_cmd },
  { "noproddir", e_noproddir, NULL, 0, 0, e_invalid_cmd },
  { 0,0,0,0,0 }
};

/*=======================================================================
 *
 * Public functions
 *
 *=====================================================================*/

/*-----------------------------------------------------------------------
 * upsact_print
 *
 * will call ups_get_cmd to get list of dependencies of a product, and 
 * print it to stdout.
 *
 * Input : t_upsugo_command *, a ugo_command
 *         t_upstyp_matched_product *, a matched product (can be null).
 *         char *, name of action.
 *         int, enum of ups command.
 *         char *, options, 'a': it will print all action commands for all instances,
 *                               default is to only print instances.
 *                          'l': will print more information on each line.
 *                          't': will indent line corresponding to level in dependency
 *                               list.
 * Output: none
 * Return: t_upsact_tree *,
 */
int upsact_print( t_upsugo_command * const ugo_cmd,
		  t_upstyp_matched_product *mat_prod,
		  const char * const act_name,
		  int ups_cmd,
		  char * sopt )
{
  static char s_sopt[] = "";
  t_upslst_item *dep_list = 0;

  if ( !sopt )
    sopt = s_sopt;

  if ( !ugo_cmd || !act_name )
    return 0;

  /* get depency list */

  dep_list = upsact_get_cmd( ugo_cmd, mat_prod, act_name, ups_cmd );

  /* option a: print all action commands for all instances */

  if ( strchr( sopt, 'a' ) ) {
    for ( ; dep_list; dep_list = dep_list->next )
      upsact_print_item( (t_upsact_item *)dep_list->data, sopt );
  }

  /* print only instances,
     we need to keep a list of already printed instances */

  else {
    t_upsugo_command *cur_ugo = 0;
    t_upslst_item *didit_list = 0;
    t_upsact_item *act0 = find_product_str( dep_list, ugo_cmd->ugo_product );

    /* add parent product to 'already done list' */

    if ( act0 )
      didit_list = upslst_add( didit_list, act0 );

    for ( ; dep_list; dep_list = dep_list->next ) {
      t_upsact_item *act_ptr = (t_upsact_item *)dep_list->data;
      if ( act_ptr->ugo == cur_ugo )
	continue;
      if ( ! find_product_ptr( didit_list, act_ptr ) ) {
	didit_list = upslst_add( didit_list, act_ptr );
	upsact_print_item( act_ptr, sopt );
	cur_ugo = act_ptr->ugo;
      }
    }    
  }

  return 1;
}

/*-----------------------------------------------------------------------
 * upsact_get_cmd
 *
 * Will get list of dependencies for a product.
 *
 * Input : t_upsugo_command *, a ugo_command.
 *         t_upstyp_matched_product *, a matched product (can be null).
 *         char *, action name.
 *         int, enum of ups command.
 * Output: none
 * Return: t_upslst_item *, a list of t_upsact_item's.
 */
t_upslst_item *upsact_get_cmd( t_upsugo_command * const ugo_cmd,
			       t_upstyp_matched_product * const mat_prod,
			       const char * const act_name,
			       int ups_cmd )
{
  t_upsact_item *new_cur;
  t_upslst_item *top_list = 0;
  t_upslst_item *dep_list = 0;

  g_ups_cmd = ups_cmd;

  if ( !ugo_cmd || !act_name )
    return 0;

  /* create a partial action structure for top product */

  if ( !(new_cur = new_act_item( ugo_cmd, mat_prod, 0, act_name )) ) {
    upserr_vplace();
    upserr_add( UPS_NO_PRODUCT_FOUND, UPS_FATAL, ugo_cmd->ugo_product );
    return 0;
  }

  /* create a list of 1'st level dependecies,
     they have precedence over products at other levels */

  top_list = get_top_prod( new_cur, act_name );
  
  /* get the list, next_cmd is recursive */

  dep_list = next_cmd( top_list, dep_list, new_cur, act_name );

  return upslst_first( dep_list );
}

/*-----------------------------------------------------------------------
 * upsact_parse_cmd
 *
 * It will translate a single action command line.
 * The returned t_upsact_cmd structure is allocated on the free store.
 *
 * Input : action line.
 * Output: none
 * Return: t_upsact_cmd *, a translated command line
 */
t_upsact_cmd *upsact_parse_cmd( const char * const cmd_str )
{
  static char trim_chars[] = " \t\n\r\f)";
  t_upsact_cmd *pcmd;
  char *act_s = (char *)cmd_str, *act_e = NULL, *act_p = NULL;
  int icmd = e_invalid_action;
  int i;
  int len;

  /* get rid of leading spaces */
  
  while ( act_s && *act_s && isspace( *(act_s) ) ){ ++act_s; }

  P_VERB_s_s( 3, "Parsing line:", act_s );
  
  if ( !act_s || !*act_s )
    return 0;

  /* create a new command structure, with space for the command string.
     the created argv array is then (just) a list of pointers into that string. */
  
  pcmd = (t_upsact_cmd *)malloc( sizeof( t_upsact_cmd ) + strlen( act_s ) + 1 );
  pcmd->pmem = (char *)(pcmd + 1);
  
  strcpy( pcmd->pmem, act_s );
  act_s = pcmd->pmem;

  /* get name of action command */

  if ( (act_p = strchr( act_s, OPEN_PAREN )) != NULL ) {

    /* save pointer to parenthese and
       get rid of trailing spaces in name of action command */

    act_e = act_p;
    while ( act_e && *act_e && isspace( *(act_e) ) ){ --act_e; }    
    len = act_e - act_s;

    /* look for action in the supported action array */
    
    for ( i = 0; g_cmd_maps[i].cmd; ++i ) {
      if ( !upsutl_strincmp( act_s, g_cmd_maps[i].cmd, (size_t)len ) ) {
	
	/* we found a match. create a pointer to a string with these parameters.
	   note - it does not include an open parenthesis */

	act_s = act_p + 1;
		 
	/* trim off whitespace & the ending ")" */

	upsutl_str_remove_edges( act_s, trim_chars );

        /* save the location in the array */

	icmd = i;
	
	break;
      }
    }
  }

  /* split parameter string into a list of arguments (fill argv) */
  
  if ( icmd != e_invalid_action ) {
    pcmd->icmd = icmd;
    pcmd->argc = parse_params( act_s, pcmd->argv );
    P_VERB_s_s_i( 3, "Parse result #arg:", g_cmd_maps[icmd].cmd, pcmd->argc );
    return pcmd;
  }
  else {
    free( pcmd );
    P_VERB_s( 3, "Parse nothing" );
    return 0;
  }
}

void upsact_cleanup( t_upslst_item *dep_list )
{
  /* here you should cleanup dep_list */
}


/*-----------------------------------------------------------------------
 * upsact_print_item
 *
 * It will print a t_upsact_item to stdout.
 *
 * options (sopt):
 *    always : print correspoding product instance.
 *    'l'    : also print instance level
 *    'a'    : also print corresponding action command
 *    't'    : also print indentions (corresponding to level)
 *
 * Input : t_upsact_item *, action item
 *         char *, sopt 
 * Output: none
 * Return: none
 */
void upsact_print_item( const t_upsact_item *const p_cur, 
			char * sopt )
{
  static char s_sopt[] = "";
  int i;
  if ( !sopt )
    sopt = s_sopt;

  if ( !p_cur )
    return;
  
  if ( strchr( sopt, 't' ) ) for ( i=0; i<p_cur->level; i++ ) { printf( "   " ); }
  if ( strchr( sopt, 'l' ) ) printf( "%d:", p_cur->level );
  printf( "%s", actitem2inststr( p_cur ) );
  if ( strchr( sopt, 'a' ) ) {
    printf( ":" );
    upsact_print_cmd( p_cur->cmd );
  }
  else printf( "\n" );
}

/*-----------------------------------------------------------------------
 * upsact_print_cmd
 *
 * It will print a t_upsact_cmd to stdout.
 *
 * Input : t_upsact_cmd *, and action command
 * Output: none
 * Return: none
 */
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

/*-----------------------------------------------------------------------
 * upsact_check_files
 *
 * Check to see if the passed action command contains references to any
 * files.  
 *
 * Input : an action command
 * Output: none
 * Return: pointer to a list of file names
 */
t_upslst_item *upsact_check_files(
			    const t_upstyp_matched_product * const a_mproduct,
			    const t_upsugo_command * const a_command_line,
			    char *const a_cmd)
{
  t_upsact_cmd *parsed_cmd = NULL;
  t_upslst_item *file_list = NULL;
  char *new_string = NULL;
  char *trans_cmd = NULL;
  
  if (a_cmd) {
    /* translate any ${UPS...} variables */
    trans_cmd = upsget_translation(a_mproduct, a_command_line, a_cmd);

    /* parse the command */
    parsed_cmd = upsact_parse_cmd(trans_cmd);
    if (parsed_cmd) {
      /* command was successfully parsed.  if there were no arguments, none 
	 of them can be files (;-) */
      if (parsed_cmd->argc > 0) {
	/* there are arguments, now depending on the command, one of these
	   arguments may or may not be a file. only look at the argument if
	   the action parsed is capable of having a file as an argument */
	switch (parsed_cmd->icmd) {
	  /* none of these actions have associated files with them */
	case e_invalid_cmd :
	case e_setupoptional:
	case e_setuprequired:
	case e_unsetupoptional:
	case e_unsetuprequired:
	case e_envappend:
	case e_envremove:
	case e_envprepend:
	case e_envset:
	case e_envunset:
	case e_pathappend:
	case e_pathremove:
	case e_pathprepend:
	case e_pathset:
	case e_filetest:
	case e_dodefaults:
	case e_nodefaults:
	case e_nosetupenv:
	case e_noproddir:
	case e_exeaccess:
	case e_uncopyman:
	case e_writecompilescript:
	  break;
	  /* the following actions contain a file or directory path.  return
	     this value to the calling routine as a list element. */
	case e_sourcecompilereq:
	case e_sourcecompileopt:
	case e_sourcerequired:
	case e_sourceoptional:
	case e_sourcereqcheck:
	case e_sourceoptcheck:
	case e_copyhtml:
	case e_copyinfo:
	case e_copyman:
	case e_copynews:
	  if (parsed_cmd->argv[0]) {
	    new_string = upsutl_str_create(parsed_cmd->argv[0],
					   STR_TRIM_DEFAULT);
	    file_list = upslst_insert(file_list, new_string);
	  }
	  break;
	  /* if there is a path included when specifying the command to execute
	     then include this file, else we don't know the location as the
	     executable is assumed to be in the user's path. in the last case,
	     do not report any files mentioned here. */
	case e_execute:
	  if (parsed_cmd->argv[0] && 
	      (! strncmp(parsed_cmd->argv[0], SLASH, 1))) {
	    /* the first letter was a slash, so we assume that this string
	       contains a directory spec and a file name.  we return this to
	       the calling routine.  if the first letter was not a slash then
	       we cannot really tell where the binary is located as it is
	       either a relative path or assumed to be in the PATH environment
	       variable */
	    new_string = upsutl_str_create(parsed_cmd->argv[0],
					   STR_TRIM_DEFAULT);
	    file_list = upslst_insert(file_list, new_string);
	  }
	  break;
	}
      }
    }
  }

  return(file_list);
}

/*-----------------------------------------------------------------------
 * upsact_free_upsact_cmd
 *
 * It will free a single t_upsact_cmd
 *
 * Input : t_upsact_cmd *, an action command
 * Output: none
 * Return: none
 */
void upsact_free_upsact_cmd( t_upsact_cmd * const act_cmd )
{
  if ( act_cmd ) 
    free( act_cmd );
}

/*=======================================================================
 *
 * Private functions
 *
 *=====================================================================*/

/*-----------------------------------------------------------------------
 * next_cmd
 *
 * Here is the work for creating a list of dependencies. 
 * It's recursive.
 *
 * Input : t_upslst_item *, list of 1'st level products (t_upsact_item's).
 *         t_upslst_item *, current list of dependent products (t_upsact_item's).
 *         t_upsact_item *, current t_upsact_item.
 *         char *, current name of action.
 *         
 * Output: none
 * Return: t_upsact_cmd *,
 */
t_upslst_item *next_cmd( t_upslst_item * const top_list,
			 t_upslst_item *dep_list,
			 t_upsact_item *const p_cur,
			 const char *const act_name )
{
  static char s_current_act_line[MAX_LINE_LEN] = "";
  static char s_current_db[MAX_LINE_LEN] = "";

  t_upslst_item *l_cmd = 0;
  t_upsact_cmd *p_cmd = 0;
  char *p_line = 0;
  int i_act = e_invalid_action;
  int ignore_errors = 0;

  if ( !p_cur )
    return dep_list;

  i_act = actname2enum( act_name );

  if ( !p_cur->act ) {

    /* if we don't have an action, take care of defaults cases, controlled by: 
       g_cmd_info.flags, g_cmd_info.uncmd_index and g_cmd_maps.icmd_undo */

    if ( !(p_cur->act = new_default_action( p_cur, act_name, i_act )) )
      return dep_list;
  }
  
  l_cmd = p_cur->act->command_list;
  for ( ; l_cmd; l_cmd = l_cmd->next ) {
    ignore_errors = 0;

    /* translate and parse command */

    p_line = upsget_translation( p_cur->mat, p_cur->ugo,
				 (char *)l_cmd->data );
    p_cmd = upsact_parse_cmd( p_line );

    if ( p_cmd && p_cmd->icmd >= 0 ) {
      p_cmd->iact = i_act;
      if ( p_cmd->icmd > e_unsetuprequired ) {

	/* here we add an action item to the list (no dependency action) */

	t_upsact_item *new_cur = (t_upsact_item *)upsmem_malloc( sizeof( t_upsact_item ) );
	new_cur->level = p_cur->level;
	new_cur->ugo = p_cur->ugo;
	new_cur->mat = p_cur->mat;
	new_cur->act = p_cur->act;
	new_cur->cmd = p_cmd;
	dep_list = upslst_add( dep_list, new_cur );
	continue;
      }
      else if ( !p_cur->ugo->ugo_j ) {

	/* here we do the dependencies: (un)setup* */

	t_upsact_item *new_act = 0;
	t_upsugo_command *new_ugo = 0;

	/* handle unsetup ... that's special, we will compare command line to the
           product actually setup and get the instance from $SETUP_prodname */

	if ( p_cmd->icmd == e_unsetuprequired ||
	     p_cmd->icmd == e_unsetupoptional ) {
	  if ( !(new_ugo = get_SETUP_prod( p_cmd, i_act )) )
	    continue;
	}

	/* get/set current db: set db to current database if database is not 
           specified on command line, else ugo_bldcmd knows what to do */

	else if ( !strstr( p_cmd->argv[0], "-z" ) && p_cur->ugo->ugo_z && 
	     dbl2dbs( s_current_db, p_cur->ugo->ugo_db ) > 0 ) {
	  strcpy( s_current_act_line, p_cmd->argv[0] ); 
	  strcat( s_current_act_line, s_current_db );
	  new_ugo = upsugo_bldcmd( s_current_act_line, g_cmd_info[i_act].valid_opts );
	}
	else {
	  new_ugo = upsugo_bldcmd( p_cmd->argv[0], g_cmd_info[i_act].valid_opts );
	}

	/* if product is at the top level, use that one */

	if ( new_ugo && p_cur->level > 0 ) {
	  new_act = find_product_str( top_list, new_ugo->ugo_product );
	}

	/* if product is already in our setup list, go to next product */
	
	if ( new_ugo && find_product_str( dep_list, new_ugo->ugo_product ) ) {
	  /* !!! free stuff */
	  continue;
	}
	
	/* new_act is only set if product found at the top level, else
           we will have to create a new_act from a ugo */

	if ( !new_act ) {
	  switch ( p_cmd->icmd ) 
	  {
	  case e_setupoptional:
	    ignore_errors = 1;
	  case e_setuprequired:	    
	    new_act = new_act_item( new_ugo, 0, 0, "SETUP");
	    break;	
	  case e_unsetupoptional:
	    ignore_errors = 1;
	  case e_unsetuprequired: 
	    new_act = new_act_item( new_ugo, 0, 0, "UNSETUP");
	    break;
	  }

	  /* ignore errors if optional (un)setup */

	  if ( !new_act ) {
	    if ( ignore_errors )
	      upserr_clear();
	    else {
	      SET_PARSE_ERROR( p_line );
	    }
	    continue;
	  }
	}

	/* here we have a new action, increment dependency level and
           parse atcion (by calling next_cmd) */

	new_act->level = p_cur->level + 1;
	dep_list = next_cmd( top_list, dep_list, new_act, act_name );
	P_VERB_s_s( 3, "Adding dependcy:", p_line );
	continue;
      }
    }
    else {
      SET_PARSE_ERROR( p_line );
    }
  }
  
  return dep_list;
}

/*-----------------------------------------------------------------------
 * parse_params
 *
 * Split an action's parameter string into into separate parameters and
 * return an array of these.The parameters in the string are separated 
 * by commas, but ignore commas within quotes.
 *
 * The routine writes into passed parameter string !!!.
 *
 * Input : char *, parameter string
 *         char **, pointer to array of arguments
 * Output: char **, pointer to array of arguments
 * Return: number of arguments found
 */
int parse_params( const char * const a_params, char **argv )
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
	ptr = NULL;              /* all done */
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

/*-----------------------------------------------------------------------
 * get_top_prod
 *
 * Will fill the passed list of action item with translated actions.
 * It will not follow depencies. It's very close to 'next_cmd'.
 * (actally, it should probaly had been a part of next_cmd).
 *
 * Input : t_upsact_item *, action item
 *         char *, action name
 * Output: none
 * Return: t_upslst_item *, a list of top action items
 */
t_upslst_item *get_top_prod( t_upsact_item *const p_cur, 
			     const char *const act_name )
{
  static char current_act_line[MAX_LINE_LEN] = "";
  static char current_db[MAX_LINE_LEN] = "";
  t_upslst_item *l_cmd = p_cur->act ? p_cur->act->command_list : 0;
  t_upsact_cmd *p_cmd = 0;
  char *p_line = 0;
  t_upslst_item *top_list = 0;
  int i_act = actname2enum( act_name );
  int i_cmd = 0;
  int ignore_errors;
  int add_item;

  for ( ; l_cmd; l_cmd = l_cmd->next ) {
    ignore_errors = 0;
    add_item = 0;
    p_line = upsget_translation( p_cur->mat, p_cur->ugo,
				 (char *)l_cmd->data );
    p_cmd = upsact_parse_cmd( p_line );
    i_cmd = p_cmd ? p_cmd->icmd : -1;
    
    if ( i_cmd >= 0 && i_cmd <= e_unsetuprequired ) {
      t_upsact_item *new_cur = 0;
      t_upsugo_command *new_ugo = 0;
      p_cmd->iact = i_act;

      if ( !strstr( p_cmd->argv[0], "-z" ) && p_cur->ugo->ugo_z && 
	   dbl2dbs( current_db, p_cur->ugo->ugo_db ) > 0 ) {
	strcpy( current_act_line, p_cmd->argv[0] ); 
	strcat( current_act_line, current_db );
	new_ugo = upsugo_bldcmd( current_act_line, g_cmd_info[i_act].valid_opts );
      }
      else {
	new_ugo = upsugo_bldcmd( p_cmd->argv[0], g_cmd_info[i_act].valid_opts );
      }

      switch (i_cmd ) 
      {
      case e_setupoptional:
	ignore_errors = 1;
      case e_setuprequired:
	new_cur = new_act_item( new_ugo, 0, 0, "SETUP");
	break;	

      case e_unsetupoptional:
	ignore_errors = 1;
      case e_unsetuprequired:
	new_cur = new_act_item( new_ugo, 0, 0, "UNSETUP");
	break;
      }

      if ( !new_cur ) {
	if ( ignore_errors )
	  upserr_clear();
	else {
	  SET_PARSE_ERROR( p_line );
	}
      }
      else {
	top_list = upslst_add( top_list, new_cur );
      }
    }
  }

  return upslst_first( top_list );
}

/*-----------------------------------------------------------------------
 * get_SETUP_prod
 *
 * Will create an ugo command from the env. variable SETUP_prodname.
 * It will compare if the passed command line correspond to the product
 * defined by SETUP_prod.
 *
 * Input : t_upsact_cmd *, action command line
 *         int, enum of main line action
 * Output: none
 * Return: t_upsugo_command *, a ugo command
 */
t_upsugo_command *get_SETUP_prod( t_upsact_cmd * const p_cmd, 
				  const int i_act )
{
  static char s_pname[256];
  char *pname = 0;
  t_upsugo_command *a_cmd_ugo = 0;
  t_upsugo_command *a_setup_ugo = 0;
  int i_cmd = 0;
  char *cmd_line = 0;

  if ( !p_cmd )
    return 0;

  i_cmd = p_cmd->icmd;
  cmd_line = p_cmd->argv[0];

  /* if it's not a simple command, let ugo do it */
  
  if ( ! strchr( cmd_line, ' ' ) ) {
    strcpy( s_pname, cmd_line );
  }
  else {
    a_cmd_ugo = upsugo_bldcmd( cmd_line, g_cmd_info[e_unsetup].valid_opts );
    strcpy( s_pname, a_cmd_ugo->ugo_product );
  }
  pname = upsutl_upcase( s_pname );

  /* make the corresponding ugo_command, using $SETUP_prod */

  a_setup_ugo = upsugo_env( pname, g_cmd_info[e_unsetup].valid_opts );

  /* check if instance is the same */

  if ( a_cmd_ugo && a_setup_ugo ) {
    int ohoh = 0;
    if ( a_cmd_ugo->ugo_qualifiers &&
	 lst_cmp_str( a_cmd_ugo->ugo_qualifiers, a_setup_ugo->ugo_qualifiers ) ) {
      ohoh = 1;
    }
    else if ( a_cmd_ugo->ugo_version  &&
	      strcmp( a_cmd_ugo->ugo_version, a_setup_ugo->ugo_version ) ) {
	ohoh = 1;
    }
    /* more work to do here ...
    else if ( a_cmd_ugo->ugo_chain ) {
      if ( lst_cmp_str( a_cmd_ugo->ugo_chain, a_setup_ugo->ugo_chain ) )
	ohoh = 1;
    }
    */

    if ( ohoh )
      upserr_add( UPS_UNSETUP_CLASH, UPS_WARNING, pname );

    upsugo_free( a_cmd_ugo ); 
  }
  
  /* if there is nothing to unsetup, do something ... maybe */
  
  if ( ! a_setup_ugo ) {

    switch ( i_cmd ) {
    case e_unsetuprequired:      
      upserr_add( UPS_NO_SETUP_ENV, UPS_WARNING, pname );
      break;
    case e_unsetupoptional:
      break;
    }
  }

  return a_setup_ugo;
}

/*-----------------------------------------------------------------------
 * get_act
 *
 * It will from a ugo command or a macthed product fetch the corresponding
 * action (t_upstyp_action). The passed ugo command or macthed product is
 * expected to be an unique instance.
 *
 * Input : t_upsugo_command *, a ugo command
 *         t_upstyp_matched_product *, a matched product (can be null)
 *         char *, name of action
 * Output: none
 * Return: t_upstyp_action *, an action (as read from a table file)
 */
t_upstyp_action *get_act( const t_upsugo_command * const ugo_cmd,
			  t_upstyp_matched_product * mat_prod,
			  const char * const act_name )
{
  t_upstyp_matched_instance *mat_inst;

  if ( !ugo_cmd || !act_name )
    return 0;

  if ( !mat_prod ) {
    t_upslst_item *l_mproduct = upsmat_instance( (t_upsugo_command *)ugo_cmd, NULL, 1 );
    if ( !l_mproduct || !l_mproduct->data )
      return 0;
    mat_prod = (t_upstyp_matched_product *)l_mproduct->data;
  }

  /* we are expecting one match, else the above should fail */

  mat_inst = (t_upstyp_matched_instance *)(upslst_first( mat_prod->minst_list ))->data;
  if ( mat_inst->table ) 
    return upskey_inst_getaction( mat_inst->table, act_name );
  else
    return 0;
}

int lst_cmp_str( t_upslst_item * const l1, 
		 t_upslst_item * const l2 )
{
  /* will compare all strings in a list of strings */

  int dc = 0;
  t_upslst_item *l_1 = upslst_first( l1 );
  t_upslst_item *l_2 = upslst_first( l2 );

  if ( (dc = (upslst_count( l_1 ) - upslst_count( l_2 )) != 0 ) )    
    return dc;

  for ( ; l_1; l_1 = l_1->next, l_2 = l_2->next ) {
    if ( (dc = strcmp( (char *)l_1->data, (char *)l_2->data )) )
      return dc;
  }

  return 0;
}

t_upsact_item *find_product_str( t_upslst_item* const dep_list,
			     const char *const prod_name )
{
  /* in a list of act_item's it will find the item corresponding to
     passed product name */

  t_upslst_item *l_ptr = upslst_first( (t_upslst_item *)dep_list );
  for ( ; l_ptr; l_ptr = l_ptr->next ) {
    t_upsact_item *p_item = (t_upsact_item *)l_ptr->data;
    if ( upsutl_stricmp( prod_name, p_item->ugo->ugo_product ) == 0 )
      return p_item;
  }
  return 0;    
}

t_upsact_item *find_product_ptr( t_upslst_item* const dep_list,
				 const t_upsact_item* const act_item )
{
  /* in a list of act_item's it will find the item corresponding to
     passed act_item pointer */

  t_upslst_item *l_ptr = upslst_first( dep_list );
  for ( ; l_ptr; l_ptr = l_ptr->next ) {
    t_upsact_item *act_ptr = (t_upsact_item *)l_ptr->data;
    if ( act_item->ugo == act_ptr->ugo )
      return act_ptr;
  }
  return 0;    
}

char *actitem2inststr( const t_upsact_item *const p_cur )
{
  /* given an act_item it will create a string representing the 
     corresponding product instance. the result is the string
     from upsget_envstr with chain information appended */

  static char buf[MAX_LINE_LEN];
  t_upstyp_matched_instance *mat_inst;
  t_upslst_item *l_item;
  
  if ( !p_cur )
    return 0;

  l_item = upslst_first( p_cur->mat->minst_list );
  mat_inst = (t_upstyp_matched_instance *)l_item->data;
  strcpy( buf, upsget_envstr( p_cur->mat->db_info, mat_inst, p_cur->ugo ) );
  l_item = upslst_first( p_cur->ugo->ugo_chain );
  if ( l_item && l_item->data ) {
    strcat( buf, " -g " );
    strcat( buf, (char *)l_item->data );
    
    /* the following should in princip never happen: a dependency should
       only be reachable by a single chain, so maybe we should print an
       error here */

    for ( l_item = l_item->next; l_item; l_item = l_item->next ) {
      if ( l_item->data ) {
	strcat( buf, ":" );
	strcat( buf, (char *)l_item->data );
      }
    } 
  }

  return buf;
}

int actname2enum( const char * const act_name )
{
  /* it will map an action name to the corresponding enum */

  int iact = e_invalid_action, i = 0;

  if ( act_name ) {
    for ( i=0; g_cmd_info[i].cmd; i++ ) {
      if (! upsutl_stricmp(g_cmd_info[i].cmd, act_name)) {
	iact = i;
	break;
      }
    }
  }
  
  return iact;
}

int dbl2dbs( char * const s_db, t_upslst_item * const l_db )
{
  /* from a list of typ_db items it will create a string of databases
     corresponding including -z option */

  t_upslst_item *l_item = 0;
  int c_db = 0;

  if ( !s_db )
    return 0;

  s_db[0] = '\0';

  if ( !(l_item = upslst_first( l_db )) )
    return 0;

  strcpy( s_db, " -z " );
  for ( ; l_item; l_item = l_item->next, c_db++ ) {
    t_upstyp_db * db = (t_upstyp_db *)l_item->data;
    strcat( s_db, db->name );
    if ( l_item->next )
      strcat( s_db, ":" );
  }
    
  return c_db;
}

void trim_delimiter( char * str )
{
  /* just to trim the delimiter: get rid of starting and
     trailing quotes. the passed string are expected to be
     trimmed of starting and trailing blanks */

  int len = strlen( str );

  if ( len < 2 )
    return;

  if ( (str[0] == '\"' && str[len-1] == '\"') ||
       (str[0] == '\'' && str[len-1] == '\'') ) {
    char *sp1 = &str[1], *sp2 = &str[len-1];
    for ( ; sp1 < sp2; str++, sp1++ ) *str = *sp1;
    *str = 0;
  }
}

t_upsact_item *new_act_item( t_upsugo_command * const ugo_cmd,
			     t_upstyp_matched_product *mat_prod,
			     const int level,
			     const char * const act_name )
{		       
  t_upsact_item *act_item = 0;

  /* from a passed ugo command or a matched product it will create a 
     new action item */

  if ( !ugo_cmd )
    return 0;

  if ( !mat_prod ) {
    t_upslst_item *l_mproduct = upsmat_instance( ugo_cmd, NULL, 1 );
    if ( !l_mproduct || !l_mproduct->data )
      return 0;
    mat_prod = (t_upstyp_matched_product *)l_mproduct->data;
  }

  act_item = (t_upsact_item *)upsmem_malloc( sizeof( t_upsact_item ) );
  act_item->level = level;
  act_item->ugo = ugo_cmd;
  act_item->mat = mat_prod;
  act_item->act = get_act( ugo_cmd, mat_prod, act_name );
  act_item->cmd = 0;

  return act_item;
}

t_upstyp_action *new_default_action( t_upsact_item *const p_cur, 
				     const char * const act_name, 
				     const int iact )
{
  /* it will create a new t_upstyp_action. It will be:
     1) reverse action, if exist (g_cmd_info.uncmd_index)
     2) a dodefault() action, if default bit set (g_cmd_info.flags) */

  t_upstyp_action *new_act = 0;
  int i_uncmd = 0;

  if ( iact == e_invalid_action )
    return 0;

  if ( (i_uncmd = g_cmd_info[iact].uncmd_index) != e_invalid_action ) {
    t_upstyp_action* act_p = get_act( p_cur->ugo, p_cur->mat, g_cmd_info[i_uncmd].cmd );

    new_act = (t_upstyp_action *)malloc( sizeof( t_upstyp_action ) );
    new_act->action = (char *)malloc( strlen( act_name ) + 1 );
    strcpy( new_act->action, act_name );
    new_act->command_list = reverse_command_list( p_cur, act_p->command_list );
  }      
  else if ( (g_cmd_info[iact].flags)&0x00000001 ) {
    new_act = (t_upstyp_action *)malloc( sizeof( t_upstyp_action ) );
    new_act->action = (char *)malloc( strlen( act_name ) + 1 );
    strcpy( new_act->action, act_name );
    new_act->command_list = 0;
    new_act->command_list = upslst_add( new_act->command_list, 
					upsutl_str_create( "dodefaults()", ' ' ) );
  }

  return new_act;
}

t_upslst_item *reverse_command_list( t_upsact_item *const p_cur, 
				     t_upslst_item *const cmd_list )
{
  /* it will, given a list of action command lines, reverse each 
     command line (if g_cmd_maps.icmd_undo is set), and return a 
     list of these. */ 

  static char buf[MAX_LINE_LEN];
  t_upslst_item *l_ucmd = 0;
  t_upslst_item *l_cmd = upslst_first( cmd_list );
  int i_uncmd = e_invalid_cmd;
  int i_cmd = e_invalid_cmd;
  int argc = 0;
  char *p_line;
  t_upsact_cmd *p_cmd;
  int i;

  for ( ; l_cmd; l_cmd = l_cmd->next ) {

    /* translate and parse command */

    p_line = upsget_translation( p_cur->mat, p_cur->ugo,
				 (char *)l_cmd->data );
    p_cmd = upsact_parse_cmd( p_line );
    if ( p_cmd && ((i_cmd = p_cmd->icmd) != e_invalid_cmd) ) {
      if ( (i_uncmd = g_cmd_maps[p_cmd->icmd].icmd_undo) != e_invalid_cmd ) {
	strcpy( buf,  g_cmd_maps[i_uncmd].cmd );

	argc = g_cmd_maps[i_cmd].max_params;
	if ( argc > g_cmd_maps[i_uncmd].max_params )
	  argc = g_cmd_maps[i_uncmd].max_params;
	if ( argc > p_cmd->argc )
	  argc = p_cmd->argc;

	strcat( buf, "(" );
	for ( i=0; i<argc; i++ ) {
	  if ( i > 0 ) strcat( buf, ", " );	    
	  strcat( buf, p_cmd->argv[i] );
	}
	strcat( buf, ")" );

	l_ucmd = upslst_add( l_ucmd, 
			     upsutl_str_create( buf, ' ' ) );
      }
      free( p_cmd );      
    }    
  }

  return upslst_first( l_ucmd );
}

/*-----------------------------------------------------------------------
 * upsact_process_commands
 *
 * Given a list of commands, call each associated function to output the
 * shell specific code.
 *
 * Input : list of upsact_cmd items
 *         a stream to write to
 * Output: 
 * Return: none
 */
void upsact_process_commands( const t_upslst_item * const a_cmd_list,
			      const FILE * const a_stream)
{
  t_upslst_item *cmd_item;
  t_upsact_item *the_cmd;

  for (cmd_item = (t_upslst_item *)a_cmd_list ; cmd_item ;
       cmd_item = cmd_item->next ) {
    the_cmd = (t_upsact_item *)cmd_item->data;
    
    /* call the function associated with the command */
    if (g_cmd_maps[the_cmd->cmd->icmd].func) {
      g_cmd_maps[the_cmd->cmd->icmd].func(
		  (t_upstyp_matched_instance *)the_cmd->mat->minst_list->data,
		  the_cmd->mat->db_info, the_cmd->ugo, a_stream, the_cmd->cmd);
      if (UPS_ERROR != UPS_SUCCESS) {
	break;
      }
    }
  }
}


/* Action handling - the following routines are the ones that output shell
 *   specific code for each action supported by UPS
 */

static void f_copyhtml( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("copyHtml");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config || !a_db_info->config->html_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "html");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n#\n", 
		    a_cmd->argv[0], a_db_info->config->html_path) < 0) {
	  FPRINTF_ERROR();
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_copyinfo( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("copyInfo");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config || !a_db_info->config->info_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "info");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n#\n", 
		    a_cmd->argv[0], a_db_info->config->info_path) < 0) {
	  FPRINTF_ERROR();
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_copyman( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const t_upsact_cmd * const a_cmd)
{
  char *buf = NULL;

  CHECK_NUM_PARAM("copyMan");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config || !a_db_info->config->man_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "man");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (a_cmd->argc == 1) {
	  /* the user specified a source in the action */
	  buf = (char *)a_cmd->argv[0];
	} else {
	  /* we have to construct a source */
	  buf = upsutl_find_manpages(a_inst, a_db_info);
	}
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n#\n", 
		    buf, a_db_info->config->man_path) < 0) {
	  FPRINTF_ERROR();
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_uncopyman( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const t_upsact_cmd * const a_cmd)
{
  char *buf = NULL;
  t_upslst_item *man_item, *man_list;

  CHECK_NUM_PARAM("uncopyMan");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config || !a_db_info->config->man_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "man");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (a_cmd->argc == 1) {
	  /* the user specified a source in the action (gotten from current
	     action */
	  buf = (char *)a_cmd->argv[0];
	} else {
	  /* we have to construct a source */
	  buf = upsutl_find_manpages(a_inst, a_db_info);
	}

	/* Get a list of all the files in the specified directory */
	upsutl_get_files(buf, ANY_MATCH, &man_list);

	for (man_item = man_list ; man_item ; man_item = man_item->next) {
	  if (fprintf((FILE *)a_stream, "rm %s/%s\n", 
		      a_db_info->config->man_path, (char *)man_item->data)
	      < 0) {
	    FPRINTF_ERROR();
	    break;
	  }
	}
	fprintf((FILE *)a_stream, "#\n");
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_copynews( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("copyNews");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config || !a_db_info->config->news_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "news");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n#\n", 
		    a_cmd->argv[0], a_db_info->config->news_path) < 0) {
	  FPRINTF_ERROR();
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_envappend( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const t_upsact_cmd * const a_cmd)
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
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_envremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "%s=\"${%s-}%s%s\";export %s\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0], delimiter, a_cmd->argv[1],
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_envremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "setenv %s \"${%s}%s%s\"\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0], delimiter,
		  a_cmd->argv[1]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_envprepend( const t_upstyp_matched_instance * const a_inst,
			  const t_upstyp_db * const a_db_info,
			  const t_upsugo_command * const a_command_line,
			  const FILE * const a_stream,
			  const t_upsact_cmd * const a_cmd)
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
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_envremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "%s=\"%s%s${%s-}\";export %s\n#\n",
		  a_cmd->argv[0], a_cmd->argv[1], delimiter, a_cmd->argv[0],
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_envremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "setenv %s \"%s%s${%s}\"\n#\n",
		  a_cmd->argv[0], a_cmd->argv[1], delimiter,
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_envremove( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const t_upsact_cmd * const a_cmd)
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
		  "upstmp=`dropit.pl %s %s %s`;\nif [ $? -eq 0 ]; then %s=$upstmp; fi\nunset upstmp;\n#\n",
		  a_cmd->argv[0], a_cmd->argv[1], delimiter,
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream,
		  "setenv upstmp \"`dropit.pl %s %s %s`\"\nif ($status == 0) setenv %s $upstmp\nunsetenv upstmp\n#\n",
		  a_cmd->argv[0], a_cmd->argv[1], delimiter,
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_envset( const t_upstyp_matched_instance * const a_inst,
		      const t_upstyp_db * const a_db_info,
		      const t_upsugo_command * const a_command_line,
		      const FILE * const a_stream,
		      const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("envSet");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=\"%s\";export %s\n#\n", a_cmd->argv[0],
		  a_cmd->argv[1], a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s \"%s\"\n#\n", a_cmd->argv[0],
		  a_cmd->argv[1]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_envunset( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("envUnset");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "unset %s\n#\n", a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "unsetenv %s\n#\n", a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_exeaccess( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("exeAccess");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream,
		  "hash %s;\nif [ $? -eq 1 ]; then return 1; fi\n#\n",
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream,
		  "whereis %s\nif ($status == 1) return 1\n#\n",
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_execute( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("execute");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=`%s`;export %s\n#\n", a_cmd->argv[1],
		  a_cmd->argv[0], a_cmd->argv[1]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s \"`%s`\"\n#\n", a_cmd->argv[1],
		  a_cmd->argv[0])< 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_filetest( const t_upstyp_matched_instance * const a_inst,
			const t_upstyp_db * const a_db_info,
			const t_upsugo_command * const a_command_line,
			const FILE * const a_stream,
			const t_upsact_cmd * const a_cmd)
{
  char *err_message;
  
  CHECK_NUM_PARAM("fileTest");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct error message */
    GET_ERR_MESSAGE((char *)a_cmd->argv[g_cmd_maps[a_cmd->icmd].max_params-1]);

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream,
		  "if [ ! %s %s ]; then\necho \"%s\";\nreturn 1;\nfi;\n#\n",
		  a_cmd->argv[1], a_cmd->argv[0], err_message) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream,
		  "if ( ! %s %s ) then\necho %s\nreturn 1\nendif\n#\n", 
		  a_cmd->argv[1], a_cmd->argv[0], err_message) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_makedir( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("makeDir");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "mkdir %s\n#\n", a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_pathappend( const t_upstyp_matched_instance * const a_inst,
			  const t_upstyp_db * const a_db_info,
			  const t_upsugo_command * const a_command_line,
			  const FILE * const a_stream,
			  const t_upsact_cmd * const a_cmd)
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
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_pathremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "%s=\"${%s-}%s%s\";export %s\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0], delimiter, a_cmd->argv[1],
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_pathremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "set %s=($%s %s)\nrehash\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0], a_cmd->argv[1]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_pathprepend( const t_upstyp_matched_instance * const a_inst,
			   const t_upstyp_db * const a_db_info,
			   const t_upsugo_command * const a_command_line,
			   const FILE * const a_stream,
			   const t_upsact_cmd * const a_cmd)
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
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_pathremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "%s=\"%s%s${%s-}\";export %s\n#\n",
		  a_cmd->argv[0], a_cmd->argv[1], delimiter, a_cmd->argv[0],
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_pathremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "set %s=(%s $%s)\nrehash\n#\n",
		  a_cmd->argv[0], a_cmd->argv[1], a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_pathremove( const t_upstyp_matched_instance * const a_inst,
			  const t_upstyp_db * const a_db_info,
			  const t_upsugo_command * const a_command_line,
			  const FILE * const a_stream,
			  const t_upsact_cmd * const a_cmd)
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
		  "upstmp=`dropit.pl %s %s %s`;\nif [ $? -eq 0 ]; then %s=$upstmp; fi\nunset upstmp;\n#\n",
		  a_cmd->argv[0], a_cmd->argv[1], delimiter,
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream,
		  "setenv upstmp \"`dropit.pl %s %s %s`\"\nif ($status == 0) set %s=$upstmp\nrehash\nunsetenv upstmp\n#\n",
		  a_cmd->argv[0], a_cmd->argv[1], delimiter, 
		  a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_pathset( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const t_upsact_cmd * const a_cmd)
{
  CHECK_NUM_PARAM("pathSet");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s;export %s\n#\n", a_cmd->argv[0],
		  a_cmd->argv[1], a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "set %s=(%s)\nrehash\n#\n", a_cmd->argv[0],
		  a_cmd->argv[1]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static int sh_output_first_check(const FILE * const a_stream,
				 const char * const a_data,
				 const int a_exit_flag, 
				 const t_upsugo_command * const a_command_line)
{
  int status;

  if ((status = fprintf((FILE *)a_stream,
	      "if [ -s %s ]; then\n  if [ ! -r %s -o ! -x %s ]; then\n    echo File to source (%s) is not readable or not executable\n", a_data,
			a_data, a_data, a_data)) < 0) {
    FPRINTF_ERROR();
  } else {
    /* check if we should put out the info we usually do when we exit */
    if (a_exit_flag == DO_EXIT) {
      upsutl_finish_temp_file(a_stream, a_command_line);
      if ((status = fprintf((FILE *)a_stream, "    return 1\n")) < 0) {
	FPRINTF_ERROR();
      }
    }
    
    if ((status >= 0) && (status = fprintf((FILE *)a_stream, "  else\n"))
	< 0) {
      FPRINTF_ERROR();
    }
  }
  return (status);
}

static int sh_output_next_part(const FILE * const a_stream,
			       const char * const a_data,
			       const int a_exit_flag, 
			       const int a_check_flag,
			       const int a_ups_env_flag,
			       const t_upstyp_matched_instance * const a_inst,
			       const t_upstyp_db * const a_db_info,
			       const t_upsugo_command * const a_command_line)
{
  int status = -1;

  /* define all of the UPS local variables that the user may need. */
  if (a_ups_env_flag == DO_UPS_ENV) {
    upsget_allout(a_stream, a_db_info, a_inst, a_command_line);
    g_LOCAL_VARS_DEF = 1;   /* we defined local variables */
  }
  if (UPS_ERROR == UPS_SUCCESS) {
    if ((status = fprintf((FILE *)a_stream, "    . %s\n", a_data)) < 0) {
      FPRINTF_ERROR();
    } else {
      if (a_check_flag == DO_CHECK) {
	if ((status = fprintf((FILE *)a_stream,
	      "    UPS_STATUS=$?\n    if [ \"$UPS_STATUS\" != \"0\" ]; then\n      echo Error $UPS_STATUS while sourcing %s\n      unset UPS_STATUS\n",
		    a_data)) < 0) {
	  FPRINTF_ERROR();
	} else {
	  upsutl_finish_temp_file(a_stream, a_command_line);
	  if ((status = fprintf((FILE *)a_stream,
		      "      return 1\n    fi\n    unset UPS_STATUS\n") < 0)) {
	      FPRINTF_ERROR();
	  } 
	}
      }
      if ((status >= 0) && (a_exit_flag == DO_EXIT)) {
	upsutl_finish_temp_file(a_stream, a_command_line);
	if ((status = fprintf((FILE *)a_stream, "    return\n") < 0)) {
	  FPRINTF_ERROR();
	}
      }
      if (status >= 0) {
	if ((status = fprintf((FILE *)a_stream, "  fi\n") < 0)) {
	  FPRINTF_ERROR();
	}
      }
    }
  }
  return(status);
}

static int csh_output_first_check(const FILE * const a_stream,
				 const char * const a_data,
				 const int a_exit_flag, 
				 const t_upsugo_command * const a_command_line)
{
  int status;

  if ((status = fprintf((FILE *)a_stream,
	      "if (-e %s) then\n  if (! -r %s || ! -x %s) then\n    echo File to source (%s) is not readable or not executable\n", a_data,
			a_data, a_data, a_data)) < 0) {
    FPRINTF_ERROR();
  } else {
    /* check if we should put out the info we usually do when we exit */
    if (a_exit_flag == DO_EXIT) {
      upsutl_finish_temp_file(a_stream, a_command_line);
      if ((status = fprintf((FILE *)a_stream, "    return 1\n")) < 0) {
	FPRINTF_ERROR();
      }
    }
    
    if ((status >= 0) && (status = fprintf((FILE *)a_stream, "  else\n"))
	< 0) {
      FPRINTF_ERROR();
    }
  }
  return (status);
}

static int csh_output_next_part(const FILE * const a_stream,
				const char * const a_data,
				const int a_exit_flag, 
				const int a_check_flag,
				const int a_ups_env_flag,
				const t_upstyp_matched_instance * const a_inst,
				const t_upstyp_db * const a_db_info,
				const t_upsugo_command * const a_command_line)
{
  int status = -1;

  /* define all of the UPS local variables that the user may need. */
  if (a_ups_env_flag == DO_UPS_ENV) {
    upsget_allout(a_stream, a_db_info, a_inst, a_command_line);
    g_LOCAL_VARS_DEF = 1;   /* we defined local variables */
  }
  if (UPS_ERROR == UPS_SUCCESS) {
    if ((status = fprintf((FILE *)a_stream, "    source %s\n", a_data)) < 0) {
      FPRINTF_ERROR();
    } else {
      if (a_check_flag == DO_CHECK) {
	if ((status = fprintf((FILE *)a_stream,
	      "    setenv UPS_STATUS $status\n    if (\"$UPS_STATUS\" != \"0\") then\n      echo \"Error $UPS_STATUS while sourcing %s\n      unsetenv UPS_STATUS\n",
		    a_data)) < 0) {
	  FPRINTF_ERROR();
	} else {
	  upsutl_finish_temp_file(a_stream, a_command_line);
	  if ((status = fprintf((FILE *)a_stream,
		      "      return 1\n    endif\n    unsetenv UPS_STATUS\n") < 0)) {
	      FPRINTF_ERROR();
	  } 
	}
      }
      if ((status >= 0) && (a_exit_flag == DO_EXIT)) {
	upsutl_finish_temp_file(a_stream, a_command_line);
	if ((status = fprintf((FILE *)a_stream, "    return\n") < 0)) {
	  FPRINTF_ERROR();
	}
      }
      if (status >= 0) {
	if ((status = fprintf((FILE *)a_stream, "  endif\n") < 0)) {
	  FPRINTF_ERROR();
	}
      }
    }
  }
  return(status);
}

static int sh_output_last_part_req(const FILE * const a_stream,
				const char * const a_data,
				const t_upsugo_command * const a_command_line)
{
  int status;

  if ((status = fprintf((FILE *)a_stream,
	      "else\n  echo File (%s)  not found\n", a_data) < 0)) {
    FPRINTF_ERROR();
  } else {
    upsutl_finish_temp_file(a_stream, a_command_line);
    if ((status = fprintf((FILE *)a_stream, "  return 1\nfi\n#\n") < 0)) {
      FPRINTF_ERROR();
    }
  }
  return(status);
}

static int csh_output_last_part_req(const FILE * const a_stream,
				const char * const a_data,
				const t_upsugo_command * const a_command_line)
{
  int status;

  if ((status = fprintf((FILE *)a_stream,
	      "else\n  echo File (%s)  not found\n", a_data) < 0)) {
    FPRINTF_ERROR();
  } else {
    upsutl_finish_temp_file(a_stream, a_command_line);
    if ((status = fprintf((FILE *)a_stream, "  return 1\nendif\n#\n") < 0)) {
      FPRINTF_ERROR();
    }
  }
  return(status);
}

static void f_sourcecompilereq( const t_upstyp_matched_instance * const a_inst,
				const t_upstyp_db * const a_db_info,
				const t_upsugo_command * const a_command_line,
				const FILE * const a_stream,
				const t_upsact_cmd * const a_cmd)
{
  /* skip this whole action if we are being called while compiling */
  if (! g_COMPILE_FLAG) {
    CHECK_NUM_PARAM("sourceCompileReq");

    /* only proceed if we have a valid number of parameters and a stream to
       write them to */
    if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
	if (sh_output_first_check(a_stream, a_cmd->argv[0], DO_EXIT,
				  a_command_line) >= 0) {
	  if (sh_output_next_part(a_stream, a_cmd->argv[0], DO_EXIT,
				  NO_CHECK, DO_NO_UPS_ENV, a_inst, a_db_info,
				  a_command_line) < 0) {
	    FPRINTF_ERROR();
	  } else {
	    (void )sh_output_last_part_req(a_stream, a_cmd->argv[0],
					   a_command_line);
	  }
	}
	break;
      case e_CSHELL:
	if (csh_output_first_check(a_stream, a_cmd->argv[0], DO_EXIT,
				   a_command_line) >= 0) {
	  if (csh_output_next_part(a_stream, a_cmd->argv[0], DO_EXIT,
				   NO_CHECK, DO_NO_UPS_ENV, a_inst,
				   a_db_info, a_command_line) < 0) {
	    FPRINTF_ERROR();
	  } else {
	    (void )csh_output_last_part_req(a_stream, a_cmd->argv[0],
					    a_command_line);
	  }
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }

      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_sourcecompileopt( const t_upstyp_matched_instance * const a_inst,
				const t_upstyp_db * const a_db_info,
				const t_upsugo_command * const a_command_line,
				const FILE * const a_stream,
				const t_upsact_cmd * const a_cmd)
{
  /* skip this whole action if we are being called while compiling */
  if (! g_COMPILE_FLAG) {
    CHECK_NUM_PARAM("sourceCompileOpt");

    /* only proceed if we have a valid number of parameters and a stream to
       write them to */
    if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
	if (sh_output_first_check(a_stream, a_cmd->argv[0], NO_EXIT,
				  a_command_line) >= 0) {
	  if (sh_output_next_part(a_stream, a_cmd->argv[0], DO_EXIT,
				  NO_CHECK, DO_NO_UPS_ENV, a_inst, a_db_info,
				  a_command_line) < 0) {
	    FPRINTF_ERROR();
	  } else {
	    SH_OUTPUT_LAST_PART_OPT();
	  }
	}
	break;
      case e_CSHELL:
	if (csh_output_first_check(a_stream, a_cmd->argv[0], NO_EXIT,
				   a_command_line) >= 0) {
	  if (csh_output_next_part(a_stream, a_cmd->argv[0], DO_EXIT,
				   NO_CHECK, DO_NO_UPS_ENV, a_inst, a_db_info,
				   a_command_line) < 0) {
	    FPRINTF_ERROR();
	  } else {
	    CSH_OUTPUT_LAST_PART_OPT();
	  }
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_vplace();
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		   g_cmd_maps[a_cmd->icmd].cmd);
      }
    }
  }
}

static void f_sourcerequired( const t_upstyp_matched_instance * const a_inst,
			      const t_upstyp_db * const a_db_info,
			      const t_upsugo_command * const a_command_line,
			      const FILE * const a_stream,
			      const t_upsact_cmd * const a_cmd)
{
  int exit_flag = NO_EXIT;
  int no_ups_env_flag = DO_UPS_ENV;

  CHECK_NUM_PARAM("sourceRequired");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Determine which flags (if any) were entered */
    GET_FLAGS();

    if (UPS_ERROR == UPS_SUCCESS) {
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
	if (sh_output_first_check(a_stream, a_cmd->argv[0], DO_EXIT,
				  a_command_line) >= 0) {
	  if (sh_output_next_part(a_stream, a_cmd->argv[0], exit_flag,
				  NO_CHECK, no_ups_env_flag, a_inst, a_db_info,
				  a_command_line) < 0) {
	    FPRINTF_ERROR();
	  } else {
	    (void )sh_output_last_part_req(a_stream, a_cmd->argv[0],
					   a_command_line);
	  }
	}
	break;
      case e_CSHELL:
	if (csh_output_first_check(a_stream, a_cmd->argv[0], DO_EXIT,
				   a_command_line) >= 0) {
	  if (csh_output_next_part(a_stream, a_cmd->argv[0], exit_flag,
				   NO_CHECK, no_ups_env_flag, a_inst,
				   a_db_info, a_command_line) < 0) {
	    FPRINTF_ERROR();
	  } else {
	    (void )csh_output_last_part_req(a_stream, a_cmd->argv[0],
					    a_command_line);
	  }
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
    } else {
      FPRINTF_ERROR();
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_sourceoptional( const t_upstyp_matched_instance * const a_inst,
			      const t_upstyp_db * const a_db_info,
			      const t_upsugo_command * const a_command_line,
			      const FILE * const a_stream,
			      const t_upsact_cmd * const a_cmd)
{
  int exit_flag = NO_EXIT;
  int no_ups_env_flag = DO_UPS_ENV;

  CHECK_NUM_PARAM("sourceOptional");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* Determine which flags (if any) were entered */
    GET_FLAGS();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (sh_output_first_check(a_stream, a_cmd->argv[0], NO_EXIT,
				a_command_line) >= 0) {
	if (sh_output_next_part(a_stream, a_cmd->argv[0], exit_flag,
				NO_CHECK, no_ups_env_flag, a_inst, a_db_info,
				a_command_line) < 0) {
	  FPRINTF_ERROR();
	} else {
	  SH_OUTPUT_LAST_PART_OPT();
	}
      }
      break;
    case e_CSHELL:
      if (csh_output_first_check(a_stream, a_cmd->argv[0], NO_EXIT,
				a_command_line) >= 0) {
	if (csh_output_next_part(a_stream, a_cmd->argv[0], exit_flag,
				NO_CHECK, no_ups_env_flag, a_inst, a_db_info,
				a_command_line) < 0) {
	  FPRINTF_ERROR();
	} else {
	  CSH_OUTPUT_LAST_PART_OPT();
	}
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_sourcereqcheck( const t_upstyp_matched_instance * const a_inst,
			      const t_upstyp_db * const a_db_info,
			      const t_upsugo_command * const a_command_line,
			      const FILE * const a_stream,
			      const t_upsact_cmd * const a_cmd)
{
  int exit_flag = NO_EXIT;
  int no_ups_env_flag = DO_UPS_ENV;

  CHECK_NUM_PARAM("sourceReqCheck");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
    /* Determine which flags (if any) were entered */
    GET_FLAGS();

    if (UPS_ERROR == UPS_SUCCESS) {
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
	if (sh_output_first_check(a_stream, a_cmd->argv[0], DO_EXIT,
				  a_command_line) >= 0) {
	  if (sh_output_next_part(a_stream, a_cmd->argv[0], exit_flag,
				  DO_CHECK, no_ups_env_flag, a_inst, a_db_info,
				  a_command_line) < 0) {
	    FPRINTF_ERROR();
	  } else {
	    (void )sh_output_last_part_req(a_stream, a_cmd->argv[0],
					   a_command_line);
	  }
	}
	break;
      case e_CSHELL:
	if (csh_output_first_check(a_stream, a_cmd->argv[0], DO_EXIT,
				  a_command_line) >= 0) {
	  if (csh_output_next_part(a_stream, a_cmd->argv[0], exit_flag,
				  DO_CHECK, no_ups_env_flag, a_inst, a_db_info,
				  a_command_line) < 0) {
	    FPRINTF_ERROR();
	  } else {
	    (void )csh_output_last_part_req(a_stream, a_cmd->argv[0],
					   a_command_line);
	  }
	}
	break;
      default:
	upserr_vplace();
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
    } else {
      FPRINTF_ERROR();
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_sourceoptcheck( const t_upstyp_matched_instance * const a_inst,
			      const t_upstyp_db * const a_db_info,
			      const t_upsugo_command * const a_command_line,
			      const FILE * const a_stream,
			      const t_upsact_cmd * const a_cmd)
{
  int exit_flag = NO_EXIT;
  int no_ups_env_flag = DO_UPS_ENV;

  CHECK_NUM_PARAM("sourceOptCheck");

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
    /* Determine which flags (if any) were entered */
    GET_FLAGS();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (sh_output_first_check(a_stream, a_cmd->argv[0], NO_EXIT,
				a_command_line) >= 0) {
	if (sh_output_next_part(a_stream, a_cmd->argv[0], exit_flag,
				DO_CHECK, no_ups_env_flag, a_inst, a_db_info,
				a_command_line) < 0) {
	  FPRINTF_ERROR();
	} else {
	  SH_OUTPUT_LAST_PART_OPT();	
	}
      }
      break;
    case e_CSHELL:
      if (csh_output_first_check(a_stream, a_cmd->argv[0], NO_EXIT,
				a_command_line) >= 0) {
	if (csh_output_next_part(a_stream, a_cmd->argv[0], exit_flag,
				DO_CHECK, no_ups_env_flag, a_inst, a_db_info,
				a_command_line) < 0) {
	  FPRINTF_ERROR();
	} else {
	  CSH_OUTPUT_LAST_PART_OPT();
	}
      }
      break;
    default:
      upserr_vplace();
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_vplace();
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

static void f_writecompilescript(
			       const t_upstyp_matched_instance * const a_inst,
			       const t_upstyp_db * const a_db_info,
			       const t_upsugo_command * const a_command_line,
			       const FILE * const a_stream,
			       const t_upsact_cmd * const a_cmd)
{
  t_upstyp_matched_product mproduct = {NULL, NULL, NULL};
  t_upslst_item *cmd_list = NULL;
  char buff[MAX_LINE_LEN];
  char time_buff[MAX_LINE_LEN];
  char *time_ptr;
  int moved_to_old = 0, moved_to_timedate = 0;
  FILE *compile_file;

  /* skip this whole action if we are being called while compiling */
  if (! g_COMPILE_FLAG) {
    CHECK_NUM_PARAM("writeCompileScript");

    /* only proceed if we have a valid number of parameters */
    if (UPS_ERROR == UPS_SUCCESS) {
      /* this action does the following -
	 1. locate the action=command section which matches the
	       entered parameter
	 2. if there is none, we are done, else, locate current compile
	       file.
	 3. if there is none, skip to next step. else, rename current one
	       if desired. (on any error before completing the compile, this
	       file will be renamed back to the original name if possible.
	       of course this is not possible if we are overwriting the current
	       file.)
	 4. open compile file
	 5. process actions and write them to the compile file.
	 6. close compile file */
      /* 1     first, setup the matched product */
      mproduct.db_info = (t_upstyp_db *)a_db_info;
      mproduct.minst_list = upslst_new((void *)a_inst);
    
      /*       get the action command list */
      cmd_list = upsact_get_cmd((t_upsugo_command *)a_command_line, &mproduct,
				a_cmd->argv[1], e_setup);
      if (UPS_ERROR == UPS_SUCCESS) {
	/* 2      now that we have the list, locate the current compile file
	          if there is one */
	if (upsutl_is_a_file(a_cmd->argv[0]) == UPS_SUCCESS) {
	  /* 3   the file exists. check argv[2] to see if we need to rename the
	         file before writing the new one. if no flag was passed then
	         just overwrite the file */
	  if (a_cmd->argc == g_cmd_maps[a_cmd->icmd].max_params) {
	    /* the flag was there, now see what it is */
	    if (! strcmp(a_cmd->argv[a_cmd->argc - 1], OLD_FLAG)) {
	      /* append ".OLD" to the file name */
	      sprintf(buff, "mv %s %s.%s\n", a_cmd->argv[0], a_cmd->argv[0],
		      OLD_FLAG);
	      DO_SYSTEM_MOVE(moved_to_old);
	    } else if (! strcmp(a_cmd->argv[a_cmd->argc - 1], DATE_FLAG)) {
	      /* append a timedate stamp to the file name */
	      time_ptr = upsutl_time_date();
	      strcpy(time_buff, time_ptr);

	      /* remove any whitespace */
	      (void )upsutl_str_remove(time_buff, WSPACE);
	      sprintf(buff, "mv %s %s.%s\n", a_cmd->argv[0], a_cmd->argv[0],
		      time_buff);
	      DO_SYSTEM_MOVE(moved_to_timedate);
	    }
	  }
	} else {
	  /* there currently is no compile file with this name.  reset the
	     error status so we only need to test for success later */
	  UPS_ERROR = UPS_SUCCESS;
	}
	/* if there was no error in renaming the file, then proceed */
	if (UPS_ERROR == UPS_SUCCESS) {
	  /* 4    open the file that we will write compiled commands out to */
	  if ((compile_file = fopen(a_cmd->argv[0], "w")) == NULL) {
	    upserr_add(UPS_OPEN_FILE, UPS_FATAL, a_cmd->argv[0]);
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fopen",
		       strerror(errno));
	  } else {
	    /* 5   process actions and write them to the compile file. mark
	           that the actions functions are being called due to a compile
		   command */
	    g_COMPILE_FLAG = 1;
	    upsact_process_commands(cmd_list, compile_file);
	    g_COMPILE_FLAG = 0;

	    /* 6   close the compile file */
	    if (fclose(compile_file) == EOF) {
	      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fclose",
			 strerror(errno));
	    }
	  }
	}
      } else {
	/* could not get the list of commands, there must not be an
	   action=argv[1] line in the file */
	upserr_add(UPS_NO_ACTION, UPS_WARNING, a_cmd->argv[1]);
      }
    }

    /* if we moved the old compile file to a backup and then got an error while
       creating the new file, we should move the old file back so we don't
       break current things */
    if ((moved_to_old || moved_to_timedate) && (UPS_ERROR != UPS_SUCCESS)) {
      /* yes we did the move, and got an error, move the file back */
      if (moved_to_old) {
	sprintf(buff, "mv %s.%s %s\n", a_cmd->argv[0], OLD_FLAG,
		a_cmd->argv[0]);
      } else {
	sprintf(buff, "mv %s.%s %s\n", a_cmd->argv[0], time_buff,
		a_cmd->argv[0]);
      }
      /* since we are at the end, it does not matter which flag we use here */
      DO_SYSTEM_MOVE(moved_to_old);
    }

    /* release any memory we acquired */
    if (mproduct.minst_list) {
      (void )upslst_free(mproduct.minst_list, ' ');
    }
    if (cmd_list) {
      upsact_cleanup(cmd_list);
    }
  }
}

static void f_dodefaults( const t_upstyp_matched_instance * const a_inst,
			  const t_upstyp_db * const a_db_info,
			  const t_upsugo_command * const a_command_line,
			  const FILE * const a_stream,
			  const t_upsact_cmd * const a_cmd)
{
  t_upsact_cmd lcl_cmd;
  static char buff[MAX_LINE_LEN];
  char *tmp_prod_dir = NULL;
  char *uprod_name;

  /* only proceed if we have a stream to write the output to */
  if (a_stream) {
    switch ( a_cmd->iact ) {
    case e_setup:	/* Define <PROD>_DIR and SETUP_<PROD> */
      /* use our local copy since we have to change it */
      lcl_cmd.iact = a_cmd->iact;
      lcl_cmd.argc = g_cmd_maps[e_envset].min_params;   /* # of args */
      lcl_cmd.icmd = e_envset;
      lcl_cmd.argv[0] = buff;

      /* since the prod_dir may come from the command line we need to check
	 if the user entered one that we have to use */
      if (a_command_line->ugo_productdir) {
	tmp_prod_dir = a_command_line->ugo_productdir;
      } else if (a_inst->version && a_inst->version->prod_dir) {
	tmp_prod_dir = a_inst->version->prod_dir;
      }
      if (a_inst->version && tmp_prod_dir && a_inst->version->product) {
	uprod_name = upsutl_upcase(a_inst->version->product);
	if (UPS_ERROR == UPS_SUCCESS) {
	  sprintf(buff, "%s_DIR", uprod_name);
	  
	  lcl_cmd.argv[1] = tmp_prod_dir;
	  f_envset(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);
	}
      }
      upsget_envout(a_stream, a_db_info, a_inst, a_command_line);
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
      /* use our local copy since we have to change it */
      lcl_cmd.iact = a_cmd->iact;
      lcl_cmd.argc = g_cmd_maps[e_copyman].min_params;   /* # of args */
      f_copyman(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
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
      /* use our local copy since we have to change it */
      lcl_cmd.iact = a_cmd->iact;
      lcl_cmd.argc = g_cmd_maps[e_uncopyman].min_params;   /* # of args */
      f_uncopyman(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
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
      /* use our local copy since we have to change it */
      lcl_cmd.iact = a_cmd->iact;
      lcl_cmd.argc = g_cmd_maps[e_envunset].min_params;   /* # of args */
      lcl_cmd.icmd = e_envunset;
      lcl_cmd.argv[0] = buff;
      if (a_inst->version && a_inst->version->product) {
	uprod_name = upsutl_upcase(a_inst->version->product);
	if (UPS_ERROR == UPS_SUCCESS) {
	  sprintf(buff, "%s_DIR", uprod_name);
	  f_envunset(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);
	  sprintf(buff, "%s%s", SETUPENV, uprod_name);
	  f_envunset(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);
	}
      }
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
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL,
		 g_cmd_maps[a_cmd->icmd].cmd);
    }
  }
}

