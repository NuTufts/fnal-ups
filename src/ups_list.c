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
#include <ctype.h>   /* toupper */

/* ups specific include files */
#include "ups.h"

/*
 * Definition of public variables.
 */
extern t_cmd_info g_cmd_info[];
/*
 * Declaration of private functions.
 */

t_upslst_item *ups_list_core(t_upsugo_command * const a_command_line ,
                             t_upslst_item * const db_list );
void list_output(const t_upslst_item * const a_mproduct_list,
		 const t_upsugo_command * const a_command_line);

void list_K(const t_upstyp_matched_instance * const instance,
            const t_upsugo_command * const a_command_line, 
            const t_upstyp_matched_product * const product);
void print_chain(const t_upstyp_matched_instance * const instance,
                 char * const string);
/*
 * Definition of global variables.
 */
#define VPREFIX "UPSLIST: "

#ifndef NULL
#define NULL 0
#endif

#define FromVersion(ELEMENT) \
{ if (!upsutl_stricmp((buffer),"" #ELEMENT ""))    \
  { valid=1; \
    if(instance->version)                               \
    { if (instance->version->ELEMENT)                   \
      { printf("\"%s\" ",instance->version->ELEMENT);   \
      } else {                                          \
        printf("\"\" ");                                \
      }                                                 \
    } else {                                            \
      printf("\"\" ");                                  \
    }                                                   \
  }                                                     \
}
#define FromTable(ELEMENT) \
{ if (!upsutl_stricmp((buffer),"" #ELEMENT ""))    \
  { valid=1; \
    if(instance->table)                                 \
    { if (instance->table->ELEMENT)                     \
      { printf("\"%s\" ",instance->table->ELEMENT);     \
      } else {                                          \
        printf("\"\" ");                                \
      }                                                 \
    } else {                                            \
      printf("\"\" ");                                  \
    }                                                   \
  }                                                     \
}
#define FromDatabase(ELEMENT,STRING) \
{ if (!upsutl_stricmp((buffer),STRING))            \
  { valid=1; \
    if(product->db_info)                                \
    { if (product->db_info->ELEMENT)                    \
      { printf("\"%s\" ",product->db_info->ELEMENT);    \
      } else {                                          \
        printf("\"\" ");                                \
      }                                                 \
    } else {                                            \
      printf("\"\" ");                                  \
    }                                                   \
  }                                                     \
}
#define FromConfig(ELEMENT,STRING) \
{ if (!upsutl_stricmp((buffer),STRING))            \
  { valid=1; \
    if(config_ptr)                                      \
    { if (config_ptr->ELEMENT)                          \
      { printf("\"%s\" ",config_ptr->ELEMENT);          \
      } else {                                          \
        printf("\"\" ");                                \
      }                                                 \
    } else {                                            \
      printf("\"\" ");                                  \
    }                                                   \
  }                                                     \
}
#define FromAny(ELEMENT) \
{ if (!upsutl_stricmp(buffer,"" #ELEMENT ""))           \
  { valid=1;                                            \
    if(instance->chain)                                 \
    { if (instance->chain->ELEMENT)                     \
      { printf("\"%s\" ",instance->chain->ELEMENT);         \
      } else {                                          \
        if(instance->version)                           \
        { if (instance->version->ELEMENT)               \
          { printf("\"%s\" ",instance->version->ELEMENT);   \
          } else {                                      \
            if (instance->table)                        \
            { if (instance->table->ELEMENT)             \
              { printf("\"%s\" ",instance->table->ELEMENT); \
              } else {                                  \
                printf("\"\" ");                        \
              }                                         \
            } else {                                    \
              printf("\"\" ");                          \
            }                                           \
          }                                             \
        } else {                                        \
          if (instance->table)                          \
          { if (instance->table->ELEMENT)               \
            { printf("\"%s\" ",instance->table->ELEMENT);   \
            } else {                                    \
              printf("\"\" ");                          \
            }                                           \
          } else {                                      \
            printf("\"\" ");                            \
          }                                             \
        }                                               \
      }                                                 \
    } else {                                            \
      if(instance->version)                             \
      { if (instance->version->ELEMENT)                 \
        { printf("\"%s\" ",instance->version->ELEMENT);     \
        } else {                                        \
          if (instance->table)                          \
          { if (instance->table->ELEMENT)               \
            { printf("\"%s\" ",instance->table->ELEMENT);   \
            } else {                                    \
              printf("\"\" ");                          \
            }                                           \
          } else {                                      \
            printf("\"\" ");                            \
          }                                             \
        }                                               \
      } else {                                          \
        if (instance->table)                            \
        { if (instance->table->ELEMENT)                 \
          { printf("\"%s\" ",instance->table->ELEMENT);     \
          } else {                                      \
            printf("\"\" ");                            \
          }                                             \
        } else {                                        \
          printf("\"\" ");                              \
        }                                               \
      }                                                 \
    }                                                   \
  }                                                     \
}
#define FromBoth(ELEMENT) \
{ if (!upsutl_stricmp(buffer,"" #ELEMENT ""))    \
  { valid=1; \
    printf("\"");                                       \
    if(instance->chain)                                 \
    { if (instance->chain->ELEMENT)                     \
      { printf("%s:",instance->chain->ELEMENT);         \
      } else {                                          \
        printf(":");                                    \
      }                                                 \
    }                                                   \
    if (instance->xtra_chains)                          \
    { for (clist = instance->xtra_chains ;              \
           clist ; clist = clist->next)                 \
      { cinst_ptr = (t_upstyp_instance *)clist->data;   \
        if(cinst_ptr->ELEMENT)                          \
        { printf("%s:", cinst_ptr->ELEMENT);            \
        } else {                                        \
          printf(":");                                  \
        }                                               \
      }                                                 \
    }                                                   \
    if(instance->version)                               \
    { if (instance->version->ELEMENT)                   \
      { printf("%s",instance->version->ELEMENT);        \
      }                                                 \
    }                                                   \
    printf("\" ");                                      \
  }                                                     \
}
#define defaults(INSTANCE) \
{   printf("\tVersion=%s", instance->INSTANCE->version);            \
    printf("\tFlavor=%s\n", instance->INSTANCE->flavor);            \
    if (instance->INSTANCE->qualifiers)                             \
    { if(strlen(instance->INSTANCE->qualifiers))                    \
      { printf("\t\tQualifiers=\"%s\"", instance->INSTANCE->qualifiers);  \
      } else {      /* damn inconsistant if you ask me */              \
        printf("\t\tQualifiers=\"\"");                                 \
      }                                                                \
    } else {                                                         \
      printf("\t\tQualifiers=\"\"");                                 \
    }                                                                \
    if (instance->xtra_chains)                                      \
    { printf("\tChains=");                                           \
    } else {                                                         \
      printf("\tChain=");                                            \
    }                                                                \
    if ( instance->INSTANCE->chain )                                \
    { printf("%s", instance->INSTANCE->chain);                      \
    } else {                                                         \
      printf("\"\"");                                                \
    }                                                                \
    if (instance->xtra_chains)                                      \
    { for (clist = instance->xtra_chains ;                          \
           clist ; clist = clist->next)                              \
      { cinst_ptr = (t_upstyp_instance *)clist->data;                \
        printf(",%s", cinst_ptr->chain );                            \
      }                                                              \
    } printf("\n");                                                  \
}
#define WAW(WHAT) \
{    if (instance->version)                                         \
    { if (instance->version->WHAT)                                  \
      { printf("%s", instance->version->WHAT);                      \
      } else {                                                       \
        printf("\"\"");                                              \
      }                                                              \
    } else {                                                         \
      printf("\"\"");                                                \
    }                                                                \
    if (instance->chain)                                            \
    { if (instance->chain->WHAT)                                    \
      { printf(",%s", instance->chain->WHAT);                       \
      } else {                                                       \
        printf(",\"\"");                                             \
      }                                                              \
      if (instance->xtra_chains)                                    \
      { for (clist = instance->xtra_chains ;                        \
             clist ; clist = clist->next)                            \
        { cinst_ptr = (t_upstyp_instance *)clist->data;              \
          if (cinst_ptr->WHAT)                                       \
          { printf(",%s", cinst_ptr->WHAT);                          \
          } else {                                                   \
            printf(",\"\"");                                         \
          }                                                          \
        }                                                            \
      }                                                              \
    } printf("\n");                                                  \
}
/* int list_error=UPS_SUCCESS;  */

int product_cmp ( const void * const d1, const void * const d2 )
{
  t_upstyp_product *a1 = (t_upstyp_product *)d1;
  t_upstyp_product *a2 = (t_upstyp_product *)d2;

    return upsutl_stricmp( a1->product, a2->product );
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
t_upslst_item *ups_list( t_upsugo_command * const a_command_line ,
			 const int verify )
{
  t_upslst_item *mproduct_list = NULL, *mproduct_item = NULL;
  t_upslst_item *minst_item = NULL;
  t_upstyp_db *db_info = 0;
  t_upslst_item *db_list = 0;
  t_upstyp_matched_product *mproduct = NULL;

  /* Get all the requested instances */
  UPS_VERIFY=verify;		/* this is REALLY the ups verify command */ 
  for (db_list = a_command_line->ugo_db ; db_list ; db_list=db_list->next) 
  { db_info = (t_upstyp_db *)db_list->data;
    mproduct_list = ups_list_core(a_command_line,db_list);
    /*  upsugo_prtlst(mproduct_list,"the products");*/
    upsver_mes(2,"From Database %s\n",db_info->name);
    mproduct_list = upslst_first(mproduct_list);  /* point to the start */
    upsver_mes(2,"Starting sort of product list\n");
    mproduct_list = upslst_sort0( mproduct_list , product_cmp );
    upsver_mes(2,"Ending sort of product list\n");
    mproduct_list = upslst_first(mproduct_list);  /* point to the start */
  if(!verify)  /* verify is list with NO output */
  {
    /* Output the requested information from the instances */
    /*  upsugo_dump(a_command_line);*/
    if (!a_command_line->ugo_K)
    { printf("DATABASE=");
      if (db_info->name)
      { printf("%s ",db_info->name);
      } else {
        printf("\"\" ");
      } printf("\n");
    }
    list_output(mproduct_list, a_command_line);
    if (UPS_ERROR==UPS_SUCCESS)
    { if (mproduct_list) 
      { upsutl_statistics(mproduct_list, g_cmd_info[e_list].cmd);
      }
    } else { 
/*      upserr_output(); 
      upserr_clear(); 
      UPS_ERROR=list_error; */
      return 0; 
    }
  } else {
    /* now we must do the extra verify things - 
	    o verify the info in the dbconfig file
            o verify the information in the instances
     */
    for (mproduct_item = mproduct_list ; mproduct_item ; 
	 mproduct_item = mproduct_item->next) 
    { mproduct = (t_upstyp_matched_product *)mproduct_item->data;
      ups_verify_dbconfig(mproduct->db_info, 
		       (t_upstyp_matched_instance *)mproduct->minst_list->data,
		       a_command_line);
      for (minst_item = mproduct->minst_list ; minst_item ;
	   minst_item = minst_item->next)
      { upserr_add(UPS_VERIFY_PRODUCT, UPS_INFORMATIONAL, mproduct->product);
	ups_verify_matched_instance(mproduct->db_info,
			       (t_upstyp_matched_instance *)minst_item->data,
			       a_command_line, mproduct->product);
      }
      /* there may be lots of messages so output them on a per product basis */
      upserr_output();
      upserr_clear();
    }    
  }
    /* free the matched products */
    (void )upsutl_free_matched_product_list(&mproduct_list);
  }
/*  UPS_ERROR=list_error; */
  return 0;
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
t_upslst_item *ups_list_core(t_upsugo_command * const a_command_line ,
                             t_upslst_item * const db_list )
{
  t_upslst_item *mproduct_list = NULL;
  int need_unique = 0;
  char * addr;

  /* if no chains were entered, ask for them all */
  if (! a_command_line->ugo_chain) { 
    addr=upsutl_str_create(ANY_MATCH,' ');
    a_command_line->ugo_chain = upslst_new(addr);
  }

  /* Get all the instances that the user requested */
  mproduct_list = upsmat_instance(a_command_line, db_list , need_unique);
  if (UPS_ERROR != UPS_SUCCESS) { upserr_output(); upserr_clear(); }

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
  t_upstyp_matched_instance *instance= NULL;
  t_upstyp_config  *config_ptr = 0;
  t_upslst_item *ul_ptr = 0;
  t_upslst_item *al_ptr = 0;
  t_upslst_item *l_ptr = 0;
  t_upstyp_action *ac_ptr = 0;
  char *nodes=0;
  int count=0;
  static char buffer[20];
  int valid; /* bogus for macros */

  for (tmp_mprod_list = (t_upslst_item *)a_mproduct_list ; tmp_mprod_list ;
       tmp_mprod_list = tmp_mprod_list->next) 
  { mproduct = (t_upstyp_matched_product *)tmp_mprod_list->data;
    for (tmp_minst_list = mproduct->minst_list ; tmp_minst_list ;
	 tmp_minst_list = tmp_minst_list->next) 
    { instance = (t_upstyp_matched_instance *)(tmp_minst_list->data);
/* A as in a single product loop */
      if (!a_command_line->ugo_K)
      { printf("\tProduct=%s",mproduct->product);
        if (instance->chain) 
        { defaults(chain)
        } else { 
          if (instance->version )
          {  defaults(version) 
          } else { 
            if (instance->table )
            {  defaults(table)
            } /* else what in the hell are we doing here ?? */
          }
        }
        if (a_command_line->ugo_l && instance->version )
        { printf("\t\tDeclared=");
          strcpy(buffer,"declared");
          FromBoth(declared);
          printf("\n");
          printf("\t\tDeclarer=");
          strcpy(buffer,"declarer");
          FromBoth(declarer);
          printf("\n");
          printf("\t\tModified=");
          strcpy(buffer,"modified");
          FromBoth(modified);
          printf("\n");
          printf("\t\tModifier=");
          strcpy(buffer,"modifier");
          FromBoth(modifier);
          printf("\n");
          printf("\t\tHome=");
          if (mproduct->db_info) 
          { config_ptr = mproduct->db_info->config;
            if (config_ptr) 
            { if (config_ptr->prod_dir_prefix) 
              { printf("%s",config_ptr->prod_dir_prefix); }
            }
          }
          if (instance->version->prod_dir)
          { printf("%s", instance->version->prod_dir);
          }
          printf("\n");

          if (instance->version->compile_dir || 
              instance->version->compile_file)
          { printf("\t\tCompile=");
            if (instance->version->compile_dir)
            { printf("%s",instance->version->compile_dir); }
            if (instance->version->compile_file)
            { printf("/%s",instance->version->compile_file); }
            printf("\n");
          } else {
            printf("\t\tNo Compile Directive\n"); /* ;) */
          }
          if (upsutl_is_authorized( instance, mproduct->db_info,&nodes))
          { printf("\t\tAuthorized, Nodes=%s\n",nodes);
          } else {
            printf("\t\tNOT Authorized, Nodes=%s\n",nodes);
          }
          printf("\t\tUPS_Dir=");
          strcpy(buffer,"ups_dir");
          FromVersion(ups_dir);
          printf("\n");
          printf("\t\tTable_Dir=");
          strcpy(buffer,"table_dir");
          FromVersion(table_dir);
          printf("\n");
          printf("\t\tTable_File=");
          strcpy(buffer,"table_file");
          FromVersion(table_file);
          printf("\n");
          printf("\t\tArchive_File=");
          strcpy(buffer,"archive_file");
          FromVersion(archive_file)
          printf("\n");
          printf("\t\tDescription=");
          strcpy(buffer,"description");
          FromTable(description);
          printf("\n");
          for ( ul_ptr = upslst_first( instance->version->user_list ); 
                ul_ptr; ul_ptr = ul_ptr->next, count++ )
          { printf("\t\t%s \n",(char *)ul_ptr->data); /* Give keys and values */
          }
        }
        if (a_command_line->ugo_l) 
        { if (instance->table)
          { for ( al_ptr = upslst_first( instance->table->action_list ); 
                  al_ptr; al_ptr = al_ptr->next, count++ )
            { ac_ptr=al_ptr->data;
              printf("\t\tAction=%s\n",ac_ptr->action);
              if ( ac_ptr->command_list )
              { l_ptr = upslst_first( ac_ptr->command_list );
                for ( ; l_ptr; l_ptr = l_ptr->next )
                { printf( "\t\t\t%s\n", (char*)l_ptr->data );
                }
              }
            }
          }
        }
        printf("\n");
      } else { 
          list_K(instance,a_command_line,mproduct);
          if (UPS_ERROR!=UPS_SUCCESS) 
          { 
/* upserr_output(); 
            upserr_clear(); 
*/
            return; 
          }
      }
    }
/* end product loop */
  }
}

void list_K(const t_upstyp_matched_instance * const instance, 
            const t_upsugo_command * const command, 
            const t_upstyp_matched_product * const product)
{
  t_upslst_item *l_ptr = 0;
  t_upstyp_instance *cinst_ptr = 0;
  t_upslst_item *clist = 0;
  t_upstyp_config  *config_ptr = 0;
  static char buffer[20];
  char *nodes=0;
  char *str_val;
  char *addr;
  int count=0;
  int exists=1;
  int valid=0;
  if (product->db_info) 
  { config_ptr = product->db_info->config;
  }
  for ( l_ptr = upslst_first( command->ugo_key ); 
        l_ptr; l_ptr = l_ptr->next, count++ )
  { valid=0;
    strcpy(buffer,l_ptr->data);
    if(!upsutl_stricmp(l_ptr->data,"+"))
    { strcpy(buffer,"product");
      FromAny(product) 
      strcpy(buffer,"version");
      FromAny(version) 
      strcpy(buffer,"flavor");
      FromAny(flavor) 
      strcpy(buffer,"qualifiers");
      FromAny(qualifiers)
      strcpy(buffer,"chain");
      print_chain(instance,buffer);
      strcpy(buffer,"+");
    }
    FromAny(product) 
    FromAny(version) 
    FromAny(flavor) 
    FromAny(qualifiers)
    if (!upsutl_stricmp(buffer,"chain")) { valid=1; }
    print_chain(instance,buffer);
    FromVersion(table_file)
    FromVersion(table_dir)
    FromVersion(ups_dir)
    FromVersion(prod_dir)
    FromVersion(archive_file)
    FromVersion(compile_file)
    FromVersion(compile_dir)
    FromTable(description)
    FromTable(man_source_dir)
    FromTable(catman_source_dir)
    FromTable(html_source_dir)
    FromTable(news_source_dir)
    FromTable(info_source_dir)
    FromVersion(origin)
    /* FromChain(chain) */
    FromBoth(declarer)
    FromBoth(declared)
    FromBoth(modifier)
    FromBoth(modified)
    if (!upsutl_stricmp(buffer,"authorized_nodes")) 
    { (void)(upsutl_is_authorized(instance, product->db_info,&nodes));
      printf("\"%s\" ",nodes);
      valid=1;
    }
    if (!upsutl_stricmp(buffer,"statistics")) 
    { if (config_ptr)
      { if (config_ptr->statistics)
        { if (strstr(config_ptr->statistics,product->product))
          { printf("\"statistics\" ");
          } else { 
            printf("\"\" ");
          }
        } else {
          printf("\"\" ");
        }
      } else {
        printf("\"\" ");
      }
      valid=1;
    }
    FromDatabase(name,"Database")
    FromConfig(upd_usercode_dir,"UPD_UserCode_Dir")
    FromConfig(setups_dir,"Setups_Dir")
    FromConfig(ups_db_version,"DB_Version")
    FromConfig(prod_dir_prefix,"Prod_dir_prefix")
    FromConfig(man_target_dir,"Man_Target_dir")
    FromConfig(html_target_dir,"Html_Target_dir")
    FromConfig(info_target_dir,"Info_Target_dir")
    if (!strcmp(l_ptr->data,"key"))
    { valid=1; 
      printf("\"%d\"",upsget_key(instance->version)); /* test */ 
    }
    if (!strncmp(l_ptr->data,"_",1))
    { str_val=0;
      valid=1;
      if (instance->chain) 
         str_val = upskey_inst_getuserval( instance->chain,l_ptr->data);
      if (instance->version && !str_val )
         str_val = upskey_inst_getuserval( instance->version,l_ptr->data);
      if (instance->table && !str_val )
         str_val = upskey_inst_getuserval( instance->table,l_ptr->data);
      if (!str_val) 
      { printf("\"\" ");  /* this gives them "" for a invalid _key */
      } else {
        if (strlen(str_val))
        { printf("\"%s\" ",str_val);
        } else { 
          printf("\"%s\" ",(char *)l_ptr->data);
        }
      } 
    }
/* look for "processed values" spam? */
    if (!strncmp(l_ptr->data,"@",1))
    { if(!upsutl_stricmp(l_ptr->data,"@table_file"))
      { valid=1;
        if (instance->version)
        { addr=upsutl_get_table_file_path(product->product,
                                          instance->version->table_file,
                                          instance->version->table_dir,
                                          instance->version->ups_dir,
                                          instance->version->prod_dir,
                                          product->db_info,
                                          exists);
          if(addr)
          { printf("\"%s\" ",addr);
          } else { 
            printf("\"\" "); 
          }
        } else {
          printf("\"\" "); 
        }
      } else {
        if(!upsutl_stricmp(l_ptr->data,"@prod_dir"))
        { valid=1;
          printf("\"");
          if (product->db_info) 
          { config_ptr = product->db_info->config;
            if (config_ptr) 
            { if (config_ptr->prod_dir_prefix) 
              { printf("%s",config_ptr->prod_dir_prefix); }
            }
          }
          printf("%s\" ", instance->version->prod_dir);
        } 
      }
    }
    if (!valid) 
    { upserr_add(UPS_INVALID_KEYWORD, UPS_WARNING,l_ptr->data,"-K"); 
/*      if (list_error!=UPS_SUCCESS) */
/*      list_error=UPS_INVALID_KEYWORD;  */
    }
  }
  printf("\n");
}
void print_chain(const t_upstyp_matched_instance * const instance,
                 char * const string)
{ t_upstyp_instance *cinst_ptr = 0;
  t_upslst_item *clist = 0;
  if (!upsutl_stricmp(string,"chain"))
  { printf("\""); /* first " */
    if(instance->chain)
    { printf("%s",instance->chain->chain);
      if (instance->xtra_chains)
      { for (clist = instance->xtra_chains ;
             clist ; clist = clist->next)
        { cinst_ptr = (t_upstyp_instance *)clist->data;
          if(cinst_ptr->chain)
          { printf(":%s", cinst_ptr->chain);
    } } } } 
    printf("\" "); /* end " */
  }
}

