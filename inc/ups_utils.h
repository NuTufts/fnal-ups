/************************************************************************
 *
 * FILE:
 *       ups_utils.h
 * 
 * DESCRIPTION: 
 *       Utility routine definitions
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
 *       28-Jul-1997, EB, first
 *
 ***********************************************************************/

#ifndef _UPS_UTILS_H_
#define _UPS_UTILS_H_

/* standard include files, if needed for .h file */

/* ups specific include files, if needed for .h file */

/*
 * Constans.
 */

/*
 * Types.
 */

/*
 * Declaration of public functions.
 */

char *upsutl_environment(const char * const a_env_var);
void upsutl_start_timing(void);
void upsutl_stop_timing(void);

/*
 * Declaration of private globals.
 */


/*
 * Declarations of public variables.
 */


#endif /* _UPS_UTILS_H_ */
