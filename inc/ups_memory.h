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

void *upsmem_malloc(const int a_bytes);
void upsmem_free(void *a_data);
void upsmem_inc_refctr(const void * const a_data);
void upsmem_dec_refctr(const void * const a_data);
void upsmem_print(void);

#endif /* _UPS_XYZ_H_ */

