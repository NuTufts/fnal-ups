/************************************************************************
 *
 * FILE:
 *       upsmat.c
 * 
 * DESCRIPTION: 
 *        Compare the instance(s) requested on the command line with those
 *        contined in the product UPS files and return a list of matches.
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
 *       25-Jul-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

/* ups specific include files */
#include "upsmat.h"
#include "upserr.h"
#include "upslst.h"
#include "upstyp.h"
#include "upsmem.h"
#include "upsfil.h"
#include "upsutl.h"

/*
 * Definition of public variables.
 */
extern int UPS_VERBOSE;

/*
 * Declaration of private functions.
 */
static t_ups_match_product *match_instance_core(
				   const t_ups_command * const a_command_line,
				   const char * const a_db,
				   const char * const a_prod_name,
				   const t_upslst_item * const a_chain_list,
				   const t_upslst_item * const a_version,
				   const int a_need_unique);
static int match_from_chain( const char * const a_product,
			     const char * const a_chain,
			     const char * const a_version,
			     const char * const a_upsdir,
			     const char * const a_productdir,
			     const char * const a_db,
			     const int a_need_unique,
			     const t_upslst_item * const a_flavor_list,
			     const t_upslst_item * const a_quals_list,
			     t_upslst_item ** const a_cinst_list,
			     t_upslst_item ** const a_vinst_list,
			     t_upslst_item ** const a_tinst_list);
static int match_from_version( const char * const a_product,
			       const char * const a_version,
			       const char * const a_upsdir,
			       const char * const a_productdir,
			       const char * const a_db,
			       const int a_need_unique,
			       const t_upslst_item * const a_flavor_list,
			       const t_upslst_item * const a_quals_list,
			       t_upslst_item ** const a_vinst_list,
			       t_upslst_item ** const a_tinst_list);
static int match_from_table( const char * const a_product,
			     const char * const a_tablefile,
			     const char * const a_tablefiledir,
			     const char * const a_upsdir,
			     const char * const a_productdir,
			     const char * const a_db,
			     const int a_need_unique,
			     const t_upslst_item * const a_flavor_list,
			     const t_upslst_item * const a_quals_list,
			     t_upslst_item ** const a_inst_list);
static char *get_table_file_path( const char * const a_prodname,
				  const char * const a_tablefile,
				  const char * const a_tablefiledir,
				  const char * const a_upsdir,
				  const char * const a_productdir,
				  const char * const a_db);
static int is_a_file(const char * const a_filename);
static int get_instance(const t_upslst_item * const a_read_instances,
			const t_upslst_item * const a_flavor_list,
			const t_upslst_item * const a_quals_list,
			const int a_need_unique,
			t_upslst_item ** const a_list_instances);

/*
 * Definition of global variables.
 */
#ifndef NULL
#define NULL
#endif

#define VPREFIX  "UPSMAT: "

#define TMP_LISTS_FREE() \
      if (tmp_flavor_list) {                                     \
	/* set to initial value first */                         \
	tmp_flavor_list->data = (void *)first_flavor;            \
	tmp_quals_list->data = (void *)first_quals;              \
	tmp_flavor_list = upslst_free(tmp_flavor_list, ' ');     \
	tmp_quals_list = upslst_free(tmp_quals_list, ' ');       \
      }
	
#define TMP_LISTS_SET()	\
     if (tmp_flavor_list == NULL) {                                        \
       tmp_flavor_list = upslst_insert(tmp_flavor_list, inst->flavor);     \
       tmp_quals_list = upslst_insert(tmp_quals_list, inst->qualifiers);   \
       /* save these first values as when we delete the list we will have  \
          to put these back on first so that the reference counter can     \
          be decremented properly. */                                      \
       first_flavor = inst->flavor;                                        \
       first_quals = inst->qualifiers;                                     \
     } else {                                                              \
       /* we already have a list just change the value pointed to */       \
       tmp_flavor_list->data = (void *)(inst->flavor);                     \
       tmp_quals_list->data = (void *)(inst->qualifiers);                  \
     }

#define USE_CMD_LINE_INFO() \
      if (a_upsdir) {                                \
	tmp_upsdir = (char *)a_upsdir;               \
      } else {                                       \
	tmp_upsdir = inst->ups_dir;                  \
      }                                              \
      if (a_productdir) {                            \
	tmp_productdir = (char *)a_productdir;       \
      } else {                                       \
	tmp_productdir = inst->prod_dir;             \
      }

