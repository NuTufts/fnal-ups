/************************************************************************
 *
 * FILE:
 *       ups_utils.h
 * 
 * DESCRIPTION: 
 *       Utility routine definitions
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

#ifndef _UPS_UTILS_H_
#define _UPS_UTILS_H_

/* standard include files, if needed for .h file */
#include <sys/types.h>

/* ups specific include files, if needed for .h file */
#include "ups_types.h"
#include "ups_list.h"

/*
 * Constans.
 */
#define DIVIDER  "#####################################################################"

/*
 * Types.
 */

/*
 * Declaration of public functions.
 */

char *upsutl_environment(const char * const a_env_var);
t_upslst_item *upsutl_free_inst_list( t_upslst_item **a_inst_list);
t_upslst_item *upsutl_get_files(const char * const a_dir,
				const char * const a_pattern);
void upsutl_start_timing(void);
void upsutl_stop_timing(void);
int upsutl_statistics(t_ups_instance const * const a_instance,
		      char const * const a_dir, char const * const a_command);
char *upsutl_time_date(void);
char *upsutl_user(void);

char  *upsutl_strstr( const char * const a_str, const char * const a_pattern);
char  *upsutl_str_create( char * const str, const char copt );
int    upsutl_str_sort( char * const, const char );
size_t upsutl_str_remove( char * const str, const char * const ct );
size_t upsutl_str_remove_edges( char * const str, const char * const ct );

/*
 * Declaration of private globals.
 */


/*
 * Declarations of public variables.
 */

#endif /* _UPS_UTILS_H_ */

