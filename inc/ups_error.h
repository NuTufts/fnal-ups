/************************************************************************
 *
 * FILE:
 *       ups_error.h
 * 
 * DESCRIPTION: 
 *       This file includes definition of error messages and associated
 *       variables.
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
 *       15-Jul-1997, EB, first
 *
 ***********************************************************************/

#ifndef _UPS_ERROR_H_
#define _UPS_ERROR_H_

/* standard include files, if needed for .h file */

/* ups specific include files, if needed for .h file */

/*
 * Constants.
 */

/* Error Messages */
#define UPS_INVALID         -1
#define UPS_OPEN_FILE       0
#define UPS_READ_FILE       1
#define UPS_INVALID_KEYWORD 2
#define UPS_NO_DATABASE     3
#define UPS_NERR            4


#define UPS_ERROR           1
#define UPS_FATAL           "ERROR"
#define UPS_WARNING         "WARNING"
#define UPS_INFORMATIONAL   "INFORMATIONAL"


/*
 * Types.
 */


/*
 * Declaration of public functions.
 */

void upserr_clear(void);
void upserr_add (const int a_error_id, ...);
void upserr_output (void);

/*
 * Declaration of private globals.
 */


/*
 * Declarations of public variables.
 */

#define upserr_place() g_ups_line=__LINE__; g_ups_file=(char *)__FILE__;

#endif /* _UPS_ERROR_H_ */