#define GET_ALL_FILES(suffix, file_list) \
	location = upsutl_get_prod_dir(the_db, prod_name);           \
	file_list = upsutl_get_files(location, suffix);              \
	upsmem_free(location);

#define CHECK_UNIQUE(a_list) \
      a_list = upslst_first(a_list);                                 \
      if (a_need_unique) {                                           \
	/* we need a unique instance, make sure we only have one */  \
	if (a_list->next) {                                          \
	  /* we have more than one, this is an error */              \
	  upserr_add(UPS_NEED_UNIQUE);                               \
	  break;                                                     \
	}                                                            \
      }
/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upsmat_match_instance
 *
 * Given the input from the command line, determine which products, versions
 * and chains were asked for.  Then call the matching routine to fetch them.
 * All products will be matched in the first database and then the second etc.
 *
 * Input : command line information,
 *         flag indicating if we only want one instance,
 * Output: None
 * Return: a list of matched products
 */
t_upslst_item *upsmat_match_instance(
				   const t_ups_command * const a_command_line,
				   const int a_need_unique)
{
  t_upslst_item *db_item, *all_products = NULL, *product_item;
  t_upslst_item *all_versions = NULL;
  t_upslst_item *chain_item, *all_chains = NULL, *all_tmp_chains = NULL;
  t_ups_match_product *mproduct = NULL;
  t_upslst_item *mproduct_list = NULL;
  char *the_db, *prod_name, *the_chain, *new_string = NULL, *location = NULL;
  char do_delete = 'd';

  if (a_command_line->ugo_db) {
    /* we have at least one db */
    for (db_item = a_command_line->ugo_db ; db_item ;
	 db_item = db_item->next) {
      the_db = (char *)(db_item->data);
      if (UPS_VERBOSE) {
	printf("%sSearching UPS database - %s\n", VPREFIX, the_db);
      }
      /* If the user did not enter a product name, get all the product names in
	 the current db. */
      if (! strcmp(a_command_line->ugo_product, ANY_MATCH)) {
	all_products = upsutl_get_files(the_db, (char *)ANY_MATCH);
      } else {
	/* we only have one to list out */
	if ((new_string = upsutl_str_create(a_command_line->ugo_product,
					    ' '))) {
	  all_products = upslst_add(all_products, new_string);
	}
      }

      if (all_products) {
	/* make sure if we need a unique instance that we only have one */
	CHECK_UNIQUE(all_products);

	/* for each product, get all the requested instances */
	for (product_item = all_products ; product_item ;
	     product_item = product_item->next) {
	  prod_name = (char *)product_item->data;
	  
	  if (UPS_VERBOSE) {
	    printf("%sLooking for Product = %s\n", VPREFIX, prod_name);
	  }
	  /* Check if chains were requested */
	  if (a_command_line->ugo_chain) {
	    for (chain_item = a_command_line->ugo_chain ; chain_item ;
		 chain_item = chain_item->next) {
	      the_chain = (char *)(chain_item->data);
	    
	      if (! strcmp(the_chain, ANY_MATCH)) {
		/* get all the chains in the current product area */
		GET_ALL_FILES((char *)CHAIN_SUFFIX, all_tmp_chains);
		
		/* Now add these chains to the master list */
		all_chains = upslst_insert_list(all_chains, all_tmp_chains);
		all_tmp_chains = 0;
	      } else {
		/* Now add this chain to the master list */
		all_chains = upslst_add(all_chains, the_chain);
	      }
	    }
	    
	    /* get chains. just do match, it will check for chains or not */
	    if (all_chains) {
	      /* make sure if we need unique instance that we only have one */
	      CHECK_UNIQUE(all_chains);
	      
	      mproduct = match_instance_core(a_command_line, the_db,
					     prod_name, all_chains, NULL, 
					     a_need_unique);
	      
	      /* no longer need chain list - free it */
	      all_chains = upslst_free(all_chains, do_delete);
	    
	      if (UPS_ERROR != UPS_SUCCESS) {
		break;
	      } else {
		if (mproduct) {
		  mproduct_list = upslst_add(mproduct_list, mproduct);
		}
	      }
	    } 
	    /* Look to see if a version was specified. */
	  } else if (a_command_line->ugo_version) {
	    if (! strcmp(a_command_line->ugo_version, ANY_MATCH)) {
	      /* get all the versions in the current product area */
	      GET_ALL_FILES((char *)VERSION_SUFFIX, all_versions);
	    } else {
	      /* we only have one to list out */
	      if ((new_string = upsutl_str_create(a_command_line->ugo_version,
						  ' '))) {
		all_versions = upslst_add(all_versions, new_string);
	      }
	    }

	    if (all_versions) {
	      /* make sure if need unique instance that we only have one */
	      CHECK_UNIQUE(all_versions);
	      
	      /* now do the instance matching */
	      mproduct = match_instance_core(a_command_line, the_db, prod_name,
					     (t_upslst_item *)NULL,
					     all_versions, a_need_unique);
	      /* no longer need version list - free it */
	      all_versions = upslst_free(all_versions, do_delete);
	      
	      if (UPS_ERROR != UPS_SUCCESS) {
		break;
	      } else {
		if (mproduct) {
		  mproduct_list = upslst_add(mproduct_list, mproduct);
		}
	      }
	    }
	  }
	}
	/* no longer need product list - free it */
	all_products = upslst_free(all_products, do_delete);
      }
    }
  } else if (a_command_line->ugo_M && a_command_line->ugo_product) {
    /* We have a table file and no db, that is ok and we have a product name */
    if (UPS_VERBOSE) {
      printf("%sNo UPS Database, using Table File - %s\n", VPREFIX,
	     (char *)a_command_line->ugo_tablefile);
    }
    mproduct = match_instance_core(a_command_line, NULL,
				   a_command_line->ugo_product,
				   NULL, NULL, a_need_unique);
  } else {
    /* we have no db and no table file or no product name, this is an error */
    upserr_add(UPS_NO_DATABASE);
  }

  /* make sure we have cleaned up */
  if (all_products) {
    /* no longer need product list - free it */
    all_products = upslst_free(all_products, do_delete);
  }
  if (all_versions) {
    /* no longer need version list - free it */
    all_versions = upslst_free(all_versions, do_delete);
  }
  if (all_chains) {
    /* no longer need chain list - free it */
    all_chains = upslst_free(all_chains, do_delete);
  }      
  return(mproduct_list);
}

