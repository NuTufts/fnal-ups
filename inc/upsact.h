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

#include "upstyp.h"

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

int upsact_translate( t_upstyp_instance * const inst_ptr,
		      const char * const action_name );
int upsact_translate_cmd( const char * const cmd_str );


/*
 * Declaration of private globals.
 */

/*
 * Declarations of public variables.
 */

#endif /* _UPSACT_H_ */
