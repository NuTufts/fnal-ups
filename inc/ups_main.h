/************************************************************************
 *
 * FILE:
 *       ups_main.h
 * 
 * DESCRIPTION: 
 *       Infomation used in the ups mainline.
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
 *       20-Nov-1997, EB, first
 *
 ***********************************************************************/

#ifndef _UPS_MAIN_H_
#define _UPS_MAIN_H_

/* standard include files, if needed for .h file */

/* ups specific include files, if needed for .h file */

/*
 * Constans.
 */

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
  e_help,
  e_unk                  /* This one must always be at the end */
};


/*
 * Types.
 */

/* all individual command information */
typedef struct s_cmd_info {
  int cmd_index;
  char *cmd;
  char *valid_opts;
  unsigned int flags;
} t_cmd_info;

/*
 * Declaration of public functions.
 */

/*
 * Declaration of private globals.
 */

/*
 * Declarations of public variables.
 */
extern t_cmd_info g_cmd_info[];

#endif /* _UPS_MAIN_H_ */