/*
 * Definition of private globals.
 */
#define G_SAVE_PATH(s) path_ptr = (char *)upsmem_malloc(s); strcpy(path_ptr, buffer)

/*
 * Definition of private functions.
 */

/*-----------------------------------------------------------------------
 * match_instance_core
 *
 * Actually do all the matching work for the product and version.
 *
 * Input : the command line input, a database, product name, list of chains,
 *         list of versions and a flag specifying a unique instance is desired.
 * Output: none
 * Return: a pointer to the instances matched from the files read.
 */
static t_ups_match_product *match_instance_core(
				   const t_ups_command * const a_command_line,
				   const char * const a_db,
				   const char * const a_prod_name,
				   const t_upslst_item * const a_chain_list,
				   const t_upslst_item * const a_version_list,
				   const int a_need_unique)
{
  t_ups_match_product *mproduct = NULL;
  t_upslst_item *vinst_list = NULL, *tinst_list = NULL;
  t_upslst_item *cinst_list = NULL;
  t_upslst_item *chain_list = NULL, *version_list = NULL;
  int num_matches;
  char *chain, *version;

  /* see if we were passed a table file. if so, don't worry about
     version and chain files, just read the table file */
  if (a_command_line->ugo_M) {
    if (UPS_VERBOSE) {
      printf("%sMatching with Table file  - %s\n", VPREFIX,
	     a_command_line->ugo_tablefile);
    }
    num_matches = match_from_table(a_prod_name,
				   a_command_line->ugo_tablefile,
				   a_command_line->ugo_tablefiledir,
				   a_command_line->ugo_upsdir,
				   a_command_line->ugo_productdir, a_db, 
				   a_need_unique, a_command_line->ugo_flavor,
				   a_command_line->ugo_qualifiers,
				   &tinst_list);
    if (UPS_ERROR == UPS_SUCCESS) {
      /* if we got some matches, fill out our matched product structure */
      if (num_matches != 0) {
	tinst_list = upslst_first(tinst_list);        /* back up to start */
	mproduct = ups_new_mp(a_db, NULL, NULL, tinst_list);
      }
    }
    
  /* see if we were passed a version. if so, don't worry
     about chains, just read the associated version file */
  } else if (a_version_list) { 
    for (version_list = (t_upslst_item *)a_version_list; version_list;
	 version_list = version_list->next) {
      /* get the version */
      version = (char *)version_list->data;
      if (UPS_VERBOSE) {
	printf("%sMatching with Version - %s\n", VPREFIX, version);
      }
      num_matches = match_from_version(a_prod_name, version,
				       a_command_line->ugo_upsdir,
				       a_command_line->ugo_productdir, a_db, 
				       a_need_unique,
				       a_command_line->ugo_flavor,
				       a_command_line->ugo_qualifiers, 
				       &vinst_list, &tinst_list);
      if (UPS_ERROR != UPS_SUCCESS) {
	/* we had an error, get out */
	break;
      }
    }

    /* We went thru the list of versions, get a matched product
       structure if we got no errors */
    if (UPS_ERROR == UPS_SUCCESS) {
      if (num_matches > 0) {
	tinst_list = upslst_first(tinst_list);        /* back up to start */
	vinst_list = upslst_first(vinst_list);        /* back up to start */
	mproduct = ups_new_mp(a_db, NULL, vinst_list, tinst_list);
      }
    }
  } else {
    /* we need to start with any requested chains and find the associated
       version and then table files. */
    for (chain_list = (t_upslst_item *)a_chain_list; chain_list;
	 chain_list = chain_list->next) {
      /* get the chain name */
      chain = (char *)chain_list->data;
      if (UPS_VERBOSE) {
	printf("%sMatching with Chain - %s\n", VPREFIX, chain);
      }
      num_matches = match_from_chain(a_prod_name, chain, 
				     a_command_line->ugo_version,
				     a_command_line->ugo_upsdir,
				     a_command_line->ugo_productdir, a_db, 
				     a_need_unique, a_command_line->ugo_flavor,
				     a_command_line->ugo_qualifiers, 
				     &cinst_list, &vinst_list, &tinst_list);
      if (UPS_ERROR != UPS_SUCCESS) {
	/* we had an error - get out */
	break;
      }
    }

    /* We went thru the list of version instances, get a matched product
       structure if we got no errors */
    if (UPS_ERROR == UPS_SUCCESS) {
      if (num_matches > 0) {
	tinst_list = upslst_first(tinst_list);        /* back up to start */
	vinst_list = upslst_first(vinst_list);        /* back up to start */
	cinst_list = upslst_first(cinst_list);        /* back up to start */
	mproduct = ups_new_mp(a_db, cinst_list, vinst_list, tinst_list);
      }
    }
      
  }

  return mproduct;
}

