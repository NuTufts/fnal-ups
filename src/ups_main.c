/************************************************************************
 *
 * FILE:
 *       ups_main.c
 * 
 * DESCRIPTION: 
 *       This is the main line for ups commands.
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
 *       18-Aug-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */

/* ups specific include files */
#include "upslst.h"
#include "upstyp.h"
#include "ups_list.h"
#include "upserr.h"

/*
 * Definition of public variables.
 */
extern int UPS_VERBOSE;

/*
 * Declaration of private functions.
 */

/*
 * Definition of global variables.
 */

#ifndef NULL
#define NULL 0
#endif

/*
 * And now for something completely different
 */
int main(int argc, char *argv[])
{
  t_ups_command *command_line = NULL;
  char *list_valid_opts = "a?cdDf:g:h:K:lm:M:noq:r:tU:vVz:Z";

  /* Figure out which command was entered TBD */
  --argc;

  /* get the options for each iteration of the command and do it */
  while (command_line = upsugo_next(argc, &argv[1], (char *)list_valid_opts)) {
    if (command_line->ugo_v) {
      UPS_VERBOSE = 1;
    }
    ups_list(command_line);
    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_output();
      break;
    }
    break;          /* there is an error with upsugo_next */
  }

  return 0;
}
