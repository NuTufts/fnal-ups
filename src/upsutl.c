/************************************************************************
 *
 * FILE:
 *       upsutl.c
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
 *       28-Jul-1997, EB, first.
 *       13-Aug-1997, LR, added string handling upsutl_str_*.
 *       26-Aug-1997, LR, fixed bug in str_sort.
 *                        added option 'p' to str_create.
 *                        added function str_crecat.
 *       11-Sep-1997, LR, added function str_stricmp, case insensitive
 *                        string comparison. It's a copy of strcasecmp.
 *       24-Sep-1997, LR, added function str_strincmp, like str_stricmp,
 *                        except it will compare most n characters.
 *       28-Oct-1997, EB, added function  upsutl_get_config
 *       29-Oct-1997, EB, added function  upsutl_is_authorized
 *
 ***********************************************************************/

/* standard include files */
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <netdb.h>        /* needed on SunOS to get MAXHOSTNAMELEN */
/* #ifdef _SYSTYPE_SVR4 */
#include <dirent.h>
/* #else
#include <sys/dir.h>
#endif */

/* ups specific include files */
#include "upsutl.h"
#include "upserr.h"
#include "upstyp.h"
#include "upsmem.h"
#include "upsfil.h"

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

#define CMP_AUTH_NODES(struct) \
    if (struct && struct->authorized_nodes) {                   \
      a_nodes = struct->authorized_nodes;                       \
      if (strcmp(struct->authorized_nodes, ANY_MATCH) &&        \
	  !strstr(struct->authorized_nodes, nodename)) {        \
	is_auth = 0;                                            \
      }                                                         \
    }


static clock_t g_start_cpu, g_finish_cpu;
static time_t g_start_w, g_finish_w;
static char g_stat_dir[] = "/statistics/";
static char g_unknown_user[] = "UNKNOWN";
static char g_buffer[FILENAME_MAX+1];
/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upsutl_find_manpages
 *
 * Return a string containing the location of the products' man pages
 *
 * Input : an instance, and db configuration info
 * Output: none
 * Return: static string containing the location of the products' man pages
 */
char *upsutl_find_manpages( const t_upstyp_matched_instance * const a_inst,
			    const t_upstyp_db * const a_db_info)
{
  t_upstyp_instance *vinst;

  g_buffer[0] = '\0';    /* we want to fill this anew */

  /* Check for a PROD_DIR_PREFIX first */
  if (a_db_info->config && a_db_info->config->prod_dir_prefix) {
    strcat(g_buffer, a_db_info->config->prod_dir_prefix);
    strcat(g_buffer, "/");
  }

  /* the information we need is in the version file match */
  if ((vinst = a_inst->version)) {
    /* see if we have a ups dir or not */
    if (vinst->ups_dir) {
      /* if UPS_DIR does not begin with a "/", use
	 PROD_DIR/UPS_DIR/toman */
      if (strcmp("/", (char *)&vinst->ups_dir[0])) {
	/* UPS_DIR does not begin with a "/" */
	if (vinst->prod_dir) {
	  strcat(g_buffer, vinst->prod_dir);
	  strcat(g_buffer, "/");
	}
      }
      strcat(g_buffer, vinst->ups_dir);
      strcat(g_buffer, "/toman/");
    } else {
      /* no UPS dir, use $PROD_DIR/ups/toman */
      if (vinst->prod_dir) {
	strcat(g_buffer, vinst->prod_dir);
	strcat(g_buffer, "/");
      }
      strcat(g_buffer, "ups/toman/");
    }
  }
  
  return ((char *)(&g_buffer[0]));
}
/*-----------------------------------------------------------------------
 * upsutl_get_hostname
 *
 * Get the current hostname. Remove any domain information.
 *
 * Input : none
 * Output: none
 * Return: pointer to a buffer with the hostname in it
 */
