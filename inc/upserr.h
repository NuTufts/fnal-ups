/************************************************************************
 *
 * FILE:
 *       upserr.h
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
 *       13-Aug-1997, LR, added UPS_LINE_TOO_LONG
 *
 ***********************************************************************/

#ifndef _UPSERR_H_
#define _UPSERR_H_

/* standard include files, if needed for .h file */

/* ups specific include files, if needed for .h file */

/*
 * Constants.
 */

/* Error Messages */
/* CAREFUL!! When adding errors, you must also edit ../src/upserr.c.
   There are two ascii arrays that will also need to be modified, one for 
   a long description (g_error_messages) and one for an ascii version of 
   the #define (g_error_ascii) 
   ======================================================================= */

#define UPS_INVALID             -1
#define UPS_SUCCESS             0
#define UPS_OPEN_FILE           1
#define UPS_READ_FILE           2
#define UPS_INVALID_KEYWORD     3
#define UPS_NO_DATABASE         4
#define UPS_TIME                5
#define UPS_NAME_TOO_LONG       6
#define UPS_NO_STAT_DIR         7
#define UPS_WRITE_FILE          8
#define UPS_INVALID_ARGUMENT    9
#define UPS_NO_VERSION_MATCH    10
#define UPS_FILENAME_TOO_LONG   11
#define UPS_NO_TABLE_MATCH      12
#define UPS_NO_FILE             13
#define UPS_NO_MEMORY           14
#define UPS_LINE_TOO_LONG       15
#define UPS_UNKNOWN_FILETYPE    16
#define UPS_NOVALUE_ARGUMENT    17
#define UPS_INVALID_ACTION      18
#define UPS_INVALID_ACTION_ARG  19
#define UPS_TOO_MANY_ACTION_ARG 20
#define UPS_INVALID_SHELL       21
#define UPS_NEED_UNIQUE         22
#define UPS_SYSTEM_ERROR        23
#define UPS_NOT_AUTH            24
#define UPS_NO_DESTINATION      25
#define UPS_ACTION_WRITE_ERROR  26
#define UPS_INVALID_ACTION_PARAMS 27
#define UPS_NOSHELL             28
#define UPS_UNSETUP_FAILED      29
#define UPS_FILE_EXISTS         30
#define UPS_NO_TABLE_FILE       31
#define UPS_VERSION_EXISTS      32
#define UPS_TABLEFILE_AND_VERSION 33
#define UPS_NO_SETUP_ENV        34
#define UPS_NO_PRODUCT          35
#define UPS_ACTION_PARSE        36
#define UPS_UNSETUP_CLASH       37
#define UPS_NO_ACTION           38
#define UPS_NO_NEW_INSTANCE     39
#define UPS_INVALID_SPECIFICATION 40 
#define UPS_NO_PRODUCT_FOUND    41 
#define UPS_DUPLICATE_INSTANCE  42
#define UPS_NERR                43      /*  this one must always be last */

#define UPS_FATAL           "ERROR"
#define UPS_WARNING         "WARNING"
#define UPS_INFORMATIONAL   "INFORMATIONAL"
#define UPS_INFORMATIONAL_LEN 13

/*
 * Types.
 */


/*
 * Declaration of public functions.
 */

void upserr_clear(void);
void upserr_add (const int a_error_id, ...);
void upserr_output (void);
void upserr_backup (void);

/*
 * Declaration of private globals.
 */


/*
 * Declarations of public variables.
 */

extern int UPS_ERROR;
extern int UPS_VERBOSE;
extern int UPS_VERIFY;
extern int g_ups_line;
extern char *g_ups_file;
extern char *g_error_ascii[];


#define upserr_place() g_ups_line=__LINE__; g_ups_file=(char *)__FILE__;
#define upserr_vplace() if (UPS_VERBOSE) { upserr_place() }

#endif /* _UPSERR_H_ */




