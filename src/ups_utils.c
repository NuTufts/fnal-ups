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
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

/* ups specific include files */
#include "ups_utils.h"
#include "ups_error.h"
#include "ups_types.h"

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

static clock_t g_start_cpu, g_finish_cpu;
static time_t g_start_w, g_finish_w;
static char g_stat_dir[] = "/statistics/";
static char g_unknown_user[] = "UNKNOWN";
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
  g_start_w = time(NULL);
  g_start_cpu = clock();
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

  g_finish_cpu = clock();
  g_finish_w = time(NULL);
  duration_cpu = ((double )(g_finish_cpu - g_start_cpu))/(double )CLOCKS_PER_SEC;
  duration_w = difftime(g_finish_w, g_start_w);

  /* Add the times to the error stack */
  upserr_add(UPS_TIME, UPS_INFORMATIONAL, duration_cpu, duration_w);
}

/*-----------------------------------------------------------------------
 * upsutl_statistics
 *
 * Save statistics on the command just executed and the product(s) involved.
 * The data saved are - 
 *         username
 *         date and time
 *         hostname
 *         command executed
 *         instance accessed (version, flavor and qualifiers)
 *
 * Input : pointer to instance for which to save statistics,
 *         pointer to directory which holds 'statistics' directory,
 *         pointer to the command that was executed
 * Output: none
 * Return: none
 */
int upsutl_statistics(t_ups_instance const * const a_instance,
		      char const * const a_dir, char const * const a_command)
{ 
  char stat_file[FILENAME_MAX+1];
  int dir_s, stat_s, file_s;
  int return_status;
  FILE *file_stream;
  char mode[] = "a,access=lock";              /* create new file or append
						 to existing one */
  char *time_date, *user;
  char tmpBuf[1000];

  /* See if we were passed a directory for the statistics files */
  if (a_dir != NULL) {
    dir_s = (int )strlen(a_dir);
    stat_s = (int )strlen(g_stat_dir);
    file_s = (int )strlen(a_instance->product);
    if ( (dir_s + stat_s + file_s + 1) < FILENAME_MAX) {
      /* Construct the filename where the statistics are to be stored. */
      strcpy(stat_file, a_dir);                 /* directory */
      strcat(stat_file, g_stat_dir);            /* stats sub-dir */
      strcat(stat_file, a_instance->product);   /* filename */

      /* See if we can open the file that we are supposed to write to. */
      if ((file_stream = fopen(stat_file, mode)) != NULL) {
	/* Get the current time and date */
	time_date = upsutl_time_date();

	/* Get the current user */
	user = upsutl_user();

	/* Write out the statistics to the file. To make the code easier
	   to follow, assume an error occurred first, so we do not have to
	   check each time.  Reset the return_status at end if all is ok. */
	return_status = UPS_WRITE_FILE;

	/* NOTE: time_date already has a carriage return in it */
	sprintf(tmpBuf, "USER = %s\n   DATE = %s", user, time_date);
	if (fwrite((void *)tmpBuf, (size_t )1, (size_t )strlen(tmpBuf),
		   file_stream) == strlen(tmpBuf)) {
	  sprintf(tmpBuf, "   COMMAND = %s\n", a_command);
	  if (fwrite((void *)tmpBuf, (size_t )1, (size_t )strlen(tmpBuf),
		     file_stream) == strlen(tmpBuf)) {
	    sprintf(tmpBuf,
		    "   FLAVOR = %s\n   QUALIFIERS = '%s'\n   VERSION = %s\n", 
		    a_instance->flavor, a_instance->qualifiers,
		    a_instance->version);
	    if (fwrite((void *)tmpBuf, (size_t )1, (size_t )strlen(tmpBuf),
		       file_stream) == strlen(tmpBuf)) {
	      sprintf(tmpBuf, "%s\n", DIVIDER);
	      /* Write out divider */
	      if (fwrite((void *)tmpBuf, (size_t )1, (size_t )strlen(tmpBuf),
			 file_stream) == strlen(tmpBuf)) {
		return_status = UPS_SUCCESS;
	      }
	    }
	  }
	} 
	/* Check to see if we encountered an error writing the file */
	if (return_status == UPS_WRITE_FILE) {
	  upserr_add(UPS_WRITE_FILE, UPS_WARNING, stat_file);
	}
      } else {
	/* Error opening file */
	upserr_add(UPS_OPEN_FILE, UPS_WARNING, stat_file);
	return_status = UPS_OPEN_FILE;
      }
    } else {
      /* Error size of directory path to file is too long */
      upserr_add(UPS_NAME_TOO_LONG, UPS_WARNING, FILENAME_MAX);
      return_status = UPS_NAME_TOO_LONG;
    }
  } else {
    /* Error no directry passed */
    upserr_add(UPS_NO_STAT_DIR, UPS_WARNING);
    return_status = UPS_NO_STAT_DIR;
  }

  /* Close the file if it was opened */
  if (file_stream != NULL) {
    fclose(file_stream);
  }
  return(return_status);
}

/*-----------------------------------------------------------------------
 * upsutl_time_date
 *
 * Return an ascii representation of the current time and date
 *
 * Input : none
 * Output: none
 * Return: character string containing the current time and date
 */
char *upsutl_time_date(void)
{
  time_t now;

  /* Get the current time and date */
  now = time(NULL);
  return (ctime(&now));
}

/*-----------------------------------------------------------------------
 * upsutl_user
 *
 * Return an ascii representation of the current user
 *
 * Input : none
 * Output: none
 * Return: character string containing the current user
 */
char *upsutl_user(void)
{
  char *username = NULL;
  struct passwd *pwd;

  /* First try to get the username by calling getpwuid.  If this does not
     work, translate the environmental variable USER.  If this does not
     work either, set the user to UNKNOWN. */
  if ((pwd = getpwuid(getuid())) == NULL) {
    if ((username = (char *)getenv("USER")) == NULL) {
      username = g_unknown_user;
    }
  } else {
    username = pwd->pw_name;
  }
  return (username);
}
