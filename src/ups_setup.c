/************************************************************************
 *
 * FILE:
 *       ups_setup.c
 * 
 * DESCRIPTION: 
 *       Setup the product
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
 *       24-Oct-1997, EB, first
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

static t_upslst_item *setup_core(const t_upsugo_command * const a_command_line,
				 const FILE * const a_temp_file,
				 const int a_ups_command);

/*
 * Definition of global variables.
 */

extern t_cmd_info g_cmd_info[];
/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * ups_setup
 *
 * Find the instance the user has requested, and process the setup actions.
 *
 * Input : command line information and an output stream
 * Output: none
 * Return: none
 */
void ups_setup( const t_upsugo_command * const a_command_line,
		const FILE * const a_temp_file, const int a_ups_command)
{
  t_upslst_item *mproduct_list = NULL;

  /* now find the desired instance and translate actions to the temp file */
  mproduct_list = setup_core(a_command_line, a_temp_file, a_ups_command);

  /* check if we got an error */
  if (UPS_ERROR == UPS_SUCCESS) {
    /* write statistics information */
    if (mproduct_list) {
      upsutl_statistics(mproduct_list, g_cmd_info[a_ups_command].cmd);
    }
  }
  /* clean up the matched product list */
  mproduct_list = upsutl_free_matched_product_list(&mproduct_list);
}

/*
 * Definition of private globals.
 */

/*
 * Definition of private functions.
 */

/*-----------------------------------------------------------------------
 * setup_core
 *
 * Cycle through the actions for unsetup possibly and setup, translate them,
 * and write them to the temp file.  This is done for all UPS product
 * requirements too.
 *
 * Input : information from the command line, a stream.
 * Output: <output>
 * Return: <return>
 */
static t_upslst_item *setup_core(const t_upsugo_command * const a_command_line,
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
					need_unique);
  if (mproduct_list && (UPS_ERROR == UPS_SUCCESS)) {
    /* get the product to be set up */
    mproduct = (t_upstyp_matched_product *)mproduct_list->data;

    /* make sure an instance was matched before proceeding */
    if (mproduct->minst_list) {
      minst = (t_upstyp_matched_instance *)(mproduct->minst_list->data);

      /* check if this product is authorized to be setup on this node */
      if (upsutl_is_authorized(minst, mproduct->db_info, dummy)) {
	/* Check if we need to unsetup this product first.  */
	if (a_command_line->ugo_k == 0) {
	  /* the command line says it is ok to do the unsetup */
	  cmd_list = upsact_get_cmd((t_upsugo_command *)a_command_line,
				    mproduct, g_cmd_info[e_unsetup].cmd);
	  if (UPS_ERROR == UPS_SUCCESS) {	  
	    /* Now output all the actions to the file */
	    upsact_process_commands(cmd_list, a_temp_file);
	  }
	  /* now clean up the memory that we used */
	  upsact_cleanup(cmd_list);
	}
	if (UPS_ERROR == UPS_SUCCESS) {	  
	  /* Now process the setup actions */
	  cmd_list = upsact_get_cmd((t_upsugo_command *)a_command_line,
				    mproduct, g_cmd_info[a_ups_command].cmd);
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
  } 

  return(mproduct_list);

}


