/************************************************************************
 *
 * FILE:
 *       upsmat.h
 * 
 * DESCRIPTION: 
 *       Prototypes etc., for instance matching
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
 *       30-Jul-1997, EB, first
 *
 ***********************************************************************/

#ifndef _UPSMAT_H_
#define _UPSMAT_H_

/* standard include files, if needed for .h file */

/* ups specific include files, if needed for .h file */
#include "upsugo.h"
#include "upstyp.h"

/*
 * Constans.
 */

/*
 * Types.
 */

/*
 * Declaration of public functions.
 */
t_upslst_item *upsmat_match_instance(
				    const t_ups_command * const a_command_line,
				    const int a_need_unique);

/*
 * Declaration of private globals.
 */


/*
 * Declarations of public variables.
 */


#endif /* _UPSMAT_H_ */
