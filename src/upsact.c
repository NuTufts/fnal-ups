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
 *           2. add an e_<action> to the enum in upsact.h
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

extern void list_K( const t_upstyp_matched_instance * const instance, 
		    const t_upsugo_command * const command, 
		    const t_upstyp_matched_product * const product);

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
#define WSPACE " \t\n\r\f"
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
#define CATMANPAGES "catman"
#define MANPAGES "man"

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
			 t_upsact_item * const p_act_itm,
			 const char * const act_name,
			 const char copt );
t_upslst_item *next_top_prod( t_upslst_item * top_list,
			      t_upsact_item *const p_act_itm, 
			      const char *const act_name );
t_upstyp_action *get_act( const t_upsugo_command * const ugo_cmd,
			  t_upstyp_matched_product * mat_prod,
			  const char * const act_name );
t_upsact_item *get_top_item( t_upsugo_command * const ugo_cmd,
			     t_upstyp_matched_product *mat_prod,
			     const char * const act_name );
char *get_mode( const t_upsact_item *const p_act_itm ); 
t_upsugo_command *get_ugosetup( t_upsact_item * p_act_itm, 
				t_upsact_cmd * const p_cmd );
t_upsact_item *find_prod_name( t_upslst_item *const dep_list,
			       const char *const prod_name );
t_upsact_item *find_prod_dep_name( t_upslst_item *const dep_list,
				   const char *const prod_name );
t_upsact_item *find_actitm_ptr( t_upslst_item* const dep_list,
				const t_upsact_item* const act_item );
t_upsact_item *find_ugo_ptr( t_upslst_item* const dep_list,
			     const t_upsact_item* const p_act_itm );
t_upsact_item *new_act_item( t_upsugo_command * const ugo_cmd,
			     t_upstyp_matched_product *mat_prod,
			     const int level,
			     const int mode,
			     const char * const act_name );
t_upsact_item *copy_act_item( const t_upsact_item * const act_itm );
t_upstyp_action *new_default_action( t_upsact_item *const p_act_itm, 
				     const char * const act_name, 
				     const int iact );
t_upslst_item *reverse_command_list( t_upsact_item *const p_act_itm,
				     t_upslst_item *const cmd_list );
char *actitem2str( const t_upsact_item *const p_act_itm );
t_upsugo_command *get_SETUP_prod( t_upsact_cmd * const p_cmd, 
				  const int i_act );
int lst_cmp_str( t_upslst_item * const l1, 
		 t_upslst_item * const l2 );
int do_exit_action( const t_upsact_cmd * const a_cmd );

/* functions to handle specific action commands */

#define ACTION_PARAMS \
  const t_upstyp_matched_instance * const a_inst,  \
  const t_upstyp_db * const a_db_info,             \
  const t_upsugo_command * const a_command_line,   \
  const FILE * const a_stream,                     \
  const t_upsact_cmd * const a_cmd

static void f_copyhtml( ACTION_PARAMS);
static void f_copyinfo( ACTION_PARAMS);
static void f_copyman( ACTION_PARAMS);
static void f_uncopyman( ACTION_PARAMS);
static void f_copycatman( ACTION_PARAMS);
static void f_uncopycatman( ACTION_PARAMS);
static void f_copynews( ACTION_PARAMS);
static void f_envappend( ACTION_PARAMS);
static void f_envprepend( ACTION_PARAMS);
static void f_envremove( ACTION_PARAMS);
static void f_envset( ACTION_PARAMS);
static void f_envsetifnotset( ACTION_PARAMS);
static void f_envunset( ACTION_PARAMS);
static void f_exeaccess( ACTION_PARAMS);
static void f_execute( ACTION_PARAMS);
static void f_filetest( ACTION_PARAMS);
static void f_pathappend( ACTION_PARAMS);
static void f_pathprepend( ACTION_PARAMS);
static void f_pathremove( ACTION_PARAMS);
static void f_pathset( ACTION_PARAMS);
static void f_addalias( ACTION_PARAMS);
static void f_unalias( ACTION_PARAMS);
static void f_sourcerequired( ACTION_PARAMS);
static void f_sourceoptional( ACTION_PARAMS);
static void f_sourcereqcheck( ACTION_PARAMS);
static void f_sourceoptcheck( ACTION_PARAMS);
static void f_sourcecompilereq( ACTION_PARAMS);
static void f_sourcecompileopt( ACTION_PARAMS);
static void f_writecompilescript( ACTION_PARAMS);
static void f_setupenv( ACTION_PARAMS);
static void f_proddir( ACTION_PARAMS);
static void f_unsetupenv( ACTION_PARAMS);
static void f_unproddir( ACTION_PARAMS);
static void f_dodefaults( ACTION_PARAMS);

#define CHECK_NUM_PARAM(action) \
    if ((a_cmd->argc < g_cmd_maps[a_cmd->icmd].min_params) ||   \
        (a_cmd->argc > g_cmd_maps[a_cmd->icmd].max_params)) {   \
      upserr_vplace();                                          \
      upserr_add(UPS_INVALID_ACTION_PARAMS, UPS_FATAL,          \
                 action, g_cmd_maps[a_cmd->icmd].min_params,    \
                 g_cmd_maps[a_cmd->icmd].max_params,            \
		 a_cmd->argc);                                  \
    }

#define OUTPUT_VERBOSE_MESSAGE(cmd)  \
    if (a_inst->version && a_inst->version->product) {                       \
      upsver_mes(1, "UPSACT: Processing action \'%s\' for product \'%s\'\n", \
                 cmd, a_inst->version->product);                             \
    } else {                                                                 \
      upsver_mes(1, "UPSACT: Processing action \'%s\'\n", cmd);              \
    }

#define GET_DELIMITER() \
    if (a_cmd->argc == g_cmd_maps[a_cmd->icmd].max_params) {            \
      /* remember arrays start at 0, so subtract one here */            \
      delimiter =                                                       \
	(a_cmd->argv[g_cmd_maps[a_cmd->icmd].max_params-1]);            \
      /* trim delimiter for quotes */                                   \
      upsutl_str_remove_end_quotes( delimiter, "\"\'", 0 );             \
    } else {                                                            \
      /* use the default, nothing was entered */                        \
      delimiter = g_default_delimiter;                                  \
    }

