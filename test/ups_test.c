/* ups main test routine

Authors:        Margaret Votava
e-mail:         "votava@fnal.gov"
 
Include files:-
===============
*/
#include "stdlib.h"
#include "upstst_cmdtable.h"

/* Globals:-
   ========= */

#if 0
char	*upstst_basename=NULL;	/* basename entered on cmdline */
#endif

/* Prototypes:-
   ============ */

void upstst_commandloop (char *prompt, upstst_cmd_table_t *);

/* function prototypes for test routines 
   ------------------------------------- */

int upstst_date(int, char **);		int upstst_echo(int, char **);
int upstst_debug_level(int, char **);
int upstst_err_output(int, char **);	int upstst_err_clear(int, char**);
int upstst_err_add(int, char**);

/*=============================================================================
Routine:
   	main program for ups_test. It's a pretty simply program. just call 
	the command loop with the prompt and list of valid commands.
==============================================================================*/

int main(void)
{

/* build the list of commands that we support and their corresponding
   function pointers. 
   ------------------------------------------------------------------ */
upstst_cmd_table_t upstst_my_cmds[] = {
	"ups_date",		upstst_date,
	"ups_echo", 		upstst_echo,
	"ups_debug",		upstst_debug_level,
  	"upserr_output",	upstst_err_output,
  	"upserr_clear",		upstst_err_clear,
  	"upserr_add",		upstst_err_add,
	NULL,			0};

upstst_commandloop ("ups_test> ", upstst_my_cmds);
return(0);
}

