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

/* ups specific include files */
#include "upsact.h"
#include "upserr.h"
#include "upslst.h"
#include "upsutl.h"
#include "upsmem.h"
#include "upskey.h"

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
  e_noproddir,
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
  e_nosetupenv
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
typedef int (*tpf_cmd)( const t_cmd_cur * const cmd_ptr, const char copt );

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

int f_setupoptional( const t_cmd_cur * const, const char );
int f_setuprequired( const t_cmd_cur * const, const char );
int f_unsetupoptional( const t_cmd_cur * const, const char );
int f_unsetuprequired( const t_cmd_cur * const, const char );
int f_envappend( const t_cmd_cur * const, const char );
int f_envremove( const t_cmd_cur * const, const char );
int f_envprepend( const t_cmd_cur * const, const char );
int f_envset( const t_cmd_cur * const, const char );
int f_envunset( const t_cmd_cur * const, const char );
int f_noproddir( const t_cmd_cur * const, const char );
int f_sourcerequired( const t_cmd_cur * const, const char );
int f_sourceoptional( const t_cmd_cur * const, const char );
int f_sourcereqcheck( const t_cmd_cur * const, const char );
int f_sourceoptcheck( const t_cmd_cur * const, const char );
int f_exeaccess( const t_cmd_cur * const, const char );
int f_execute( const t_cmd_cur * const, const char );
int f_filetest( const t_cmd_cur * const, const char );
int f_copyhtml( const t_cmd_cur * const, const char );
int f_copyinfo( const t_cmd_cur * const, const char );
int f_copyman( const t_cmd_cur * const, const char );
int f_copynews( const t_cmd_cur * const, const char );
int f_dodefaults( const t_cmd_cur * const, const char );
int f_nodefaults( const t_cmd_cur * const, const char );
int f_nosetupenv( const t_cmd_cur * const, const char );

/*
 * Definition of global variables.
 */

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
  { "setupoptional", e_setupoptional, f_setupoptional },
  { "setuprequired", e_setuprequired, f_setuprequired },
  { "unsetupoptional", e_unsetupoptional, f_unsetupoptional },
  { "unsetuprequired", e_unsetuprequired, f_unsetuprequired },
  { "envappend", e_envappend, f_envappend },
  { "envremove", e_envremove, f_envremove },
  { "envprepend", e_envprepend, f_envprepend },
  { "envset", e_envset, f_envset },
  { "envunset", e_envunset, f_envunset },
  { "noproddir", e_noproddir, f_noproddir },
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
  { "nodefaults", e_nodefaults, f_nodefaults },
  { "nosetupenv", e_nosetupenv, f_nosetupenv },
  { 0,0,0 }
};


/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upsact_translate
 *
 * Given an action, it's parameters and a stream pointer, write the shell 
 * or cshell code corresponding to the passed action to the stream.
 *
 * Input : action line, pointer to db config info
 * Output: none
 * Return: pointer to string of translated action
 */
