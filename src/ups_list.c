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
  t_upstyp_match_product *mproduct = NULL;

  /* Get all the requested instances */
  mproduct_list = ups_list_core(a_command_line);
  upsugo_prtlst(mproduct_list,"the products");
  mproduct_list = upslst_first(mproduct_list);  /* point to the start */

  /* Output the requested information from the instances */
  upsugo_dump(a_command_line);
  list_output(mproduct_list, a_command_line);

  /* free the matched products */
  for (tmp_mprod_list = mproduct_list ; tmp_mprod_list ; 
       tmp_mprod_list = tmp_mprod_list->next) {
    mproduct = (t_upstyp_match_product *)tmp_mprod_list->data;
    ups_free_mp(mproduct);      /* free the data */
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
  t_upstyp_match_product *mproduct = NULL;
  t_upslst_item *tmp_mprod_list = NULL;
  t_upslst_item *tmp_chain_list = NULL;
  t_upslst_item *tmp_vers_list = NULL;
  t_upslst_item *tmp_table_list = NULL;
  t_upstyp_instance *instance = NULL;

  for (tmp_mprod_list = (t_upslst_item *)a_mproduct_list ; tmp_mprod_list ;
       tmp_mprod_list = tmp_mprod_list->next) {
    mproduct = (t_upstyp_match_product *)tmp_mprod_list->data;
    if (a_command_line->ugo_chain) {
       for (tmp_chain_list = mproduct->chain_list ; tmp_chain_list ;
            tmp_chain_list = tmp_chain_list->next) 
       { instance = (t_upstyp_instance *)tmp_chain_list->data;
         printf("C:PRODUCT=%s, CHAIN=%s, VERSION=%s, ", instance->product,
                 instance->chain, instance->version);
         printf("FLAVOR=%s, QUALIFIERS=%s\n", instance->flavor,
                 instance->qualifiers);
       }
    } else { 
       for (tmp_vers_list = mproduct->version_list ; tmp_vers_list ;
            tmp_vers_list = tmp_vers_list->next) 
       { instance = (t_upstyp_instance *)tmp_vers_list->data;
         printf("V:PRODUCT=%s, VERSION=%s, ", instance->product,
                instance->version);
         printf("FLAVOR=%s, QUALIFIERS=%s\n", instance->flavor,
                 instance->qualifiers);
       }
    }
    for (tmp_table_list = mproduct->table_list ; tmp_table_list ;
	 tmp_table_list = tmp_table_list->next) {
      instance = (t_upstyp_instance *)tmp_table_list->data;
      printf("T:PRODUCT=%s, VERSION=%s, ", instance->product,
	     instance->version);
      printf("FLAVOR=%s, QUALIFIERS=%s\n", instance->flavor,
	     instance->qualifiers);
    }
  }
}

