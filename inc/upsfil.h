/************************************************************************
 *
 * FILE:
 *       upsfil.h
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

#ifndef _UPSFIL_H_
#define _UPSFIL_H_

/* standard include files, if needed for .h file */
#include <stdio.h>

/* ups specific include files, if needed for .h file */
#include "upstyp.h"

/* public typdef's */

/*
 * Declaration of public functions
 */

t_ups_product  *upsfil_read_file( const char * const ups_file );
int            upsfil_write_file( t_ups_product * const prod_ptr,
				  const char * const ups_file );
void           g_print_product( t_ups_product * const prod_ptr );

/*
 * Declarations of public variables
 */

#define VERSION_SUFFIX  ".version"
#define CHAIN_SUFFIX    ".chain"

#endif /* _UPSFIL_H_ */







