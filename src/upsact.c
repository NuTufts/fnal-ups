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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* ups specific include files */
#include "upsact.h"
#include "upserr.h"
#include "upslst.h"
#include "upsutl.h"
#include "upsmem.h"
#include "upskey.h"
#include "upsugo.h"

/*
 * Definition of public variables.
 */

/*
 * Private constants
 */

#define DQUOTE '"'
#define COMMA ','
#define OPEN_PAREN '('
#define MAX_ARGC 100
#define WSPACE " \t\n\r\f"
#define LCL_VAR_PREFACE1 "$UPS_"
#define LCL_VAR_PREFACE2 "${UPS_"

enum {
  e_invalid_action = -1,
  e_setupoptional = 0,
  e_setuprequired,
  e_unsetupoptional,
  e_unsetuprequired,
  e_envappend,
  e_envremove,
  e_envprepend,
  e_envset,
  e_envunset,
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
  e_copynews,
  e_dodefaults,
  e_nodefaults,
  e_nosetupenv,
  e_noproddir,
  e_forkactions,
  e_sourceactions
};

/*
 * Private types
 */

/* this one is just to bundle current information */
typedef struct s_cmd_cur {
  const t_upstyp_instance *inst;
  const char *action_name;
  const struct s_cmd_map *map;
  int argc;
  char *argv[MAX_ARGC];
} t_cmd_cur;

/* this one is the type of a action command handler */
typedef void (*tpf_cmd)( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
			 const FILE * const a_stream,
			 const struct s_cmd_map * const a_cmd_map,
			 const int a_argc,
			 const char ** const a_argv);

/* this one is the type for a single action command */
typedef struct s_cmd_map {
  char *cmd;
  int  icmd;
  tpf_cmd func;
} t_cmd_map;

/*
 * Declaration of private functions.
 */

int upsact_params( char * const a_params, char *argv[] );
int upsact_parse( const char * const a_action_line, t_cmd_cur * const cmd_cur );

/* functions to handle specific action commands */

void f_copyhtml( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv);
void f_copyinfo( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv);
void f_copyman( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc,
		const char ** const a_argv);
void f_copynews( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv);
void f_envappend( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc,
		  const char ** const a_argv);
void f_envprepend( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc,
		   const char ** const a_argv);
void f_envremove( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc,
		  const char ** const a_argv);
void f_envset( const t_upstyp_matched_instance * const a_inst,
	       const t_upstyp_db * const a_db_info,
	       const t_upsugo_command * const a_command_line,
	       const FILE * const a_stream,
	       const struct s_cmd_map * const a_cmd_map,
	       const int a_argc,
	       const char ** const a_argv);
void f_envunset( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv);
void f_exeaccess( const t_upstyp_matched_instance * const a_inst,
		  const t_upstyp_db * const a_db_info,
		  const t_upsugo_command * const a_command_line,
		  const FILE * const a_stream,
		  const struct s_cmd_map * const a_cmd_map,
		  const int a_argc,
		  const char ** const a_argv);
void f_execute( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc,
		const char ** const a_argv);
void f_filetest( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv);
void f_sourcerequired( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc,
		       const char ** const a_argv);
void f_sourceoptional( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc,
		       const char ** const a_argv);
void f_sourcereqcheck( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc,
		       const char ** const a_argv);
void f_sourceoptcheck( const t_upstyp_matched_instance * const a_inst,
		       const t_upstyp_db * const a_db_info,
		       const t_upsugo_command * const a_command_line,
		       const FILE * const a_stream,
		       const struct s_cmd_map * const a_cmd_map,
		       const int a_argc,
		       const char ** const a_argv);
void f_dodefaults( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc,
		   const char ** const a_argv);

#define CHECK_NUM_PARAM(action) \
    if ((a_argc < min_num_params) || (a_argc > max_num_params)) {   \
      upserr_add(UPS_INVALID_ACTION_PARAMS, UPS_FATAL, action,      \
                 min_num_params, max_num_params, a_argc);           \
    }

