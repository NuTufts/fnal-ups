/************************************************************************
 *
 * FILE:
 *       ups_list.c
 * 
 * DESCRIPTION: 
 *       This is the 'ups list' command.
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
 *       14-Aug-1997, EB, first
 *       Sep 19 1997, DjF, Taking over...
 *
 ***********************************************************************/

/* standard include files */
#include <string.h>
#include <stdio.h>

/* ups specific include files */
#include "upslst.h"
#include "upstyp.h"
#include "upsmem.h"
#include "upstyp.h"
#include "upserr.h"
#include "upsutl.h"
#include "upsmat.h"
#include "upsfil.h"
#include "ups_list.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */

t_upslst_item *ups_list_core(t_upsugo_command * const a_command_line);
void list_output(const t_upslst_item * const a_mproduct_list,
		 const t_upsugo_command * const a_command_line);

/*
 * Definition of global variables.
 */
#define VPREFIX "UPSLIST: "

#ifndef NULL
#define NULL 0
#endif

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * ups_list
 *
 * This is the main line for the 'ups list' command.
 *
 * Input : argc, argv
 * Output: 
 * Return: 
 */
void ups_list( t_upsugo_command * const a_command_line )
{
  t_upslst_item *mproduct_list = NULL;
  t_upslst_item *tmp_mprod_list = NULL;
  t_upstyp_matched_product *mproduct = NULL;

  /* Get all the requested instances */
  mproduct_list = ups_list_core(a_command_line);
  /*  upsugo_prtlst(mproduct_list,"the products");*/
  mproduct_list = upslst_first(mproduct_list);  /* point to the start */

  /* Output the requested information from the instances */
  /*  upsugo_dump(a_command_line);*/
  list_output(mproduct_list, a_command_line);

  /* free the matched products */
  for (tmp_mprod_list = mproduct_list ; tmp_mprod_list ; 
       tmp_mprod_list = tmp_mprod_list->next) {
    mproduct = (t_upstyp_matched_product *)tmp_mprod_list->data;
    ups_free_matched_product(mproduct);      /* free the data */
  }
  /* now free the list */
  tmp_mprod_list = upslst_free(tmp_mprod_list, ' ');

}

/*-----------------------------------------------------------------------
 * ups_list_core
 *
 * Take the command line parameters and read in all of the desired instances
 *
 * Input : <input>
 * Output: <output>
 * Return: <return>
 */
t_upslst_item *ups_list_core(t_upsugo_command * const a_command_line)
{
  t_upslst_item *mproduct_list = NULL;
  int need_unique = 0;


  /* if no chains were entered, ask for them all */
  if (! a_command_line->ugo_chain) { 
    a_command_line->ugo_chain = upslst_new((void *)ANY_MATCH);
  }

  /* Get all the instances that the user requested */
  mproduct_list = upsmat_match_instance(a_command_line, need_unique);

  return(mproduct_list);
}

/*-----------------------------------------------------------------------
 * list_output
 *
 * Output all of the desired information from the matched instances TBD
 *
 * Input : <input>
 * Output: <output>
 * Return: <return>
 */
void list_output(const t_upslst_item * const a_mproduct_list,
		 const t_upsugo_command * const a_command_line)
{
  t_upstyp_matched_product *mproduct = NULL;
  t_upslst_item *tmp_mprod_list = NULL;
  t_upslst_item *tmp_minst_list = NULL, *clist = NULL;
  t_upstyp_instance *cinst_ptr = NULL;
  t_upstyp_matched_instance *minst_ptr = NULL;

  for (tmp_mprod_list = (t_upslst_item *)a_mproduct_list ; tmp_mprod_list ;
       tmp_mprod_list = tmp_mprod_list->next) {
    mproduct = (t_upstyp_matched_product *)tmp_mprod_list->data;
    for (tmp_minst_list = mproduct->minst_list ; tmp_minst_list ;
	 tmp_minst_list = tmp_minst_list->next) {
      minst_ptr = (t_upstyp_matched_instance *)(tmp_minst_list->data);
      if (minst_ptr->chain) {
	printf("C:PRODUCT=%s, CHAIN=%s, VERSION=%s, ",
	       minst_ptr->chain->product,
	       minst_ptr->chain->chain, minst_ptr->chain->version);
	printf("FLAVOR=%s, QUALIFIERS=%s\n", minst_ptr->chain->flavor,
	       minst_ptr->chain->qualifiers);
      }
      if (minst_ptr->xtra_chains) {
	for (clist = minst_ptr->xtra_chains ; clist ; clist = clist->next) {
	  cinst_ptr = (t_upstyp_instance *)clist->data;
	  printf("C:PRODUCT=%s, CHAIN=%s, VERSION=%s, ",
		 cinst_ptr->product, cinst_ptr->chain, cinst_ptr->version);
	  printf("FLAVOR=%s, QUALIFIERS=%s\n", cinst_ptr->flavor,
		 cinst_ptr->qualifiers);
	}
      }
      if (minst_ptr->version ) {
	printf("V:PRODUCT=%s, VERSION=%s, ", minst_ptr->version->product,
	       minst_ptr->version->version);
	printf("FLAVOR=%s, QUALIFIERS=%s\n", minst_ptr->version->flavor,
	       minst_ptr->version->qualifiers);
      }
      if (minst_ptr->table) {
	printf("T:PRODUCT=%s, ", minst_ptr->table->product);
	printf("FLAVOR=%s, QUALIFIERS=%s\n", minst_ptr->table->flavor,
	       minst_ptr->table->qualifiers);
      }
    }
    printf("\n\n");
  }
}

