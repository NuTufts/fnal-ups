/************************************************************************
 *
 * FILE:
 *       ups_c_list.c
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
 *
 ***********************************************************************/

/* standard include files */
#include <string.h>
#include <stdio.h>

/* ups specific include files */
#include "ups_list.h"
#include "ups_memory.h"
#include "ups_types.h"
#include "ups_error.h"
#include "ups_utils.h"
#include "ups_match.h"
#include "ups_files.h"
#include "ups_commands.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */

t_upslst_item *upsc_list_core(t_ups_command * const a_command_line,
			      const char * const a_db);
void list_output(const t_upslst_item * const a_mproduct_list,
		 const t_ups_command * const a_command_line,
		 const char * const a_db);

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
 * ups_c_list
 *
 * This is the main line for the 'ups list' command.
 *
 * Input : argc, argv
 * Output: 
 * Return: 
 */
void ups_c_list( t_ups_command * const a_command_line )
{
  t_upslst_item *mproduct_list = NULL, *db_item= NULL;
  t_upslst_item *tmp_mprod_list = NULL;
  t_ups_match_product *mproduct = NULL;

  /* cycle through all of the databases, listing what is desired from each */
  for ( db_item = (t_upslst_item *)a_command_line->ugo_db ; db_item ;
	db_item = db_item->next ) {
    
    /* Get all the requested instances */
    mproduct_list = upsc_list_core(a_command_line, (char *)(db_item->data));
    mproduct_list = upslst_first(mproduct_list);  /* point to the start */

    /* Output the requested information from the instances */
    list_output(mproduct_list, a_command_line, (char *)(db_item->data));

    /* free the matched products */
    for (tmp_mprod_list = mproduct_list ; tmp_mprod_list ; 
	 tmp_mprod_list = tmp_mprod_list->next) {
      mproduct = (t_ups_match_product *)tmp_mprod_list->data;
      ups_free_mp(mproduct);      /* free the data */
    }
    /* now free the list */
    tmp_mprod_list = upslst_free(tmp_mprod_list, ' ');
  }
}

/*-----------------------------------------------------------------------
 * upsc_list_core
 *
 * Take the command line parameters and read in all of the desired instances
 *
 * Input : <input>
 * Output: <output>
 * Return: <return>
 */
