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
#define UPS_INVALID           -1
#define UPS_SUCCESS           0
#define UPS_OPEN_FILE         1
#define UPS_READ_FILE         2
#define UPS_INVALID_KEYWORD   3
#define UPS_NO_DATABASE       4
#define UPS_TIME              5
#define UPS_NAME_TOO_LONG     6
#define UPS_NO_STAT_DIR       7
#define UPS_WRITE_FILE        8
#define UPS_NERR              9
#define UPS_INVALID_ARGUMENT  10

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

extern int UPS_VERBOSE;
extern int g_ups_line;
extern char *g_ups_file;


#define upserr_place() g_ups_line=__LINE__; g_ups_file=(char *)__FILE__;
#define upserr_vplace() if (UPS_VERBOSE) { upserr_place() }

#endif /* _UPS_ERROR_H_ */

