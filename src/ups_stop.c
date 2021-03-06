/************************************************************************
 *
 * FILE:
 *       ups_stop.c
 * 
 * DESCRIPTION: 
 *       Stop the product
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
 *       1-Dec-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>

/* ups specific include files */
#include "upserr.h"
#include "upsugo.h"
#include "upsutl.h"
#include "upsmat.h"
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

extern t_cmd_info g_cmd_info[];
/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * ups_stop
 *
 * Cycle through the actions for stop, translate them,
 * and write them to the temp file.  This is done for all UPS product
 * requirements too.
 *
 * Input : information from the command line, a stream.
 * Output: <output>
 * Return: <return>
 */
t_upslst_item *ups_stop(const t_upsugo_command * const a_command_line,
			const FILE * const a_temp_file,
			const int a_ups_command)
{
  t_upslst_item *mproduct_list = NULL;
  t_upstyp_matched_product *mproduct = NULL;
  t_upstyp_matched_instance *minst = NULL;
  t_upslst_item *cmd_list;
  char *dummy = NULL;
  int need_unique = 1;

  /* get all the requested instances */
  mproduct_list = upsmat_instance((t_upsugo_command *)a_command_line,
				  NULL, need_unique);
  if (mproduct_list && (UPS_ERROR == UPS_SUCCESS)) {
    /* get the product to be stopped */
    mproduct = (t_upstyp_matched_product *)mproduct_list->data;

    /* make sure an instance was matched before proceeding */
    if (mproduct->minst_list) {
      minst = (t_upstyp_matched_instance *)(mproduct->minst_list->data);

      /* check if this product is authorized to be stopped on this node */
      if (upsutl_is_authorized(minst, mproduct->db_info, &dummy)) {
	if (UPS_ERROR == UPS_SUCCESS) {	  
	  /* Now process the stop actions */
	  cmd_list = upsact_get_cmd((t_upsugo_command *)a_command_line,
				    mproduct, g_cmd_info[a_ups_command].cmd,
				    a_ups_command);
	  if (UPS_ERROR == UPS_SUCCESS) {
	    upsact_process_commands(cmd_list, a_temp_file);
	  }
	  /* now clean up the memory that we used */
	  upsact_cleanup(cmd_list);
	}
      } else {
	upserr_add(UPS_NOT_AUTH, UPS_FATAL, mproduct->product);
      }
    }
  } else {
    if (a_command_line->ugo_version && 
	(NOT_EQUAL_ANY_MATCH(a_command_line->ugo_version))) {
      upserr_add(UPS_NO_INSTANCE, UPS_INFORMATIONAL,
		 a_command_line->ugo_product,
		 (char *)a_command_line->ugo_qualifiers->data,
		 a_command_line->ugo_version, "version",
		 "(or may not exist)");
    } else if (a_command_line->ugo_chain) {
      upserr_add(UPS_NO_INSTANCE, UPS_INFORMATIONAL,
		 a_command_line->ugo_product,
		 (char *)a_command_line->ugo_qualifiers->data,
		 (char *)a_command_line->ugo_chain->data, "chain",
		 "(or may not exist)");
    }
  }

  return(mproduct_list);

}

/*
 * Definition of private globals.
 */

/*
 * Definition of private functions.
 */


