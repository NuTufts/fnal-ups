/************************************************************************
 *
 * FILE:
 *       ups_create.c
 * 
 * DESCRIPTION: 
 *       Create a template table file
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
 *       3-Dec-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>
#include <string.h>

/* ups specific include files */
#include "upserr.h"
#include "upsugo.h"
#include "upsutl.h"
#include "upslst.h"
#include "upstyp.h"
#include "upsfil.h"

/*
 * Definition of public variables.
 */


/*
 * Declaration of private functions.
 */
# define COMMENT_LINES 17
/*
 * Definition of global variables.
 */

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * ups_create
 *
 * Create a template table file based on the command line parameters. If
 * the file already exists, exit with an error.
 *
 * Input : information from the command line
 * Output: <output>
 * Return: <return>
 */
t_upslst_item *ups_create(const t_upsugo_command * const a_command_line,
			  const int a_ups_command)
{
  t_upslst_item *qualifier_list;
  static char buffer[MAX_LINE_LEN];
  t_upstyp_product *product;
  t_upstyp_instance *instance;
  t_upslst_item *inst_list = NULL, *flavor_item, *quals_item;
  char *flavor, *quals;
  int did_comments=0;          /* Add comments only once and only on new file */
  static char *comment[COMMENT_LINES]={
"# Several additional specifications examples and variables",
"# MAN_SOURCE_DIR=ups/toman",
"# INFO_SOURCE_DIR=ups/toinfo",
"# HTML_SOURCE_DIR=\?\?\?",
"# CATMAN_SOURCE_DIR=ups/toman",
"# NEWS_SOURCE_DIR=\?\?\?",
"# Action=setup",
"# - Defines the environmental that allows product to unsetup",
"#     setupEnv()",
"# - Defines the PRODUCT_DIR environment",
"#     prodDir()",
"# - Put product in your path",
"#     pathPrepend(PATH, ${UPS_PROD_DIR}/bin, :)",
"# - or add an alias based on it's name",
"#     addAlias(${UPS_PROD_NAME},${UPS_PROD_DIR}/bin/${UPS_PROD_NAME} %s)",
"# - If you need a source alias/procedure with mine and users options",
"#     addAlias(doit,'${UPS_SOURCE} `${UPS_PROD_DIR}/bin/exefile myopt %s`')"
};

  /* make sure at least a name of a table file was passed */
  if (a_command_line->ugo_m) {
    /* see if the specified file exists. if so, error */
    if (a_command_line->ugo_M) {
      (void )sprintf(buffer, "%s/%s", a_command_line->ugo_tablefiledir,
		     a_command_line->ugo_tablefile);
    } else {
      (void )sprintf(buffer, "%s", a_command_line->ugo_tablefile);
    }
    if (upsutl_is_a_file(&buffer[0]) != UPS_SUCCESS) {
      /* if no qualifiers were entered, use "" */
      if (! (qualifier_list = a_command_line->ugo_qualifiers)) {
	/* we have no qualifiers on the command line */
	qualifier_list = upslst_insert(qualifier_list, 
				       upsutl_str_create("",' '));
      }
      product = ups_new_product();
      if(!did_comments)
      { for ( did_comments=0; did_comments<=COMMENT_LINES; did_comments++)
        product->comment_list = 
              upslst_add(product->comment_list,
                         upsutl_str_create(comment[did_comments],' '));
      }

      /* no file - we can create it. first fill out the instances. if no
         particular list of flavors were entered, then just put flavor=any 
	 in the file. */
      if (a_command_line->ugo_f) {
	for (flavor_item = a_command_line->ugo_flavor ; flavor_item ;
	     flavor_item = flavor_item->next) {
	  flavor = (char *)flavor_item->data;
	  if (strcmp(flavor, ANY_FLAVOR)) {
	    for (quals_item = qualifier_list ; quals_item ;
		 quals_item = quals_item->next) {
	      quals = (char *)quals_item->data;
	      instance = ups_new_instance();
	      instance->flavor = upsutl_str_create(flavor, ' ');
	      instance->qualifiers = upsutl_str_create(quals, ' ');
	      if (a_command_line->ugo_description) {
		instance->description = 
		  upsutl_str_create(a_command_line->ugo_description, ' ');
	      }
	      /* add the new instance to the list */
	      inst_list = upslst_add(inst_list, instance);
	    }
	    /* move back to the beginning of the list for the next flavor */
	    qualifier_list = upslst_first(qualifier_list);
	  }
	}
      } else {
	/* just add one instance, a flavor=any instance */
	  flavor = ANY_FLAVOR;
	  for (quals_item = qualifier_list ; quals_item ;
	       quals_item = quals_item->next) {
	    quals = (char *)quals_item->data;
	    instance = ups_new_instance();
	    instance->flavor = upsutl_str_create(flavor, ' ');
	    instance->qualifiers = upsutl_str_create(quals, ' ');
	    if (a_command_line->ugo_description) {
	      instance->description = 
		upsutl_str_create(a_command_line->ugo_description, ' ');
	    }
	    /* add the new instance to the list */
	    inst_list = upslst_add(inst_list, instance);
	  }
	  /* move back to the beginning of the list for the next flavor */
	  qualifier_list = upslst_first(qualifier_list);
      }
      product->instance_list = upslst_first(inst_list);

      /* write the table file */
      product->file = upsutl_str_create("TABLE", ' ');
      (void )upsfil_write_file(product, buffer, ' ', NOJOURNAL);   
    } else {
      upserr_add(UPS_FILE_EXISTS, UPS_FATAL, buffer);
    }

  } else {
    upserr_add(UPS_NO_TABLE_FILE, UPS_FATAL);
  }
  return (NULL);
}

/*
 * Definition of private globals.
 */

/*
 * Definition of private functions.
 */

