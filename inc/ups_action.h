/************************************************************************
 *
 * FILE:
 *       ups_action.h
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

#ifndef _UPS_ACTION_H_
#define _UPS_ACTION_H_

/* standard include files, if needed for .h file */

/* ups specific include files, if needed for .h file */

/*
 * Constans.
 */

#define OPEN_PAREN '('

/*
 * Types.
 */

/*
 * Declaration of public functions.
 */

int upsact_parse( char * const a_action_line, char ** const a_params,
		  int * const a_action_val);

/*
 * Declaration of private globals.
 */

/*
 * Declarations of public variables.
 */

#endif /* _UPS_ACTION_H_ */
