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
#include <stdlib.h>

/* ups specific include files */
#include "ups_main.h"
#include "upslst.h"
#include "upstyp.h"
#include "ups_setup.h"
#include "ups_unsetup.h"
#include "ups_start.h"
#include "ups_stop.h"
#include "ups_configure.h"
#include "ups_unconfigure.h"
#include "ups_list.h"
#include "ups_create.h"
#include "ups_tailor.h"
#include "ups_unk.h"
#include "upserr.h"
#include "upsutl.h"
#include "upshlp.h"
#include "upsget.h"

/*
 * Definition of public variables.
 */
extern int UPS_VERBOSE;
extern int g_LOCAL_VARS_DEF;

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
  {e_setup,       "setup",       "?B:cde:f:g:H:jkm:M:noO:q:r:tU:vVz:Z"},
  {e_unsetup,     "unsetup",     "?cde:f:g:H:jm:M:noO:q:tU:vVz:Z"},
  {e_list,        "list",        "a?cdDf:g:h:H:K:lm:M:noq:r:tU:vVz:Z"},
  {e_configure,   "configure",   "?cdf:g:H:m:M:noO:q:r:tU:vVz:Z"},
  {e_copy,        "copy",        "?A:cCdf:g:H:m:M:noO:p:P:q:r:tT:U:vVWXz:Z"},
  {e_declare,     "declare",     "?A:cCdf:g:H:m:M:noO:p:q:r:tT:U:vVz:Z"},
  {e_depend,      "depend",      "?f:H:K:m:M:q:r:U:vVz:Z"},
  {e_exist,       "exist",       "?B:cde:f:g:H:jkm:M:oO:q:r:tU:vVz:Z"},
  {e_modify,      "modify",      "?A:Ef:H:m:M:op:q:r:T:U:vVx:z:Z"},
  {e_start,       "start",       "?cdf:g:H:m:M:noO:q:r:tU:vVwz:Z"},
  {e_stop,        "stop",        "?cdf:g:H:m:M:noO:q:r:tU:vVz:Z"},
  {e_tailor,      "tailor",      "?cdf:g:h:H:K:m:M:noO:q:r:tU:vVz:Z"},
  {e_unconfigure, "unconfigure", "?cdf:g:H:m:M:noO:q:r:tU:vVz:Z"},
  {e_undeclare,   "undeclare",   "?cdf:g:H:m:M:noO:q:r:tU:vVyYz:Z"},
  {e_create,      "create",      "?f:H:m:M:p:q:vZ"},
  {e_get,         "get",         "?cdf:Fg:H:m:M:noq:r:tU:vVz:Z"},
  {e_validate,    "validate",    "?cdf:g:h:H:lm:M:nNoq:r:StU:vVz:Z"},
  /* the following one must always be at the end and contains all options */
  {e_unk,         NULL,
            "a?A:B:cCdDeEf:Fg:h:H:jkK:lm:M:nNoO:p:P:q:r:StT:uU:vVwW:x:XyYz:Z"}
};

/*
 * And now for something completely different
 */
