/************************************************************************
 *
 * FILE:
 *       test_ups_match.c
 * 
 * DESCRIPTION: 
 *       Test the instance matching routines
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
 *       6-Aug-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>
#include <string.h>

/* ups specific include files */
#include "upsmat.h"
#include "upserr.h"
#include "upslst.h"
#include "upstyp.h"
#include "upsfil.h"
#include "upsmem.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */
#ifdef GET_INSTANCE
static void test_get_instance(const int argc, char *argv[]);
int get_instance(const t_upslst_item * const a_read_instances,
			const t_upslst_item * const a_flavor_list,
			const t_upslst_item * const a_quals_list,
			const int a_need_unique,
			t_upslst_item **a_list_instances);
#endif

#ifdef MATCH_TABLE
static void test_match_table(const int argc, char *argv[]);
int match_from_table( const char * const a_product,
		      const char * const a_tablefile,
		      const char * const a_tablefiledir,
		      const char * const a_upsdir,
		      const char * const a_productdir,
		      const char * const a_db,
		      const int a_need_unique,
		      const t_upslst_item * const a_flavor_list,
		      const t_upslst_item * const a_quals_list,
		      t_upslst_item ** const a_tinst_list);
#endif

#ifdef MATCH_VERSION
static void test_match_version(const int argc, char *argv[]);
int match_from_version( const char * const a_product,
			const char * const a_version,
			const char * const a_upsdir,
			const char * const a_productdir,
			const char * const a_db,
			const int a_need_unique,
			const t_upslst_item * const a_flavor_list,
			const t_upslst_item * const a_quals_list,
			t_upslst_item ** const a_vinst_list,
			t_upslst_item ** const a_tinst_list);
#endif

