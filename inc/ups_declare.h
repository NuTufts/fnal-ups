/************************************************************************
 *
 * FILE:
 *       ups_declare.h
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
 *       Mon Dec 15, Djf, First
 ***********************************************************************/

#ifndef _UPS_DECLARE_H_
#define _UPS_DECLARE_H_

/* ups specific include files, if needed for .h file */
#include "upsugo.h"

/*
 * Constans.
 */

/*
 * Types.
 */

/*
 * Declaration of public functions.
 */

void ups_declare(t_upsugo_command * const a_command_line);

#endif /* _UPS_DECLARE_H_ */