#define GET_DELIMITER() \
    if (a_argc == max_num_params) {                               \
      /* remember arrays start at 0, so subtract one here */      \
      delimiter = (char *)&a_argv[max_num_params-1];              \
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

static char *g_local_vars[] = {
  /* 0 */     "$UPS_PROD_NAME",
  /* 1 */     "$UPS_PROD_VERSION",
  /* 2 */     "$UPS_PROD_FLAVOR",
  /* 3 */     "$UPS_OS_FLAVOR",
  /* 4 */     "$UPS_PROD_QUALIFIERS",
  /* 5 */     "${UPS_PROD_NAME}_DIR",
  /* 6 */     "$UPS_SHELL",
  /* 7 */     "$UPS_OPTIONS",
  /* 8 */     "$UPS_VERBOSE",
  /* 9 */     "$UPS_EXTENDED",
  /* 10 */    "$UPS_FLAGS",
  /* 11 */    "$UPS_FLAGSDEPEND",
  /* 12 */    "$UPS_THIS_DB"
};

/* These action commands are listed in order of use.  Hopefully the more
   used actions are at the front of the list. Also the ones most used by
   setup and unsetup are at the front of the array.  The actions in this
   array MUST appear in the same order in the following enumeration */

static t_cmd_map g_cmd_maps[] = {
  { "setupoptional", e_setupoptional, NULL },
  { "setuprequired", e_setuprequired, NULL },
  { "unsetupoptional", e_unsetupoptional, NULL },
  { "unsetuprequired", e_unsetuprequired, NULL },
  { "envappend", e_envappend, f_envappend },
  { "envremove", e_envremove, f_envremove },
  { "envprepend", e_envprepend, f_envprepend },
  { "envset", e_envset, f_envset },
  { "envunset", e_envunset, f_envunset },
  { "sourcerequired", e_sourcerequired, f_sourcerequired },
  { "sourceoptional", e_sourceoptional, f_sourceoptional },
  { "sourcereqcheck", e_sourcereqcheck, f_sourcereqcheck },
  { "sourceoptcheck", e_sourceoptcheck, f_sourceoptcheck },
  { "exeaccess", e_exeaccess, f_exeaccess },
  { "execute", e_execute, f_execute },
  { "filetest", e_filetest, f_filetest },
  { "copyhtml", e_copyhtml, f_copyhtml },
  { "copyinfo", e_copyinfo, f_copyinfo },
  { "copyman", e_copyman, f_copyman },
  { "copynews", e_copynews, f_copynews },
  { "dodefaults", e_dodefaults, f_dodefaults },
  { "nodefaults", e_nodefaults, NULL },
  { "nosetupenv", e_nosetupenv, NULL },
  { "noproddir", e_noproddir, NULL},
  { "forkactions", e_forkactions, NULL},
  { "sourceactions", e_sourceactions, NULL},
  { 0,0,0 }
};


/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upsact_get_shell_code
 *
 * Write the actions' equivalent shell specific code to the stream.
 * We assume the following has been done already -
 *        the action has been parsed and verified
 *        all supported local UPS environment variables in the action have
 *            been replaced with their value.
 *
 * Input : instance information, command line information and the
 *         parsed action information.
 * Output: none
 * Return: pointer to string of translated action
 */
void upsact_get_shell_code( const t_upstyp_matched_instance * const a_inst,
			    const t_upstyp_db * const a_db_info,
			    const t_upsugo_command * const a_command_line,
			    const FILE * const a_stream,
			    const t_cmd_cur * const a_actcmd)
{

  /* Call the function associated with the action command */
  if (g_cmd_maps[a_actcmd->map->icmd].func) {
    g_cmd_maps[a_actcmd->map->icmd].func( a_inst, a_db_info, a_command_line,
					a_stream, a_actcmd->map,
					a_actcmd->argc,
					(const char ** const)(&(a_actcmd->argv[0])));
  }
}

/*-----------------------------------------------------------------------
 * upsact_translate_cmd
 *
 * It will translate a single action command line.
 *
 * Input : action line.
 * Output: none
 * Return: 
 */
int upsact_translate_cmd( const char * const cmd_str )
{
  int ierr = UPS_SUCCESS;  
  t_cmd_cur cmd_cur;

  cmd_cur.map = NULL;
  cmd_cur.argc = 0;
  
  if ( upsact_parse( cmd_str, &cmd_cur ) != e_invalid_action ) { 
    
    /* Replace all ups pre-set local variables with their actual values in
       the parameters before they are split up. */
    /* new_params = upsact_local_vars(params, a_local_vars); */

    /* handle action */
    
  }
  else {
    /* invalid action */
    upserr_add( UPS_INVALID_ACTION, UPS_WARNING, cmd_str );
    ierr = UPS_INVALID_ACTION;
  }

  return ierr;
}

/*-----------------------------------------------------------------------
 * upsact_local_vars
 *
 * Search the action line to see if it contains any of the supported
 * pre-set ups local variables.  If it does, replace the local variable
 * with it's value.
 *
 * Input : parameter string, local variable value structure
 * Output: none
 * Return: string with local variable substitution performed or pointer
 *         to original string if no substitution.
 */
char *upsact_local_var( char * const a_params )
                       /*
		       const t_ups_lcl_vars * const a_local_vars)
		       */
{
  char *local_var = NULL;
  char *new_string = a_params;
  /* int lcl_action_type */;

  if ((local_var = strstr(a_params, LCL_VAR_PREFACE1)) != NULL) {
    /* This string does contain a ups pre-set local variable in it.  Now
       we have to figure out which one and create a new string as the
       translation of the local variable will probably be longer than
       the variable itself. Use the local_var as input to the routine
       as it already points to the */
  } else if ((local_var = strstr(a_params, LCL_VAR_PREFACE2)) != NULL) {
    /* This string contains the preset local variable ${UPS_PROD_NAME}_DIR.
       Translate it */
    /*
    new_string = upsutl_str_replace(a_params, ,
				    a_local_vars->ups_prod_name_dir);
    */
  }
  return new_string;
}

/*-----------------------------------------------------------------------
 * upsact_params
 *
 * Given an action's parameters split them up into separate params and
 * return a linked list of the separate parameters.  The parameters are
 * separated by commas, but ignore commas within quotes.
 *
 * called by upsact_parse.
 *
 * Input : parameter string
 *         pointer to array of arguments
 * Output: pointer to array of arguments
 * Return: number of arguments found
 */
int upsact_params( char * const a_params, char **argv )
{
  char *ptr = a_params, *saved_ptr = NULL;
  char *act_str, *new_ptr;
  int count = 0;

  while ( ptr && *ptr ) {

    if ( count >= MAX_ARGC ) {
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

	act_str = upsutl_str_create( ptr, STR_TRIM_DEFAULT );
	upsutl_str_remove_edges( act_str, WSPACE );
	argv[count++] = act_str;
	saved_ptr = NULL;         /* no longer valid, we got the param */
	ptr = NULL;               /* all done */
      }
      else {
	
	/* point string just past double quote */
	
	ptr = ++new_ptr;
      }
      break;
      
    case COMMA:     /* found a comma */       

      if ( saved_ptr ) {
	
	/* we have a param, add it to the list */
	
	*ptr = '\0';                /* temporary so only take param */
	act_str = upsutl_str_create( saved_ptr, STR_TRIM_DEFAULT );
	upsutl_str_remove_edges( act_str, WSPACE );
	argv[count++] = act_str;
	*ptr = COMMA;              /* restore */
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
    
    act_str = upsutl_str_create( saved_ptr, STR_TRIM_DEFAULT );
    upsutl_str_remove_edges( act_str, WSPACE );
    argv[count++] = act_str;
  }
  
  return count;
}

/*-----------------------------------------------------------------------
 * upsact_parse
 *
 * Given an action return the parameters as a separate string, and an integer
 * corresponding to the action string.  In later routines more strcmp's can
 * be avoided.
 *
 * Input : action line
 * Output: action parameter string, integer action value
 * Return: enum of corresponding action
 */
int upsact_parse( const char * const a_action_line, t_cmd_cur * const cmd_cur )
{
  static char trim_chars[] = " \t\n\r\f)";
  const char *act_s = a_action_line, *act_e = NULL;
  char *param_str = NULL;
  int icmd = e_invalid_action;
  int i;
  int len;

  /* First split the line to separate the action from the parameters. Locate
     the first non space character and the first '(' as that will define the
     action */

  /* get rid of leading spaces */
  
  while ( act_s && *act_s && isspace( (int )*(act_s) ) ){ ++act_s; };

  if ( !act_s || !*act_s )
    return e_invalid_action;

  if ( (act_e = strchr( act_s, OPEN_PAREN )) != NULL ) {
    len = act_e - act_s;

    /* look for action in the supported action array */
    
    for ( i = 0; g_cmd_maps[i].cmd; ++i ) {
      if ( !upsutl_strincmp( act_s, g_cmd_maps[i].cmd, (size_t)len ) ) {
	
	/* we found a match. create a new string with these parameters.
	   note - it does not include an open parenthesis */
	
	if ( (param_str = upsutl_str_create( (char *)&act_s[len+1],
			   STR_TRIM_DEFAULT) ) != NULL ) {
	    
	  /* trim off beginning & ending whitespace & the ending ")" */
	    
	  upsutl_str_remove_edges( param_str, trim_chars );
	  icmd = i;                /* save the location in the array */
	}
	break;
      }
    }
  }

  /* fill cmd_cur and split parameter string into a list of arguments */
  
  if ( icmd != e_invalid_action ) {
    cmd_cur->map = &g_cmd_maps[icmd];
    cmd_cur->argc = upsact_params( param_str, cmd_cur->argv );
  }
  
    
  return icmd;
}


/* Action handling */

void f_copyHtml( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv)
{
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->html_path) {
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "html");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n", 
		    a_argv[0], a_db_info->config->html_path) != EOF) {
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_copyInfo( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv)
{
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->info_path) {
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "info");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n", 
		    a_argv[0], a_db_info->config->info_path) != EOF) {
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_copyMan( const t_upstyp_matched_instance * const a_inst,
		const t_upstyp_db * const a_db_info,
		const t_upsugo_command * const a_command_line,
		const FILE * const a_stream,
		const struct s_cmd_map * const a_cmd_map,
		const int a_argc,
		const char ** const a_argv)
{
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->man_path) {
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "man");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n", 
		    a_argv[0], a_db_info->config->man_path) != EOF) {
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
	upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
      }
    }
  }
}

