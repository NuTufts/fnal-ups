/************************************************************************
 *
 * FILE:
 *       ups_modify.c
 * 
 * DESCRIPTION: 
 *       This is the 'ups modify' command.
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
 *      Tue Feb 24, DjF, Starting...
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

#define EDIT_PGM "vi"

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
t_upslst_item *ups_modify( t_upsugo_command * const uc , 
                          const FILE * const tmpfile,
                          const int ups_command)
{
  t_upslst_item *mproduct_list = NULL;
  t_upslst_item *minst_list = NULL;
  t_upslst_item *chain_list = NULL;
  t_upslst_item *cmd_list = NULL;
  char * original_file;
  char * input="   ";
  int rc;
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
/*  t_upslst_item *vinst_list;                 version instance list */
/*  t_upstyp_instance *vinst;                  version instance      */
  t_upstyp_instance *new_vinst;             /* new version instance  */
  char *username;
  char *loc;
  struct tm *mytime;
  char *declared_date;
  char *unchain;
  t_upslst_item *save_next;
  t_upslst_item *save_prev;
  time_t seconds=0;
  char * save_table_dir;		/* match won't work "how I want" */
  char * save_table_file;		/* with table specifications     */
  char *editor;
  editor = getenv("EDITOR");
  if (editor == NULL || *editor == '\0')
  { editor = EDIT_PGM; }
  uc->ugo_m=0;
  uc->ugo_M=0;
  UPS_VERIFY=1;
  save_table_dir=uc->ugo_tablefiledir;
  save_table_file=uc->ugo_tablefile;
  uc->ugo_tablefile=0;
  uc->ugo_tablefiledir=0;
  if (!uc->ugo_anyfile)
  { upserr_add(UPS_INVALID_SPECIFICATION, UPS_FATAL, "modify", 
               "Specification must include a file specification with -N");
    return(mproduct_list);
  }
  save_chain=uc->ugo_chain;
  save_flavor=uc->ugo_flavor;
  save_qualifiers=uc->ugo_qualifiers;
  save_version=uc->ugo_version;
  username=upsutl_str_create(upsutl_user(), STR_TRIM_DEFAULT);
/*  seconds=time(0);
  mytime = localtime(&seconds);
  mytime->tm_mon++;  correct jan=0 */
  declared_date = upsutl_str_create(upsutl_time_date(STR_TRIM_DEFAULT),
				    STR_TRIM_DEFAULT);
  while((loc=strchr(declared_date,' '))!=0)
  { *loc='_';
  }
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

if (strcmp(uc->ugo_product,"*"))
{
 uc->ugo_chain = upslst_new(upsutl_str_create(ANY_MATCH,' '));
 uc->ugo_version=0;
 uc->ugo_flavor = upslst_new(upsutl_str_create(ANY_MATCH,' '));
 uc->ugo_qualifiers = upslst_new(upsutl_str_create(ANY_MATCH,' '));
 for (db_list = uc->ugo_db ; db_list ; db_list=db_list->next) 
 { db_info = (t_upstyp_db *)db_list->data;
   mproduct_list = upsmat_instance(uc, db_list , not_unique);
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
 ***********************************************************************/
 upserr_output(); /* display current errors first */
 upserr_clear();
/* if the file was not read before this will read and report errors.
   but if the file was read with the normal process it's in the file
   cache and will not repeat the errors */
 if (!upsfil_is_in_cache(uc->ugo_anyfile))
 { fprintf(stdout,"WARNING - File specified not in instance specified\n");
   fprintf(stdout,"cannot check validity of instance only file format\n");
 }
 (void)upsfil_read_file(uc->ugo_anyfile);
 fprintf(stdout,"Pre modification verification pass complete\n");
 fprintf(stdout,"Do you wish to continue?");
 (void)fgets(input,3,stdin);
 if(upsutl_strincmp(input,"y",1)) { return(mproduct_list); }
 upsfil_flush(); /* flush the cache we are going to modify the file!!! */
 if ((original_file = tmpnam(0)) != 0)
 { sprintf(buffer,"cp %s %s",uc->ugo_anyfile,original_file);
   if (!system(buffer))
   { sprintf(buffer,"%s %s",EDIT_PGM,uc->ugo_anyfile);
     if (!system(buffer))
     { if (strcmp(uc->ugo_product,"*"))
       { mproduct_list = upsmat_instance(uc, db_list , not_unique);
       }
       (void)upsfil_read_file(uc->ugo_anyfile);
       fprintf(stdout,"Do you wish to save this modification?");
       (void)fgets(input,3,stdin);
       if(!upsutl_strincmp(input,"y",1)) 
       { sprintf(buffer,"cp %s %s.%s",original_file,
                        uc->ugo_anyfile,
                        declared_date);
         if (system(buffer))
         { fprintf(stdout,"Cannot save dated file copy of %s\n",
                   uc->ugo_anyfile);
         }
       } else { 
         sprintf(buffer,"cp %s %s",original_file,uc->ugo_anyfile);
         if (system(buffer))
         { fprintf(stdout,"Cannot restore original file %s copy in %s\n",
                   uc->ugo_anyfile,original_file);
           return(mproduct_list);
         }
       }
     } else { 
       fprintf(stdout,"Unable to edit file, check $EDITOR\n");
       return(mproduct_list);
     }
   } else { 
     fprintf(stdout,"Unable to create temporary work space\n");
     return(mproduct_list);
   }
 } else { 
   fprintf(stdout,"Unable to generate temporary file name(tmpnam)\n");
 }
 return(mproduct_list);
}
