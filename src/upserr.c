/************************************************************************
 *
 * FILE:
 *       ups_error.c
 * 
 * DESCRIPTION: 
 *       This module contains the error handling functions.
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

/* standard include files */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>

/* ups specific include files */
#include "upserr.h"

/*
 * Definition of public variables.
 */

int UPS_ERROR = UPS_SUCCESS;    /* start out with no errors */
int UPS_VERBOSE = 0;            /* start out not verbose */
int g_ups_line = 0;
char *g_ups_file = '\0';

/*
 * Declaration of private functions.
 */


/*
 * Definition of global variables.
 */
#define G_BUFSIZE 2000
#define G_ERROR_INIT -1
#define G_ERROR_BUF_MAX 30

static int g_buf_counter = G_ERROR_INIT;   /* pointer to current message */
static int g_buf_start = G_ERROR_INIT;     /* pointer to oldest message */

static char *g_error_buf[G_ERROR_BUF_MAX];

/* And now the error messages */
static char *g_error_messages[UPS_NERR] = {
/* 00 */  "%s: Success.\n",
/* 01 */  "%s: Unable to open file %s.\n",
/* 02 */  "%s: Unable to read file %s.\n",
/* 03 */  "%s: Invalid keyword - %s, in %s found.\n",
/* 04 */  "%s: No database specified on command line or in $PRODUCTS.\n",
/* 05 */  "%s: CPU time used - %f, Wall clock time used %f.\n",
/* 06 */  "%s: File name and path too long, must be less than %d bytes.\n",
/* 07 */  "%s: No statistics directory specified.\n",
/* 08 */  "%s: Unable to write file %s.\n",
/* 09 */  "%s: Invalid argument specified \"%s\"\n",
/* 10 */  "%s: No instance matches were made between the chain file (%s) and the version (%s)\n",
/* 11 */  "%s: The passed filename was longer than the allowed system maximum (%d)\n",
/* 12 */  "%s: No instance matches were made between the version file (%s) and the table file (%s)\n",
/* 13 */  "%s: File not found - %s\n",
/* 14 */  "%s: Could not malloc %d bytes\n",
/* 15 */  "%s: MAX_LINE_LEN exceeded in \"%s\"\n",
/* 16 */  "%s: Unknown file type \"%s\"\n",
/* 17 */  "%s: Invalid value \"%s\" for argument \"%s\"\n",
/* 18 */  "%s: Invalid action - %s\n",
/* 19 this is the last */
};

char *g_error_ascii[] = {
   /* UPS_SUCCESS           0  */ "UPS_SUCCESS",
   /* UPS_OPEN_FILE         1  */ "UPS_OPEN_FILE",
   /* UPS_READ_FILE         2  */ "UPS_READ_FILE",
   /* UPS_INVALID_KEYWORD   3  */ "UPS_INVALID_KEYWORD",
   /* UPS_NO_DATABASE       4  */ "UPS_NO_DATABASE",
   /* UPS_TIME              5  */ "UPS_TIME",
   /* UPS_NAME_TOO_LONG     6  */ "UPS_NAME_TOO_LONG",
   /* UPS_NO_STAT_DIR       7  */ "UPS_NO_STAT_DIR",
   /* UPS_WRITE_FILE        8  */ "UPS_WRITE_FAIL",
   /* UPS_INVALID_ARGUMENT  9  */ "UPS_INVALID_ARGUMENT",
   /* UPS_NO_VERSION_MATCH  10 */ "UPS_NO_VERSION_MATCH",
   /* UPS_FILENAME_TOO_LONG 11 */ "UPS_FILENAME_TOO_LONG",
   /* UPS_NO_TABLE_MATCH    12 */ "UPS_NO_TABLE_MATCH",
   /* UPS_NO_FILE           13 */ "UPS_NO_FILE",
   /* UPS_NO_MEMORY         14 */ "UPS_NO_MEMORY",
   /* UPS_LINE_TOO_LONG     15 */ "UPS_LINE_TOO_LONG",
   /* UPS_UNKNOWN_FILETYPE  16 */ "UPS_UNKNOWN_FILETYPE",
   /* UPS_NOVALUE_ARGUMENT  17 */ "UPS_NOVALUE_ARGUMENT",
   /* UPS_INVALID_ACTION    18 */ "UPS_INVALID_ACTION",
   /* UPS_NERR              19 */ "UPS_NERR",
   0 };

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upserr_add
 *
 * Add the requested message to the error buf.  If the error buf is too
 * big, overwrite the oldest error message.
 *
 * Input : error message number, error severity and associated information
 * Output: none
 * Return: none
 */
