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
static void check_for_unsetup ( const t_upslst_item * const a_cmd_list,
				const FILE * const a_stream);

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
  t_upslst_item *mproduct_list = NULL, *new_mproduct_list = NULL;
  t_upstyp_matched_product *mproduct = NULL, *new_mproduct = NULL;
  t_upstyp_matched_instance *minst = NULL;
  t_upslst_item *cmd_list = NULL;
  t_upsugo_command *new_command_line;
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
	  /* the command line says it is ok to do the unsetup.  we must
	     get the value of the env variable SETUP_<prodname> and use
	     it to locate the instance that was previously set up.  if
	     this variable does not exist, we cannot do the unsetup.  then
	     we must walk through the cmd_list for the current instances'
	     unsetup actions and check each UPS product dependency to see
	     if a SETUP_<dep> exists and then do the unsetup */
	  if (new_command_line = upsugo_env(mproduct->product,
					   g_cmd_info[e_unsetup].valid_opts)) {
	    /* we found the SETUP_<PROD> so get a new instance based on the
	       env variable. */
	    new_mproduct_list = upsmat_instance(new_command_line, need_unique);
	    if (new_mproduct_list && (UPS_ERROR == UPS_SUCCESS)) {
	      /* get the product to be set up */
	      new_mproduct =
		(t_upstyp_matched_product *)new_mproduct_list->data;
	      /* make sure an instance was matched before proceeding */
	      if (new_mproduct->minst_list) {
		cmd_list = upsact_get_cmd(new_command_line, new_mproduct,
					  g_cmd_info[e_unsetup].cmd);
	      }
	    }
	  } else {
	    /* There was no env variable.  so use the current instance to
	       locate the unsetup actions. */
	    cmd_list = upsact_get_cmd((t_upsugo_command * )a_command_line, 
				      mproduct, g_cmd_info[e_unsetup].cmd);
	  }
	  if (cmd_list && (UPS_ERROR == UPS_SUCCESS)) {
	    /* Now walk thru all the actions.  for each UPS product
	       encountered, check if a SETUP_<product> exists.  if it does
	       output all the unsetup actions to the file. */
	    check_for_unsetup(cmd_list, a_temp_file);
	  }
	  /* now clean up the memory that we used */
	  upsact_cleanup(cmd_list);
	  new_mproduct_list = upsutl_free_matched_product_list(
							   &new_mproduct_list);
	}

	if (UPS_ERROR != UPS_SUCCESS) {	  
	  upserr_add(UPS_UNSETUP_FAILED, UPS_WARNING, mproduct->product);
	}
	/* Now process the setup actions */
	cmd_list = upsact_get_cmd((t_upsugo_command *)a_command_line,
				  mproduct, g_cmd_info[a_ups_command].cmd);
	if (UPS_ERROR == UPS_SUCCESS) {
	  upsact_process_commands(cmd_list, a_temp_file);
	}
	/* now clean up the memory that we used */
	upsact_cleanup(cmd_list);

      } else {
	upserr_add(UPS_NOT_AUTH, UPS_FATAL, mproduct->product);
      }
    }
  } 

  return(mproduct_list);

}

/*-----------------------------------------------------------------------
 * check_for_unsetup
 *
 * Walk through the list of commands.  for each new product that is
 * encountered, see if the environment variable SETUP_<product> exists.  if
 * it does, , translate the variable, find the appropriate instance and
 * write the unsetup commands to the stream.
 *
 * Input : a list of commands(t_upsact_item), an output stream
 * Output: none
 * Return: none
 */
static void check_for_unsetup ( const t_upslst_item * const a_cmd_list,
				const FILE * const a_stream)
{


}