#define CHECK_FOR_PATH(thePath, theDelimiter)  \
    /* if the variable is path then it must be of a certain case */  \
    if ( upsutl_stricmp(thePath,a_cmd->argv[0])) {                   \
      /* it was not equal to path so use the original value */       \
      pathPtr = a_cmd->argv[0];                                      \
    } else {                                                         \
      /* it was path, make sure we use a right one by using ours */  \
      pathPtr = thePath;                                             \
      /* and the delimiter must be set right too */                  \
      delimiter = theDelimiter;                                      \
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
	if (! upsutl_stricmp(a_cmd->argv[i], EXIT)) {                      \
	  exit_flag = DO_EXIT;                                             \
	  continue;                                                        \
	}                                                                  \
	if (! upsutl_stricmp(a_cmd->argv[i], NO_UPS_ENV)) {                \
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

#define GET_MAN_SOURCE(pages)      \
   if (a_cmd->argc == 1) {                                  \
     /* the user specified a source in the action */        \
     buf = (char *)a_cmd->argv[0];                          \
   } else {                                                 \
     /* we have to construct a source */                    \
     buf = upsutl_find_manpages(a_inst, a_db_info, pages);  \
   }

#define SET_SETUP_PROD() \
   upsget_envout(a_stream, a_db_info, a_inst, a_command_line);

#define DO_SYSTEM_MOVE(move_flag)  \
   if (system(g_buff) != 0) {                                               \
     /* error from system call */                                         \
     upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "system", strerror(errno));  \
   } else {                                                               \
     move_flag = 1;                                                       \
   }

#define SET_PARSE_ERROR( str ) \
    upserr_vplace(); \
    upserr_add( UPS_ACTION_PARSE, UPS_FATAL, str );

#define SET_NO_ACTION_ERROR( str ) \
    upserr_vplace(); \
    upserr_add( UPS_NO_ACTION, UPS_FATAL, str );

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
static char g_buff[MAX_LINE_LEN];
static char *g_shPath = "PATH";
static char *g_cshPath = "path";
static char *g_shDelimiter = ":";
static char *g_cshDelimiter = " ";

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
  {e_uncurrent,     "uncurrent", 0, 0x00000001, e_current},
  {e_undevelopment, "undevelopment", 0, 0x00000000, e_development},
  {e_unnew,         "unnew", 0, 0x00000000, e_new},
  {e_unold,         "unold", 0, 0x00000000, e_old},
  {e_untest,        "untest", 0, 0x00000000, e_test},
  {e_unchain,       "unchain", 0, 0x00000000, e_chain},
  {e_setup,       "setup",       "?B:cde:f:g:H:jkm:M:noO:q:r:stU:vVz:Z", 0x00000001, e_invalid_action},
  {e_unsetup,     "unsetup",     "?cde:f:g:H:jm:M:noO:q:stU:vVz:Z", 0x00000001, e_setup},
  {e_list,        "list",        "a?cdf:g:h:H:K:lm:M:noq:r:tU:vVz:Z0123", 0x00000000, e_invalid_action},
  {e_configure,   "configure",   "?cdf:g:H:m:M:noO:q:r:stU:vVz:Z", 0x00000000, e_invalid_action},
  {e_copy,        "copy",        "?A:b:cCdD:f:g:H:m:M:noO:p:q:r:tT:u:U:vVWXz:Z0123", 0x00000000, e_invalid_action},
  {e_declare,     "declare",     "?A:b:cCdD:f:g:H:m:M:noO:p:q:r:tT:u:U:vVz:Z0123", 0x00000000, e_invalid_action},
  {e_depend,      "depend",      "?cdotg:f:H:K:lm:M:q:r:U:vVz:Z", 0x00000000, e_invalid_action},
  {e_exist,       "exist",       "?B:cde:f:g:H:jkm:M:oO:q:r:tU:vVz:Z", 0x00000000, e_invalid_action},
  {e_modify,      "modify",      "a?A:Ef:H:m:M:Nop:q:r:T:U:vVx:z:Z", 0x00000000, e_invalid_action},
  {e_start,       "start",       "?cdf:g:H:m:M:noO:q:r:stU:vVwz:Z", 0x00000000, e_invalid_action},
  {e_stop,        "stop",        "?cdf:g:H:m:M:noO:q:r:stU:vVz:Z", 0x00000000, e_invalid_action},
  {e_tailor,      "tailor",      "?cdf:g:h:H:K:m:M:noO:q:r:stU:vVz:Z", 0x00000000, e_invalid_action},
  {e_unconfigure, "unconfigure", "?cdf:g:H:m:M:noO:q:r:stU:vVz:Z", 0x00000000, e_configure},
  {e_undeclare,   "undeclare",   "?cdf:g:H:m:M:noO:q:r:tU:vVyYz:Z0123", 0x00000000, e_declare},
  {e_create,      "create",      "?f:H:m:M:p:q:vZ", 0x00000000, e_invalid_action},
  {e_get,         "get",         "?cdf:Fg:H:m:M:noq:r:tU:vVz:Z", 0x00000000, e_invalid_action},
  {e_validate,    "validate",    "?cdf:g:h:H:lm:M:nNoq:r:StU:vVz:Z", 0x00000000, e_invalid_action},
  {e_flavor,      "flavor",      "?f:H:l0123", 0x00000000, e_invalid_action},
  {e_verify,      "verify",        "a?cdf:g:h:H:K:lm:M:noq:r:tU:vVz:Z0123", 0x00000000, e_invalid_action},
  {e_help,        "help",
            "a?A:b:B:cCdD:eEf:Fg:h:H:jkK:lm:M:nNoO:p:q:r:sStT:u:U:vVwW:x:XyYz:Z", 0x00000000, e_invalid_action},
  /* the following one must always be at the end and contains all options */
  {e_unk,         NULL,
            "a?A:b:B:cCdD:eEf:Fg:h:H:jkK:lm:M:nNoO:p:q:r:sStT:u:U:vVwW:x:XyYz:Z", 0x00000000, e_invalid_action}
};

/* These action commands are listed in order of use.  Hopefully the more
 * used actions are at the front of the list. Also the ones most used by
 * setup and unsetup are at the front of the array.  The actions in this
 * array MUST appear in the same order as in the above enumeration. NOTE:
 * the 4th and 5th parameters are the minimum and maximum parameters expected
 * by the action. The six parameter is the corresponding 'undo' command.
 */
t_cmd_map g_cmd_maps[] = {
  { "setupoptional", e_setupoptional, NULL, 1, 1, e_unsetupoptional },
  { "setuprequired", e_setuprequired, NULL, 1, 1, e_unsetuprequired },
  { "unsetupoptional", e_unsetupoptional, NULL, 1, 1, e_setupoptional },
  { "unsetuprequired", e_unsetuprequired, NULL, 1, 1, e_setuprequired },
  { "exeactionoptional", e_exeactionoptional, NULL, 1, 1, e_rev_exeactionoptional },
  { "exeactionrequired", e_exeactionrequired, NULL, 1, 1, e_rev_exeactionrequired },
  { "rev_exeactionoptional", e_rev_exeactionoptional, NULL, 1, 1, e_invalid_cmd },
  { "rev_exeactionrequired", e_rev_exeactionrequired, NULL, 1, 1, e_invalid_cmd },
  { "sourcecompilereq", e_sourcecompilereq, f_sourcecompilereq, 1, 1, e_invalid_cmd },
  { "sourcecompileopt", e_sourcecompileopt, f_sourcecompileopt, 1, 1, e_invalid_cmd },
  { "envappend", e_envappend, f_envappend, 2, 3, e_envremove },
  { "envremove", e_envremove, f_envremove, 2, 3, e_invalid_cmd },
  { "envprepend", e_envprepend, f_envprepend, 2, 3, e_envremove },
  { "envset", e_envset, f_envset, 2, 2, e_envunset },
  { "envsetifnotset", e_envsetifnotset, f_envsetifnotset, 2, 2, e_envunset },
  { "envunset", e_envunset, f_envunset, 1, 1, e_invalid_cmd },
  { "pathappend", e_pathappend, f_pathappend, 2, 3, e_pathremove },
  { "pathremove", e_pathremove, f_pathremove, 2, 3, e_pathappend},
  { "pathprepend", e_pathprepend, f_pathprepend, 2, 3, e_pathremove },
  { "pathset", e_pathset, f_pathset, 2, 2, e_envunset },
  { "addalias", e_addalias, f_addalias, 2, 2, e_unalias },
  { "unalias", e_unalias, f_unalias, 1, 1, e_invalid_cmd },
  { "sourcerequired", e_sourcerequired, f_sourcerequired, 1, 3, e_sourceoptional },
  { "sourceoptional", e_sourceoptional, f_sourceoptional, 1, 3, e_sourceoptional },
  { "sourcereqcheck", e_sourcereqcheck, f_sourcereqcheck, 1, 3, e_sourceoptcheck },
  { "sourceoptcheck", e_sourceoptcheck, f_sourceoptcheck, 1, 3, e_sourceoptcheck },
  { "exeaccess", e_exeaccess, f_exeaccess, 1, 1, e_invalid_cmd },
  { "execute", e_execute, f_execute, 1, 2, e_invalid_cmd },
  { "filetest", e_filetest, f_filetest, 2, 3, e_invalid_cmd },
  { "copyhtml", e_copyhtml, f_copyhtml, 1, 1, e_invalid_cmd },
  { "copyinfo", e_copyinfo, f_copyinfo, 1, 1, e_invalid_cmd },
  { "copyman", e_copyman, f_copyman, 0, 1, e_uncopyman },
  { "uncopyman", e_uncopyman, f_uncopyman, 0, 1, e_copyman},
  { "copycatman", e_copycatman, f_copycatman, 0, 1, e_uncopycatman },
  { "uncopycatman", e_uncopycatman, f_uncopycatman, 0, 1, e_copycatman},
  { "copynews", e_copynews, f_copynews, 1, 1, e_invalid_cmd },
  { "writecompilescript", e_writecompilescript, f_writecompilescript, 2, 3, e_invalid_cmd },
  { "dodefaults", e_dodefaults, f_dodefaults, 0, 0, e_dodefaults },
  { "setupenv", e_setupenv, f_setupenv, 0, 0, e_unsetupenv },
  { "proddir", e_proddir, f_proddir, 0, 0, e_unproddir },
  { "unsetupenv", e_unsetupenv, f_unsetupenv, 0, 0, e_setupenv },
  { "unproddir", e_unproddir, f_unproddir, 0, 0, e_proddir },
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
 *         char *, options, 'l': it will print all action commands for all instances,
 *                               default is to only print instances.
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
  t_upslst_item *dep_l = 0;

  if ( !sopt )
    sopt = s_sopt;

  if ( !ugo_cmd || !act_name )
    return 0;

  /* get depency list */

  if ( strchr( sopt, 'l' ) ) 
    dep_list = upsact_get_cmd( ugo_cmd, mat_prod, act_name, ups_cmd );
  else
    dep_list = upsact_get_dep( ugo_cmd, mat_prod, act_name, ups_cmd );

  /* option l: print all action commands for all instances */

  if ( strchr( sopt, 'l' ) ) {

    for ( dep_l = upslst_first( dep_list); dep_l; dep_l = dep_l->next ) {
      upsact_print_item( (t_upsact_item *)dep_l->data, sopt );
    }
  }

  /* print only instances */

  else {

    t_upstyp_matched_product *mat_prod;
    t_upstyp_matched_instance *mat_inst;
    t_upslst_item *l_mproduct;
    t_upsact_item dep_act_itm; 

    /* list dependencies in reverse order */

    for ( dep_l = upslst_last( dep_list ); dep_l; dep_l = dep_l->prev ) {
      t_upsact_item *act_ptr = (t_upsact_item *)dep_l->data;
      t_upsugo_command *dep_ugo = act_ptr->dep_ugo;      

      l_mproduct = upsmat_instance( dep_ugo, NULL, 1 );
      if ( !l_mproduct || !l_mproduct->data )
	continue;

      mat_prod = (t_upstyp_matched_product *)l_mproduct->data;
      mat_inst = 
	(t_upstyp_matched_instance *)(upslst_first( mat_prod->minst_list ))->data;

      if ( strchr( sopt, 'K' ) ) {

	dep_ugo->ugo_key = ugo_cmd->ugo_key;      
	list_K( mat_inst, dep_ugo, mat_prod );
	dep_ugo->ugo_key = 0;
      }
      else {

	dep_act_itm.ugo = dep_ugo;
	dep_act_itm.mat = mat_prod;
	printf( "%s\n", actitem2str( &dep_act_itm) );
      }


      upsutl_free_matched_product_list( &l_mproduct );
    }

  }

  upsact_cleanup( upslst_first( dep_list ) );
  return 1;
}

