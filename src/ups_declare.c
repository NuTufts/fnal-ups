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
#include <time.h>

/* ups specific include files */
#include "ups.h"

/*
 * Definition of public variables.
 */
extern t_cmd_info g_cmd_info[];

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
#define DECLARE "declare"

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
t_upslst_item *ups_declare( t_upsugo_command * const uc ,
			    const FILE * const tmpfile, const int ups_command)
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
  t_upslst_item *save_flavor;
  t_upslst_item *save_qualifiers;
  t_upslst_item *save_chain;
  char * save_version;
  t_upstyp_product *product;
  char buffer[FILENAME_MAX+1];
  char *file=buffer;
  char *the_chain;
  char *the_flavor;
  char *the_qualifiers;
  char *saddr;				/* start address for -O manipulation */
  char *eaddr;				/* end address for -O manipulation */
  char *naddr;				/* new address for -O manipulation */
  t_upslst_item *cinst_list;                /* chain instance list */
  t_upstyp_instance *cinst;                 /* chain instance      */
  t_upstyp_instance *new_cinst;             /* new chain instance  */
  t_upstyp_instance *tinst;                 /*   table instance      */
  t_upstyp_instance *new_vinst;             /* new version instance  */
  char *username;
  struct tm *mytime;
  char *declared_date;
  char *unchain;
  t_upslst_item *save_next;
  t_upslst_item *save_prev;
  time_t seconds=0;
  char * save_table_dir;		/* match won't work "how I want" */
  char * save_table_file;		/* with table specifications     */
  uc->ugo_m=0;
  uc->ugo_M=0;
  save_table_dir=uc->ugo_tablefiledir;
  save_table_file=uc->ugo_tablefile;
  uc->ugo_tablefile=0;
  uc->ugo_tablefiledir=0;
  save_chain=uc->ugo_chain;
  save_flavor=uc->ugo_flavor;
  save_qualifiers=uc->ugo_qualifiers;
  save_version=uc->ugo_version;

  if (!uc->ugo_product || !uc->ugo_version )
  { upserr_add(UPS_INVALID_SPECIFICATION, UPS_FATAL, "Declare", 
               "Specification must include a product and a version");
    return 0;
  }
  if ((int)(upslst_count(uc->ugo_flavor) != 2) ) /* remember any */
  { upserr_add(UPS_INVALID_SPECIFICATION, UPS_FATAL, "Declare", 
               "Specification must include a single flavor");
    return 0;
  }
  mproduct_list = upsmat_instance(uc, db_list , not_unique);
  if (UPS_ERROR != UPS_SUCCESS) 
  {  return 0; 
  }
/* if they are defining a version ONLY and it allready exists fail */
  if ( !uc->ugo_chain && uc->ugo_version) 
  if (mproduct_list)
  { upserr_add(UPS_INVALID_SPECIFICATION, UPS_FATAL, "Declare", 
               "Exact product definition exists");
    return 0;
  }
  if (mproduct_list)
  { if (uc->ugo_chain) { uc->ugo_version=0; }
    ups_undeclare(uc, tmpfile, e_undeclare);
    uc->ugo_version=save_version;
/*  { upserr_add(UPS_INVALID_SPECIFICATION, UPS_FATAL, "Declare", 
               "Exact product definition exists");
    return 0; */
  }
  username=upsutl_str_create(upsutl_user(), STR_TRIM_DEFAULT);
  seconds=time(0);
  mytime = localtime(&seconds);
  mytime->tm_mon++; /* correct jan=0 */
  declared_date = upsutl_str_create(upsutl_time_date(STR_TRIM_DEFAULT),
				    STR_TRIM_DEFAULT);
/* (char *) malloc((size_t)(9));
  sprintf(declared_date,"%d-%d-%d",
          mytime->tm_mon,mytime->tm_mday,mytime->tm_year);
*/

