/************************************************************************
 *
 * FILE:
 *       ups_unk.c
 * 
 * DESCRIPTION: 
 *       This is the unknown command handler.
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
 *       16-Oct-1997, EFB, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>

/* ups specific include files */
#include "ups_unk.h"

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
 * ups_unk
 *
 * The unknown command handler
 *
 * Input : command line information
 * Output: none
 * Return: none
 */
void ups_unk(const t_upsugo_command * const a_command_line,
	     const char * const a_unk_cmd, const int a_ups_command)
{

  printf("I'm sorry, I don't know what to do with the command \"%s\"\n",
	 a_unk_cmd);
}