/*-----------------------------------------------------------------------
 * match_from_chain
 *
 * Given the the command line inputs, return the matched instances read in
 * from the chain file, version file, and the table file.
 *
 * Input : product name, chain name, version from command line,
 *         table file name, table file directory
 *         product ups directory, product root directory,
 *         db name, unique instance flag,
 *         pointer to a list of flavors to match,
 *         pointer to a list of qualifiers to match,
 *         pointer to a list of instances (chain, version and table)
 * Output: pointer to updated lists of matched instances
 * Return: number of instances added to the list
 */
static int match_from_chain( const char * const a_product,
			     const char * const a_chain,
			     const char * const a_version,
			     const char * const a_upsdir,
			     const char * const a_productdir,
			     const char * const a_db,
			     const int a_need_unique,
			     const t_upslst_item * const a_flavor_list,
			     const t_upslst_item * const a_quals_list,
			     t_upslst_item ** const a_cinst_list,
			     t_upslst_item ** const a_vinst_list,
			     t_upslst_item ** const a_tinst_list)
{
  int file_chars = 0, num_matches = 0, tmp_num_matches = 0;
  char buffer[FILENAME_MAX+1];
  t_ups_product *read_product;
  t_upslst_item *cinst;
  t_upslst_item *tmp_flavor_list = NULL, *tmp_quals_list = NULL;
  t_ups_instance *inst;
  char *first_flavor, *first_quals;
  char *tmp_upsdir, *tmp_productdir;
  int do_need_unique = 1;

  /* Get total length of chain file name including path */
  file_chars = (int )(strlen(a_chain) + strlen(a_product) + strlen(a_db) + 
               sizeof(CHAIN_SUFFIX) + 4);
  if (file_chars <= FILENAME_MAX) {
    sprintf(buffer, "%s/%s/%s%s", a_db, a_product, a_chain, CHAIN_SUFFIX);
    if ((read_product = upsfil_read_file(&buffer[0])) != NULL) {
      /* get all the instances that match command line input */
      tmp_num_matches = get_instance(read_product->instance_list,
				     a_flavor_list, a_quals_list,
				     a_need_unique, a_cinst_list);
      if (UPS_VERBOSE) {
	printf("%sFound %d instances in %s\n", VPREFIX, tmp_num_matches,
	       buffer);
      }

      /* we do not need the info read from the file.  we have taken what we
	 want and put it on the a_cinst_list */
      ups_free_product(read_product);

      /* for each instance that was matched, open the version file, and only
	 look for the instance that matches the instance found in the chain
	 file.  this insures that an instance in a chain file is
	 matched only with an instance in the associated version file. */
      for (cinst = *a_cinst_list ; cinst ; cinst = cinst->next) {
	/* get a table file */
	inst = (t_ups_instance *)cinst->data;

	/* check to see if a specific version was entered along with the
	   chain.  if so, then we only match those chains that point to the
	   same version as that which was entered. */
	if (a_version && strcmp(a_version, ANY_MATCH)) {
	  /* compare the entered version with the one associated with the
	     chain. if they do not match, get the next chain */
	  if (strcmp(inst->version, a_version)) {
	    /* They do not equal skip, this chain, get the next one */
	    continue;
	  }
	}

	/* make 2 lists (tmp_flavor_list and tmp_quals_list), one of the 
	   desired flavor and one of the desired qualifier to match */
	TMP_LISTS_SET();

	/* see if any command line info should override what we read from the
	   files  - set tmp_upsdir, tmp_productdir */
	USE_CMD_LINE_INFO();

	if (UPS_VERBOSE) {
	  printf("%sMatching with Version %s in Product %s\n", VPREFIX,
		 inst->version, inst->product);
	  printf("%sUsing Flavor = %s, and Qualifiers = %s\n", VPREFIX,
		 (char *)(tmp_flavor_list->data),
		 (char *)(tmp_quals_list->data));
	}
	tmp_num_matches = match_from_version(inst->product, inst->version,
					     tmp_upsdir, tmp_productdir, a_db,
					     do_need_unique, tmp_flavor_list,
					     tmp_quals_list, a_vinst_list,
					     a_tinst_list);
	if (tmp_num_matches == 0) {
	  /* We should have had a match, this is an error */
	  upserr_add(UPS_NO_VERSION_MATCH, UPS_FATAL, buffer,
		     inst->version);

	  /* clean up */
	  num_matches = 0;
	  *a_cinst_list = upsutl_free_inst_list(a_cinst_list);

	  break;                        /* stop any search */
	}

	/* keep a running count of the number of matches found */
	++num_matches;
      }

      /* we no longer need the lists */
      TMP_LISTS_FREE();

    } else {
      /* Could not read file */
      upserr_add(UPS_READ_FILE, UPS_FATAL, buffer);
    }
  } else {
    upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, file_chars);
  }

