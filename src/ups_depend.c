/************************************************************************
 *
 * FILE:
 *       ups_depend.c
 * 
 * DESCRIPTION: 
 *       To list product dependencies
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
 *       17-Dec-1997, LR, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>

/* ups specific include files */
#include "ups_depend.h"
#include "upserr.h"
#include "upsugo.h"
#include "upsact.h"
#include "ups_main.h"
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

/*-----------------------------------------------------------------------
 * ups_depend
 *
 * To list product dependencies
 *
 * Input : command line structure,
 *         ups command,
 *         enum of ups command,
 * Output: none
 * Return: none
 */
void ups_depend( t_upsugo_command * const u_cmd, 
		 const char * const s_cmd,
		 const int e_cmd )
{
  if ( u_cmd->ugo_v > 1 )
    upsact_print( u_cmd, 0, "setup", e_cmd, "tal" );
  else if ( u_cmd->ugo_v > 0 )
    upsact_print( u_cmd, 0, "setup", e_cmd, "tl" );
  else
    upsact_print( u_cmd, 0, "setup", e_cmd, "" );
}

/*
 * Definition of private functions.
 */

