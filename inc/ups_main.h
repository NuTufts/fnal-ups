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
  e_setup,
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
  /* This one must always be at the end */
  e_unk
};


/*
 * Types.
 */

/*
 * Declaration of public functions.
 */

/*
 * Declaration of private globals.
 */

/*
 * Declarations of public variables.
 */


#endif /* _UPS_MAIN_H_ */
