/************************************************************************
 *
 * FILE:
 *       ups_utils.c
 * 
 * DESCRIPTION: 
 *       Test some of the utility routines.
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
 *       29-Jul-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */

/* ups specific include files */
#include "ups_utils.h"
#include "ups_error.h"
#include "ups_match.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */

/*
 * Definition of global variables.
 */

/*
 * Definition of public functions.
 */

int main (void)
{
  /* *************************************************************************
     test the writing of the statistics file
     ************************************************************************/

  /* First set up the stuff we need - an instance, a database and a command.
     The database, product combination better be someplace writeable */

  char db[] = ".";
  t_ups_instance instance;
  char command[] = "ups -c -f global thermonuclear_war";
  t_upslst_item *list_ptr;

  instance.product = "middleEast";
  instance.version = "v1_0";
  instance.qualifiers = "debug, build";
  instance.flavor = "global";

  if (upsutl_statistics(&instance, db, command) != UPS_SUCCESS) {
    upserr_output();
  }

  /* Now append to the file */
  if (upsutl_statistics(&instance, db, command) != UPS_SUCCESS) {
    upserr_output();
  }

  /* Now write a new file */
  instance.product = "WarGames";
  if (upsutl_statistics(&instance, db, command) != UPS_SUCCESS) {
    upserr_output();
  }

  /* Now test the function upsutl_get_files */
  list_ptr = upsutl_get_files("/usrdevel/s1/berman/upsdb/tigger", 
			      (char *)ANY_MATCH);

  return 0;
}