return num_matches;
}

/*-----------------------------------------------------------------------
 * match_from_version
 *
 * Given the the command line inputs, return the matched instances read in
 * from the version file and the table file.
 *
 * Input : product name, product version, 
 *         table file name, table file directory
 *         product ups directory, product root directory,
 *         db name, unique instance flag,
 *         pointer to a list of flavors to match,
 *         pointer to a list of qualifiers to match,
 *         pointer to a list of instances (version and table)
 * Output: pointer to updated lists of matched instances
 * Return: number of instances added to the list
 */
static int match_from_version( const char * const a_product,
			       const char * const a_version,
			       const char * const a_upsdir,
			       const char * const a_productdir,
			       const char * const a_db,
			       const int a_need_unique,
			       const t_upslst_item * const a_flavor_list,
			       const t_upslst_item * const a_quals_list,
			       t_upslst_item ** const a_vinst_list,
			       t_upslst_item ** const a_tinst_list)
{

  int file_chars, num_matches = 0, tmp_num_matches = 0;
  char buffer[FILENAME_MAX+1];
  t_ups_product *read_product;
  t_upslst_item *vinst;
  t_upslst_item *tmp_flavor_list = NULL, *tmp_quals_list = NULL;
  t_ups_instance *inst;
  char *first_flavor, *first_quals;
  char *tmp_upsdir, *tmp_productdir;
  int do_need_unique = 1;

  /* Get total length of version file name including path */
  file_chars = (int )(strlen(a_version) + strlen(a_product) + strlen(a_db) + 
               sizeof(VERSION_SUFFIX) + 4);
  if (file_chars <= FILENAME_MAX) {
    sprintf(buffer, "%s/%s/%s%s", a_db, a_product, a_version,
	    VERSION_SUFFIX);
    if ((read_product = upsfil_read_file(&buffer[0])) != NULL) {
      /* get all the instances that match command line input */
      tmp_num_matches = get_instance(read_product->instance_list,
				     a_flavor_list, a_quals_list,
				     a_need_unique, a_vinst_list);
      if (UPS_VERBOSE) {
	printf("%sFound %d instances in %s\n", VPREFIX, tmp_num_matches,
	       buffer);
      }

      /* we do not need the info read from the file.  we have taken what we
	 want and put it on the a_vinst_list */
      ups_free_product(read_product);

      /* for each instance that was matched, open the table file, and only
	 look for the instance that matches the instance found in the version
	 file.  this insures that an instance in a version file is
	 matched only with an instance in the associated table file. */
      for (vinst = *a_vinst_list ; vinst ; vinst = vinst->next) {

	/* get an instance and thus a table file */
	inst = (t_ups_instance *)vinst->data;

	/* It is ok if we do not have a table file.  then we just do whatever
	   the default action is */
	if (inst->table_file) {
	  /* make 2 lists (tmp_flavor_list and tmp_quals_list), one of the 
	     desired flavor and one of the desired qualifier to match */
	  TMP_LISTS_SET();

	  /* see if any command line info should override what we read from the
	     files - set tmp_upsdir, tmp_productdir */
	  USE_CMD_LINE_INFO();

	  if (UPS_VERBOSE) {
	    printf("%sMatching with Version %s in Product %s using Table file %s\n",
		   VPREFIX, inst->version, inst->product, inst->table_file);
	    printf("%sUsing Flavor %s and Qualifiers %s\n", VPREFIX,
		   (char *)(tmp_flavor_list->data),
		   (char *)(tmp_quals_list->data));
	  }
	  tmp_num_matches = match_from_table(inst->product, inst->table_file,
					     inst->table_dir, tmp_upsdir,
					     tmp_productdir, a_db,
					     do_need_unique, tmp_flavor_list, 
					     tmp_quals_list, a_tinst_list);
	  if (tmp_num_matches == 0) {
	    /* We should have had a match, this is an error */
	    upserr_add(UPS_NO_TABLE_MATCH, UPS_FATAL, buffer,
		       inst->table_file);

	    /* clean up */
	    num_matches = 0;
	    *a_vinst_list = upsutl_free_inst_list(a_vinst_list);
	    *a_tinst_list = upsutl_free_inst_list(a_tinst_list);
	    break;                        /* stop any search */
	  }

	  /* keep a running total of the matches we found */
	  ++num_matches;
	} else {
	  /* we have no table file so we must add a null list element */
	  *a_tinst_list = upslst_add(*a_tinst_list, NULL);
	}
      }

      /* we no longer need the lists */
      TMP_LISTS_FREE();
	
    } else {
      /* Could not read file */
      upserr_add(UPS_READ_FILE, UPS_FATAL, buffer);
    }
  } else {
    upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, file_chars);
  }

  return num_matches;
}