void f_copyNews( const t_upstyp_matched_instance * const a_inst,
		 const t_upstyp_db * const a_db_info,
		 const t_upsugo_command * const a_command_line,
		 const FILE * const a_stream,
		 const struct s_cmd_map * const a_cmd_map,
		 const int a_argc,
		 const char ** const a_argv)
{
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {

    /* Make sure we have somewhere to copy the files to. */
    if (!a_db_info->config->news_path) {
      upserr_add(UPS_NO_DESTINATION, UPS_WARNING, "news");
    } else {  
      switch ( a_command_line->ugo_shell ) {
      case e_BOURNE:
      case e_CSHELL:
	if (fprintf((FILE *)a_stream, "cp %s/* %s\n", 
		    a_argv[0], a_db_info->config->news_path) != EOF) {
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
	}
	break;
      default:
	upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
      }
      if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 3;
  char *delimiter;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=\${%s-}%s%s;export %s\n", a_argv[0], a_argv[0],
		  delimiter, a_argv[1], a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s \${%s}%s%s\n", a_argv[0], a_argv[0],
		  delimiter, a_argv[1]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 3;
  char *delimiter;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s%s\${%s-};export %s\n", a_argv[0], a_argv[1],
		  delimiter, a_argv[0], a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s %s%s\${%s}\n", a_argv[0], a_argv[1],
		  delimiter, a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }

    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 3;
  char *delimiter;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "upstmp=`dropit.pl %s %s %s`;\nif [ $? -eq 0 ]; then %s=$upstmp; fi\nunset upstmp;\n",
		  a_argv[0], a_argv[1], delimiter, a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv upstmp \"`dropit.pl %s %s %s`\"\nif ($status == 0) setenv %s $upstmp\nunsetenv upstmp\n",
		  a_argv[0], a_argv[0], delimiter, a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    
      if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 2;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s;export %s\n", a_argv[0], a_argv[1],
		  a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s %s\n", a_argv[0], a_argv[1]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "unset %s\n", a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "unsetenv %s\n", a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "hash %s;\nif [ $? -eq 1 ]; then return 1; fi\n",
		  a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "whereis %s\nif ($status == 1) return 1\n",
		  a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 1;
  int max_num_params = 2;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=`%s`;export %s\n", a_argv[1], a_argv[0],
		  a_argv[1]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv %s \"`%s`\"\n", a_argv[1], a_argv[0])
	  != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 3;
  char *err_message;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct error message */
    GET_ERR_MESSAGE((char *)a_argv[max_num_params-1]);

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "if [ ! %s %s ]; then\necho %s;\nreturn 1;\nfi;\n",
		  a_argv[1], a_argv[0], err_message) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "if ( ! %s %s ) return 1\n", 
		  a_argv[1], a_argv[0], err_message) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 3;
  char *delimiter;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=${%s-}%s%s;export %s\n", a_argv[0], a_argv[0],
		  delimiter, a_argv[1], a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "set %s=($%s %s)\nrehash\n", a_argv[0], a_argv[0],
		  a_argv[1]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 3;
  char *delimiter;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s%s\${%s-};export %s\n", a_argv[0], a_argv[1],
		  delimiter, a_argv[0], a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "set %s=(%s $%s)\nrehash\n", a_argv[0], a_argv[1],
		  a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }

    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 3;
  char *delimiter;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    /* get the correct delimiter */
    GET_DELIMITER();

    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "upstmp=`dropit.pl %s %s %s`;\nif [ $? -eq 0 ]; then %s=$upstmp; fi\nunset upstmp;\n",
		  a_argv[0], a_argv[1], delimiter, a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "setenv upstmp \"`dropit.pl %s %s %s`\"\nif ($status == 0) set %s=$upstmp\nrehash\nunsetenv upstmp\n",
		  a_argv[0], a_argv[0], delimiter, a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    
      if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 2;
  int max_num_params = 2;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "%s=%s;export %s\n", a_argv[0], a_argv[1],
		  a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "set %s=(%s)\nrehash\n", a_argv[0], a_argv[1]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, ". %s;\n", a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "source %s\n", a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "if [ -r %s ]; then . %s; fi\n", a_argv[0],
		  a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "if (-r %s) source %s\n", a_argv[0], a_argv[0])
	  != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, ". %s;\nif [ $? -eq 1 ]; then return 1; fi;\n",
		  a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "source %s\nif ($status == 1) return 1/n",
		  a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
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
  int min_num_params = 1;
  int max_num_params = 1;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "if [ -r %s ]; then\n  . %s;\n  if [ $? -eq 1 ]; then return 1; fi;\nfi;\n", 
		  a_argv[0], a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "if (-r %s) then\n  source %s\n  if ($status == 1) return 1/nendif\n", 
		  a_argv[0], a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