char *upsutl_get_hostname( void )
{
  char *tmp_buf;
  char nodename[MAXHOSTNAMELEN];

  /* get the hostname */
  if (! gethostname(nodename, MAXHOSTNAMELEN)) {
    /* If buffer is of the form node.fnal.gov, seek to first '.' and terminate
       the string. */
    tmp_buf = nodename;
    for (; *tmp_buf != '\0'; ++tmp_buf) { 
       if (*tmp_buf == '.')  { 
           *tmp_buf = '\0';
           break;
       }
    }
    tmp_buf = nodename;
  } else {
    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "gethostname", strerror(errno));
    tmp_buf = NULL;
  }
  return(tmp_buf);
}
/*-----------------------------------------------------------------------
 * upsutl_is_authorized
 *
 * Determine if the passed product instance is authorized for use on the
 * current node.  The authorization information is stored in either the 
 * dbconfig file info or in the product instance info.  
 *
 * Input : Matched instance structure, database configuration info and a 
 *         product name
 * Output: list of nodes on which the product is authorized
 * Return: 0 if not authorized, else 1.
 */
int upsutl_is_authorized( const t_upstyp_matched_instance * const a_minst,
			  const t_upstyp_db * const a_db_info, 
			  const char *a_nodes)
{
  int is_auth = 1;
  char *nodename = NULL;

  a_nodes = NULL;

  /* get the hostname */
  if ((nodename = upsutl_get_hostname())) {
    /* first check in the product instances to see if there is any
       authorization information there.  check in each file to make sure
       that if there is a specific list of nodes, that the current node is
       on it. in other words, assume the instance is authorized until told
       differently. */
    CMP_AUTH_NODES(a_minst->table);
    if (is_auth) {
      CMP_AUTH_NODES(a_minst->version);
    }
    if (is_auth) {
      CMP_AUTH_NODES(a_minst->chain);
    }
    
    if (is_auth) {
      if (a_db_info) {
	CMP_AUTH_NODES(a_db_info->config);
      }
    }

    /* if no authorized nodes were specified, then we assume all */
    if (! a_nodes) {
      a_nodes = ANY_MATCH;
    }
  } else {
    /* error when getting the node name */
    is_auth = 0;
  }
  return(is_auth);
}
/*-----------------------------------------------------------------------
 * upsutl_get_config
 *
 * Read the configuration file associated with the passed database
 *
 * Input : Name of the ups database
 * Output: none
 * Return: A product structure with the config info in it.
 */
t_upstyp_product *upsutl_get_config( const char * const a_db)
{
  t_upstyp_product *read_product = NULL;
  char *buffer = NULL;
  
  buffer = (char *)malloc(strlen(a_db) + (unsigned int )CONFIG_SIZE +
			  (unsigned int ) 4);
  if (buffer) {
    sprintf(buffer, "%s/../%s", a_db, CONFIG_FILE);
    read_product = upsfil_read_file(buffer);
    if (UPS_ERROR == UPS_NO_FILE) {
      upserr_backup();
      upserr_backup();
    }
  } else {
    upserr_add(UPS_NO_MEMORY, UPS_WARNING, strlen(buffer));
  }
  
  return(read_product);
}

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
 * upsutl_free_matched_product_list
 *
 * Free a matched product list.
 *
 * Input : pointer to matched product list
 * Output: none
 * Return: NULL
 */
t_upslst_item *upsutl_free_matched_product_list(
			            t_upslst_item ** const a_mproduct_list)
{
  t_upslst_item *mproduct_item;
  t_upstyp_matched_product *mproduct;


  /* make sure we are at the beginning of the list */
  *a_mproduct_list = upslst_first(*a_mproduct_list);

  for (mproduct_item = *a_mproduct_list ; mproduct_item ; 
       mproduct_item = mproduct_item->next) {
    mproduct = (t_upstyp_matched_product *)mproduct_item->data;
    (void )ups_free_matched_product(mproduct);
  }

  /* Now free the list */
  *a_mproduct_list = upslst_free(*a_mproduct_list, ' ');

  return NULL;
}
/*-----------------------------------------------------------------------
 * upsutl_free_matched_instance_list
 *
 * Given a matched instance list, free all the matched instances and the
 * list too.
 * 
 * Input : matched instance list pointer
 * Output: none
 * Return: Null
 */
