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

void list_K(const t_upstyp_matched_instance * const instance,
            const t_upsugo_command * const a_command_line);
/*
 * Definition of global variables.
 */
#define VPREFIX "UPSLIST: "

#ifndef NULL
#define NULL 0
#endif

#define FromVersion(ELEMENT) \
{ if (!upsutl_stricmp((l_ptr->data),"" #ELEMENT ""))    \
  { if(instance->version)                               \
    { if (instance->version->ELEMENT)                   \
      { printf("%s ",instance->version->ELEMENT);       \
      } else {                                          \
        printf("\"\" ");                                \
      }                                                 \
    } else {                                            \
      printf("\"\" ");                                  \
    }                                                   \
  }                                                     \
}
#define FromEither(ELEMENT) \
{ if (!upsutl_stricmp((l_ptr->data),"" #ELEMENT ""))    \
  { if(instance->chain)                                 \
    { if (instance->chain->ELEMENT)                     \
      { printf("%s ",instance->chain->ELEMENT);         \
      } else {                                          \
        if(instance->version)                           \
        { if (instance->version->ELEMENT)               \
          { printf("%s ",instance->version->ELEMENT);   \
          } else {                                      \
            printf("\"\" ");                            \
          }                                             \
        } else {                                        \
          printf("\"\" ");                              \
        }                                               \
      }                                                 \
    } else {                                            \
      if(instance->version)                             \
      { if (instance->version->ELEMENT)                 \
        { printf("%s ",instance->version->ELEMENT);     \
        } else {                                        \
          printf("\"\" ");                              \
        }                                               \
      } else {                                          \
        printf("\"\" ");                                \
      }                                                 \
    }                                                   \
  }                                                     \
}
#define FromBoth(ELEMENT) \
{ if (!upsutl_stricmp((l_ptr->data),"" #ELEMENT ""))    \
  { if(instance->chain)                                 \
    { if (instance->chain->ELEMENT)                     \
      { printf("%s:",instance->chain->ELEMENT);         \
      } else {                                          \
        printf(":");                                    \
      }                                                 \
    } else {                                            \
      printf(":");                                      \
    }                                                   \
    if (instance->xtra_chains)                          \
    { for (clist = instance->xtra_chains ;              \
           clist ; clist = clist->next)                 \
      { cinst_ptr = (t_upstyp_instance *)clist->data;   \
        printf("%s:", cinst_ptr->ELEMENT);              \
      }                                                 \
    }                                                   \
    if(instance->version)                               \
    { if (instance->version->ELEMENT)                   \
      { printf("%s ",instance->version->ELEMENT);       \
      } else {                                          \
        printf(" ");                                    \
      }                                                 \
    } else {                                            \
      printf(" ");                                      \
    }                                                   \
  }                                                     \
}

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
  char * addr;

  /* if no chains were entered, ask for them all */
  if (! a_command_line->ugo_chain) { 
/* The following won't work, a list item will be eventually managed
   with our memory routines and since this in not allocated with
   the ups_malloc the extra stuff won't be there... DjF
    a_command_line->ugo_chain = upslst_new((void *)ANY_MATCH);
*/
    addr=upsutl_str_create("*",' ');
    a_command_line->ugo_chain = upslst_new(addr);
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
       tmp_mprod_list = tmp_mprod_list->next) 
  { mproduct = (t_upstyp_matched_product *)tmp_mprod_list->data;
    for (tmp_minst_list = mproduct->minst_list ; tmp_minst_list ;
	 tmp_minst_list = tmp_minst_list->next) 
    { minst_ptr = (t_upstyp_matched_instance *)(tmp_minst_list->data);
/* A as in a single product loop */
      if (!a_command_line->ugo_K) /* keywords is a whole different animal */ 
      { printf("DATABASE: %s\n",mproduct->db_info->name);
        printf("PRODUCT: %s\n",mproduct->product);
        if (minst_ptr->chain) 
        { printf ("VERSION: %s\n", minst_ptr->chain->version);
          printf("FLAVOR: %s\n", minst_ptr->chain->flavor);
          printf("QUALIFIERS: %s\n", minst_ptr->chain->qualifiers);
          if (minst_ptr->xtra_chains) 
          { printf("CHAINS: %s", minst_ptr->chain->chain);
            for (clist = minst_ptr->xtra_chains ; clist ; clist = clist->next)
            { cinst_ptr = (t_upstyp_instance *)clist->data;
              printf(",%s", cinst_ptr->chain );
            }
          } else { 
            printf("CHAIN: %s", minst_ptr->chain->chain);
          } printf("\n");
        } else { 
          if (minst_ptr->version )
          {  printf("VERSION: %s\n", minst_ptr->version->version);
             printf("FLAVOR: %s\n", minst_ptr->version->flavor);
	     printf("QUALIFIERS: %s\n", minst_ptr->version->qualifiers);
             printf("CHAIN:\n");
          } else { 
            printf("No chain or version WHAT???\n");
          }
        }
        if (a_command_line->ugo_l && minst_ptr->version )
        { printf("HOME: %s\n", minst_ptr->version->prod_dir);
          printf("UPS: %s\n", minst_ptr->version->ups_dir);
          printf("TABLE_DIR: %s\n", minst_ptr->version->table_dir);
          printf("TABLE_FILE: %s\n", minst_ptr->version->table_file);
        }
        printf("\n\n");
      } else { 
          list_K(minst_ptr,a_command_line);
      }
    }
/* end product loop */
  }
}
/*
      if (minst_ptr->table) {
	printf("T:PRODUCT=%s, ", minst_ptr->table->product);
	printf("FLAVOR=%s, QUALIFIERS=%s\n", minst_ptr->table->flavor,
	       minst_ptr->table->qualifiers);
      }
*/

void list_K(const t_upstyp_matched_instance * const instance, 
            const t_upsugo_command * const command)
{
  t_upslst_item *l_ptr = 0;
  t_upstyp_instance *cinst_ptr = 0;
  t_upslst_item *clist = 0;
  int count=0;
  for ( l_ptr = upslst_first( command->ugo_key ); 
        l_ptr; l_ptr = l_ptr->next, count++ )
  { FromBoth(declarer)
    FromBoth(declared)
    FromBoth(chain)
    FromVersion(table_file)
    FromVersion(table_dir)
    FromVersion(ups_dir)
    FromVersion(prod_dir)
    FromVersion(archive_file)
    FromVersion(description)
    FromVersion(db_dir)
    FromEither(flavor)
    FromEither(product)
    FromEither(version)
    FromEither(qualifiers)
  }
  printf("\n");
}