void f_doDefaults( const t_upstyp_matched_instance * const a_inst,
		   const t_upstyp_db * const a_db_info,
		   const t_upsugo_command * const a_command_line,
		   const FILE * const a_stream,
		   const struct s_cmd_map * const a_cmd_map,
		   const int a_argc,
		   const char ** const a_argv)
{
  int min_num_params = 0;
  int max_num_params = 0;
  
  CHECK_NUM_PARAM(a_cmd_map->cmd);

  /* only proceed if we have a valid number of parameters and a stream to write
     them to */
  if ((UPS_ERROR == UPS_SUCCESS) && a_stream) {
  
    switch ( a_command_line->ugo_shell ) {
    case e_BOURNE:
      if (fprintf((FILE *)a_stream, "if [ -r %s ]; then\n  . %s;\n  if [ $? -eq 1 ]; then return 1; fi;\nfi;\n", 
		  a_argv[0], a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    case e_CSHELL:
      if (fprintf((FILE *)a_stream, "if (-r %s) then\n  source %s\n  if ($status == 1) return 1/nendif\n", 
		  a_argv[0], a_argv[0]) != EOF) {
	upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fprintf", strerror(errno));
      }
      break;
    default:
      upserr_add(UPS_INVALID_SHELL, UPS_FATAL, a_command_line->ugo_shell);
    }
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_add(UPS_ACTION_WRITE_ERROR, UPS_FATAL, a_cmd_map->cmd);
    }
  }
}