t_upslst_item *upsutl_free_matched_instance_list(
					   t_upslst_item ** const a_minst_list)
{
  t_upslst_item *list_item = NULL;
  t_upstyp_matched_instance *minst_ptr = NULL;
  
  /* make sure we are at the beginning of the list */
  *a_minst_list = upslst_first(*a_minst_list);

  /* free the instances */
  for (list_item = *a_minst_list; list_item; list_item = list_item->next) {
    minst_ptr = (t_upstyp_matched_instance *)(list_item->data);
    if (minst_ptr->chain) {
      ups_free_instance(minst_ptr->chain);
    }
    if (minst_ptr->version) {
      ups_free_instance(minst_ptr->version);
    }
    if (minst_ptr->table) {
      ups_free_instance(minst_ptr->table);
    }
  }

  /* Now free the list */
  *a_minst_list = upslst_free(*a_minst_list, ' ');

  return NULL;
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
t_upslst_item *upsutl_free_inst_list( t_upslst_item ** const a_inst_list)
{
  t_upslst_item *list_item = NULL, *tmp_inst_list = NULL;
  
  /* make sure we are at the beginning of the list */
  *a_inst_list = upslst_first(*a_inst_list);

  /* free the instances */
  tmp_inst_list = *a_inst_list;
  for (list_item = tmp_inst_list; list_item; list_item = list_item->next) {
    ups_free_instance((t_upstyp_instance *)(list_item->data));
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

    /* this guy will free 'dir' */
    closedir(dir);

    /* point back to the beginning of the list */
    file_list = upslst_first(file_list);
  } else {
    upserr_add(UPS_OPEN_FILE, UPS_WARNING, a_dir);
  }
  return (file_list);
}

/*-----------------------------------------------------------------------
 * upsutl_get_prod_dir
 *
 * Given a database and a product name return a character string of the
 * product directory.
 * 
 * Input : Database and product name
 * Output: none
 * Return: product directory string
 */
char *upsutl_get_prod_dir(const char * const a_db,
			  const char * const a_prod_name)
{
  char *prod_dir = NULL;
  int prod_name_len;

  /* make sure a_db and a_prod_name are not both NULL */
  if (a_db || a_prod_name) {
    prod_name_len = (int )strlen(a_prod_name);
    prod_dir = (char *)upsmem_malloc((int )strlen(a_db) + prod_name_len + 2);
    if (a_db) {
      strcpy(prod_dir, a_db);       /* add the db directory */
    }

    /* only add the / divider if we have both a db and a product name */
    if (a_db && a_prod_name) {
      strncat(prod_dir, "/", 1);        /* and a / */
    }

    if (a_prod_name) {                            /* and the product name */
      strncat(prod_dir, a_prod_name, (unsigned int )prod_name_len);
    }
  }

  return(prod_dir);
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
 *         pointer to the command that was executed
 *         flag which indicates if we should write statistics for all
 *            instances no matter what
 * Output: none
 * Return: none
 */
void upsutl_statistics(t_upslst_item const * const a_mproduct_list,
		       char const * const a_command)
{ 
  
  t_upslst_item *minst_item = NULL, *mproduct_item = NULL;
  t_upstyp_matched_product *mproduct = NULL;
  t_upstyp_matched_instance *minst = NULL;
  char *tmp_stat;
  char stat_file[FILENAME_MAX+1];
  int dir_s, stat_s, file_s, global_yes = 0;
  FILE *file_stream = 0;
  char mode[] = "a,access=lock";              /* create new file or append
						 to existing one */
  char *time_date, *user;

  /* Get the current time and date */
  time_date = upsutl_time_date();

  /* Get the current user */
  user = upsutl_user();

  for (mproduct_item = (t_upslst_item *)a_mproduct_list ;
       mproduct_item ; mproduct_item = mproduct_item->next) {
    mproduct = (t_upstyp_matched_product *)mproduct_item->data;
    if ((mproduct->db_info->config) && 
	(tmp_stat = mproduct->db_info->config->statistics)) {
      if ((! strcmp(tmp_stat, ANY_MATCH)) ||
	  (strstr(tmp_stat, mproduct->product))) {
	global_yes = 1;      /* write statistics for everything in this db */
      }
    }
    for (minst_item = (t_upslst_item *)mproduct->minst_list ;
	 minst_item ; minst_item = minst_item->next) {
      minst = (t_upstyp_matched_instance *)minst_item->data;
      /* check to see if the instance indicates to keep statistics */
      if (global_yes || minst->chain->statistics ||
	                minst->version->statistics ||
	                minst->table->statistics) {
	if (! file_stream ) {       /* we need to still open the stream */
	  dir_s = (int )strlen(mproduct->db_info->name);
	  stat_s = (int )strlen(g_stat_dir);
	  file_s = (int )strlen(mproduct->product);
	  if ( (dir_s + stat_s + file_s + 3) < FILENAME_MAX) {
	    /* Construct the filename where the statistics are to be stored. */
	    strcpy(stat_file, mproduct->db_info->name);   /* directory */
	    strcat(stat_file, "/..");                     /* move up 1 level */
	    strcat(stat_file, g_stat_dir);                /* stats sub-dir */

	    /* check to make sure the statistics directory exists first */
	    if (upsutl_is_a_file(stat_file) == UPS_NO_FILE) {
	      /* no statistics directory, this is a warning error */
	      upserr_add(UPS_NO_FILE, UPS_WARNING, stat_file);
	    } else {

	      strcat(stat_file, mproduct->product);         /* filename */

	      /* See if we can open the file we are supposed to write to. */
	      if ((file_stream = fopen(stat_file, mode)) == NULL) {
		/* Error opening file */
		upserr_add(UPS_SYSTEM_ERROR, UPS_WARNING, "fopen",
			   strerror(errno));
	      }
	    }
	  } else {
	    /* Error size of directory path to file is too long */
	    upserr_add(UPS_NAME_TOO_LONG, UPS_WARNING, FILENAME_MAX);
	  }
	}
	if (file_stream) {
	  /* NOTE: time_date already has a carriage return in it */
	  if (fprintf(file_stream, "USER = %s\n   DATE = %s", user, time_date)
	      > 0) {
	    if (fprintf(file_stream, "   COMMAND = %s\n", a_command) > 0) {
	      if (fprintf(file_stream,
		    "   FLAVOR = %s\n   QUALIFIERS = '%s'\n   VERSION = %s\n", 
		    minst->version->flavor, minst->version->qualifiers,
		    minst->version->version) > 0) {
		/* Write out divider */
		if (fprintf(file_stream, "%s\n", DIVIDER) < 0) {
		  upserr_add(UPS_SYSTEM_ERROR, UPS_WARNING, "fprintf",
			     strerror(errno));
		  break;
		}
	      } else {
		upserr_add(UPS_SYSTEM_ERROR, UPS_WARNING, "fprintf",
			   strerror(errno));
		break;
	      }
	    } else {
	      upserr_add(UPS_SYSTEM_ERROR, UPS_WARNING, "fprintf",
			 strerror(errno));
	      break;
	    }
	  } else {
	    upserr_add(UPS_SYSTEM_ERROR, UPS_WARNING, "fprintf",
		       strerror(errno));
	    break;
	  } 
	} else {     /* error opening file stream already added to buffer */
	  break;
	}
      }
    }

    /* Close the file if it was opened */
    if (file_stream != NULL) {
      fclose(file_stream);
      file_stream = NULL;
    }
  }
}

/*
 * This array is designed for mapping upper and lower case letter
 * together for a case independent comparison. The mappings are
 * based upon ascii character sequences.
 * Used by upsutl_stricmp and upsutl_strincmp.
 */
static unsigned char stricmp_charmap[] = {
      '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
      '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
      '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
      '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
      '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
      '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
      '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
      '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
      '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
      '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
      '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
      '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
      '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
      '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
      '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
      '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
      '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
      '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
      '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
      '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
      '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
      '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
      '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
      '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
      '\300', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
      '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
      '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
      '\370', '\371', '\372', '\333', '\334', '\335', '\336', '\337',
      '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
      '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
      '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
      '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

/*-----------------------------------------------------------------------
 * upsutl_stricmp
 *
 * A case insencitive version of strcmp. It's a directly copy of
 * strcasecmp (found e.g in gnu's libc), which is normaly not
 * supported in ANSI C.
 *
 * Input : char *, first string to compare.
 *         char *, second string to compare.
 * Output: none
 * Return: as strcmp.
 */
int upsutl_stricmp( const char *s1, const char *s2 )
{
  register unsigned char u1, u2;

  for (;;) {
    u1 = (unsigned char) *s1++;
    u2 = (unsigned char) *s2++;
    if (stricmp_charmap[u1] != stricmp_charmap[u2]) {
      return stricmp_charmap[u1] - stricmp_charmap[u2];
    }
    if (u1 == '\0') {
      return 0;
    }
  }
}

/*-----------------------------------------------------------------------
 * upsutl_strincmp
 *
 * A case insencitive version of strncmp. It's based on 
 * strcasecmp (found e.g in gnu's libc), which is normaly not
 * supported in ANSI C.
 *
 * Input : char *, first string to compare.
 *         char *, second string to compare.
 *         size_t, number of characters to compare.
 * Output: none
 * Return: as strcmp.
 */
int upsutl_strincmp( const char *s1, const char *s2, const size_t n )
{
  register unsigned char u1, u2;
  register int i = 0;

  for (;;) {
    u1 = (unsigned char) *s1++;
    u2 = (unsigned char) *s2++;
    if (stricmp_charmap[u1] != stricmp_charmap[u2]) {
      return stricmp_charmap[u1] - stricmp_charmap[u2];
    }
    if (++i == (int )n || u1 == '\0') {
      return 0;
    }
  }
}

/*-----------------------------------------------------------------------
 * upsutl_stricmp
 *
 * A case insensitive version of strcmp. It's a directly copy of
 * strcasecmp (found e.g in gnu's libc), which is normaly not
 * supported in ANSI C.
 *
 * Input : char *, string to upcase.
 * Output: none
 * Return: upcased string.
 */
char *upsutl_upcase(const char * const a_str)
{
  static char dum_str[MAX_LINE_LEN];
  int i, len = (int )strlen(a_str);

  for (i = 0 ; i < len ; ++i ) {
    dum_str[i] = (char )toupper((int )a_str[i]);
  }
  
  /* NULL terminate the string */
  dum_str[i] = '\0';
  
  return (char *)(&dum_str[0]);
  
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
char *upsutl_strstr( const char * const a_str, const char * const a_pattern )
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
 *         char, options, STR_TRIM will trim edges of passed string.
 *             ,          STR_TRIM_PACK will trim full string (packing).
 *             ,          STR_TRIM_DEFAULT will do nothing.
 * Output: none
 * Return: char *, new string.
 */
char *upsutl_str_create( char * const str, const char copt )
{
  char *new_str = 0;
  
  if ( !str ) return 0;

  if ( copt == STR_TRIM || copt == STR_TRIM_PACK ) {
    
    /* copy overhead only when option 't' or 'p' is passed */

    static char bad_chars[] = " \t\n\r\f\"";
    static char buf[MAX_LINE_LEN];
    if ( strlen( str ) >= MAX_LINE_LEN ) {    
      upserr_add( UPS_LINE_TOO_LONG, UPS_FATAL, "upsutl_str_create" );
      return 0;
    }    
    strcpy( buf, str );
    if ( copt == STR_TRIM_PACK )
      upsutl_str_remove( buf, bad_chars );
    else
      upsutl_str_remove_edges( buf, bad_chars );
    
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
 * upsutl_str_crecat
 *
 * It will concatenate the two passed string into a new created string.
 *
 * Input : char *, string to be copied.
 *         char *, string to be concatenate.
 * Output: none
 * Return: char *, new string.
 */
char *upsutl_str_crecat( char * const str1, char * const str2 )
{
  char *new_str = 0;
  
  if ( !str1 ) return 0;

  if ( !str2 ) {
    new_str = upsutl_str_create( str1, ' ' );
  }
  else {
    new_str = (char *)malloc( strlen( str1 ) + strlen( str2 ) +
			      (unsigned int )1 );
    strcpy( new_str, str1 );
    strcat( new_str, str2 );
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
 * Output: char *, sorted string
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
  if ( (i=strlen( cp0 )) > max_len ) max_len = i;
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

/*-----------------------------------------------------------------------
 * upsutl_is_a_file
 *
 * Given a filename including path, see if the named file exists.
 *
 * Input : file name and path
 * Output: none
 * Return: UPS_SUCCESS if file exists, else UPS_NO_FILE
 */
int upsutl_is_a_file(const char * const a_filename)
{
  int status = UPS_SUCCESS;
  struct stat buf;

  if (stat(a_filename, &buf) == -1) {
    status = UPS_NO_FILE;
  }

  return(status);
  
}

/*
 * Definition of private functions
 */

/* used by qsort */
int qsort_cmp_string( const void * c1, const void * c2 )
{
  return strcmp( (const char *)c1, (const char *)c2 );
}





