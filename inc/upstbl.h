/************************************************************************
 *
 * FILE:
 *       upstbl.h
 * 
 * DESCRIPTION: 
 *       An implementation of a table.
 *       Code from "C Interfaces and Implementations" by David R. Hanson.
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
 *       09-Dec-1997, LR, first.
 *
 ***********************************************************************/
#ifndef _UPSTBL_H_
#define _UPSTBL_H_

#define T t_upstbl
typedef struct T *T;

/* Table interface */

extern T     upstbl_new( int hint );
extern void  upstbl_free( T * const table );
extern int   upstbl_length( T const table );
extern void  *upstbl_put( T const table, 
			  const void * const key,
			  void * const value );
extern void  *upstbl_get( T const table, 
			  const void * const key );
extern void  *upstbl_remove( T const table, 
			     const void * const key);
extern void  upstbl_map( T const table,
			 void apply(const void *, void **, void *),
			 void * const cl );
extern void  **upstbl_to_array( T const table, 
				void * const end );
extern void upstbl_dump( T const table, const int iopt );

/* Atom interface */

extern int         upstbl_atom_length( const char * const str );
extern const char  *upstbl_atom_new( const char * const str, const int len );
extern const char  *upstbl_atom_string( const char * const str );
extern const char  *upstbl_atom_int( const int n );

#undef T
#endif /* _UPSTBL_H */