#ifdef MATCH_CHAIN
static void test_match_chain(const int argc, char *argv[]);
int match_from_chain( const char * const a_product,
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
#endif

static void test_match(const int argc, char *argv[]);
static void print_inst(t_upslst_item * const inst_list);
static char *get_ups_string(const char * const old_string);


/*
 * Definition of global variables.
 */

/*
 * Definition of public functions.
 */

int main(const int argc, char *argv[])
{
  test_match(argc, argv);
  upserr_output();

  return 1;
}

static void test_match(const int argc, char *argv[])
{
  t_ups_match_product *mproduct = NULL;
  t_upslst_item *mproduct_list = NULL, *mproduct_item;
  t_upslst_item *flavor_list = NULL, *quals_list = NULL;
  int need_unique = 0, i;
  char *new_string = NULL;
  char *ups_db = "/home/t2/berman/work/erupt/erupt_database/db";
  t_ups_command command_line;

  /* Calling structure:
     test_upsmat unique chain product version flavor quals flavor quals */


  if (! strcmp(argv[1],"1")) {
    need_unique = 1;
  }

  for (i = 5; i < argc; i += 2) {
    new_string = get_ups_string(argv[i]);
    flavor_list = upslst_add(flavor_list, new_string);
    new_string = get_ups_string(argv[i+1]);
    quals_list = upslst_add(quals_list, new_string);
  }

  /* point back to the first elements of the list */
  flavor_list = upslst_first(flavor_list);
  quals_list = upslst_first(quals_list);

  command_line.ugo_product = get_ups_string(argv[3]);
  if (strcmp(argv[4], "")) {
    command_line.ugo_version = get_ups_string(argv[4]);
  } else {
    command_line.ugo_version = NULL;
  }
  command_line.ugo_flavor = flavor_list;
  command_line.ugo_qualifiers = quals_list;
  new_string = get_ups_string(ups_db);
  command_line.ugo_db = upslst_new(new_string);
  command_line.ugo_tablefile = (char *)NULL;
  command_line.ugo_tablefiledir = (char *)NULL;
  command_line.ugo_productdir = (char *)NULL;
  command_line.ugo_upsdir = (char *)NULL;
  new_string = get_ups_string(argv[2]);
  command_line.ugo_chain = upslst_new(new_string);

  mproduct_list = upsmat_match_instance(&command_line, need_unique);
  if (mproduct_list) {
    mproduct_list = upslst_first(mproduct_list);
    for (mproduct_item = mproduct_list ; mproduct_item ; 
	 mproduct_item = mproduct_item->next) {
      mproduct = (t_ups_match_product *)mproduct_item->data;
      printf("\nChain Instances:\n");
      print_inst(mproduct->chain_list);
      printf("\nVersion Instances:\n");
      print_inst(mproduct->version_list);
      printf("\nTable Instances:\n");
      print_inst(mproduct->table_list);
    }
  } else {
    printf("No instances matched\n");
  }
}

#ifdef GET_INSTANCE
static void test_get_instance(const int argc, char *argv[])
{
  char cfile[] = "/usrdevel/s1/berman/upsdb/tigger/current.chain";
  char vfile[] = "/usrdevel/s1/berman/upsdb/tigger/v2_0.version";
  char tfile[] = "/usrdevel/s1/berman/upsdb/tigger/v2_0.tbl";
  t_upslst_item *flavor_list = NULL, *quals_list = NULL, *inst_list = NULL;
  int need_unique = 0, i, num_matches;
  t_ups_product *product = NULL;
  char *new_string = NULL;

  if (! strcmp(argv[1],"1")) {
    need_unique = 1;
  }

  for (i = 2; i < argc; i += 2) {
    new_string = get_ups_string(argv[i]);
    flavor_list = upslst_add(flavor_list, new_string);
    new_string = get_ups_string(argv[i+1]);
    quals_list = upslst_add(quals_list, new_string);
  }

  /* point back to the first elements of the list */
  flavor_list = upslst_first(flavor_list);
  quals_list = upslst_first(quals_list);

  if ((product = upsfil_read_file(&cfile[0])) != NULL) {
    print_inst(product->instance_list);
    num_matches = get_instance(product->instance_list, flavor_list,
			       quals_list, need_unique, &inst_list);
    printf("\nNumber of matches found is %d, and they are -\n",num_matches);
    print_inst(inst_list);
  }
}
#endif

#ifdef MATCH_TABLE
static void test_match_table(const int argc, char *argv[])
{
  t_upslst_item *flavor_list = NULL, *quals_list = NULL, *inst_list = NULL;
  int need_unique = 0, i, num_matches;
  char *new_string = NULL;

  if (! strcmp(argv[1],"1")) {
    need_unique = 1;
  }

  for (i = 7; i < argc; i += 2) {
    new_string = get_ups_string(argv[i]);
    flavor_list = upslst_add(flavor_list, new_string);
    new_string = get_ups_string(argv[i+1]);
    quals_list = upslst_add(quals_list, new_string);
  }

  /* point back to the first elements of the list */
  flavor_list = upslst_first(flavor_list);
  quals_list = upslst_first(quals_list);

  num_matches = match_from_table("tigger", (char *)argv[2], (char *)argv[3],
				 (char *)argv[4], (char *)argv[5],
				 (char *)argv[6], need_unique, 
				 flavor_list, quals_list, &inst_list);
  printf("\nNumber of matches found is %d, and they are -\n",num_matches);
  print_inst(inst_list);
}
#endif

#ifdef MATCH_VERSION
static void test_match_version(const int argc, char *argv[])
{
  t_upslst_item *flavor_list = NULL, *quals_list = NULL;
  t_upslst_item *vinst_list = NULL, *tinst_list = NULL;
  int need_unique = 0, i, num_matches;
  char *new_string = NULL;
  char *ups_db = "/usrdevel/s1/berman/upsdb";

  if (! strcmp(argv[1],"1")) {
    need_unique = 1;
  }

  for (i = 2; i < argc; i += 2) {
    new_string = get_ups_string(argv[i]);
    flavor_list = upslst_add(flavor_list, new_string);
    new_string = get_ups_string(argv[i+1]);
    quals_list = upslst_add(quals_list, new_string);
  }

  /* point back to the first elements of the list */
  flavor_list = upslst_first(flavor_list);
  quals_list = upslst_first(quals_list);

  num_matches = match_from_version("tigger", "v2_0", (char *)NULL,
				   (char *)NULL, ups_db, need_unique, 
				   flavor_list, quals_list, &vinst_list,
				   &tinst_list);
  printf("\nNumber of matches found in version file is %d -\n",num_matches);
  print_inst(vinst_list);
  printf("\nNumber of matches found in table file is %d -\n",num_matches);
  print_inst(tinst_list);
}
#endif

#ifdef MATCH_CHAIN
static void test_match_chain(const int argc, char *argv[])
{
  t_upslst_item *flavor_list = NULL, *quals_list = NULL;
  t_upslst_item *vinst_list = NULL, *tinst_list = NULL;
  t_upslst_item *cinst_list = NULL;
  int need_unique = 0, i, num_matches;
  char *new_string = NULL;
  char *ups_db = "/usrdevel/s1/berman/upsdb";

  if (! strcmp(argv[1],"1")) {
    need_unique = 1;
  }

  for (i = 3; i < argc; i += 2) {
    new_string = get_ups_string(argv[i]);
    flavor_list = upslst_add(flavor_list, new_string);
    new_string = get_ups_string(argv[i+1]);
    quals_list = upslst_add(quals_list, new_string);
  }

  /* point back to the first elements of the list */
  flavor_list = upslst_first(flavor_list);
  quals_list = upslst_first(quals_list);

  num_matches = match_from_chain("tigger", argv[2], "*", (char *)NULL,
				 (char *)NULL, ups_db, need_unique, 
				 flavor_list, quals_list, &cinst_list,
				 &vinst_list, &tinst_list);
  printf("\nNumber of matches found in chain file is %d -\n",num_matches);
  print_inst(cinst_list);
  printf("\nNumber of matches found in version file is %d -\n",num_matches);
  print_inst(vinst_list);
  printf("\nNumber of matches found in table file is %d -\n",num_matches);
  print_inst(tinst_list);
}
#endif

static char *get_ups_string(const char * const old_string)
{
  int bytes;
  char *new_string;

  bytes = (int )strlen(old_string);
  new_string = (char *)upsmem_malloc(bytes);
  strcpy(new_string, old_string);

  return new_string;
}

static void print_inst(t_upslst_item * const inst_list)
{
  t_ups_instance *instPtr = NULL;
  t_upslst_item *item = NULL;

  for (item = inst_list ; item ; item = item->next) {
    instPtr = (t_ups_instance *)item->data;
    if (instPtr->product) {
      printf("PRODUCT = %s   ", instPtr->product);
    }
    if (instPtr->version) {
      printf("VERSION = %s   ", instPtr->version);
    }
    if (instPtr->chain) {
      printf("CHAIN = %s\n", instPtr->chain);
    }
    if (instPtr->flavor) {
      printf("FLAVOR = %s   ", instPtr->flavor);
    }
    if (instPtr->qualifiers) {
      printf("QUALIFIERS = %s\n\n", instPtr->qualifiers);
    }
  }
}