void upserr_add (const int a_error_index, ...)
{
  va_list args;
  char buf[G_BUFSIZE];
  char *tmpBufPtr;

  /* Initialize */
  buf[0] = '\0';
  UPS_ERROR = a_error_index;

  if ( (a_error_index < UPS_NERR) && (a_error_index > UPS_INVALID)) {
    /* format the error and put it in the error buf */
    va_start(args, a_error_index);
    vsprintf(buf, g_error_messages[a_error_index], args);
    va_end(args);
  }
  else {
    /* This was an invalid error message request */
    sprintf(buf, "ERROR: Invalid error message number %d.\n", a_error_index);
    UPS_ERROR = UPS_INVALID;
  }

  /* Check if we need to add error location information to output too */
  if (UPS_VERBOSE && g_ups_line) {
    sprintf(buf, "%s (line number %d in file %s)\n", buf, g_ups_line,
	    g_ups_file);
    g_ups_line = 0;          /* reset so next time do not give false info */
  }

  /* Malloc space for the message so we can save it and copy it in. */
  tmpBufPtr = (char *)malloc(strlen(buf) + 1);  /* leave room for the \0 too */
  strcpy(tmpBufPtr, buf);

  /* Add the error message to the error buf.  If we are at the bottom of
     the buf, go back to the top and overwrite the oldest message */
  if (++g_buf_counter == G_ERROR_BUF_MAX) {
    /* we have reached the end of the buf, go to the start */
    g_buf_counter = 0;
  }

  if (g_buf_counter == g_buf_start) {
    /* the buf is full, we must delete the oldest message to make room */
    free(g_error_buf[g_buf_start++]);
    if (g_buf_start == G_ERROR_BUF_MAX) {
      /* we have reached the end of the buf, go to the start */
      g_buf_start = 0;
    }
  }

  /* check if this is our first message*/
  if (g_buf_start == G_ERROR_INIT) {
    /* yes, move to start of buffer */
    g_buf_start = 0;
  }

  g_error_buf[g_buf_counter] = tmpBufPtr;
}

/*-----------------------------------------------------------------------
 * upserr_clear
 *
 * Clear out the error buf.  All messages currently in the buf are lost.
 *
 * Input : none
 * Output: none
 * Return: none
 */
void upserr_clear (void)
{
  int i;

  /* only do something if there are messages in the buffer */
  if (g_buf_start != G_ERROR_INIT) {
    /* free all of the error message bufs */
    for (i = g_buf_start; i <= g_buf_counter; ++i) {
      if (i < G_ERROR_BUF_MAX) {
	free(g_error_buf[i]);
      } else {
	i = G_ERROR_INIT;
      }
    }

    /* catch the last one we missed */
    free(g_error_buf[g_buf_counter]);

    /* Reset */
    g_buf_counter = G_ERROR_INIT;

    /* Reset */
    g_buf_start = G_ERROR_INIT;
  }

  UPS_ERROR = UPS_SUCCESS;
}

/*-----------------------------------------------------------------------
 * upserr_output
 *
 * Output the error buf to stderr if it is not empty.
 *
 * Input : none
 * Output: none
 * Return: none
 */
void upserr_output (void)
{
  int i;

  /* only do something if there are messages in the buffer */
  if (g_buf_start != G_ERROR_INIT) {
    for (i = g_buf_start; i != g_buf_counter; ++i) {
      if (i < G_ERROR_BUF_MAX) {
	fputs(g_error_buf[i], stderr);
      } else {
	i = G_ERROR_INIT;
      }
    }

    /* catch the last one we missed */
    fputs(g_error_buf[g_buf_counter], stderr);
  }
}

