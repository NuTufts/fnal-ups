/************************************************************************
 *
 * FILE:
 *       ups_utils.c
 * 
 * DESCRIPTION: 
 *       UPs utility routines
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
 *       28-Jul-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdlib.h>
#include <time.h>

/* ups specific include files */
#include "ups_utils.h"
#include "ups_error.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */

/*
 * Definition of global variables.
 */
#ifndef NULL
#define NULL 0
#endif

static clock_t start_cpu, finish_cpu;
static time_t start_w, finish_w;

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upsutl_environment
 *
 * Translate the name of an environmental variable and return the value.
 *
 * Input : Name of the environmental variable
 * Output: none
 * Return: The value of the variable
 */
char * upsutl_environment(const char * const a_env_var)
{
  return (getenv(a_env_var));
}

/*-----------------------------------------------------------------------
 * upsutl_start_timing
 *
 * Start timing the current function
 *
 * Input : none
 * Output: none
 * Return: none
 */
void upsutl_start_timing(void)
{
  start_w = time(NULL);
  start_cpu = clock();
}

/*-----------------------------------------------------------------------
 * upsutl_stop_timing
 *
 * Stop timing the current function.  Return the duration as part of the
 * error stack.
 *
 * Input : none
 * Output: none
 * Return: none
 */
void upsutl_stop_timing(void)
{
  double duration_cpu, duration_w;

  finish_cpu = clock();
  finish_w = time(NULL);
  duration_cpu = ((double )(finish_cpu - start_cpu))/ (double )CLOCKS_PER_SEC;
  duration_w = difftime(finish_w, start_w);

  /* Add the times to the error stack */
  upserr_add(UPS_TIME, UPS_INFORMATIONAL, duration_cpu, duration_w);
}

/*
 * Definition of private globals.
 */

/*
 * Definition of private functions.
 */