int upsact_translate( t_upstyp_instance * const inst_ptr,
		      const char * const action_name )
{
  t_upstyp_action *act_ptr = NULL;
  t_upslst_item *cmd_list = NULL;
  int ierr = UPS_SUCCESS;  
  t_cmd_cur cmd_cur;

  cmd_cur.inst = inst_ptr;
  cmd_cur.action_name = action_name;
  cmd_cur.map = NULL;
  cmd_cur.argc = 0;
  
  /* get action */
  
  if ( !(act_ptr = upskey_inst_getaction( inst_ptr, action_name )) ) {
    return f_dodefaults( &cmd_cur, 's' );
  }
  
  /* parse each command line for action */

  cmd_list = upslst_first( act_ptr->command_list );
  for ( ; cmd_list; cmd_list = cmd_list->next ) {
    
    char *cmd_str = (char *)cmd_list->data;      
  
    if ( upsact_parse( cmd_str, &cmd_cur ) != e_invalid_action ) { 
    
      /* Replace all ups pre-set local variables with their actual values in
	 the parameters before they are split up. */
      /* new_params = upsact_local_vars(params, a_local_vars); */

      /* handle action */
    
      ierr = g_cmd_maps[cmd_cur.map->icmd].func( &cmd_cur, 's' );
    }
    else {
      /* invalid action */
      upserr_add( UPS_INVALID_ACTION, UPS_WARNING, cmd_str );
      ierr = UPS_INVALID_ACTION;
      continue;
    }
  }

  return ierr;
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
    
    ierr = g_cmd_maps[cmd_cur.map->icmd].func( &cmd_cur, 's' );
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
      upserr_add( UPS_TO_MANY_ACTION_ARG, UPS_FATAL, a_params );
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
  
  while ( act_s && *act_s && isspace( *(act_s) ) ){ ++act_s; };

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

int check_argc( const int argc[] )
{
  return 1;
}

void print_act( const t_cmd_cur * const cmd_cur )
{
  int i;
  int icmd;
  
  if ( !cmd_cur || !cmd_cur->map )
    return;

  icmd = cmd_cur->map->icmd;
  
  printf( "%s( ", g_cmd_maps[icmd].cmd );
  for ( i = 0; i < cmd_cur->argc; i++ ) {
    if ( i == cmd_cur->argc - 1 ) 
      printf( "%s", cmd_cur->argv[i] );
    else
      printf( "%s, ", cmd_cur->argv[i] );
  }
  printf( " )\n" ); 
}

int f_setupoptional( const t_cmd_cur * const cmd_cur, const char shell )
{
  return f_setuprequired( cmd_cur, shell );
}

int f_setuprequired( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};

  /* check number of arguments */
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );

  switch ( shell ) {

  case 's':

    /*
      call upsugo_bldcmd( char * const cmdstr, char * const validopts );
    */

    break;

  default:
    return UPS_INVALID_SHELL;
  }
    
  return UPS_SUCCESS;
}

int f_unsetupoptional( const t_cmd_cur * const cmd_cur, const char shell )
{
  return f_unsetuprequired( cmd_cur, shell );
}

int f_unsetuprequired( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {
  case 's':
    /*
    printf( "#\n#The following are wrong:\n" );
    printf( "#it should be about g_argv[0]\n" );
    printf( "unset %s_DIR\n", cmd_cur->inst->product );
    printf( "unset SETUP_%s\n", cmd_cur->inst->product );
    */
    break;

  default:
    return UPS_INVALID_SHELL;
  }
    
  return UPS_SUCCESS;
}

int f_envappend( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {2,3};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );

  switch ( shell ) {

  case 's':
    if ( cmd_cur->argc > 2 ) 
      printf( "%s=${%s}%s%s; export %s\n",
	       cmd_cur->argv[0], cmd_cur->argv[0], cmd_cur->argv[1], cmd_cur->argv[2], cmd_cur->argv[0] );
    else
      printf( "%s=${%s}:%s; export %s\n",
	       cmd_cur->argv[0], cmd_cur->argv[0], cmd_cur->argv[1], cmd_cur->argv[0] );	      
    break;

  default:
    return UPS_INVALID_SHELL;
  }

  return UPS_SUCCESS;
}

int f_envremove( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {2,3};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );

  switch ( shell ) {

  case 's':
    if ( cmd_cur->argc > 2 )      
      printf( "upstmp1=`dropit.pl %s %s %s`\n",
	       cmd_cur->argv[0], cmd_cur->argv[1], cmd_cur->argv[2] );
    else
      printf( "upstmp1=`dropit.pl %s %s :`\n",
	       cmd_cur->argv[0], cmd_cur->argv[1] );
    printf( "upstmp2=$?\n" );
    printf( "if [ $upstmp2 -eq 0 ]\n" );
    printf( "then\n" );
    printf( "  %s=$upstmp1; export %s\n", cmd_cur->argv[0], cmd_cur->argv[0] );
    printf( "fi\n" );
    printf( "unset upstmp1\n" );
    printf( "unset upstmp2\n" );
    
    break;

  default:
    return UPS_INVALID_SHELL;
  }
  
  return UPS_SUCCESS;
}

int f_envprepend( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {2,3};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {

  case 's':
    if ( cmd_cur->argc > 2 ) 
      printf( "%s=%s%s${%s}; export %s\n",
	       cmd_cur->argv[0], cmd_cur->argv[1], cmd_cur->argv[2], cmd_cur->argv[0], cmd_cur->argv[0] );
    else
      printf( "%s=%s:${%s}; export %s\n",
	       cmd_cur->argv[0], cmd_cur->argv[1], cmd_cur->argv[0], cmd_cur->argv[0] );	      
    break;

  default:
    return UPS_INVALID_SHELL;
  }
  
  return UPS_SUCCESS;
}

