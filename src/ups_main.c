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
#include <string.h>

/* ups specific include files */
#include "upslst.h"
#include "upstyp.h"
#include "ups_list.h"
#include "ups_unk.h"
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

/* all individual command information */
typedef struct s_cmd_info {
  int cmd_index;
  char *cmd;
  char *valid_opts;
} t_cmd_info;

enum {
  e_setup,
  e_unsetup,
  e_list,
  e_configure,
  e_copy,
  e_declare,
  e_depend,
  e_exist,
  e_modify,
  e_start,
  e_stop,
  e_tailor,
  e_unconfigure,
  e_undeclare,
  e_create,
  e_get,
  e_validate,
  e_unk
};

t_cmd_info g_cmd_info[] = {
  {e_setup,       "setup",       "?B:cde:f:g:jkm:M:noO:q:r:tU:vVz:Z"},
  {e_unsetup,     "unsetup",     "?cde:f:g:jm:M:noO:q:tU:vVz:Z"},
  {e_list,        "list",        "a?cdDf:g:h:K:lm:M:noq:r:tU:vVz:Z"},
  {e_configure,   "configure",   "?cdf:g:m:M:noO:q:r:tU:vVz:Z"},
  {e_copy,        "copy",        "?A:cCdf:g:m:M:noO:p:P:q:r:tT:U:vVWXz:Z"},
  {e_declare,     "declare",     "?A:cCdf:g:m:M:noO:p:q:r:tT:U:vVz:Z"},
  {e_depend,      "depend",      "?f:K:m:M:q:r:U:vVz:Z"},
  {e_exist,       "exist",       "?B:cde:f:g:jkm:M:oO:q:r:tU:vVz:Z"},
  {e_modify,      "modify",      "?A:Ef:m:M:op:q:r:T:U:vVx:z:Z"},
  {e_start,       "start",       "?cdf:g:m:M:noO:q:r:tU:vVwz:Z"},
  {e_stop,        "stop",        "?cdf:g:m:M:noO:q:r:tU:vVz:Z"},
  {e_tailor,      "tailor",      "?cdf:g:K:m:M:noO:q:r:tU:vVz:Z"},
  {e_unconfigure, "unconfigure", "?cdf:g:m:M:noO:q:r:tU:vVz:Z"},
  {e_undeclare,   "undeclare",   "?cdf:g:m:M:noO:q:r:tU:vVyYz:Z"},
  {e_create,      "create",      "?f:m:M:p:q:vZ"},
  {e_get,         "get",         "?cdf:Fg:m:M:noq:r:tU:vVz:Z"},
  {e_validate,    "validate",    "?cdf:g:h:lm:M:nNoq:r:StU:vVz:Z"},
  /* the following one must always be at the end and contains all options */
  {e_unk,         NULL,
            "a?A:B:cCdDeEf:Fg:h:jkK:lm:M:nNoO:p:P:q:r:StT:uU:vVwW:x:XyYz:Z"}
};

/*
 * And now for something completely different
 */
int main(int argc, char *argv[])
{
  t_upsugo_command *command_line = NULL;
  int i = 0;

  /* Figure out which command was entered */
  while (g_cmd_info[i].cmd) {
    if (! strcmp(g_cmd_info[i].cmd, argv[1])) {
      break;
    }
    ++i;
  }

  /* skip the command name */
  --argc;

  /* get the options for each iteration of the command and do it */
  while (command_line = upsugo_next(argc, &argv[1],
				    g_cmd_info[i].valid_opts)) {

    switch (g_cmd_info[i].cmd_index) {
       case e_setup: ups_unk(command_line, argv[1]);
	 break;
       case e_unsetup: ups_unk(command_line, argv[1]);
	 break;
       case e_list: ups_list(command_line);
	 break;
       case e_configure: ups_unk(command_line, argv[1]);
	 break;
       case e_copy: ups_unk(command_line, argv[1]);
	 break;
       case e_declare: ups_unk(command_line, argv[1]);
	 break;
       case e_depend: ups_unk(command_line, argv[1]);
	 break;
       case e_exist: ups_unk(command_line, argv[1]);
	 break;
       case e_modify: ups_unk(command_line, argv[1]);
	 break;
       case e_start: ups_unk(command_line, argv[1]);
	 break;
       case e_stop: ups_unk(command_line, argv[1]);
	 break;
       case e_tailor: ups_unk(command_line, argv[1]);
	 break;
       case e_unconfigure: ups_unk(command_line, argv[1]);
	 break;
       case e_undeclare: ups_unk(command_line, argv[1]);
	 break;
       case e_create: ups_unk(command_line, argv[1]);
	 break;
       case e_get: ups_unk(command_line, argv[1]);
	 break;
       case e_validate: ups_unk(command_line, argv[1]);
	 break;
       case e_unk: ups_unk(command_line, argv[1]);
	 break;
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      upserr_output();
      break;
    }
  }

  return 0;
}
