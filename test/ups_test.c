/* ups main test routine

Authors:        Margaret Votava
e-mail:         "votava@fnal.gov"
 
Include files:-
===============
*/
#include "stdlib.h"
#include "upstst_cmdtable.h"
#include "ups.h"

/* Globals:-
   ========= */

extern t_upstyp_product *upstst_file;

/* Prototypes:-
   ============ */

void upstst_commandloop (char *prompt, upstst_cmd_table_t *);

/* function prototypes for test routines 
   ------------------------------------- */

int upstst_date(int, char **);		int upstst_echo(int, char **);
int upstst_debug_level(int, char **);
int upstst_err_output(int, char **);	int upstst_err_clear(int, char**);
int upstst_err_add(int, char**);
int upstst_fil_read_file(int, char**);  int upstst_fil_write_file(int, char**);
int upstst_ugo_env(int, char**);  	int upstst_ugo_next(int, char**);
int upstst_ugo_bldcmd(int, char**);
int upstst_mat_instance(int, char**); 	
int upstst_get_translation(int, char**);int upstst_get_allout(int, char**);
int upstst_list(int, char**);
int upstst_act_print(int, char**);	int upstst_act_process_commands(int, char**);

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
	"upsfil_read_file",	upstst_fil_read_file,
	"upsfil_write_file",	upstst_fil_write_file,
	"upsugo_env",		upstst_ugo_env,
	"upsugo_next",		upstst_ugo_next,
	"upsugo_bldcmd",	upstst_ugo_bldcmd,
	"upsmat_instance",	upstst_mat_instance,
	"upsget_translation",	upstst_get_translation,
	"upsget_allout",	upstst_get_allout,
	"ups_list",		upstst_list,
	"upsact_print",		upstst_act_print,
	"upsact_process_commands",upstst_act_process_commands,
	NULL,			0};

upstst_commandloop ("ups_test> ", upstst_my_cmds);
return(0);
}

