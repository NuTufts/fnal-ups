/************************************************************************
 *
 * FILE:
 *       ups_declare.c
 * 
 * DESCRIPTION: 
 *       This is the 'ups declare' command.
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
 *       Mon Dec 15, DjF, Starting...
 *
 ***********************************************************************/

/* standard include files */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>   /* toupper */

/* ups specific include files */
#include "ups.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */

/*
 * Definition of global variables.
 */

#ifndef NULL
#define NULL 0
#endif

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * ups_declare
 *
 * This is the main line for the 'ups declare' command.
 *
 * Input : argc, argv
 * Output: 
 * Return: 
 */
void ups_declare( t_upsugo_command * const a_command_line )
{
  t_upslst_item *mproduct_list = NULL;
  t_upslst_item *tmp_mprod_list = NULL;
  t_upslst_item *tmp_minst_list = NULL;
  t_upstyp_db *db_info = 0;
  t_upslst_item *db_list = 0;
  t_upstyp_matched_product *mproduct = NULL;
  int need_unique = 0;
  t_upslst_item *save_flavor;
  t_upslst_item *save_qualifiers;

  /* Get all the requested instances */

  if (!a_command_line->ugo_product || !a_command_line->ugo_version )
  { printf("To Delcare a product you must specify a product and a version \n");
        abort();
  }
  save_flavor=a_command_line->ugo_flavor;
  save_qualifiers=a_command_line->ugo_qualifiers;
  a_command_line->ugo_flavor = upslst_new((void *)ANY_MATCH);
  a_command_line->ugo_qualifiers = upslst_new((void *)ANY_MATCH);

  for (db_list = a_command_line->ugo_db ; db_list ; db_list=db_list->next) 
  { db_info = (t_upstyp_db *)db_list->data;
    mproduct_list = upsmat_instance(a_command_line, db_list , need_unique);
    upsver_mes(2,"From Database %s\n",db_info->name);
    mproduct_list = upslst_first(mproduct_list);  /* point to the start */
    if ( mproduct_list ) /* found atleast one WILD CARD match */ 
    { if ( mproduct_list->next )  /* found multiple products, not possible as of yet!! */
      { upserr_output();  abort(); } 
      /* now let's look for an EXACT match */
      for (tmp_minst_list = mproduct->minst_list ; tmp_minst_list ;
           tmp_minst_list = tmp_minst_list->next)
      { minst_ptr = (t_upstyp_matched_instance *)(tmp_minst_list->data)
        if (minst_ptr->version)
        { /* if(minst->version->flavor) */
        }
      }
      printf("call ups modify for this"); 
      mproduct = (t_upstyp_matched_product *)mproduct_list->data;
      printf("\tPRODUCT=%s\n",mproduct->product);
    } else {
      upsver_mes(1,"Instance is unique creating definition for %s version \n",
                 a_command_line->ugo_product,a_command_line->ugo_version);
    }
    mproduct = (t_upstyp_matched_product *)mproduct_list->data;
    ups_free_matched_product(mproduct);      /* free the data */
    mproduct_list = upslst_free(mproduct_list, ' ');
  }
}