/************************************************************************
 *
 * Find the right database to work with
 *
 * Check for product in ANY database if so use that database 
 * otherwise set to first database listed
 *
 ***********************************************************************/

 uc->ugo_chain = upslst_new(upsutl_str_create(ANY_MATCH,' '));
 uc->ugo_version=0;
 uc->ugo_flavor = upslst_new(upsutl_str_create(ANY_MATCH,' '));
 uc->ugo_qualifiers = upslst_new(upsutl_str_create(ANY_MATCH,' '));
 for (db_list = uc->ugo_db ; db_list ; db_list=db_list->next) 
 { db_info = (t_upstyp_db *)db_list->data;
   mproduct_list = upsmat_instance(uc, db_list , not_unique);
   if (UPS_ERROR != UPS_SUCCESS) 
   { return 0; 
   } 
/*   if (UPS_ERROR != UPS_SUCCESS) { upserr_clear(); }  */
   if (mproduct_list)    /* the product does exist */ 
   { upsver_mes(1,"Product %s currently exist in database %s\n",
                uc->ugo_product,
                db_info->name);
     break; 
   } db_info=0;
  } 
  if (!db_info) 
  { db_list = upslst_first(uc->ugo_db);
    db_info = (t_upstyp_db *)db_list->data;
  } 
/* restore everything */
  uc->ugo_chain=upslst_free(uc->ugo_chain,'d');
  uc->ugo_chain=save_chain;
  uc->ugo_version=save_version;
  uc->ugo_flavor=upslst_free(uc->ugo_flavor,'d');
  uc->ugo_flavor=save_flavor;
  uc->ugo_qualifiers=upslst_free(uc->ugo_qualifiers,'d');
  uc->ugo_qualifiers=save_qualifiers;
