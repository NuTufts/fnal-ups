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
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>

/* ups specific include files */
#include "ups_error.h"

/*
 * Definition of public variables.
 */

extern int UPS_VERBOSE = 0;     /* start out not verbose */
extern int g_ups_line = 0;
extern char *g_ups_file = '\0';

/*
 * Declaration of private functions.
 */


/*
 * Definition of global variables.
 */
#define G_BUFSIZE 2000
#define G_ERROR_INIT -1
#define G_ERROR_BUF_MAX 30

static char *g_error_buf[G_ERROR_BUF_MAX];

/* And now the error messages */
static char *g_error_messages[] = {
  "%s: Unable to open file %s.\n",
  "%s: Unable to read file %s.\n",
  "%s: Invalid keyword - %s, in %s found.\n",
  "%s: No database specified on command line or in \$PRODUCTS.\n",
  "%s: CPU time used - %f, Wall clock time used %f.\n"
};

static int g_buf_counter = G_ERROR_INIT;   /* pointer to current message */
static int g_buf_start = G_ERROR_INIT;     /* pointer to oldest message */

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

  if ( (a_error_index < UPS_NERR) && (a_error_index > UPS_INVALID)) {
    /* format the error and put it in the error buf */
    va_start(args, a_error_index);
    vsprintf(buf, g_error_messages[a_error_index], args);
    va_end(args);
  }
  else {
    /* This was an invalid error message request */
    sprintf(buf, "ERROR: Invalid error message number %d.\n", a_error_index);
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