t_upslst_item *upsc_list_core(t_ups_command * const a_command_line,
			      const char * const a_db)
{
  t_upslst_item *all_products = NULL, *all_versions = NULL;
  t_upslst_item *all_chains = NULL;
  t_upslst_item *tmp_products = NULL, *tmp_versions = NULL;
  t_upslst_item *mproduct_list = NULL;
  t_ups_match_product *mproduct = NULL;

  char *prod_name = NULL, *prod_dir = NULL;
  char *new_string = NULL;
  char do_delete = 'd';
  int need_unique = 0;

  /* If the user did not enter a product name, get all the product names in
     the current db. */
  if (! strcmp(a_command_line->ugo_product, ANY_MATCH)) {
    all_products = upsutl_get_files(a_db, (char *)ANY_MATCH);
  } else {
    /* we only have one product to list out */
    if (new_string = upsutl_str_create(a_command_line->ugo_product)) {
      all_products = upslst_add(all_products, new_string);
    } 
  }
  
  if (all_products) {
    /* for each product, get all the requested instances */
    for (tmp_products = all_products ; tmp_products ;
	 tmp_products = tmp_products->next) {
      prod_name = (char *)tmp_products->data;
      a_command_line->ugo_product = prod_name;
      
      /* If all versions were specified get them all */
      if (a_command_line->ugo_version && 
	  (! strcmp(a_command_line->ugo_version, ANY_MATCH))) {
	prod_dir = (char *)upsmem_malloc((int )(strlen(a_db) + 
						strlen(prod_name)) + 2);
	prod_dir = strcpy(prod_dir, a_db);           /* add the db directory */
	prod_dir = strcat(prod_dir, "/");            /* and a / */
	prod_dir = strcat(prod_dir, prod_name);      /* and the product name */

	/* get 2 lists, one with all the chains and one with all the
	   versions (minus any suffix (e.g. - .chain and .version) */
	all_versions = upsutl_get_files(prod_dir, (char *)VERSION_SUFFIX);
	all_chains = upsutl_get_files(prod_dir, (char *)CHAIN_SUFFIX);
	upsmem_free(prod_dir);
      } else {
	if (a_command_line->ugo_version) {
	  /* we only have one version to list out */
	  if (new_string = upsutl_str_create(a_command_line->ugo_version)) {
	    all_versions = upslst_add(all_versions, new_string);
	  } 
	}
      }

      if (all_versions) {
	/* start at the beginning of the list */
	all_versions = upslst_first(all_versions);
	
	/* For each version get the requested instances */
	for (tmp_versions = all_versions ; tmp_versions ; 
	     tmp_versions = tmp_versions->next) {
	  a_command_line->ugo_version = (char *)tmp_versions->data;

	  /* now do the instance matching */
	  mproduct = upsmat_match_instance(a_command_line, a_db, need_unique);
	  if (UPS_ERROR != UPS_SUCCESS) {
	    break;
	  } else {
	    if (mproduct) {
	      mproduct_list = upslst_add(mproduct_list, mproduct);
	    }
	  }
	}

	/* no longer need version list - free it */
	all_versions = upslst_free(all_versions, do_delete);

	/* try to match the versions obtained with any chains that might
	   point to them TBD */


      } else {
	/* no versions were specified, try to get chains.  just do the match,
	   it will check for chains or not */
	mproduct = upsmat_match_instance(a_command_line, a_db, need_unique);

	if (UPS_ERROR != UPS_SUCCESS) {
	  break;
	} else {
	  if (mproduct) {
	    mproduct_list = upslst_add(mproduct_list, mproduct);
	  }
	}
      }
    }

    /* no longer need product list - free it */
    all_products = upslst_free(all_products, do_delete);
  }

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
		 const t_ups_command * const a_command_line,
		 const char * const a_db)
{
  t_ups_match_product *mproduct = NULL;
  t_upslst_item *tmp_mprod_list = NULL;
  t_upslst_item *tmp_chain_list = NULL;
  t_upslst_item *tmp_vers_list = NULL;
  t_upslst_item *tmp_table_list = NULL;
  t_ups_instance *instance = NULL;

  for (tmp_mprod_list = (t_upslst_item *)a_mproduct_list ; tmp_mprod_list ;
       tmp_mprod_list = tmp_mprod_list->next) {
    mproduct = (t_ups_match_product *)tmp_mprod_list->data;

    for (tmp_chain_list = mproduct->chain_list ; tmp_chain_list ;
	 tmp_chain_list = tmp_chain_list->next) {
      instance = (t_ups_instance *)tmp_chain_list->data;
      printf("C:PRODUCT=%s, CHAIN=%s, VERSION=%s, ", instance->product,
	     instance->chain, instance->version);
      printf("FLAVOR=%s, QUALIFIERS=%s\n", instance->flavor,
	     instance->qualifiers);
    }

    for (tmp_vers_list = mproduct->version_list ; tmp_vers_list ;
	 tmp_vers_list = tmp_vers_list->next) {
      instance = (t_ups_instance *)tmp_vers_list->data;
      printf("V:PRODUCT=%s, VERSION=%s, ", instance->product,
	     instance->version);
      printf("FLAVOR=%s, QUALIFIERS=%s\n", instance->flavor,
	     instance->qualifiers);
    }

    for (tmp_table_list = mproduct->table_list ; tmp_table_list ;
	 tmp_table_list = tmp_table_list->next) {
      instance = (t_ups_instance *)tmp_table_list->data;
      printf("T:PRODUCT=%s, VERSION=%s, ", instance->product,
	     instance->version);
      printf("FLAVOR=%s, QUALIFIERS=%s\n", instance->flavor,
	     instance->qualifiers);
    }
  }
}

