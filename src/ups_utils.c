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
 *       13-Aug-1997, LR, added string handling upsutl_str_*
 *
 ***********************************************************************/

/* standard include files */
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
/* #ifdef _SYSTYPE_SVR4 */
#include <dirent.h>
/* #else
#include <sys/dir.h>
#endif */

/* ups specific include files */
#include "ups_utils.h"
#include "ups_error.h"
#include "ups_types.h"
#include "ups_memory.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */
static int qsort_cmp_string( const void *, const void * ); /* used by qsort */

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
 * upsutl_free_inst_list
 *
 * Given an instance list, free all the instances and the list too.
 * 
 * Input : instance list pointer
 * Output: none
 * Return: Null
 */
t_upslst_item *upsutl_free_inst_list( t_upslst_item **a_inst_list)
{
  t_upslst_item *list_item = NULL, *tmp_inst_list = NULL;
  
  /* make sure we are at the beginning of the list */
  *a_inst_list = upslst_first(*a_inst_list);

  /* free the instances */
  tmp_inst_list = *a_inst_list;
  for (list_item = tmp_inst_list; tmp_inst_list;
       tmp_inst_list = tmp_inst_list->next) {
    ups_free_instance((t_ups_instance *)(tmp_inst_list->data));
  }

  /* Now free the list */
  *a_inst_list = upslst_free(*a_inst_list, ' ');

  return NULL;
}
/*-----------------------------------------------------------------------
 * upsutl_get_files
 *
 * Given a directory, return a listing of all the files which
 * have the specified character string in them.  The character string is
 * removed from the returned string.
 * 
 * Input : Directory where to get files, pattern to match in files
 * Output: none
 * Return: A list of the file names
 */