/*-----------------------------------------------------------------------
 * upsact_get_dep
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
t_upslst_item *upsact_get_dep( t_upsugo_command * const ugo_cmd,
			       t_upstyp_matched_product * const mat_prod,
			       const char * const act_name,
			       int ups_cmd )
{
  t_upsact_item *act_itm;
  t_upslst_item *top_list = 0;
  t_upslst_item *dep_list = 0;

  g_ups_cmd = ups_cmd;

  if ( !ugo_cmd || !act_name )
    return 0;

  /* create a partial action structure for top product */

  if ( !(act_itm = get_top_item( ugo_cmd, mat_prod, act_name )) ) {
    upserr_vplace();
    upserr_add( UPS_NO_PRODUCT_FOUND, UPS_FATAL, ugo_cmd->ugo_product );
    return 0;
  }

  /* create a list of 1'st level dependecies,
     these instances have precedence over products at lower (higher ?) levels */

  top_list = next_top_prod( top_list, act_itm, act_name );
  
  /* add top product to dep list */

  act_itm->dep_ugo = ugo_cmd;
  dep_list = upslst_add( dep_list, act_itm );

  /* get the dependency list */

  dep_list = next_cmd( top_list, dep_list, act_itm, act_name, 'd' );

  /* clean up top list */

  upsact_cleanup( top_list );

  return upslst_first( dep_list );
}

