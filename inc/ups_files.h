/************************************************************************
 *
 * FILE:
 *       ups_files.h
 * 
 * DESCRIPTION: 
 *       Will read an ups file, and fill corresponding data structures.
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
 *       07-jul-1997, LR, first
 *
 ***********************************************************************/

#ifndef _UPS_FIL_H_
#define _UPS_FIL_H_

/* standard include files, if needed for .h file */
#include <stdio.h>

/* ups specific include files, if needed for .h file */
#include "ups_types.h"

/* public typdef's */

/*
 * Declaration of public functions
 */

t_ups_product *upsfil_read_file( FILE *fh );
void          g_print_product( t_ups_product *prod_ptr );

/*
 * Declarations of public variables
 */

#endif /* _UPS_FIL_H_ */







