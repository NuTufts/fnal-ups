/************************************************************************
 *
 * FILE:
 *       ups_tailor.c
 * 
 * DESCRIPTION: 
 *       Tailor the product
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
#include <string.h>

/* ups specific include files */
#include "upserr.h"
#include "upsfil.h"
#include "upsugo.h"
#include "upsutl.h"
#include "upsmat.h"
#include "upsmem.h"
#include "upsact.h"
#include "ups_main.h"
/*
 * Definition of public variables.
 */


/*
 * Declaration of private functions.
 */

static t_upslst_item *tailor_core(
				const t_upsugo_command * const a_command_line,
				const FILE * const a_temp_file,
				const int a_ups_command);

/*
 * Definition of global variables.
 */

static t_upslst_item *g_mproduct_list = NULL;
extern t_cmd_info g_cmd_info[];
/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * ups_tailor
 *
 * Find the instance the user has requested, and process the tailor actions.
 *
 * Input : command line information and an output stream
 * Output: none
 * Return: none
 */
void ups_tailor( const t_upsugo_command * const a_command_line,
	       const FILE * const a_temp_file, const int a_ups_command)
{
  t_upslst_item *mproduct_list = NULL;

  /* now find the desired instance and translate actions to the temp file */
  mproduct_list = tailor_core(a_command_line, a_temp_file, a_ups_command);

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

/*-----------------------------------------------------------------------
 * ups_tailor_update_nodes
 *
 * Once the product has been successfully tailored, the version file must be
 * updated to reflect this fact.
 *
 * Input : none
 * Output: none
 * Return: none
 */
void ups_tailor_update_nodes(void)
{
  char buffer[MAX_LINE_LEN], *nodename, *new_nodes;
  t_upstyp_matched_product *mproduct;
  t_upstyp_matched_instance *minst;
  t_upstyp_product *read_product;
  t_upslst_item *inst_item;
  t_upstyp_instance *inst;
  int len = 0;

  /* we cannot do anything unless we have saved information from the call to
     ups_tailor */
  if (g_mproduct_list) {
    /* we must read in the version file again.  the output from instance
       matching only returned the instances from the file that matched what
       we wanted.  we need to get all the instances, alter the one we need
       to alter, and then write out the file again. */
    mproduct = (t_upstyp_matched_product *)g_mproduct_list->data;
    minst = (t_upstyp_matched_instance *)mproduct->minst_list->data;
    /* we only need to continue if we originally read in a version file */
    if (minst->version) {   
      /* the version file should be in db/prod/prod_version.version */
      sprintf(&buffer[0], "%s/%s/%s%s", mproduct->db_info->name,
	      mproduct->product, minst->version->version, VERSION_SUFFIX);
      read_product = upsfil_read_file(&buffer[0]);
      if ((UPS_ERROR == UPS_SUCCESS) && read_product) {
	/* locate the instance that matches the one we tailored */
	for (inst_item = read_product->instance_list ; inst_item ;
	     inst_item = inst_item->next) {
	  inst = (t_upstyp_instance *)inst_item->data;
	  if (! strcmp(inst->flavor, minst->version->flavor) &&
	      ! strcmp(inst->qualifiers, minst->version->qualifiers)) {
	    /* we found the matching instance, add the current node to the
	       list of authorized nodes */
	    nodename = upsutl_get_hostname();
	    if (inst->authorized_nodes) {
	      /* leave room for the separator */
	      len = (int)strlen(inst->authorized_nodes) + 1;
	    }
	    /* don't forget the trailing null */
	    len += (int)strlen(nodename) + 1;
	    new_nodes = (char *)upsmem_malloc(len);
	    if (inst->authorized_nodes) {
	      sprintf(new_nodes, "%s%s", inst->authorized_nodes,
		      UPS_SEPARATOR);
	    }
	    new_nodes = strcat(new_nodes,nodename);

	    /* free the old string */
	    upsmem_free(inst->authorized_nodes);
	    inst->authorized_nodes = new_nodes;

	    /* now rewrite the file out */
	    (void )upsfil_write_file(read_product, buffer);

	    /* no need to look through any more instances */
	    break;
	  }
	}
	ups_free_product(read_product);
      }
    }
  }
}
/*-----------------------------------------------------------------------
 * ups_tailor_cleanup
 *
 * Free any memory that was malloced in ups_tailor.  this cannot be done
 * at the end of ups_tailor because we need to keep the memory of which
 * instance was tailored so that after the temp file is sourced in the main
 * line (successfully), the version file can be updated.  only then can the
 * memory be freed.
 *
 * Input : none
 * Output: none
 * Return: none
 */
void ups_tailor_cleanup(void)
{
 /* clean up the matched product list */
  if (g_mproduct_list) {
    g_mproduct_list = upsutl_free_matched_product_list(&g_mproduct_list);
  }
}

/*
 * Definition of private globals.
 */

/*
 * Definition of private functions.
 */

/*-----------------------------------------------------------------------
 * tailor_core
 *
 * Cycle through the actions for tailor, translate them,
 * and write them to the temp file.  This is done for all UPS product
 * requirements too.
 *
 * Input : information from the command line, a stream.
 * Output: <output>
 * Return: <return>
 */
static t_upslst_item *tailor_core(
				 const t_upsugo_command * const a_command_line,
				 const FILE * const a_temp_file,
				 const int a_ups_command)
{
  t_upslst_item *mproduct_list = NULL;
  t_upstyp_matched_product *mproduct = NULL;
  t_upslst_item *cmd_list;
  int need_unique = 1;

  /* get all the requested instances */
  mproduct_list = upsmat_instance((t_upsugo_command *)a_command_line,
				  NULL, need_unique);
  if (mproduct_list && (UPS_ERROR == UPS_SUCCESS)) {
    /* get the product to be tailored */
    mproduct = (t_upstyp_matched_product *)mproduct_list->data;

    /* make sure an instance was matched before proceeding */
    if (mproduct->minst_list) {
      /* Now process the tailor actions */
      cmd_list = upsact_get_cmd((t_upsugo_command *)a_command_line,
				mproduct, g_cmd_info[a_ups_command].cmd);
      if (UPS_ERROR == UPS_SUCCESS) {
	upsact_process_commands(cmd_list, a_temp_file);
      }
      /* now clean up the memory that we used */
      upsact_cleanup(cmd_list);
    }
  } 

  return(mproduct_list);

}


