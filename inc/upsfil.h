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

t_upstyp_product  *upsfil_read_file( const char * const ups_file );
int            upsfil_write_file( t_upstyp_product * const prod_ptr,
				  const char * const ups_file );
void           g_print_product( t_upstyp_product * const prod_ptr );

/*
 * Declarations of public variables
 */

#define VERSION_SUFFIX  ".version"
#define CHAIN_SUFFIX    ".chain"

/* enum of known file types (changes here should be reflected in cfilei) */
enum e_ups_file {
  e_file_version = 0,
  e_file_table,
  e_file_chain,
  e_file_dbconfig,
  e_file_unknown,
  e_file_count
};


#endif /* _UPSFIL_H_ */