/************************************************************************
 *
 * If there was any chain specified at all we need to look at chain files
 *
 ***********************************************************************/

     if (uc->ugo_chain)
     { for (chain_list = uc->ugo_chain ; chain_list ;
         chain_list = chain_list->next) 
       { the_chain = (char *)(chain_list->data);
         uc->ugo_version=0;
         uc->ugo_flavor = upslst_new(upsutl_str_create(ANY_MATCH,' '));
         uc->ugo_qualifiers = upslst_new(upsutl_str_create(ANY_MATCH,' '));
           save_next = chain_list->next;
           save_prev = chain_list->prev;
           chain_list->next=0;
           chain_list->prev=0;
           uc->ugo_chain=chain_list;
         mproduct_list = upsmat_instance(uc, db_list , not_unique);
         if (UPS_ERROR != UPS_SUCCESS) 
         { upsfil_clear_journal_files(); 
           upserr_vplace();
           return 0; 
         }
           chain_list->next = save_next;
           chain_list->prev = save_prev;
         if (mproduct_list)  /* the chain exists */
         { upsver_mes(1,"Chain %s currently exist\n",the_chain);
           uc->ugo_flavor=save_flavor;
           uc->ugo_qualifiers=save_qualifiers;
           uc->ugo_chain=chain_list;
           mproduct_list = upsmat_instance(uc, db_list , need_unique);
           if (UPS_ERROR != UPS_SUCCESS) 
           { upsfil_clear_journal_files();
             upserr_vplace();
             return 0; 
           }
           if (mproduct_list)  /* different version only */
           { upsver_mes(1,"same flavor and qualifiers exist\n");
             mproduct_list = upslst_first(mproduct_list);
             mproduct = (t_upstyp_matched_product *)mproduct_list->data;
             minst_list = (t_upslst_item *)mproduct->minst_list;
             minst = (t_upstyp_matched_instance *)(minst_list->data);
             cinst = (t_upstyp_instance *)minst->chain;
             product = upsget_chain_file(db_info->name,
                                         uc->ugo_product,
                                         the_chain, &file);
             strcpy(buffer,file);
             if ((UPS_ERROR == UPS_SUCCESS) && product )
             { cinst_list=upsmat_match_with_instance( cinst, product );
               cinst=cinst_list->data;
               product->instance_list = 
                  upslst_delete(product->instance_list,cinst,'d');
               upsver_mes(1,"Deleting %s chain of version %s\n",
                             the_chain,
                             cinst->version);
               (void )upsfil_write_file(product, buffer,' ',JOURNAL); 
               product->instance_list=upslst_free(product->instance_list,'d'); 
               unchain = (char *) malloc((size_t)(strlen(the_chain)+3));
               sprintf(unchain,"un%s",the_chain);
               cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                                          mproduct, unchain,ups_command);
               if (UPS_ERROR == UPS_SUCCESS) 
               { upsact_process_commands(cmd_list, tmpfile); 
                 upsact_cleanup(cmd_list);
               } else {
                 upsfil_clear_journal_files();
                 upserr_vplace();
                 return 0 ;
               }
             }
           } /* Get chain file (maybe again) */
           sprintf(buffer,"%s/%s/%s%s",
                   db_info->name,
                   uc->ugo_product,
                   the_chain,CHAIN_SUFFIX);
           if (upsfil_exist(file))
           { product = upsget_chain_file(db_info->name,
                                       uc->ugo_product,
                                       the_chain, &file);
           /* allready there  strcpy(buffer,file); */
           /* if(!product) Chain deleted was all only one */ 
           } else { 
             product = ups_new_product();
             product->file = upsutl_str_create( CHAIN, ' ' );
             product->product=upsutl_str_create( uc->ugo_product, ' ' );
             product->chain = upsutl_str_create( the_chain, ' ' );
             product->version = upsutl_str_create( save_version, ' ' );
           }
         } else { /* new chain does NOT exist at all */
           product = ups_new_product();
           sprintf(buffer,"%s/%s/%s%s",
                   db_info->name,
                   uc->ugo_product,
                   the_chain,CHAIN_SUFFIX);
           product->file = upsutl_str_create( CHAIN, ' ' );
           product->product=upsutl_str_create( uc->ugo_product, ' ' );
           product->chain = upsutl_str_create( the_chain, ' ' );
           product->version = upsutl_str_create( save_version, ' ' );
         }
         /* build new chain instance */
         new_cinst=ups_new_instance();
         new_cinst->product=upsutl_str_create( uc->ugo_product, ' ' );
         new_cinst->version=upsutl_str_create(save_version,' ');
         the_flavor=save_flavor->data;
         new_cinst->flavor=upsutl_str_create(the_flavor,' ');
         the_qualifiers=save_qualifiers->data;
         new_cinst->qualifiers=upsutl_str_create(the_qualifiers,' ');
         new_cinst->declarer=upsutl_str_create(username,' ');
         new_cinst->declared=upsutl_str_create(declared_date,' ');
         new_cinst->modifier=upsutl_str_create(username,' ');
         new_cinst->modified=upsutl_str_create(declared_date,' ');
         product->instance_list = 
            upslst_add(product->instance_list,new_cinst);
         upsver_mes(1,"Adding %s chain version %s to %s\n",
                    the_chain,
                    new_cinst->version,
                    buffer);
         (void )upsfil_write_file(product, buffer,' ',JOURNAL);  
        }
      }
/************************************************************************
 *
 * Chains have been complete on the version file...
 *
 ***********************************************************************/
