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

/*
 * Definition of public variables.
 */


/*
 * Declaration of private functions.
 */

static t_upslst_item *setup_core(const t_upsugo_command * const a_command_line,
				 int a_first_pass,
				 const FILE * const a_temp_file);

/*
 * Definition of global variables.
 */


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
		const FILE * const a_temp_file)
{
  t_upslst_item *mproduct_list = NULL;
  int first_pass = 1;

  /* now find the desired instance and translate actions to the temp file */
  mproduct_list = setup_core(a_command_line, first_pass, a_temp_file);

  /* check if we got an error */
  if (UPS_ERROR == UPS_SUCCESS) {
    /* write the closing information to the temp file */

    /* write statistics information */
    if (mproduct_list) {
      upsutl_statistics(mproduct_list, "setup");
    }
  }

}

/*
 * Definition of private globals.
 */

/*-----------------------------------------------------------------------
 * setup_core
 *
 * Cycle through the actions for unsetup possibly and setup, translate them,
 * and write them to the temp file.  This is done for all UPS product
 * requirements too.
 *
 * Input : information from the command line, a flag indicating if this is
 *         the first time through this routine, a stream.
 * Output: <output>
 * Return: <return>
 */
static t_upslst_item *setup_core(const t_upsugo_command * const a_command_line,
				 int a_first_pass,
				 const FILE * const a_temp_file)
{
  t_upslst_item *mproduct_list = NULL;
  t_upstyp_matched_product *mproduct = NULL;
  t_upstyp_matched_instance *minst = NULL;
  char *dummy = NULL;
  int need_unique = 1;
  int not_first_pass = 0;

  /* get all the requested instances */
  mproduct_list = upsmat_match_instance((t_upsugo_command *)a_command_line,
					need_unique);
  if (mproduct_list && (UPS_ERROR == UPS_SUCCESS)) {
    /* get the product to be set up */
    mproduct = (t_upstyp_matched_product *)mproduct_list->data;
    minst = (t_upstyp_matched_instance *)(mproduct->minst_list->data);

    /* check if this product is authorized to be setup on this node */
    if (upsutl_is_authorized(minst, mproduct->db_info, dummy)) {
      /* Check if we need to unsetup this product first.  There are two
	 


      /* We need to keep a list of the dependencies which involve setting up
	 UPS managed products.  This is only done if this is the top level
	 product, i.e. the product referenced on the command line.  This is
	 because any product in the top level list of dependencies takes
	 precedence over lower level dependencies even when they refer to
	 different instances. */
      if (a_first_pass) {
	/* This is the top level. Look thru all of the dependencies for
	   setup and save a list of those which are UPS product setups. */
	
	
      }

        printf("we got to here\n");      
    } else {
      upserr_add(UPS_NOT_AUTH, UPS_FATAL, mproduct->product);
    }
  } 

  return(mproduct_list);

}

/*
 * Definition of private functions.
 */