/*-----------------------------------------------------------------------
 * match_from_table
 *
 * Given the the command line inputs, return the matched instances read in
 * from the table file.
 *
 * Input : product name, a table file, a table file directory,
 *         products' ups directory, product root directory,
 *         db name, unique instance flag,
 *         pointer to a list of flavors to match,
 *         pointer to a list of qualifiers to match,
 *         pointer to a list of instances (table)
 * Output: pointer to an updated list of matched instances
 * Return: number of instances added to the list
 */
static int match_from_table( const char * const a_product,
			     const char * const a_tablefile,
			     const char * const a_tablefiledir,
			     const char * const a_upsdir,
			     const char * const a_productdir,
			     const char * const a_db,
			     const int a_need_unique,
			     const t_upslst_item * const a_flavor_list,
			     const t_upslst_item * const a_quals_list,
			     t_upslst_item ** const a_tinst_list)
{
  char *full_table_file;
  t_ups_product *read_product;
  int num_matches = 0;

  full_table_file = get_table_file_path(a_product, a_tablefile,
					a_tablefiledir, a_upsdir, a_productdir,
					a_db);

  if (full_table_file != NULL) {
    if ((read_product = upsfil_read_file(full_table_file)) != NULL) {
      /* get all the instances that match command line input */
       num_matches = get_instance(read_product->instance_list, a_flavor_list,
				  a_quals_list, a_need_unique, a_tinst_list);
       if (UPS_VERBOSE) {
	 printf("%sFound %d instances in %s\n", VPREFIX, num_matches,
		full_table_file);
       }

      /* we do not need the info read from the file.  we have taken what we
	 want and put it on the tinst_list */
      ups_free_product(read_product);
    }
  } else {
    /* Could not find the specified file  - ERROR */
    upserr_add(UPS_NO_FILE, UPS_FATAL, a_tablefile);
  }
    
    return num_matches;
}