/* We want NOTHING to do with chains at this point - it is out of sync */
    uc->ugo_chain=0;
    uc->ugo_version=save_version;  /* we must match of version now */
    uc->ugo_flavor = upslst_new(upsutl_str_create(ANY_MATCH,' '));
    uc->ugo_qualifiers = upslst_new(upsutl_str_create(ANY_MATCH,' '));
    mproduct_list = upsmat_instance(uc, db_list , not_unique);
    if (UPS_ERROR != UPS_SUCCESS) 
    { upsfil_clear_journal_files();
      upserr_vplace();
      return 0; 
    }
    if (mproduct_list) 
    {  uc->ugo_flavor=save_flavor;
       uc->ugo_qualifiers=save_qualifiers;
       mproduct_list = upsmat_instance(uc, db_list , need_unique);
       if (UPS_ERROR != UPS_SUCCESS) 
       { upsfil_clear_journal_files();
         upserr_vplace();
         return 0; 
       }
       if (!mproduct_list) 
       { upsver_mes(1,"Version exists adding additional instance\n");
         product = upsget_version_file(db_info->name,
                                       uc->ugo_product,
                                       uc->ugo_version,
                                       &file);
         strcpy(buffer,file);
       } else { 
         upsver_mes(1,"Instance in version file allready exists\n");
         buffer[0]=0; /* don't create instance */
         mproduct_list = upslst_first(mproduct_list);
         mproduct = (t_upstyp_matched_product *)mproduct_list->data;
       }
    } else { /* new version does NOT exist at all */
      product = ups_new_product();
      sprintf(buffer,"%s/%s/%s%s",
              db_info->name,
              uc->ugo_product,
              uc->ugo_version,VERSION_SUFFIX);
      product->file = upsutl_str_create(VERSION,' ');
      product->product=upsutl_str_create(uc->ugo_product,' ');
      product->version = upsutl_str_create(uc->ugo_version,' ');
    }
    /* build new version instance */
    if (buffer[0]!=0) /* instance doesn't exist */
    { new_vinst=ups_new_instance();
      new_vinst->product=upsutl_str_create(uc->ugo_product,' ');
      new_vinst->version=upsutl_str_create(save_version,' ');
      the_flavor=save_flavor->data;
      new_vinst->flavor=upsutl_str_create(the_flavor,' ');
      the_qualifiers=save_qualifiers->data;
      new_vinst->qualifiers=upsutl_str_create(the_qualifiers,' ');
      new_vinst->declarer=upsutl_str_create(username,' ');
      new_vinst->declared=upsutl_str_create(declared_date,' ');
      new_vinst->modifier=upsutl_str_create(username,' ');
      new_vinst->modified=upsutl_str_create(declared_date,' ');
      new_vinst->prod_dir=upsutl_str_create(uc->ugo_productdir,' ');
/*      new_vinst->table_dir=uc->ugo_tablefiledir;
        new_vinst->table_file=uc->ugo_tablefile;   */
      uc->ugo_tablefiledir=save_table_dir;
      uc->ugo_tablefile=save_table_file;
      new_vinst->table_dir=upsutl_str_create(save_table_dir,' ');
      new_vinst->table_file=upsutl_str_create(save_table_file,' ');
      new_vinst->ups_dir=upsutl_str_create(uc->ugo_upsdir,' ');
      new_vinst->origin=upsutl_str_create(uc->ugo_origin,' ');
      new_vinst->compile_file=upsutl_str_create(uc->ugo_compile_file,' ');
      new_vinst->compile_dir=upsutl_str_create(uc->ugo_compile_dir,' ');
      new_vinst->archive_file=upsutl_str_create(uc->ugo_archivefile,' ');
/* If I'm creating a totally matched version I have to create the matched 
   product structure by hand since it really doesn't exist yet on disk
   and a call to get it will fail
*/
      minst = ups_new_matched_instance();
      minst->version=new_vinst;
      tinst = upsmat_version(new_vinst,db_info);
      minst->table=tinst;
      minst_list = upslst_add(minst_list,minst);
      mproduct = ups_new_matched_product(db_info, uc->ugo_product,
                                         minst_list);
      if (!uc->ugo_r )
      { upserr_add(UPS_NO_INSTANCE, UPS_INFORMATIONAL, 
               uc->ugo_product, "product home", 
               "\nSpecification did not include a -r for product directory");
      }
      if (uc->ugo_O)
      { if ( strchr(uc->ugo_options,':') == 0)
        { new_vinst->user_list = 
             upslst_add(new_vinst->user_list,upsutl_str_create(uc->ugo_options,' '));
        } else {
          saddr=uc->ugo_options;
          while ((eaddr = strchr(saddr,':')) != 0 )
          { *eaddr='\0';
            naddr=upsutl_str_create(saddr,' ');
            new_vinst->user_list = 
               upslst_add(new_vinst->user_list,naddr);
            *eaddr=':'; /* put back : want to free whole list later */
            saddr=eaddr+1;
          }
          naddr=upsutl_str_create(saddr,' ');
          new_vinst->user_list = 
             upslst_add(new_vinst->user_list,naddr);
        }
      }
      if (uc->ugo_L)
      { new_vinst->statistics=upsutl_str_create("",' '); 
      }
      product->instance_list = 
          upslst_add(product->instance_list,new_vinst);
      upsver_mes(1,"Adding version %s to %s\n",
                 new_vinst->version,
                 buffer);
      (void )upsfil_write_file(product, buffer, ' ', JOURNAL);  
    } 
