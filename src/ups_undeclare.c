/************************************************************************
 *
 * FILE:
 *       ups_undeclare.c
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
 *       Fir Jan 09, DjF, Starting...
 *
 ***********************************************************************/

/* standard include files */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

#define CHAIN "chain"
#define VERSION "version"
#define UNDECLARE "undeclare"

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * ups_undeclare
 *
 * This is the main line for the 'ups undeclare' command.
 *
 * Input : 
 * Output: 
 * Return: 
 */
void ups_undeclare( t_upsugo_command * const uc )
{
  t_upslst_item *mproduct_list = NULL;
  t_upslst_item *minst_list = NULL;
  t_upslst_item *chain_list = NULL;
  t_upslst_item *cmd_list = NULL;
  t_upstyp_db *db_info = 0;
  t_upslst_item *db_list = 0;
  t_upstyp_matched_product *mproduct = NULL;
  t_upstyp_matched_instance *minst = NULL;
  int not_unique = 0;
  int need_unique = 1;
  char * save_version;
  t_upstyp_product *product;
  char buffer[FILENAME_MAX+1];
  char *file=buffer;
  char *the_chain;
  char *the_flavor;
  char *the_qualifiers;
  t_upslst_item *cinst_list;                /* chain instance list */
  t_upstyp_instance *cinst;                 /* chain instance      */
  t_upstyp_instance *new_cinst;             /* new chain instance  */
  t_upslst_item *vinst_list;                /* version instance list */
  t_upstyp_instance *vinst;                 /* version instance      */
  t_upstyp_instance *new_vinst;             /* new version instance  */
  t_upslst_item *save_next;
  t_upslst_item *save_prev;
  char *username;
  struct tm *mytime;
  char *declared_date;
  char *unchain;
  time_t seconds=0;

  FILE *tmpfile;

  if (!uc->ugo_product || !uc->ugo_version )
  { printf("To undeclare a product you must specify a product and a version or chain(s) \n");
    exit(1);
  }
  if (!uc->ugo_f || (int)(upslst_count(uc->ugo_flavor) > 1) )
  { printf("To Declare a product you must specify A flavor \n");
    exit(1);
  }
/*
  if ((int)(upslst_count(uc->ugo_chain) > 1) )
  { printf("Multiple chains specified\n");
    exit(1);
  } */
  mproduct_list = upsmat_instance(uc, db_list , need_unique);
  if (!mproduct_list)
  { printf("No match doing NOTHING\n");
    exit(1);
  }
  username=upsutl_user();
  seconds=time(0);
  mytime = localtime(&seconds);
  mytime->tm_mon++; /* correct jan=0 */
  declared_date = (char *) malloc((size_t)(9));
  sprintf(declared_date,"%d-%d-%d",
          mytime->tm_mon,mytime->tm_mday,mytime->tm_year);

/************************************************************************
 *
 * Find the right database to work with
 *
 * Check for product in ANY database if so use that database 
 * otherwise set to first database listed
 *
 ***********************************************************************/
/* may still be in multiple databases pick first and stay there! */
 for (db_list = uc->ugo_db ; db_list ; db_list=db_list->next) 
 { db_info = (t_upstyp_db *)db_list->data;
   uc->ugo_chain = upslst_new((void *)ANY_MATCH);
   uc->ugo_version=0;
   uc->ugo_flavor = upslst_new((void *)ANY_MATCH);
   uc->ugo_qualifiers = upslst_new((void *)ANY_MATCH);
   mproduct_list = upsmat_instance(uc, db_list , not_unique);
   if (mproduct_list)    /* the product does exist */ 
   { upsver_mes(1,"Product %s currently exist in database %s\n",
                uc->ugo_product,
                db_info->name);
     break; 
   } db_info=0;
  } 

/************************************************************************
 *
 * If there was any chain specified at all we need to look at chain files
 *
 ***********************************************************************/

     if (uc->ugo_chain)
     { for (chain_list = uc->ugo_chain ; chain_list ;
         chain_list = chain_list->next)
       { the_chain = (char *)(chain_list->data);
          save_next = chain_list->next;
          save_prev = chain_list->prev;
          chain_list->next=0;
          chain_list->prev=0;
          mproduct_list = upsmat_instance(uc, db_list , need_unique);
          chain_list->next = save_next;
          chain_list->prev = save_prev;
          if (mproduct_list)
          { upsver_mes(1,"Match on chain found \n");
            mproduct_list = upslst_first(mproduct_list);
            mproduct = (t_upstyp_matched_product *)mproduct_list->data;
            minst_list = (t_upslst_item *)mproduct->minst_list;
            minst = (t_upstyp_matched_instance *)(minst_list->data);
            cinst = (t_upstyp_instance *)minst->chain;
            product = upsget_chain_file(db_info->name,
                                        uc->ugo_product,
                                        the_chain, &file);
            if ((UPS_ERROR == UPS_SUCCESS) && product )
            { cinst_list=upsmat_match_with_instance( cinst, product );
              cinst=cinst_list->data;
              product->instance_list = 
                 upslst_delete(product->instance_list,cinst,'d');
              upsver_mes(1,"Deleting %s of version %s\n",
                            the_chain,
                            cinst->version);
              (void )upsfil_write_file(product, file,' '); 
              unchain = (char *) malloc((size_t)(strlen(the_chain)+3));
              sprintf(unchain,"un%s",the_chain);
              cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                                         mproduct, unchain);
              if (UPS_ERROR == UPS_SUCCESS) 
              { upsact_process_commands(cmd_list, tmpfile);
              }
              upsact_cleanup(cmd_list);
            }
          } 
       } 
    }
/* An else would work too, I'm not sure of what the final code will do! */
    if (!uc->ugo_chain)
    { exit (0); /* removed chain only when specified */ 
    }
/************************************************************************
 *
 * Chains have been complete on the version file...
 *
 ***********************************************************************/
/* We want NOTHING to do with chains at this point - it is out of sync */
    uc->ugo_chain=0;
    mproduct_list = upsmat_instance(uc, db_list , need_unique);
    if (mproduct_list) 
    { mproduct_list = upslst_first(mproduct_list);
      mproduct = (t_upstyp_matched_product *)mproduct_list->data;
      minst_list = (t_upslst_item *)mproduct->minst_list;
      minst = (t_upstyp_matched_instance *)(minst_list->data);
      vinst = (t_upstyp_instance *)minst->version;
      product = upsget_version_file(db_info->name,
                                    uc->ugo_product,
                                    uc->ugo_version, 
                                    &file);
      if ((UPS_ERROR == UPS_SUCCESS) && product )
      { vinst_list=upsmat_match_with_instance( vinst, product );
        vinst=vinst_list->data;
        product->instance_list = 
           upslst_delete(product->instance_list,vinst,'d');
        upsver_mes(1,"Deleting version %s\n",
                      vinst->version);
        (void )upsfil_write_file(product, file,' '); 
        cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                                   mproduct, UNDECLARE);
        if (UPS_ERROR == UPS_SUCCESS) 
        { upsact_process_commands(cmd_list, tmpfile);
        }
        upsact_cleanup(cmd_list);
      }
    }
}