t_upslst_item * upsutl_get_files(const char * const a_dir,
				 const char * const a_pattern)
{
/* #ifdef _SYSTYPE_SVR4 */
  struct dirent *dir_line = NULL;
/* #else
  struct direct *dir_line = NULL;
#endif */
  t_upslst_item *file_list = NULL;
  DIR *dir = NULL;
  char *new_string = NULL;
  
  if ((dir = opendir(a_dir))) {
    if (! strcmp(a_pattern, ANY_MATCH)) {
      /* read each directory item and add it to the list */
      while ((dir_line = readdir(dir)) != NULL) {
	if (strcmp(dir_line->d_name, ".") && strcmp(dir_line->d_name, "..")) {
	  if ((new_string = upsutl_str_create(dir_line->d_name, ' '))) {
	    file_list = upslst_add(file_list, (void *)new_string);
	  } else {
	    /* the only error from upsutl_str_create is a memory error, clean
	       up and get out */
	    upslst_free(file_list, 'd');
	    break;
	  }
	}
      }
    } else {
      /* read each directory item and if it contains the pattern, remove
	 the pattern from the file name and add it to the list */
      while ((dir_line = readdir(dir)) != NULL) {
	if (strcmp(dir_line->d_name, ".") && strcmp(dir_line->d_name, "..")) {
	  new_string = upsutl_strstr(dir_line->d_name, a_pattern);
	  if (UPS_ERROR == UPS_SUCCESS) {
	    if (new_string) {
	      file_list = upslst_add(file_list, (void *)new_string);
	    }
	  } else {
	    /* the only error from upsutl_strstr is a memory error, clean up
	       and get out */
	    upslst_free(file_list, 'd');
	    break;
	  }
	}
      }
    }
    closedir(dir);

    /* point back to the beginning of the list */
    file_list = upslst_first(file_list);
  } else {
    upserr_add(UPS_OPEN_FILE, UPS_ERROR, a_dir);
  }
  return (file_list);
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
  FILE *file_stream = 0;
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
 * upsutl_strstr
 *
 * Search for the specified pattern in the string.  If it is not there,
 * return NULL, else copy the string to a new string, remove the pattern
 * and return the new string.
 *
 * Input : A string to search, a pattern to search for.
 * Output: none
 * Return: A pointer to a new string without the pattern or NULL.
 */
char *upsutl_strstr( const char * const a_str, const char * const a_pattern)
{
  char *substr = NULL, *new_string = NULL;
  int pat_len, str_len, substr_pos;
  int i, j;

  if ((substr = strstr(a_str, a_pattern))) {
    /* The pattern was found */
    str_len = (int )strlen(a_str);
    if ((new_string = upsutl_str_create((char *)a_str, ' '))) {
      /* create a new string without the pattern */
      pat_len = (int )strlen(a_pattern);
      substr_pos = (int )(substr - a_str);   /* char position of sub string */
      for (i = substr_pos, j = substr_pos + pat_len ; j <= str_len ;
	   ++i, ++j ) {
	new_string[i] = new_string[j];
      }

    } else {
      upserr_add(UPS_NO_MEMORY, UPS_FATAL, str_len);
    }
  }

  return new_string;
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

/*-----------------------------------------------------------------------
 * upsutl_str_create
 *
 * Will create a sring on the heap, using upsmem. 
 *
 * Input : char *, string to be copied.
 *         char, options, 't' will trim edges of passed string.
 * Output: none
 * Return: char *, new string.
 */
char *upsutl_str_create( char * const str, const char copt )
{
  char *new_str = 0;
  
  if ( ! str ) return 0;

  if ( copt == 't' ) {
    
    /* copy overhead only when option 't' is passed */
    
    static char buf[MAX_LINE_LEN];
    if ( strlen( str ) >= MAX_LINE_LEN ) {    
      upserr_add( UPS_LINE_TOO_LONG, UPS_FATAL, "upsutl_str_create" );
      return 0;
    }    
    strcpy( buf, str );
    upsutl_str_remove_edges( buf, " \t\n\r\f\"" );
    
    new_str = (char *)upsmem_malloc( (int)strlen( buf ) + 1 );
    strcpy( new_str, buf );      
  }
  else {      
    new_str = (char *)upsmem_malloc( (int)strlen( str ) + 1 );
    strcpy( new_str, str );      
  }
  
  return new_str;
}

/*-----------------------------------------------------------------------
 * upsutl_str_sort
 *
 * Will sort items in a string, str. Item are delimeted by character c.
 * (e.g "dd,ttt,b,aaa," becomes "aaa,b,dd,ttt")
 *
 * Input : char *, string to be sorted
 *         char, delimeting character
 * Output: none
 * Return: int, number if items sorted
 */
int upsutl_str_sort( char * const str, const char c )
{
  static char buf[MAX_LINE_LEN];
  char *cp, *cp0;  
  char ct[2] = "\0\0";
  size_t max_len = 0, count = 0;
  unsigned int i = 0;

  if ( !str || strlen( str ) <= 0 ) return 0;
  
  ct[0] = c;
  memset( buf, 0, MAX_LINE_LEN );
  
  /* get max len of an item */

  cp0 = str;
  while ( (cp = strchr( cp0, (int)c )) ) {
    if ( max_len < (size_t)(cp-cp0) ) max_len = (size_t)(cp-cp0);
    cp0 = cp+1;
    count++;
  }
  ++max_len;
  ++count;

  /* check size */
  
  if ( count*max_len >= MAX_LINE_LEN ) {    
    upserr_add( UPS_LINE_TOO_LONG, UPS_FATAL, "upsutl_str_sort" );
    return 0;
  }

  /* split, fill buf with evenly spaced items (qsort likes that) */

  count = 0;
  cp = strtok( str, ct );
  do {
    strcpy( &buf[count*max_len], cp );
    count++;
  } while( (cp=strtok( 0, ct )) );

  /* sort */
  
  qsort( buf, count, max_len, qsort_cmp_string );

  /* merge, write back to input */

  strcpy( str, &buf[0] );
  for ( i=1; i<count; i++ ) {
    strcat( str, ct );
    strcat( str, &buf[i*max_len] );
  }

  return (int)count;
}

/*-----------------------------------------------------------------------
 * upsutl_str_remove
 *
 * Will erase characters, defined in str_remove, from in str.
 *
 * Input : char *, string to be trimmed
 *         char *, string of character to remove
 * Output: none
 * Return: int, length of trimmed string
 */
size_t upsutl_str_remove( char * const str, const char * const str_remove )
{
  char *cpf = str, *cp = str;  

  if ( !str || strlen( str ) <= 0 ) return 0;

  while ( cpf && *cpf ) {
    if ( ! strchr( str_remove, (int)*cpf ) ) {
      *cp = *cpf;
      cp++;
    }
    cpf++;
  }
  *cp = 0;

  return strlen( str );    
}

/*-----------------------------------------------------------------------
 * upsutl_str_remove_edges
 *
 * Will erase trailing and starting characters, defined in str_remove,
 * from str
 *
 * Input : char *, string to be trimmed
 *         char *, string of character to remove
 * Output: none
 * Return: int, length of trimmed string
 */
size_t upsutl_str_remove_edges( char * const str, const char * const str_remove )
{
  char *cp = str;
  char *cstart = 0, *cend = 0;
  size_t count = 0;
  
  if ( !str || strlen( str ) <= 0 ) return 0;
  
  while ( *cp && strchr( str_remove, (int)*cp ) ){ cp++; }
  cstart = cp;
  count = strlen( str );
  cp = &str[count - 1];
  while ( count && strchr( str_remove, (int)*cp ) ){ cp--; count--; }
  cend = cp;

  if ( cend >= cstart && cstart >= str ) {
    count = (size_t)(cend-cstart) + 1;
    memmove( str, cstart, count );
    str[count] = 0;
  }
  else {
    str[0] = 0;
  }

  return strlen( str );
}

/*
 * Definition of private functions
 */

/* used by qsort */
int qsort_cmp_string( const void * c1, const void * c2 )
{
  return strcmp( (const char *)c1, (const char *)c2 );
}