/* Process the declare action */
    cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                               mproduct, g_cmd_info[ups_command].cmd,
                               ups_command);
    if (UPS_ERROR == UPS_SUCCESS) 
    { upsact_process_commands(cmd_list, tmpfile); 
      upsact_cleanup(cmd_list);
    } else {
      upsfil_clear_journal_files();
      upserr_vplace();
      return 0;
    } 
    if (!uc->ugo_C) /* Don't do anything but declare actions */
                    /* Do configure actions                  */
    { cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                                 mproduct, g_cmd_info[e_configure].cmd,
                               ups_command);
      if (UPS_ERROR == UPS_SUCCESS) 
      { upsact_process_commands(cmd_list, tmpfile); 
        upsact_cleanup(cmd_list);
      } else {
        upsfil_clear_journal_files();
        upserr_vplace();
        return 0;
      }
/* Let them know if there is a tailor action for this product */
      cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                                 mproduct, g_cmd_info[e_tailor].cmd,
                                 ups_command);
      if (UPS_ERROR == UPS_SUCCESS) 
      { if (cmd_list)
        { upsver_mes(0,"A UPS tailor exists for this product\n");
          upsact_cleanup(cmd_list);
        }
      } 
      uc->ugo_chain=save_chain;
      if (uc->ugo_chain)
      { uc->ugo_flavor=save_flavor;
        uc->ugo_qualifiers=save_qualifiers;
        uc->ugo_version=save_version;
        for (chain_list = uc->ugo_chain ; chain_list ;
             chain_list = chain_list->next)
        { the_chain = (char *)(chain_list->data);
          save_next = chain_list->next;
          save_prev = chain_list->prev;
          chain_list->next=0;
          chain_list->prev=0;
          uc->ugo_chain=chain_list;
          cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                                    mproduct, the_chain,
                                    ups_command);
          if (UPS_ERROR == UPS_SUCCESS) 
          { upsact_process_commands(cmd_list, tmpfile);
            upsact_cleanup(cmd_list);
          } else {
            upsfil_clear_journal_files();
            upserr_vplace();
            return 0;
          } 
          if (strstr(the_chain,"current"))
          { cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                                       mproduct, g_cmd_info[e_start].cmd,
                                       ups_command);
            if (!cmd_list)
            { cmd_list = upsact_get_cmd((t_upsugo_command *)uc,
                                         mproduct, g_cmd_info[e_stop].cmd,
                                         ups_command);
            }
            /*          if (UPS_ERROR == UPS_SUCCESS)  */
            if (cmd_list)
            { upsver_mes(0,"A UPS start/stop exists for this product\n");
              upsact_cleanup(cmd_list);
            } 
          }
          chain_list->next = save_next;
          chain_list->prev = save_prev;
        }
      }
    } 
    return 0;
}