int main(int argc, char *argv[])
{
  t_upsugo_command *command_line = NULL;
  FILE *temp_file = NULL;
  char *temp_file_name = NULL;
  int i = 0, empty_temp_file = 0, keep_temp_file = 0;
  int rstatus = 0;              /* assume success */

  if (argv[1]) {
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

      /* we will need this later after command_line goes away, so keep track
	 of it here. only attempt to save it if it has not already been
	 saved */
      if (! keep_temp_file ) {
	keep_temp_file = command_line->ugo_V;
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
	      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fopen",
			 strerror(errno));
	    }
	  } else {
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "tmpnam", strerror(errno));
	  }
	}

	if (UPS_ERROR == UPS_SUCCESS) {
	  switch (g_cmd_info[i].cmd_index) {
	  case e_setup: ups_setup(command_line, temp_file, e_setup);
	    break;
	  case e_unsetup: ups_unsetup(command_line, temp_file, e_unsetup);
	    break;
	  case e_list: ups_list(command_line);
	    break;
	  case e_configure: ups_configure(command_line, temp_file,
					  e_configure);
	    break;
	  case e_copy: ups_unk(command_line, argv[1], e_copy);
	    break;
	  case e_declare: ups_unk(command_line, argv[1], e_declare);
	    break;
	  case e_depend: ups_unk(command_line, argv[1], e_depend);
	    break;
	  case e_exist: ups_setup(command_line, temp_file, e_exist);
	    break;
	  case e_modify: ups_unk(command_line, argv[1], e_modify);
	    break;
	  case e_start: ups_start(command_line, temp_file, e_start);
	    break;
	  case e_stop: ups_stop(command_line, temp_file, e_stop);
	    break;
	  case e_tailor: ups_tailor(command_line, temp_file, e_tailor);
	    break;
	  case e_unconfigure: ups_unconfigure(command_line, temp_file,
					      e_unconfigure);
	    break;
	  case e_undeclare: ups_unk(command_line, argv[1], e_undeclare);
	    break;
	  case e_create: ups_create(command_line, e_create);
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
	case e_unk: (void )upshlp_command(NULL);       /* print out all help */
	  break;
	  /* specific help */
	default:    (void )upshlp_command(g_cmd_info[i].cmd);
	}
      }
      if (command_line->ugo_Z) {
	upsutl_stop_timing();
      }

      /* write any closing info to the file */
      if (g_LOCAL_VARS_DEF) {
	/* undefine the local env variables */
	(void )upsget_remall(temp_file, command_line);
      }

      if (UPS_ERROR != UPS_SUCCESS) {
	rstatus = 1;                   /* return an error to the user */
	break;
      }
    }
  } else {
    /* no parameters were entered - give help */
    (void )upshlp_command(NULL);       /* print out all help */
  }

  /* close the temp file */
  if (temp_file) {
    /* look and see where we are */
    if (ftell(temp_file) == 0L) {
      /* we are at the beginning of the file, nothing was written to it */
      empty_temp_file = 1;
    } else {      
      /* we usually tell the file to delete itself.  however the user may
	 override this */
      if (! keep_temp_file) {
	fprintf(temp_file, "/bin/rm -f %s\n", temp_file_name);
      }
    }
    if (fclose(temp_file) == EOF) {
      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fclose", strerror(errno));
    }
    /* if nothing was written to the file, delete it, */
    if (empty_temp_file) {
      (void )remove(temp_file_name);
    } else {
      if (UPS_ERROR == UPS_SUCCESS ) {
	switch (g_cmd_info[i].cmd_index) {
	case e_setup: 
	case e_unsetup: 
	  /* output the name of the temp file that was created */
	  (void )printf("%s\n", temp_file_name);
	  break;
	case e_exist:
	  /* just get rid of the file. (unless asked not to) we do not need it,
	     we just wanted to see if we could create it */
	  if (! keep_temp_file) {
	    (void )remove(temp_file_name);
	  } else {
	    (void )printf("%s\n", temp_file_name);
	  }
	  break;
	default:
	  /* source the file within the current process context */
	  (void )printf("(to be sourced) %s\n", temp_file_name); /* temp */
	  /*if (system(temp_file_name) <= 0) {
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "system",
		       strerror(errno));
	    }*/
	  
	}
      } else {  /* there was an error while doing the command */
	switch (g_cmd_info[i].cmd_index) {
	case e_setup:
	case e_unsetup:
	  /* we must remove the file because otherwise the setup/unsetup 
	     command will try and source it and only half change the user's
	     environment */
	  (void )remove(temp_file_name);
	  break;
	default:
	  /* keep the file if we were asked to */
	  if (! keep_temp_file) {
	    (void )remove(temp_file_name);
	  } else {
	    (void )printf("%s\n", temp_file_name);
	  }
	}
      }
    }
  }

  /* output any errors and the timing information */
  upserr_output();


  return rstatus;
}

