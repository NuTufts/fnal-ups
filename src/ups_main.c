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
#include <sys/types.h>
#include <sys/stat.h>

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
#include "ups_depend.h"
#include "ups_touch.h"
#include "upserr.h"
#include "upsutl.h"
#include "upshlp.h"
#include "upsget.h"
#include "upsact.h"
#include "upsver.h"
#include "ups_declare.h"
#include "ups_undeclare.h"
#include "ups_flavor.h"
#include "ups_get.h"
#include "ups_copy.h"
#include "ups_modify.h"

/*
 * Definition of public variables.
 */
extern int UPS_VERBOSE;
extern int g_keep_temp_file;
extern char *g_temp_file_name;
extern t_cmd_info g_cmd_info[];
int g_simulate;

/*
 * Declaration of private functions.
 */

/*
 * Definition of global variables.
 */
#ifndef NULL
#define NULL 0
#endif

#define KEEP_OR_REMOVE_FILE()   \
   if (! g_keep_temp_file) {                  \
     (void )remove(g_temp_file_name);         \
   } else {                                 \
     (void )printf("%s\n", g_temp_file_name); \
   }

/*
 * And now for something completely different
 */
int main(int argc, char *argv[])
{
  t_upsugo_command *command_line = NULL, temp_command_line;
  FILE *temp_file = NULL;
  int i = e_setup, empty_temp_file = 0;
  int rstatus = 0;              /* assume success */
  t_upslst_item *mproduct_list  = NULL;

  if (argv[1] && (strcmp(argv[1],"-?"))) {
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
      if (! g_keep_temp_file ) {
	g_keep_temp_file = command_line->ugo_V;
      }
      if (! g_simulate) {
	g_simulate = command_line->ugo_s;
      }


      if (!command_line->ugo_help && (g_cmd_info[i].cmd_index != e_help)) {
	/* no help requested - do the command */

	/* open the temp file. this is where shell specific action code will\
	   be put */
	if (! temp_file ) {                /* only open it once. */
	  /* let the system get me a buffer */
	  if ((g_temp_file_name = tmpnam(NULL)) != NULL) {
	    if ((temp_file = fopen(g_temp_file_name,"w")) == NULL) {
	      /* error in open */
	      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fopen",
			 strerror(errno));
	      upserr_add(UPS_OPEN_FILE, UPS_FATAL, g_temp_file_name);
	    }
	  } else {
	    upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "tmpnam", strerror(errno));
	  }
	}

	if (UPS_ERROR == UPS_SUCCESS) {
	  switch (g_cmd_info[i].cmd_index) {
	  case e_setup: mproduct_list = 
			  ups_setup(command_line, temp_file, e_setup);
	    break;
	  case e_unsetup: mproduct_list = 
			    ups_unsetup(command_line, temp_file, e_unsetup);
	    break;
	  case e_list: mproduct_list = ups_list(command_line,0);
	    break;
	  case e_verify: mproduct_list = 
			   ups_list(command_line,1); /* verify IS list !! */
	    break;
	  case e_configure: mproduct_list =
			   ups_configure(command_line, temp_file, e_configure);
	    break;
	  case e_copy: mproduct_list = 
			 ups_copy(command_line, temp_file, e_copy);
	    break;
	  case e_depend: mproduct_list = 
			   ups_depend(command_line, argv[1], e_depend);
	    break;
	  case e_exist: mproduct_list = 
			  ups_setup(command_line, temp_file, e_setup);
	    break;
	  case e_modify: mproduct_list = 
			   ups_modify(command_line, temp_file, e_modify);
	    break;
	  case e_start: mproduct_list = 
			  ups_start(command_line, temp_file, e_start);
	    break;
	  case e_stop: mproduct_list = 
			 ups_stop(command_line, temp_file, e_stop);
	    break;
	  case e_tailor: mproduct_list =
			   ups_tailor(command_line, temp_file, e_tailor);
	    break;
	  case e_unconfigure: mproduct_list =
				ups_unconfigure(command_line, temp_file,
						e_unconfigure);
	    break;
	  case e_undeclare: mproduct_list =
			   ups_undeclare(command_line, temp_file, e_undeclare);
	    break;
	  case e_flavor: ups_flavor(command_line);
	    break;
	  case e_create: ups_create(command_line, e_create);
	    break;
	  case e_get: mproduct_list = ups_get(command_line, temp_file, e_get);
	    break;
	  case e_declare: mproduct_list =
			    ups_declare(command_line, temp_file, e_declare);
	    break;
	  case e_touch: mproduct_list =
			    ups_touch(command_line, temp_file, e_touch);
	    break;
	  case e_unk: mproduct_list = 
			ups_unk(command_line, temp_file, argv[1]);
	    break;
	  }

	  /* write out statistics info */
	  if ((UPS_ERROR == UPS_SUCCESS) && mproduct_list) {
	    upsutl_statistics(mproduct_list, g_cmd_info[i].cmd);
	  }

	  if (mproduct_list) {
	    /* clean up the matched product list */
	    mproduct_list = upsutl_free_matched_product_list(&mproduct_list);
	  }
	}
      } else {
	/* output help */
	switch (g_cmd_info[i].cmd_index) {
	case e_help:
	case e_unk: (void )upshlp_command(NULL);       /* print out all help */
	  break;
	  /* specific help */
	default:    (void )upshlp_command(g_cmd_info[i].cmd);
	}
      }
      if (command_line->ugo_Z) {
	upsutl_stop_timing();
      }

      if (UPS_ERROR != UPS_SUCCESS) {
	rstatus = 1;                   /* return an error to the user */
	break;
      }

      /* we need to save the shell info here as the next call to upsugo_next
	 will free the structure we have now.  and we will need the shell info
	 later */
      temp_command_line.ugo_shell = command_line->ugo_shell;
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
      upsver_mes(1,"Empty temp file deleted %s\n",g_temp_file_name);
    } else {      
      /* write any closing info to the file */
      (void )upsutl_finish_temp_file(temp_file, &temp_command_line, "");
    }
    if (fclose(temp_file) == EOF) {
      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "fclose", strerror(errno));
    }
    /* if nothing was written to the file, delete it, */
    if (empty_temp_file) {
      (void )remove(g_temp_file_name);
      switch (g_cmd_info[i].cmd_index) {
      case e_setup: 
      case e_unsetup: 
	/* output the name of the a null file so the automatic sourcing does
	   not give an error */
	(void )printf("/dev/null\n");
	break;
      }
    } else {
      if (UPS_ERROR == UPS_SUCCESS ) {
	switch (g_cmd_info[i].cmd_index) {
	case e_setup: 
	case e_unsetup: 
	  /* see if this is only a simulation first */
	  if (g_simulate) {
	    /* yes, output this to short circuit the automatic sourcing */
	    printf("/dev/null\n");
	  }
	  /* output the name of the temp file that was created */
	  (void )printf("%s\n", g_temp_file_name);
	  
	  /* if we were asked to save the file, output the name again so the
	     user can see it. the first output was eaten by the sourcing */
	  if (g_keep_temp_file) {
	    (void )printf("%s\n", g_temp_file_name);
	  }
	  break;
	case e_exist:
	  /* just get rid of the file. (unless asked not to) we do not need it,
	     we just wanted to see if we could create it */
	  KEEP_OR_REMOVE_FILE();
	  break;
	default:
	  /* check to see if we are supposed to execute the file or just
	     report that it is there. */
	  if (g_simulate) {
	    /* simulation only, print the file name */
	    (void )printf("%s\n", g_temp_file_name);
	  } else {	
	    if (system(g_temp_file_name) <= 0) {
	      upserr_add(UPS_SYSTEM_ERROR, UPS_FATAL, "system",
			 strerror(errno));
	      KEEP_OR_REMOVE_FILE();
	    }
	  }
	  
	}
      } else {  /* there was an error while doing the command */
	switch (g_cmd_info[i].cmd_index) {
	case e_setup:
	case e_unsetup:
	  /* we must remove the file because otherwise the setup/unsetup 
	     command will try and source it and only half change the user's
	     environment */
	  (void )remove(g_temp_file_name);
	  /* print the following so automatic sourcing does'nt give an error */
	  printf("/dev/null\n");
	  break;
	default:
	  /* keep the file if we were asked to */
	  KEEP_OR_REMOVE_FILE();
	}
      }
    }
  }

  /* output any errors and the timing information */
  upserr_output();

  return rstatus;
}