/*-----------------------------------------------------------------------
 * upsact_get_cmd
 *
 * Will get list of action commands for a product, following dependencies.
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
  t_upsact_item *act_itm;
  t_upslst_item *top_list = 0;
  t_upslst_item *dep_list = 0;

  g_ups_cmd = ups_cmd;

  if ( !ugo_cmd || !act_name )
    return 0;

  /* create a partial action structure for top product */

  if ( !(act_itm = get_top_item( ugo_cmd, mat_prod, act_name )) ) {
    upserr_vplace();
    upserr_add( UPS_NO_PRODUCT_FOUND, UPS_FATAL, ugo_cmd->ugo_product );
    return 0;
  }

  /* create a list of 1'st level dependecies,
     these instances have precedence over products at lower (higher ?) levels */

  top_list = next_top_prod( top_list, act_itm, act_name );
  
  /* get the dependency list */

  dep_list = next_cmd( top_list, dep_list, act_itm, act_name, 'c' );

  /* clean up top list */

  upsact_cleanup( top_list );

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
  static char trim_chars[] = " \t\n\r\f);";
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
     the created argv array is just a list of pointers into that string. */
  
  pcmd = upsact_new_upsact_cmd( act_s );
  act_s = pcmd->pbuf;

  /* get name of action command */

  if ( (act_p = strchr( act_s, OPEN_PAREN )) != NULL ) {

    /* save pointer to parenthese and
       get rid of trailing spaces in name of action command */

    act_e = act_p - 1;
    while ( act_e && *act_e && isspace( *(act_e) ) ){ --act_e; }    
    len = act_e - act_s + 1;

    /* look for action command in the supported action array */
    
    for ( i = 0; g_cmd_maps[i].cmd; ++i ) {

	if ( len != strlen(g_cmd_maps[i].cmd) )
	continue;

      if ( !upsutl_strincmp( act_s, g_cmd_maps[i].cmd, (size_t)len ) ) {
	
	/* we found a match. create a pointer to a string with these parameters.
	   note - it does not include an open parenthesis */

	act_s = act_p + 1;
		 
	/* trim off whitespace & the ending ")", we will also get rid
           of ending ';' */

	upsutl_str_remove_edges( act_s, trim_chars );

        /* save the location in the array */

	icmd = g_cmd_maps[i].icmd;
	
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
    upsmem_free( pcmd );
    P_VERB_s( 3, "Parse nothing" );
    return 0;
  }
}

void upsact_cleanup( t_upslst_item *dep_list )
{
  t_upslst_item * l_ptr = upslst_first( dep_list );

  if ( !l_ptr )
    return;

  while( l_ptr ) {
    t_upsact_item *act_ptr = l_ptr->data;
    l_ptr = upslst_delete( l_ptr, act_ptr, ' ' );
    upsact_free_act_item( act_ptr );
  }
}


/*-----------------------------------------------------------------------
 * upsact_print_item
 *
 * It will print a t_upsact_item to stdout.
 *
 * options (sopt):
 *    always : print correspoding product instance.
 *    'l'    : also print corresponding action commands
 *    't'    : also print indentions (corresponding to level)
 *
 * Input : t_upsact_item *, action item
 *         char *, sopt 
 * Output: none
 * Return: none
 */
void upsact_print_item( const t_upsact_item *const act_itm, 
			char * sopt )
{
  static char s_sopt[] = "";
  int i;
  if ( !sopt )
    sopt = s_sopt;

  if ( !act_itm )
    return;

  if ( strchr( sopt, 't' ) ) for ( i=0; i<act_itm->level; i++ ) { printf( "   " ); }
  printf( "%s", actitem2str( act_itm ) );
  if ( strchr( sopt, 'l' ) ) {
    printf( ":" );
    upsact_print_cmd( act_itm->cmd );
  }
  else 
    printf( "\n" );
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
  
  if ( !cmd_cur ) {
    printf( "\n" ); 
    return;
  }

  icmd = cmd_cur->icmd;
  
  printf( "%s(", g_cmd_maps[icmd].cmd );
  for ( i = 0; i < cmd_cur->argc; i++ ) {
    if ( i == cmd_cur->argc - 1 ) 
      printf( "%s", cmd_cur->argv[i] );
    else
      printf( "%s, ", cmd_cur->argv[i] );
  }
  printf( ")\n" ); 
}

/*-----------------------------------------------------------------------
 * upsact_new_upsact_cmd
 *
 * It will create a new upsact_cmd structure
 *
 * Input : char*, a string representing an action command
 * Output: none
 * Return: t_upsact_cmd *, a pointer to an action command structure
 */
t_upsact_cmd *upsact_new_upsact_cmd( const char * const act_str )
{
  t_upsact_cmd *cmd_ptr = 0;

  cmd_ptr = (t_upsact_cmd *)upsmem_malloc( sizeof( t_upsact_cmd ) + strlen( act_str ) + 1 );
  cmd_ptr->pbuf = (char *)(cmd_ptr + 1);
  
  strcpy( cmd_ptr->pbuf, act_str );

  return cmd_ptr;
}

/*-----------------------------------------------------------------------
 * upsact_free_act_cmd
 *
 * It will free a single t_upsact_cmd
 *
 * Input : t_upsact_cmd *, a pointer to an action command structure
 * Output: none
 * Return: none
 */
void upsact_free_act_cmd( t_upsact_cmd * const act_cmd )
{
  if ( act_cmd )
    upsmem_free( act_cmd );
}

void upsact_free_act_item( t_upsact_item * const act_itm ) 
{

  if ( act_itm && upsmem_get_refctr( act_itm ) <= 0 ) {
    if ( act_itm->ugo ) {
      upsmem_dec_refctr( act_itm->ugo );
      if ( upsmem_get_refctr( act_itm->ugo ) <= 0 )
	upsugo_free( act_itm->ugo );
    }
       
    if ( act_itm->mat && upsmem_get_refctr( act_itm->mat ) <= 1 ) {
      upsmem_dec_refctr( act_itm->mat );
      if ( upsmem_get_refctr( act_itm->mat ) <= 0 )
	ups_free_matched_product( act_itm->mat );
    }

    /* don't touch the t_upstyp_action */

    if ( act_itm->cmd ) {
      upsmem_dec_refctr( act_itm->cmd );
      upsact_free_act_cmd( act_itm->cmd );
    }

    upsmem_free( act_itm );
  }
}

int upsact_action2enum( const char * const act_name )
{
  /* it will map an action name to the corresponding enum */

  int iact = e_invalid_action, i = 0;

  if ( act_name ) {
    for ( i=0; g_cmd_info[i].cmd; i++ ) {
      if (! upsutl_stricmp(g_cmd_info[i].cmd, act_name)) {
	iact = g_cmd_info[i].cmd_index;
	break;
      }
    }
  }
  
  return iact;
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
	case e_envsetifnotset:
	case e_envunset:
	case e_pathappend:
	case e_pathremove:
	case e_pathprepend:
	case e_pathset:
	case e_addalias:
	case e_unalias:
	case e_filetest:
	case e_dodefaults:
	case e_setupenv:
	case e_proddir:
	case e_unsetupenv:
	case e_unproddir:
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

/*=======================================================================
 *
 * Private functions
 *
 *=====================================================================*/

/*-----------------------------------------------------------------------
 * next_top_prod
 *
 * Will fill the passed list of action item with translated actions.
 * It will not follow depencies. It's very close to 'next_cmd'.
 * It's recursive.
 *
 * Input : t_upsact_item *, action item
 *         char *, action name
 * Output: none
 * Return: t_upslst_item *, a list of top action items
 */
t_upslst_item *next_top_prod( t_upslst_item * top_list,
			      t_upsact_item *const p_act_itm, 
			      const char *const act_name )
{
  t_upslst_item *l_cmd = 0;
  t_upsact_cmd *p_cmd = 0;
  char *p_line = 0;
  int i_act = e_invalid_action;
  int i_cmd = e_invalid_cmd;

  if ( p_act_itm && p_act_itm->act ) {
    i_act = upsact_action2enum( act_name );
    l_cmd = p_act_itm->act->command_list;
  }
  else {
    return top_list;
  }

  for ( ; l_cmd; l_cmd = l_cmd->next ) {
    p_line = upsget_translation( p_act_itm->mat, p_act_itm->ugo,
				 (char *)l_cmd->data );
    p_cmd = upsact_parse_cmd( p_line );
    i_cmd = p_cmd ? p_cmd->icmd : e_invalid_cmd;

    if ( !p_cmd || p_cmd->icmd == e_invalid_cmd ) {
      SET_PARSE_ERROR( p_line );
      continue;
    }
    else if ( i_cmd > e_exeactionrequired ) {

      /* STANDARD action commands to be added to the list
	 ================================================ */

      continue;
    }
    
    if ( i_cmd > e_unsetuprequired ) {

      /* EXECUTE another action
	 ====================== */

      char *action_name = (char *)act_name;
      t_upsact_item *new_act_itm = 0;
      
      /* quit if option P set and optional command */

      if ( p_act_itm->ugo->ugo_P && !(i_cmd & 1) )
	continue;

      new_act_itm = new_act_item( p_act_itm->ugo, p_act_itm->mat,  
				  p_act_itm->level, i_cmd, p_cmd->argv[0] );

      /* ignore errors if optional exeaction */

      if ( !new_act_itm || !new_act_itm->act ) {
	if ( i_cmd & 1 ) {	  
	  SET_NO_ACTION_ERROR( p_cmd->argv[0] );
	  SET_PARSE_ERROR( p_line );
	}
	else {
	  upserr_clear();
	}
	upsact_free_act_item( new_act_itm );
	continue;
      }

      /* note: action name is parents action name */

      top_list = next_top_prod( top_list, new_act_itm,  action_name );
      P_VERB_s_s( 3, "Execute action:", p_line );
    }
    else if ( !(p_act_itm->ugo->ugo_j) ) {

      /* DEPENDENCIES
	 ============ */

      t_upsact_item *new_act_itm = 0;
      t_upsugo_command *new_ugo = 0;
      p_cmd->iact = i_act;

      /* quit if option P set and optional command */

      if ( p_act_itm->ugo->ugo_P && !(i_cmd & 1) )
	continue;

      /* get the ugo command */

      new_ugo = get_ugosetup( p_act_itm, p_cmd ); 

      /* new_ugo can be null if doing unsetup */

      if ( ! new_ugo && (i_cmd & 2) )
	continue;

      /* get the action item */

      if ( i_cmd & 2 ) 
	new_act_itm = new_act_item( new_ugo, 0, 0, i_cmd, "unsetup");
      else
	new_act_itm = new_act_item( new_ugo, 0, 0, i_cmd, "setup");

      if ( !new_act_itm ) {
	if ( i_cmd & 1 ) {
	  SET_PARSE_ERROR( p_line );
	}
	else {
	  upserr_clear();
	}
      }
      else {

	/* add a un/setup action item (product) */

	top_list = upslst_add( top_list, new_act_itm );
      }
    }
  }

  return upslst_first( top_list );
}

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
 *         char, option, 'd': will make a list of only depencies
 *               else : will make a list of commands
 *         
 * Output: none
 * Return: t_upsact_cmd *,
 */
t_upslst_item *next_cmd( t_upslst_item * const top_list,
			 t_upslst_item *dep_list,
			 t_upsact_item *const p_act_itm,
			 const char *const act_name,
			 const char copt)
{
  t_upslst_item *l_cmd = 0;
  t_upsact_cmd *p_cmd = 0;
  char *p_line = 0;
  int i_act = e_invalid_action;
  int i_cmd = e_invalid_cmd;

  if ( !p_act_itm )
    return dep_list;

  i_act = upsact_action2enum( act_name );

  if ( copt != 'd' && !p_act_itm->act ) {

    /* if we don't have an action, take care of defaults cases, controlled by: 
       g_cmd_info.flags, g_cmd_info.uncmd_index and g_cmd_maps.icmd_undo */

    if ( !(p_act_itm->act = new_default_action( p_act_itm, act_name, i_act )) )
      return dep_list;
  }
  
  l_cmd = p_act_itm->act->command_list;
  for ( ; l_cmd; l_cmd = l_cmd->next ) {

    /* translate and parse command */

    p_line = upsget_translation( p_act_itm->mat, p_act_itm->ugo,
				 (char *)l_cmd->data );
    p_cmd = upsact_parse_cmd( p_line );

    if ( !p_cmd || p_cmd->icmd == e_invalid_cmd ) {
      SET_PARSE_ERROR( p_line );
      continue;
    }

    p_cmd->iact = i_act;
    i_cmd = p_cmd->icmd;

    if ( i_cmd > e_rev_exeactionrequired ) {

      /* STANDARD action commands to be added to the list
	 ================================================ */

      if ( copt != 'd' ) {

	t_upsact_item *new_act_itm = copy_act_item( p_act_itm );
	new_act_itm->cmd = p_cmd;
	dep_list = upslst_add( dep_list, new_act_itm );

	/* check if we should continue */

	if ( do_exit_action( p_cmd ) )
	  break;
      }

    }
    else if ( i_cmd > e_unsetuprequired ) {

      /* EXECUTE another action
	 ====================== */

      char *action_name = (char *)act_name;
      t_upsact_item *new_act_itm = 0;
      
      /* quit if option P set and optional command */

      if ( p_act_itm->ugo->ugo_P && !(i_cmd & 1) )
	continue;

      /* note: level is not incremented */

      new_act_itm = new_act_item( p_act_itm->ugo, p_act_itm->mat,  
				  p_act_itm->level, i_cmd, p_cmd->argv[0] );


      /* ignore errors if optional exeaction */

      if ( !new_act_itm || !new_act_itm->act ) {
	if ( i_cmd & 1 ) {
	  SET_NO_ACTION_ERROR( p_cmd->argv[0] );
	  SET_PARSE_ERROR( p_line );
	}
	else {
	  upserr_clear();
	}
	upsact_free_act_item( new_act_itm );
	continue;
      }

      /* SPECIAL, if ghost commands e_rev_exeactionoptional or e_rev_exeactionrequired 
         then reverse commands */

      if ( i_cmd == e_rev_exeactionoptional || 
	   i_cmd == e_rev_exeactionrequired ) {

	t_upstyp_action *old_act = new_act_itm->act;
	t_upstyp_action *new_act = 0;

	new_act = (t_upstyp_action *)malloc( sizeof( t_upstyp_action ) );
	new_act->action = (char *)malloc( strlen( old_act->action ) + 1 );
	strcpy( new_act->action, old_act->action );
	new_act->command_list = reverse_command_list( new_act_itm, old_act->command_list );

	new_act_itm->act = new_act;
      }

      /* note: action name is parents action name */

      dep_list = next_cmd( top_list, dep_list, new_act_itm,  action_name, copt );
      P_VERB_s_s( 3, "Execute action:", p_line );
    }
    else {

      /* DEPENDENCIES
	 ============ */

      char *action_name = (char *)act_name;
      t_upsugo_command *new_ugo = 0;
      t_upsact_item *set_act_itm = 0;
      t_upsact_item *new_act_itm = 0;

      /* quit if option P set and optional command */

      if ( p_act_itm->ugo->ugo_P && !(i_cmd & 1) )
	continue;
      
      /* get the ugo command */

      new_ugo = get_ugosetup( p_act_itm, p_cmd ); 

      /* new_ugo can be null if doing unsetup */

      if ( ! new_ugo && (i_cmd & 2) )
	continue;

      /* if product is at the top level, use that instance */
      
      if ( new_ugo && p_act_itm->level > 0 )
	new_act_itm = find_prod_name( top_list, new_ugo->ugo_product );

      /* if product is already in our setup list, go to next product */

      if ( new_ugo && copt == 'd' ) {
	if ( find_prod_dep_name( dep_list, new_ugo->ugo_product ) ) { 
	  continue;
	}
      }
      else if ( new_ugo ) {
	if ( find_prod_name( dep_list, new_ugo->ugo_product ) ) { 
	  continue;
	}
      }
	
      /* add a dependency item */

      if ( copt == 'd' ) {
	set_act_itm = copy_act_item( p_act_itm );
	set_act_itm->cmd = p_cmd;
	if ( new_act_itm ) {
	  new_ugo = new_act_itm->ugo;
	  upsmem_inc_refctr( new_ugo );
	}

	set_act_itm->dep_ugo = new_ugo;
	dep_list = upslst_add( dep_list, set_act_itm );
      }

      /* don't follow link if opt j set */

      if ( p_act_itm->ugo->ugo_j )
	continue;

      /* get action item (new_act_itm is only set if product found at the 
	 top level) */

      if ( !new_act_itm ) {

	if ( i_cmd & 2 ) 
	  action_name = g_cmd_info[e_unsetup].cmd;
	else
	  action_name = g_cmd_info[e_setup].cmd;

	new_act_itm = new_act_item( new_ugo, 0, 0, i_cmd, action_name );

	/* ignore errors if optional (un)setup */

	if ( !new_act_itm ) {
	  if ( i_cmd & 1 ) {
	    SET_PARSE_ERROR( p_line );
	  }
	  else {
	    upserr_clear();
	  }
	  continue;
	}
      }

      /* new action, increment dependency level and parse it */

      new_act_itm->level = p_act_itm->level + 1;
      P_VERB_s_s( 3, "Adding dependcy:", p_line );
      dep_list = next_cmd( top_list, dep_list, new_act_itm, action_name, copt );

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
	upsutl_str_remove_end_quotes( saved_ptr, "\"\'", WSPACE );
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

    upsutl_str_remove_end_quotes( saved_ptr, "\"\'", WSPACE );
    argv[count++] = saved_ptr;
  }
  
  return count;
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

  /* fetch product name,
     if it's not a simple command, let ugo do it */

  if ( ! strchr( cmd_line, ' ' ) ) {
    strcpy( s_pname, cmd_line );
  }
  else {
    a_cmd_ugo = upsugo_bldcmd( cmd_line, g_cmd_info[e_unsetup].valid_opts );
    strcpy( s_pname, a_cmd_ugo->ugo_product );
    upsugo_free( a_cmd_ugo );
  }
  pname = upsutl_upcase( s_pname );

  /* fetch, from the environment the product defined in 
     $SETUP_prod */

  a_setup_ugo = upsugo_env( pname, g_cmd_info[e_unsetup].valid_opts );

  /* check if instance is the same */

  /*
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
    else if ( a_cmd_ugo->ugo_chain ) {
      if ( lst_cmp_str( a_cmd_ugo->ugo_chain, a_setup_ugo->ugo_chain ) )
	ohoh = 1;
    }

    if ( ohoh )
      upserr_add( UPS_UNSETUP_CLASH, UPS_WARNING, pname );

  }
  
  
  if ( ! a_setup_ugo ) {

    switch ( i_cmd ) {
    case e_unsetuprequired:      
      upserr_add( UPS_NO_SETUP_ENV, UPS_WARNING, pname );
      break;
    case e_unsetupoptional:
      break;
    }
  }

  */

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

t_upsugo_command *get_ugosetup( t_upsact_item *p_act_itm,
				t_upsact_cmd *const p_cmd )
{
  /* it will, for un/setup commmand lines build the corresponding
     ugo commans structure, using upsugo_bldcmd */

  static char s_cmd[MAX_LINE_LEN] = "";

  t_upsugo_command *a_ugo = 0;
  int i_act = p_cmd->iact;
  int i_cmd = p_cmd->icmd;

  /* handle unsetup ... that's special, we will compare command line to the
     product actually setup and get the instance from $SETUP_<prodname>.
     currently only 'ups depend' will not use the current environment. */

  if ( (i_cmd & 2) && g_ups_cmd != e_depend ) {

    a_ugo = get_SETUP_prod( p_cmd, i_act );
  }
  else {

    char *p_cmd_argv = p_cmd->argv[0];

    /* if no database defined on command line, add '-z db:db..' to the 
       un/setup command, also be sure that there is a database defined
       in current ugo */

    if ( ! strstr( p_cmd_argv, "-z" ) && p_act_itm->ugo->ugo_z &&
	  p_act_itm->ugo->ugo_db && p_act_itm->ugo->ugo_db->data ) {

      t_upslst_item *l_item = upslst_first( p_act_itm->ugo->ugo_db );

      /* from a list of typ_db items it will create a string of databases
	 including the -z string */

      strcpy( s_cmd, p_cmd->argv[0] ); 
      strcat( s_cmd, " -z " );
      for ( ; l_item && l_item->data; l_item = l_item->next ) {
	t_upstyp_db * db = (t_upstyp_db *)l_item->data;
	strcat( s_cmd, db->name );
	if ( l_item->next && l_item->next->data )
	  strcat( s_cmd, ":" );
      }

      p_cmd_argv = s_cmd;
    }
	
    a_ugo = upsugo_bldcmd( p_cmd_argv, g_cmd_info[i_act].valid_opts );
  }

  return a_ugo;
}

t_upsact_item *get_top_item( t_upsugo_command * const ugo_cmd,
			     t_upstyp_matched_product *mat_prod,
			     const char * const act_name )
{
  /* it will create an action item for the top product */

  t_upsact_item *act_itm;
  int i_act = e_invalid_action;
  int i_mod = e_invalid_cmd;

  if ( !ugo_cmd || !act_name )
    return 0;

  /* set a 'fake' action command mode */

  i_act = upsact_action2enum( act_name );
  if ( i_act == e_setup )
    i_mod = e_setuprequired;
  else if ( i_act == e_unsetup )
    i_mod = e_unsetuprequired;

  /* create a partial action structure for top product */

  act_itm = new_act_item( ugo_cmd, mat_prod, 0, i_mod, act_name );

  return act_itm;
}

char *get_mode( const t_upsact_item *const p_act )
{
  /* will buil a string representing the action items mode:
     'nc' where n is the action level, c is a single character,
     describing: 'o' optional action, 'r' required action and
     't' top level action */

  static char s_mode[8];
  char *sm = "--";

  if ( p_act->mode == e_invalid_cmd ) {
    sm = "--";
  }
  else if ( p_act->mode & 4 ) {  /* exeaction */
    if ( p_act->mode & 1 ) 
      sm = "er";  /* required */
    else
      sm = "eo";  /* optional */
  }
  else if ( p_act->mode & 2 ) {  /* unsetup */
    if ( p_act->mode & 1 ) 
      sm = "ur";
    else
      sm = "uo";
  }
  else {                         /* setup */
    if ( p_act->mode & 1 ) 
      sm = "sr";
    else
      sm = "so";
  }

  sprintf( s_mode, "%d:%s", p_act->level, sm );

  return s_mode;
}


t_upsact_item *find_ugo_ptr( t_upslst_item* const dep_list,
			     const t_upsact_item* const act_item )
{
  /* in a list of act_item's it will find the item corresponding to
     passed act_item ugo pointer */

  t_upslst_item *l_ptr = upslst_first( dep_list );
  for ( ; l_ptr; l_ptr = l_ptr->next ) {
    t_upsact_item *act_ptr = (t_upsact_item *)l_ptr->data;
    if ( act_item->ugo == act_ptr->ugo )
      return act_ptr;
  }
  return 0;    
}

t_upsact_item *find_actitm_ptr( t_upslst_item* const dep_list,
				const t_upsact_item* const act_item )
{
  /* in a list of act_item's it will find the item corresponding to
     passed act_item pointer */

  t_upslst_item *l_ptr = upslst_first( dep_list );
  for ( ; l_ptr; l_ptr = l_ptr->next ) {
    t_upsact_item *act_ptr = (t_upsact_item *)l_ptr->data;
    if ( act_item == act_ptr )
      return act_ptr;
  }
  return 0;    
}

t_upsact_item *find_prod_dep_name( t_upslst_item *const dep_list,
				   const char *const prod_name )
{
  /* in a list of act_item's it will find the item corresponding to
     passed product dependency name */

  t_upslst_item *l_ptr = upslst_first( (t_upslst_item *)dep_list );
  for ( ; l_ptr; l_ptr = l_ptr->next ) {
    t_upsact_item *p_item = (t_upsact_item *)l_ptr->data;
    if ( upsutl_stricmp( prod_name, p_item->dep_ugo->ugo_product ) == 0 )
      return p_item;
  }
  return 0;    
}

t_upsact_item *find_prod_name( t_upslst_item* const dep_list,
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

char *actitem2str( const t_upsact_item *const p_act_itm )
{
  /* given an act_item it will create a string representing the 
     corresponding product instance. the result is the string
     from upsget_envstr with chain information appended */

  static char buf[MAX_LINE_LEN];
  t_upstyp_matched_instance *mat_inst;
  t_upslst_item *l_item;
  
  if ( !p_act_itm )
    return 0;

  /* instance id */

  l_item = upslst_first( p_act_itm->mat->minst_list );
  mat_inst = (t_upstyp_matched_instance *)l_item->data;
  strcpy( buf, upsget_envstr( p_act_itm->mat->db_info, mat_inst, p_act_itm->ugo ) );


  /* chain information */

  l_item = upslst_first( p_act_itm->ugo->ugo_chain );
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

t_upsact_item *copy_act_item( const t_upsact_item * const act_itm )
{
  /* from a passed upsact_item it will make a copy */

  t_upsact_item *new_act_itm = 0;

  new_act_itm = (t_upsact_item *)upsmem_malloc( sizeof( t_upsact_item ) );
  memset( new_act_itm, 0, sizeof( t_upsact_item ) );

  new_act_itm->level = act_itm->level;
  new_act_itm->mode = act_itm->mode;

  new_act_itm->ugo = act_itm->ugo;
  upsmem_inc_refctr( act_itm->ugo );

  new_act_itm->mat = act_itm->mat;
  upsmem_inc_refctr( act_itm->mat );

  /* the action pointer is original from a t_upstyp_product */     
  new_act_itm->act = act_itm->act;

  new_act_itm->cmd = act_itm->cmd;  

  return new_act_itm;
}

t_upsact_item *new_act_item( t_upsugo_command * const ugo_cmd,
			     t_upstyp_matched_product *mat_prod,
			     const int level,
			     const int mode,
			     const char * const act_name )
{		       
  t_upsact_item *act_itm = 0;

  /* from a passed ugo command or a matched product it will create a 
     new action item */

  if ( !ugo_cmd )
    return 0;

  if ( !mat_prod ) {
    t_upslst_item *l_mproduct = upsmat_instance( ugo_cmd, NULL, 1 );
    if ( !l_mproduct || !l_mproduct->data ) {
      upserr_vplace();
      upserr_add( UPS_NO_MATCH, UPS_FATAL, ugo_cmd->ugo_product );
      return 0;
    }
    mat_prod = (t_upstyp_matched_product *)l_mproduct->data;
    upslst_free( l_mproduct, ' ' );
  }
  else {
    upsmem_inc_refctr( mat_prod );
  }

  act_itm = (t_upsact_item *)upsmem_malloc( sizeof( t_upsact_item ) );
  memset( act_itm, 0, sizeof( t_upsact_item ) );

  act_itm->level = level;
  act_itm->mode = mode;

  act_itm->ugo = ugo_cmd;
  upsmem_inc_refctr( act_itm->ugo );

  act_itm->mat = mat_prod;

  if ( act_name )
    act_itm->act = get_act( ugo_cmd, mat_prod, act_name );
  act_itm->cmd = 0;

  return act_itm;
}

t_upstyp_action *new_default_action( t_upsact_item *const p_act_itm, 
				     const char * const act_name, 
				     const int iact )
{
  /* it will create a new t_upstyp_action. It will be:
     1) reverse action, if exist (g_cmd_info.uncmd_index)
     2) a dodefault() action, if default bit set (g_cmd_info.flags) */

  t_upstyp_action *p_unact = 0;
  t_upstyp_action *p_act = 0;
  int i_uncmd = 0;

  if ( iact == e_invalid_action )
    return 0;

  if ( (i_uncmd = g_cmd_info[iact].uncmd_index) != e_invalid_action &&
       (p_act = get_act( p_act_itm->ugo, p_act_itm->mat, g_cmd_info[i_uncmd].cmd )) ) {

    p_unact = (t_upstyp_action *)malloc( sizeof( t_upstyp_action ) );
    p_unact->action = (char *)malloc( strlen( act_name ) + 1 );
    strcpy( p_unact->action, act_name );
    p_unact->command_list = reverse_command_list( p_act_itm, p_act->command_list );
  }      
  else if ( (g_cmd_info[iact].flags)&0x00000001 ) {
    p_unact = (t_upstyp_action *)malloc( sizeof( t_upstyp_action ) );
    p_unact->action = (char *)malloc( strlen( act_name ) + 1 );
    strcpy( p_unact->action, act_name );
    p_unact->command_list = 0;
    p_unact->command_list = upslst_add( p_unact->command_list, 
					upsutl_str_create( "dodefaults()", ' ' ) );
  }

  return p_unact;
}

t_upslst_item *reverse_command_list( t_upsact_item *const p_act_itm, 
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

    p_line = upsget_translation( p_act_itm->mat, p_act_itm->ugo,
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
	
	/* handle first argument */

	if ( i_cmd == e_sourcerequired ||
	     i_cmd == e_sourceoptional ||
	     i_cmd == e_sourcereqcheck ||
	     i_cmd == e_sourceoptcheck ) {
	  
	  /* SPECIAL case for source*( file_path, ... ), 
	     we will prepend or remove an 'un' from file name */

	  char *fp = p_cmd->argv[0];
	  char *fn = 0;

	  /* handle directory path ... who said DOS ? */

	  if ( (fn = strrchr( fp, '/' )) ) {
	    fn++;
	    strncat( buf, fp, (size_t)( fn - fp ) );
	  }
	  else
	    fn = fp;

	  /* modify file name */

	  if ( !upsutl_strincmp( fn, "un", 2 ) )
	    fn += 2; 	         /* remove 'un' */
	  else
	    strcat( buf, "un" ); /* prepend 'un', always lower case */

	  strcat( buf, fn );

	  /* SPECIAL end */ 

	}
	else {
	  
	  /* normal */

	  if ( argc > 0 )
	    strcat( buf, p_cmd->argv[0] );
	}

	/* handle the rest of the arguments */

	for ( i=1; i<argc; i++ ) {
	  strcat( buf, ", " );	    
	  strcat( buf, p_cmd->argv[i] );
	}
	strcat( buf, ")" );

	/* use insert, to reverse the order of commands */

	l_ucmd = upslst_insert( l_ucmd, 
				upsutl_str_create( buf, ' ' ) );
      }
      upsmem_free( p_cmd );      
    }    
  }

  return upslst_first( l_ucmd );
}

int do_exit_action( const t_upsact_cmd * const a_cmd ) 
{
  /* 
     this one is called when action commands are parsed in 'next_cmd'.

     it will check if passed action have an EXIT flag set:
        - by default sourcecompile has always EXIT flag set.
	- we will only exit for required commands, the decision
          to exit for optional commands are taken in the produced
          script itself.
  */


  int exit_flag;
  int no_ups_env_flag;
  int icmd;

  if ( ! a_cmd ) 
    return 0;

  icmd = a_cmd->icmd;

  switch ( icmd ) {

  case e_sourcecompilereq:
      return 1;
      break;

  case e_sourcerequired:
  case e_sourcereqcheck:
    GET_FLAGS();
    if ( exit_flag == DO_EXIT )
      return 1;
    break;

  default:
    return 0;
  }

  return 0;
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

static void f_copyhtml( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("copyHtml");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_copyinfo( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("copyInfo");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_copyman( ACTION_PARAMS)
{
  char *buf = NULL;

  CHECK_NUM_PARAM("copyMan");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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
	GET_MAN_SOURCE(MANPAGES);
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

static void f_copycatman( ACTION_PARAMS)
{
  char *buf = NULL;

  CHECK_NUM_PARAM("copyCatMan");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config || !a_db_info->config->catman_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "catman");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	GET_MAN_SOURCE(CATMANPAGES);
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n#\n", 
		    buf, a_db_info->config->catman_path) < 0) {
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

static void f_uncopyman( ACTION_PARAMS)
{
  char *buf = NULL;
  t_upslst_item *man_item, *man_list = NULL;

  CHECK_NUM_PARAM("uncopyMan");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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
	GET_MAN_SOURCE(MANPAGES);

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
	/* cleanup memory */
	man_list = upslst_free(man_list, 'd');

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

static void f_uncopycatman( ACTION_PARAMS)
{
  char *buf = NULL;
  t_upslst_item *man_item, *man_list = NULL;

  CHECK_NUM_PARAM("uncopyCatMan");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config || !a_db_info->config->catman_path) {
      upserr_vplace();
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "catman");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	GET_MAN_SOURCE(CATMANPAGES);

	/* Get a list of all the files in the specified directory */
	upsutl_get_files(buf, ANY_MATCH, &man_list);

	for (man_item = man_list ; man_item ; man_item = man_item->next) {
	  if (fprintf((FILE *)a_stream, "rm %s/%s\n", 
		      a_db_info->config->catman_path, (char *)man_item->data)
	      < 0) {
	    FPRINTF_ERROR();
	    break;
	  }
	}
	/* cleanup memory */
	man_list = upslst_free(man_list, 'd');

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

static void f_copynews( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("copyNews");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_envappend( ACTION_PARAMS)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("envAppend");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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
      if (fprintf((FILE *)a_stream, "if [ \"${%s:-}\" = \"\" ]; then\n  %s=\"%s\"\nelse\n  %s=\"${%s}%s%s\"\nfi\nexport %s\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0], a_cmd->argv[1],
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
      if (fprintf((FILE *)a_stream, "if (! ${?%s}) then\n  setenv %s \"%s\"\nelse\n  setenv %s \"${%s}%s%s\"\nendif\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0], a_cmd->argv[1], 
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

static void f_envprepend( ACTION_PARAMS)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("envPrepend");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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
      if (fprintf((FILE *)a_stream, "if [ \"${%s:-}\" = \"\" ]; then\n  %s=\"%s\"\nelse\n  %s=\"%s%s${%s}\"\nfi\nexport %s\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0], a_cmd->argv[1],
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
      if (fprintf((FILE *)a_stream, "if (! ${?%s}) then\n  setenv %s \"%s\"\nelse\n  setenv %s \"%s%s${%s}\"\nendif\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0], a_cmd->argv[1], 
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

static void f_envremove( ACTION_PARAMS)
{
  char *delimiter;
  
  CHECK_NUM_PARAM("envRemove");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream,
		  "upstmp=`dropit -p \"$%s\" -i'%s' -d'%s' %s`;\nif [ $? -eq 0 -a \"$upstmp\" != \"%s\" ]; then %s=$upstmp; fi\nunset upstmp;\n#\n",
		  a_cmd->argv[0], delimiter, delimiter, a_cmd->argv[1],
		  delimiter, a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream,
	     "setenv upstmp \"`dropit -p \"$%s\" -i'%s' -d'%s' %s`\"\nif ($status == 0 && \"$upstmp\" != \"%s\") setenv %s \"$upstmp\"\nunsetenv upstmp\n#\n",
		  a_cmd->argv[0], delimiter, delimiter, a_cmd->argv[1],
		  delimiter, a_cmd->argv[0]) < 0) {
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

static void f_envset( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("envSet");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_envsetifnotset( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("envSetIfNotSet");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream,
		  "if [ ! ${%s-} ]; then %s=\"%s\";export %s\n#\n",
		  a_cmd->argv[0], a_cmd->argv[0],
		  a_cmd->argv[1], a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "if (! ${?%s}) setenv %s \"%s\"\n#\n",
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

static void f_envunset( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("envUnset");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_exeaccess( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("exeAccess");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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
		  "which %s\nif ($status == 1) exit 1\n#\n",
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

static void f_execute( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("execute");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (a_cmd->argc == g_cmd_maps[a_cmd->icmd].min_params) {
	if (fprintf((FILE *)a_stream, "%s\n#\n", a_cmd->argv[0]) < 0) {
	  FPRINTF_ERROR();
	}
      } else {
	if (fprintf((FILE *)a_stream, "%s=`%s`;export %s\n#\n", a_cmd->argv[1],
		    a_cmd->argv[0], a_cmd->argv[1]) < 0) {
	  FPRINTF_ERROR();
	}
      }
      break;
    case e_CSHELL:
      if (a_cmd->argc == g_cmd_maps[a_cmd->icmd].min_params) {
	if (fprintf((FILE *)a_stream, "%s\n#\n", a_cmd->argv[0])< 0) {
	  FPRINTF_ERROR();
	}
      } else {
	if (fprintf((FILE *)a_stream, "setenv %s \"`%s`\"\n#\n",
		    a_cmd->argv[1], a_cmd->argv[0])< 0) {
	  FPRINTF_ERROR();
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

static void f_filetest( ACTION_PARAMS)
{
  char *err_message;
  
  CHECK_NUM_PARAM("fileTest");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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
		  "if ( ! %s %s ) then\necho \"%s\"\nexit 1\nendif\n#\n", 
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

static void f_pathappend( ACTION_PARAMS)
{
  char *delimiter;
  char *env_to_set;
  char *pathPtr;
  
  CHECK_NUM_PARAM("pathAppend");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      CHECK_FOR_PATH(g_shPath, g_shDelimiter);
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_pathremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "%s=\"${%s-}%s%s\";export %s\n#\n",
		  pathPtr, pathPtr, delimiter, a_cmd->argv[1], pathPtr) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      CHECK_FOR_PATH(g_cshPath, g_cshDelimiter);
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_pathremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "set %s=($%s %s)\nrehash\n#\n",
		  pathPtr, pathPtr, a_cmd->argv[1]) < 0) {
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

static void f_pathprepend( ACTION_PARAMS)
{
  char *delimiter;
  char *pathPtr;
  
  CHECK_NUM_PARAM("pathPrepend");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      CHECK_FOR_PATH(g_shPath, g_shDelimiter);
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_pathremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "%s=\"%s%s${%s-}\";export %s\n#\n",
		  pathPtr, a_cmd->argv[1], delimiter, pathPtr, pathPtr) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      CHECK_FOR_PATH(g_cshPath, g_cshDelimiter);
      if (g_COMPILE_FLAG) {
	/* we are being called during a compile, we need to output extra
	   stuff */
	f_pathremove(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
      }
      if (fprintf((FILE *)a_stream, "set %s=(%s $%s)\nrehash\n#\n",
		  pathPtr, a_cmd->argv[1], pathPtr) < 0) {
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

static void f_pathremove( ACTION_PARAMS)
{
  char *delimiter;
  char *pathPtr;
  
  CHECK_NUM_PARAM("pathRemove");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      CHECK_FOR_PATH(g_shPath, g_shDelimiter);
      if (fprintf((FILE *)a_stream,
		  "upstmp=`dropit -p \"$%s\" -i'%s' -d'%s' %s`;\nif [ $? -eq 0 -a \"$upstmp\" != \"%s\" ]; then %s=$upstmp; fi\nunset upstmp;\n#\n",
		  pathPtr, delimiter, delimiter, a_cmd->argv[1], delimiter, 
		  pathPtr) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      CHECK_FOR_PATH(g_cshPath, g_cshDelimiter);
      if (fprintf((FILE *)a_stream,
          "setenv upstmp \"`dropit -p \"$%s\" -i'%s' -d'%s' %s`\"\nif ($status == 0 && \"$upstmp\" != \"%s\") set %s=$upstmp\nrehash\nunsetenv upstmp\n#\n",
		  pathPtr, delimiter, delimiter, a_cmd->argv[1], delimiter,
		  pathPtr) < 0) {
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

static void f_pathset( ACTION_PARAMS)
{
  char *delimiter;
  char *pathPtr;

  CHECK_NUM_PARAM("pathSet");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      CHECK_FOR_PATH(g_shPath, g_shDelimiter);
      if (fprintf((FILE *)a_stream, "%s=\"%s\";export %s\n#\n", pathPtr,
		  a_cmd->argv[1], pathPtr) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      CHECK_FOR_PATH(g_cshPath, g_cshDelimiter);
      if (fprintf((FILE *)a_stream, "set %s=(%s)\nrehash\n#\n", pathPtr,
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

#define g_SHPARAM "$@"
#define g_CSHPARAM "\!*"
#define g_ACTPARAM "%s"

static void f_addalias( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("addAlias");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      sprintf(g_buff, "%s () { %s }\n#\n", a_cmd->argv[0], a_cmd->argv[1]);
      if (strstr(g_buff, g_ACTPARAM)) {
	/* the string is already in there, add the parameter string */
	if (fprintf((FILE *)a_stream, g_buff, g_SHPARAM) < 0) {
	  FPRINTF_ERROR();
	}
      } else {
	/* the string is not there */
	if (fprintf((FILE *)a_stream, "%s", g_buff) < 0) {
	  FPRINTF_ERROR();
	}
      }
      break;
    case e_CSHELL:
      sprintf(g_buff, "alias %s \"%s\"\n#\n", a_cmd->argv[0], a_cmd->argv[1]);
      if (strstr(g_buff, g_ACTPARAM)) {
	/* the string is already in there, add the parameter string */
	if (fprintf((FILE *)a_stream, g_buff, g_CSHPARAM) < 0) {
	  FPRINTF_ERROR();
	}
      } else {
	/* the string is not there */
	if (fprintf((FILE *)a_stream, "%s", g_buff) < 0) {
	  FPRINTF_ERROR();
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

static void f_unalias( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("unAlias");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "unset %s\n\n", a_cmd->argv[0]) < 0) {
	FPRINTF_ERROR();
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "unalias %s\n#\n", a_cmd->argv[0]) < 0) {
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

  if ((status = fprintf((FILE *)a_stream, "if [ -s %s ]; then\n", a_data))
      < 0) {
    FPRINTF_ERROR();
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
    upsget_allout(a_stream, a_db_info, a_inst, a_command_line, "  ");
    g_LOCAL_VARS_DEF = 1;   /* we defined local variables */
  }
  if (UPS_ERROR == UPS_SUCCESS) {
    if ((status = fprintf((FILE *)a_stream, "  . %s\n", a_data)) < 0) {
      FPRINTF_ERROR();
    } else {
      if (a_check_flag == DO_CHECK) {
	if ((status = fprintf((FILE *)a_stream,
	      "  UPS_STATUS=$?\n  if [ \"$UPS_STATUS\" != \"0\" ]; then\n    echo \"Error $UPS_STATUS while sourcing %s\"\n    unset UPS_STATUS\n",
		    a_data)) < 0) {
	  FPRINTF_ERROR();
	} else {
	  upsutl_finish_temp_file(a_stream, a_command_line, "    ");
	  if ((status = fprintf((FILE *)a_stream,
		      "    return 1\n  fi\n  unset UPS_STATUS\n") < 0)) {
	      FPRINTF_ERROR();
	  } 
	}
      }
      if ((status >= 0) && (a_exit_flag == DO_EXIT)) {
	upsutl_finish_temp_file(a_stream, a_command_line, "  ");
	if ((status = fprintf((FILE *)a_stream, "  return\n") < 0)) {
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

  if ((status = fprintf((FILE *)a_stream, "if (-e %s) then\n", a_data))
       < 0) {
    FPRINTF_ERROR();
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
    upsget_allout(a_stream, a_db_info, a_inst, a_command_line, "  ");
    g_LOCAL_VARS_DEF = 1;   /* we defined local variables */
  }
  if (UPS_ERROR == UPS_SUCCESS) {
    if ((status = fprintf((FILE *)a_stream, "  source %s\n", a_data)) < 0) {
      FPRINTF_ERROR();
    } else {
      if (a_check_flag == DO_CHECK) {
	if ((status = fprintf((FILE *)a_stream,
	      "  setenv UPS_STATUS \"$status\"\n  if (\"$UPS_STATUS\" != \"0\") then\n    echo \"Error $UPS_STATUS while sourcing %s\n    unsetenv UPS_STATUS\n",
		    a_data)) < 0) {
	  FPRINTF_ERROR();
	} else {
	  upsutl_finish_temp_file(a_stream, a_command_line, "    ");
	  if ((status = fprintf((FILE *)a_stream,
		      "    exit 1\n  endif\n  unsetenv UPS_STATUS\n") < 0)) {
	      FPRINTF_ERROR();
	  } 
	}
      }
      if ((status >= 0) && (a_exit_flag == DO_EXIT)) {
	upsutl_finish_temp_file(a_stream, a_command_line, "  ");
	if ((status = fprintf((FILE *)a_stream, "  exit\n") < 0)) {
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
	      "else\n  echo \"File (%s)  not found\"\n", a_data) < 0)) {
    FPRINTF_ERROR();
  } else {
    upsutl_finish_temp_file(a_stream, a_command_line, "  ");
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
	      "else\n  echo \"File (%s)  not found\"\n", a_data) < 0)) {
    FPRINTF_ERROR();
  } else {
    upsutl_finish_temp_file(a_stream, a_command_line, "  ");
    if ((status = fprintf((FILE *)a_stream, "  exit 1\nendif\n#\n") < 0)) {
      FPRINTF_ERROR();
    }
  }
  return(status);
}

static void f_sourcecompilereq( ACTION_PARAMS)
{
  /* skip this whole action if we are being called while compiling */
  if (! g_COMPILE_FLAG) {
    CHECK_NUM_PARAM("sourceCompileReq");

    OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_sourcecompileopt( ACTION_PARAMS)
{
  /* skip this whole action if we are being called while compiling */
  if (! g_COMPILE_FLAG) {
    CHECK_NUM_PARAM("sourceCompileOpt");

    OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_sourcerequired( ACTION_PARAMS)
{
  int exit_flag = NO_EXIT;
  int no_ups_env_flag = DO_UPS_ENV;

  CHECK_NUM_PARAM("sourceRequired");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_sourceoptional( ACTION_PARAMS)
{
  int exit_flag = NO_EXIT;
  int no_ups_env_flag = DO_UPS_ENV;

  CHECK_NUM_PARAM("sourceOptional");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_sourcereqcheck( ACTION_PARAMS)
{
  int exit_flag = NO_EXIT;
  int no_ups_env_flag = DO_UPS_ENV;

  CHECK_NUM_PARAM("sourceReqCheck");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_sourceoptcheck( ACTION_PARAMS)
{
  int exit_flag = NO_EXIT;
  int no_ups_env_flag = DO_UPS_ENV;

  CHECK_NUM_PARAM("sourceOptCheck");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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

static void f_writecompilescript(ACTION_PARAMS)
{
  t_upstyp_matched_product mproduct = {NULL, NULL, NULL};
  t_upslst_item *cmd_list = NULL;
  char time_buff[MAX_LINE_LEN];
  char *time_ptr;
  int moved_to_old = 0, moved_to_timedate = 0;
  FILE *compile_file;

  /* skip this whole action if we are being called while compiling */
  if (! g_COMPILE_FLAG) {
    CHECK_NUM_PARAM("writeCompileScript");

    OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

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
				a_cmd->argv[1], a_cmd->icmd );
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
	      sprintf(g_buff, "mv %s %s.%s\n", a_cmd->argv[0], a_cmd->argv[0],
		      OLD_FLAG);
	      DO_SYSTEM_MOVE(moved_to_old);
	    } else if (! strcmp(a_cmd->argv[a_cmd->argc - 1], DATE_FLAG)) {
	      /* append a timedate stamp to the file name */
	      time_ptr = upsutl_time_date();
	      strcpy(time_buff, time_ptr);

	      /* remove any whitespace */
	      (void )upsutl_str_remove(time_buff, WSPACE);
	      sprintf(g_buff, "mv %s %s.%s\n", a_cmd->argv[0], a_cmd->argv[0],
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
	sprintf(g_buff, "mv %s.%s %s\n", a_cmd->argv[0], OLD_FLAG,
		a_cmd->argv[0]);
      } else {
	sprintf(g_buff, "mv %s.%s %s\n", a_cmd->argv[0], time_buff,
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

static void f_setupenv( ACTION_PARAMS)
{
  CHECK_NUM_PARAM("setupEnv");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
    SET_SETUP_PROD();
  }
}

static void f_unsetupenv( ACTION_PARAMS)
{
  t_upsact_cmd lcl_cmd;
  char *uprod_name;

  CHECK_NUM_PARAM("unSetupEnv");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
    /* we will be calling the envunset action */
    lcl_cmd.iact = a_cmd->iact;
    lcl_cmd.argc = g_cmd_maps[e_envunset].min_params;   /* # of args */
    lcl_cmd.icmd = e_envunset;
    lcl_cmd.argv[0] = g_buff;

    if (a_inst->version && a_inst->version->product) {
      uprod_name = upsutl_upcase(a_inst->version->product);
      if (UPS_ERROR == UPS_SUCCESS) {
	sprintf(g_buff, "%s%s", SETUPENV,uprod_name);
	f_envunset(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);
      }
    }
  }
}

static void f_proddir( ACTION_PARAMS)
{
  t_upsact_cmd lcl_cmd;
  char *tmp_prod_dir = NULL;
  char *uprod_name;

  CHECK_NUM_PARAM("prodDir");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
    /* we will be calling the envset action */
    lcl_cmd.iact = a_cmd->iact;
    lcl_cmd.argc = g_cmd_maps[e_envset].min_params;   /* # of args */
    lcl_cmd.icmd = e_envset;
    lcl_cmd.argv[0] = g_buff;

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
	sprintf(g_buff, "%s_DIR", uprod_name);
	
	lcl_cmd.argv[1] = tmp_prod_dir;
	f_envset(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);
      }
    }

  }
}

static void f_unproddir( ACTION_PARAMS)
{
  t_upsact_cmd lcl_cmd;
  char *uprod_name;

  CHECK_NUM_PARAM("unProdDir");

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
    /* we will be calling the envunset action */
    lcl_cmd.iact = a_cmd->iact;
    lcl_cmd.argc = g_cmd_maps[e_envunset].min_params;   /* # of args */
    lcl_cmd.icmd = e_envunset;
    lcl_cmd.argv[0] = g_buff;

    if (a_inst->version && a_inst->version->product) {
      uprod_name = upsutl_upcase(a_inst->version->product);
      if (UPS_ERROR == UPS_SUCCESS) {
	sprintf(g_buff, "%s_DIR", uprod_name);
	f_envunset(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);
      }
    }
  }
}

static void f_dodefaults( ACTION_PARAMS)
{
  t_upsact_cmd lcl_cmd;
  char *uprod_name;

  OUTPUT_VERBOSE_MESSAGE(g_cmd_maps[a_cmd->icmd].cmd);

  /* only proceed if we have a stream to write the output to */
  if (a_stream) {
    switch ( a_cmd->iact ) {
    case e_setup:	/* Define <PROD>_DIR and SETUP_<PROD> */
      /* use our local copy since we have to change it - we will be calling
	 the proddir action */
      lcl_cmd.iact = a_cmd->iact;
      lcl_cmd.argc = g_cmd_maps[e_proddir].min_params;   /* # of args */
      lcl_cmd.icmd = e_proddir;
      f_proddir(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);

      SET_SETUP_PROD();
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
      f_copycatman(a_inst, a_db_info, a_command_line, a_stream, a_cmd);
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
      lcl_cmd.argc = g_cmd_maps[e_unproddir].min_params;   /* # of args */
      lcl_cmd.icmd = e_unproddir;
      f_unproddir(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);
      lcl_cmd.argc = g_cmd_maps[e_unsetupenv].min_params;   /* # of args */
      lcl_cmd.icmd = e_unsetupenv;
      f_unsetupenv(a_inst, a_db_info, a_command_line, a_stream, &lcl_cmd);
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