/*-----------------------------------------------------------------------
 * get_table_file_path
 *
 * Given table file and the directories, find the absolute path
 * to the file.  This depends on the following hierarchy for places to look
 * for the existence of the table file  -
 *
 *    Look in each of the following successively till the file is found.  If
 *    the file is not found, it is an error.  If one of the pieces is missing,
 *    say - ups_dir - then that step is skipped.
 *
 *         tablefiledir/tablefile
 *         ./tablefile
 *         ups_dir/tablefile
 *         prod_dir/ups_dir/tablefile
 *         db/prodname/tablefile
 * 
 *
 * Input : product name
 *         table file name
 *         table file directory
 *         ups directory
 *         product dir
 *         ups database directory
 * Output: none
 * Return: table file location
 */
static char *get_table_file_path( const char * const a_prodname,
				  const char * const a_tablefile,
				  const char * const a_tablefiledir,
				  const char * const a_upsdir,
				  const char * const a_productdir,
				  const char * const a_db)
{
  char buffer[FILENAME_MAX+1];   /* max size of file name and path on system */
  char *path_ptr = NULL;
  int file_chars, total_chars;
  int found = 0;

  if (a_tablefile != NULL) {
    file_chars = (int )strlen(a_tablefile) + 2;  /* length plus trailing null 
						    and leading '/' */
    /* try tablefiledir/tablefile */
    if (a_tablefiledir != NULL) {
      if ((total_chars = file_chars + (int )strlen(a_tablefiledir))
	  <= FILENAME_MAX) {
	sprintf(buffer, "%s/%s", a_tablefiledir, a_tablefile);
	if (is_a_file(buffer) == UPS_SUCCESS) {
	  G_SAVE_PATH(total_chars);      /* found it - save it */
	  found = 1;
	}
      } else {
	if (UPS_VERBOSE) {
	  upserr_place();
	  upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	}
      }
    }
    /* try ./tablefile */
    if ((found == 0) && ((total_chars = file_chars + 1) <= FILENAME_MAX)) {
      sprintf(buffer, "./%s", a_tablefile);
      if (is_a_file(buffer) == UPS_SUCCESS) {
	G_SAVE_PATH(file_chars);            /* found it */
	found = 1;
      }
    } else {
      if (UPS_VERBOSE) {
	upserr_place();
	upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
      }
    }
    /* try ups_dir/tablefile */
    if ((found == 0) && (a_upsdir != NULL)) {
      if ((total_chars = file_chars + (int )strlen(a_upsdir))
	  <= FILENAME_MAX) {
	sprintf(buffer, "%s/%s", a_upsdir, a_tablefile);
	if (is_a_file(buffer) == UPS_SUCCESS) {
	  G_SAVE_PATH(total_chars);         /* found it */
	  found = 1;
	}
      } else {
	if (UPS_VERBOSE) {
	  upserr_place();
	  upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	}
      }
    }
    /* try prod_dir/ups_dir/tablefile */
    if ((found == 0) && (a_upsdir != NULL) && (a_productdir != NULL)) {
      if ((total_chars += (int )strlen(a_productdir) + 1) <= FILENAME_MAX) {
	sprintf(buffer, "%s/%s/%s", a_productdir, a_upsdir, a_tablefile);
	if (is_a_file(buffer) == UPS_SUCCESS) {
	  G_SAVE_PATH(total_chars);        /* found it */
	  found = 1;
	}
      } else {
	if (UPS_VERBOSE) {
	  upserr_place();
	  upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	}
      }
    }
    /* try db/prod_name/tablefile */
    if ((found == 0) && (a_db != NULL) && (a_prodname != NULL)) {
      if ((total_chars = file_chars + (int )(strlen(a_prodname) + strlen(a_db))
	                 + 1)
	  <= FILENAME_MAX) {
	sprintf(buffer, "%s/%s/%s", a_db, a_prodname, a_tablefile);
	if (is_a_file(buffer) == UPS_SUCCESS) {
	  G_SAVE_PATH(total_chars);        /* found it */
	}
      } else {
	if (UPS_VERBOSE) {
	  upserr_place();
	  upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	}
      }
    }
  }

return path_ptr;
}


