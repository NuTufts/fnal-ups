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
#include <stdio.h>
#include <errno.h>

/* ups specific include files */
#include "ups_main.h"
#include "upslst.h"
#include "upstyp.h"
#include "ups_setup.h"
#include "ups_list.h"
#include "ups_unk.h"
#include "upserr.h"
#include "upsutl.h"
#include "upshlp.h"

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

/* The enum is defined in ups_main.h */
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
  FILE *temp_file = NULL;
  char *temp_file_name = NULL;
  int i = 0, empty_temp_file = 0;

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
  while ((command_line = upsugo_next(argc, &argv[1],
				    g_cmd_info[i].valid_opts))) {

    if (command_line->ugo_Z) {
      upsutl_start_timing();
    }

    if (!command_line->ugo_help) {
      /* no help requested - do the command */

      /* open the temp file. this is where shell specific action code will\
         be put */
      if (! temp_file ) {                /* only open it once. */
	/* let the system get me a buffer */
	if ((temp_file_name = tmpnam(NULL)) != NULL) {
	  if ((temp_file = fopen(temp_file_name,"w")) == NULL) {
	    /* error in open */
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fopen", strerror(errno));
	  }
	} else {
	  upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "tmpnam", strerror(errno));
	}
      }

      if (UPS_ERROR == UPS_SUCCESS) {
	switch (g_cmd_info[i].cmd_index) {
	case e_setup: ups_setup(command_line, temp_file, e_setup);
	  break;
	case e_unsetup: ups_unk(command_line, argv[1], e_unsetup);
	  break;
	case e_list: ups_list(command_line);
	  break;
	case e_configure: ups_unk(command_line, argv[1], e_configure);
	  break;
	case e_copy: ups_unk(command_line, argv[1], e_copy);
	  break;
	case e_declare: ups_unk(command_line, argv[1], e_declare);
	  break;
	case e_depend: ups_unk(command_line, argv[1], e_depend);
	  break;
	case e_exist: ups_unk(command_line, argv[1], e_exist);
	  break;
	case e_modify: ups_unk(command_line, argv[1], e_modify);
	  break;
	case e_start: ups_unk(command_line, argv[1], e_start);
	  break;
	case e_stop: ups_unk(command_line, argv[1], e_stop);
	  break;
	case e_tailor: ups_unk(command_line, argv[1], e_tailor);
	  break;
	case e_unconfigure: ups_unk(command_line, argv[1], e_unconfigure);
	  break;
	case e_undeclare: ups_unk(command_line, argv[1], e_undeclare);
	  break;
	case e_create: ups_unk(command_line, argv[1], e_create);
	  break;
	case e_get: ups_unk(command_line, argv[1], e_get);
	  break;
	case e_validate: ups_unk(command_line, argv[1], e_validate);
	  break;
	case e_unk: ups_unk(command_line, argv[1], e_unk);
	  break;
	}
      }
    } else {
      /* output help */
      switch (g_cmd_info[i].cmd_index) {
      case e_unk: (void )upshlp_command(NULL);         /* print out all help */
	break;
      default:    (void )upshlp_command(g_cmd_info[i].cmd); /* specific help */
      }
    }
    if (command_line->ugo_Z) {
      upsutl_stop_timing();
    }

    if (UPS_ERROR != UPS_SUCCESS) {
      break;
    }
  }

  /* output any errors and the timing information */
  upserr_output();

  /* close the temp file */
  if (temp_file) {
    /* look and see where we are */
    if (ftell(temp_file) == 0L) {
      /* we are at the beginning of the file, nothing was written to it */
     empty_temp_file = 1;
    }
    if (fclose(temp_file) == EOF) {
      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fclose", strerror(errno));
    }
    /* if nothing was written to the file, delete it, */
    if (empty_temp_file) {
      (void )remove(temp_file_name);
    } else {
      /* output the name of the temp file that was created */
      (void )printf("%s\n", temp_file_name);
    }
  }

  return 0;
}