int f_envset( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {2,2};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {

  case 's':
      printf( "%s=%s; export %s\n",
	       cmd_cur->argv[0], cmd_cur->argv[1], cmd_cur->argv[0] );
    break;

  default:
    return UPS_INVALID_SHELL;
  }
  
  return UPS_SUCCESS;
}

int f_envunset( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};

  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {

  case 's':
      printf( "unset %s\n", cmd_cur->argv[0] );
    break;

  default:
    return UPS_INVALID_SHELL;
  }
  
  return UPS_SUCCESS;
}

/* ??? */
int f_noproddir( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  print_act( cmd_cur );	  
  return UPS_SUCCESS;
}

int f_sourcerequired( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {

  case 's':
      printf( ". ${%s}.sh\n", cmd_cur->argv[0] );
    break;

  default:
    return UPS_INVALID_SHELL;
  }
  
  return UPS_SUCCESS;
}

int f_sourceoptional( const t_cmd_cur * const cmd_cur, const char shell )
{
  return f_sourcerequired( cmd_cur, shell );
}

int f_sourcereqcheck( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {

  case 's':
    printf( ". ${%s}.sh\n", cmd_cur->argv[0] );
    printf( "upstmp=$?\n" );
    printf( "if [ $upstmp -eq 1 ]\n" );
    printf( "then\n" );
    printf( "  unset upstmp\n" );
    printf( "  return 1\n" );
    printf( "fi\n" );
    printf( "unset upstmp\n" );
    break;

  default:
    return UPS_INVALID_SHELL;
  }
  
  return UPS_SUCCESS;
}

int f_sourceoptcheck( const t_cmd_cur * const cmd_cur, const char shell )
{
  return f_sourcereqcheck( cmd_cur, shell );
}

int f_exeaccess( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {

  case 's':
    printf( "hash %s\n", cmd_cur->argv[0] );
    printf( "upstmp=$?\n" );
    printf( "if [ $upstmp -eq 1 ]\n" );
    printf( "then\n" );
    printf( "  unset upstmp\n" );
    printf( "  return 1\n" );
    printf( "fi\n" );
    printf( "unset upstmp\n" );
    break;

  default:
    return UPS_INVALID_SHELL;
  }
  
  return UPS_SUCCESS;
}

int f_execute( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,2};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {

  case 's':
    if ( cmd_cur->argc == 2 )
      printf( "%s = `%s`; export %s\n",
	       cmd_cur->argv[1], cmd_cur->argv[0], cmd_cur->argv[1] );
    else
      printf( "`%s`\n",cmd_cur->argv[0] );
    break;

  default:
    return UPS_INVALID_SHELL;
  }
  
  return UPS_SUCCESS;
}

int f_filetest( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {2,3};
  
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  
  print_act( cmd_cur );
  
  switch ( shell ) {

  case 's':
    printf( "if [ ! %s %s ]\n", cmd_cur->argv[1], cmd_cur->argv[0] );
    printf( "then\n" );
    if ( cmd_cur->argc == 3 )
      printf( "  echo %s\n", cmd_cur->argv[2] );
    printf( "  return 1\n" );
    printf( "fi\n" );
    break;

  default:
    return UPS_INVALID_SHELL;
  }

  return UPS_SUCCESS;
}

int f_copyhtml( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  print_act( cmd_cur );	  
  return UPS_SUCCESS;
}

int f_copyinfo( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  print_act( cmd_cur );	  
  return UPS_SUCCESS;
}

int f_copyman( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  print_act( cmd_cur );	  
  return UPS_SUCCESS;
}

int f_copynews( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {1,1};
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  print_act( cmd_cur );	  
  return UPS_SUCCESS;
}

int f_dodefaults( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {0,0};
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  print_act( cmd_cur );	  
  return UPS_SUCCESS;
}

int f_nodefaults( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {0,0};
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  print_act( cmd_cur );	  
  return UPS_SUCCESS;
}

int f_nosetupenv( const t_cmd_cur * const cmd_cur, const char shell )
{
  static int argc[2] = {0,0};
  if ( !check_argc( argc ) )
    return UPS_INVALID_ACTION_ARG;
  print_act( cmd_cur );	  
  return UPS_SUCCESS;
}
