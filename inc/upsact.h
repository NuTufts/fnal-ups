/************************************************************************
 *
 * FILE:
 *       upsact.h
 * 
 * DESCRIPTION: 
 *       Prototypes etc. needed when handling actions.
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

#ifndef _UPSACT_H_
#define _UPSACT_H_

/* standard include files, if needed for .h file */
#include <stdio.h>

/* ups specific include files, if needed for .h file */

#include "upstyp.h"
#include "upsugo.h"
#include "upsmat.h"
#include "upslst.h"
#include "upstyp.h"

/*
 * Constans.
 */
#define UPS_MAX_ARGC 100

/* commands and situations that could be in ACTION=situation line */
enum {
  e_invalid_action = -1, /* From here to e_setup is the actions there is */
  e_current,             /* NOT UPS commands */
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
  e_unchain,
  e_setup,               /* This one starts the UPS commands */
  e_unsetup,
  e_list,
  e_configure,
  e_copy,
  e_declare,
  e_depend,
  e_exist,
  e_modify,
  e_start,
  e_stop,
  e_tailor,
  e_unconfigure,
  e_undeclare,
  e_create,
  e_get,
  e_validate,
  e_flavor,
  e_help,
  e_unk                  /* This one must always be at the end */
};


/* there must be one of these for each action and they must be in the same
   order as in the array in upsact.c */
enum {
  e_invalid_cmd = -1,
  e_setupoptional = 0,
  e_setuprequired,
  e_unsetupoptional,
  e_unsetuprequired, /* this action have to be the last setup/unsetup action */
  e_sourcecompilereq,
  e_sourcecompileopt,
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
  e_makedir,
  e_copyhtml,
  e_copyinfo,
  e_copyman,
  e_uncopyman,
  e_copynews,
  e_writecompilescript,
  e_dodefaults,
  e_nodefaults,
  e_nosetupenv,
  e_noproddir
};

/*
 * Types.
 */

typedef struct upsact_cmd {
  int  iact;
  int  icmd;
  int  argc;
  char *argv[UPS_MAX_ARGC];
  char *pmem;
} t_upsact_cmd;

typedef struct upsact_item {
  int                        level;
  t_upsugo_command           *ugo;
  t_upstyp_matched_product   *mat;
  t_upstyp_action            *act;
  t_upsact_cmd               *cmd;
} t_upsact_item;

/* all individual command information */
typedef struct s_cmd_info {
  int cmd_index;
  char *cmd;
  char *valid_opts;
  unsigned int flags;
  int uncmd_index;
} t_cmd_info;

/* this one is the type of a action command handler */
typedef void (*tpf_cmd)( const t_upstyp_matched_instance * const a_inst,
			 const t_upstyp_db * const a_db_info,
			 const t_upsugo_command * const a_command_line,
                         const FILE * const a_stream,
                         const t_upsact_cmd * const a_cmd);

/* this one is the type for a single action command */
typedef struct s_cmd_map {
  char *cmd;
  int  icmd;
  tpf_cmd func;
  int  min_params;
  int  max_params;
  int  icmd_undo;
} t_cmd_map;

/*
 * Declaration of public functions.
 */

void upsact_process_commands( const t_upslst_item * const a_cmd_list,
			      const FILE * const a_stream);
t_upslst_item *upsact_get_cmd( t_upsugo_command * const ugo_cmd,
			       t_upstyp_matched_product * const mat_prod,
			       const char * const act_name,
			       int ups_cmd );
int upsact_print( t_upsugo_command * const ugo_cmd,
		  t_upstyp_matched_product *mat_prod,
		  const char * const act_name,
		  int ups_cmd,
		  char * sopt );
t_upsact_cmd *upsact_parse_cmd( const char * const cmd_str );
void upsact_cleanup( t_upslst_item *dep_list );
t_upslst_item *upsact_check_files(
			    const t_upstyp_matched_product * const a_mproduct,
			    const t_upsugo_command * const a_command_line,
			    char * const a_cmd);
void upsact_free_upsact_cmd( t_upsact_cmd * const act_cmd );
void upsact_print_cmd( const t_upsact_cmd * const cmd_cur );
void upsact_print_item( const t_upsact_item *const p_cur, char * sopt );
int  upsact_action2enum( const char * const act_name );

#endif /* _UPSACT_H_ */