/*-----------------------------------------------------------------------
 * is_a_file
 *
 * Given a filename including path, see if the named file exists.
 *
 * Input : file name and path
 * Output: none
 * Return: UPS_SUCCESS if file exists, else UPS_NO_FILE
 */
static int is_a_file(const char * const a_filename)
{
  int status = UPS_SUCCESS;
  struct stat buf;

  if (stat(a_filename, &buf) == -1) {
    status = UPS_NO_FILE;
  }

  return(status);
  
}

/*-----------------------------------------------------------------------
 * get_instance
 *
 * Given a list of instances locate the best match with the flavor and
 * qualifier arrays.
 *
 * Input : a list of instances read in from a file,
 *         list of flavors to match,
 *         list of qualifiers to match,
 *         flag indicating if a unique instance is desired
 *         a list (empty or not) of instances
 * Output: pointer to last element on updated list of instances
 * Return: number of instances added to the list
 */
static int get_instance(const t_upslst_item * const a_read_instances,
			const t_upslst_item * const a_flavor_list,
			const t_upslst_item * const a_quals_list,
			const int a_need_unique,
			t_upslst_item ** const a_list_instances)
{
  int got_match;
  t_upslst_item *tmp_list, *tmp_flavor_list, *tmp_quals_list;
  t_ups_instance *instance;
  t_upslst_item *first_matched_inst = NULL;
  char *flavor = NULL, *quals = NULL;
  int num_matches  = 0, want_all_f = 1, want_all_q = 1;

  /* loop over all the flavors from the flavor list */
  for (tmp_flavor_list = (t_upslst_item *)a_flavor_list; tmp_flavor_list ;
       tmp_flavor_list = tmp_flavor_list->next) {
    flavor = (char *)tmp_flavor_list->data;
    got_match = 0;

    /* Check to see if the flavors match or want any flavor */
    for (tmp_list = (t_upslst_item *)a_read_instances; tmp_list ;
	 tmp_list = tmp_list->next) {
      instance = (t_ups_instance *)(tmp_list->data);
      if ((! strcmp(instance->flavor, flavor)) ||
	  (! (want_all_f = strcmp(flavor, ANY_MATCH)))) {

	/* They do - now compare the qualifiers */
	for (tmp_quals_list = (t_upslst_item *)a_quals_list; tmp_quals_list ;
	     tmp_quals_list = tmp_quals_list->next) {
	  quals = (char *)tmp_quals_list->data;
	  if ((! strcmp(instance->qualifiers, quals)) ||
	      (! (want_all_q = strcmp(quals, ANY_MATCH)))) {
	    /* They do. Save the instances in the order they came in. */
	    *a_list_instances = upslst_add(*a_list_instances,
					   (void *)instance);
	    if (first_matched_inst == NULL) {
	      /* Save this so we can return it - this is the first new instance
	         we added to the list this time */
	      first_matched_inst = *a_list_instances;
	    }
	    ++num_matches;
	    got_match = 1;
	    break;
  	  }
	}
	/* if we got a match and we only want one, break.  if we got a match
	   and want as many as we can get but not any-match (*), break */
	if (got_match && (a_need_unique || (want_all_f && want_all_q))) {
	  /* go get the next flavor */
	  break;
	}
      }
    }
    /* If we only want one instance - we got it, leave now */
    if ((a_need_unique) && got_match) {
      break;
    }
  }

  /* if we matched 1 or more instances this time, return a pointer to the first
     instance that was matched.  if no match was made, return what we were
     passed */
  if (first_matched_inst != NULL) {
    *a_list_instances = first_matched_inst;
  }

  return num_matches;
}
