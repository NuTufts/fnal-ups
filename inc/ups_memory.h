/************************************************************************
 *
 * FILE:
 *       ups_memory.h
 * 
 * DESCRIPTION: 
 *       The file prototypes all the UPS memory handline functions
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
 *       25-Jul-1997, EB, first
 *
 ***********************************************************************/

#ifndef _UPS_MEMORY_H_
#define _UPS_MEMORY_H_

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

void *umem_malloc(const int a_bytes);
void umem_free(void *a_data);
void umem_inc_refctr(const void * const a_data);

#endif /* _UPS_XYZ_H_ */
