/*
 *
 * FILE:
 *       upsget.c
 * 
 * DESCRIPTION: 
 *       This file contains routines to translate ups support variables
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
 *       12-Nov-1997, DjF, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <pwd.h>

/* ups specific include files */

#include "ups.h"
 
/*
 * Definition of public variables.
 */

#define g_UPSGET "UPSGET: "
#define UPSPRE "${UPS_"
#define TILDE "~"
static char g_buffer[FILENAME_MAX+1];

#define get_element(STRING,ELEMENT)              \
{ STRING=0;                                      \
  if(instance->chain)                            \
  { if (instance->chain->ELEMENT)                \
    { STRING=instance->chain->ELEMENT;           \
    } else {                                     \
      if(instance->version)                      \
      { if (instance->version->ELEMENT)          \
        { STRING=instance->version->ELEMENT;     \
        } else {                                 \
          if (instance->table)                   \
          { if (instance->table->ELEMENT)        \
            { STRING=instance->table->ELEMENT; } \
          }                                      \
        }                                        \
      } else {                                   \
        if (instance->table)                     \
        { if (instance->table->ELEMENT)          \
          { STRING=instance->table->ELEMENT; }   \
        }                                        \
      }                                          \
    }                                            \
  } else {                                       \
    if(instance->version)                        \
    { if (instance->version->ELEMENT)            \
      { STRING=instance->version->ELEMENT;       \
      } else {                                   \
        if (instance->table)                     \
        { if (instance->table->ELEMENT)          \
          { STRING=instance->table->ELEMENT; }   \
        }                                        \
      }                                          \
    } else {                                     \
      if (instance->table)                       \
      { if (instance->table->ELEMENT)            \
        { STRING=instance->table->ELEMENT; }     \
      }                                          \
    }                                            \
  }                                              \
}


char *upsget_translation_tilde( char * const oldstr );
 
typedef char * (*var_func)(const t_upstyp_db * const db_info_ptr,
                           const t_upstyp_matched_instance * const instance,
                           const t_upsugo_command * const command_line );

typedef struct s_var_sub {
  char *string;
  var_func func;
} t_var_sub;

static t_var_sub g_var_subs[] = {
  { "${UPS_PROD_NAME", upsget_product },
  { "${UPS_PROD_VERSION", upsget_version },
  { "${UPS_PROD_FLAVOR", upsget_flavor },
  { "${UPS_OS_FLAVOR", upsget_OS_flavor },
  { "${UPS_PROD_QUALIFIERS", upsget_qualifiers },
  { "${UPS_PROD_DIR", upsget_prod_dir },
  { "${UPS_SHELL", upsget_shell },
  { "${UPS_OPTIONS", upsget_options },
  { "${UPS_VERBOSE", upsget_verbose },
  { "${UPS_EXTENDED", upsget_extended },
  { "${UPS_THIS_DB", upsget_this_db},
  { "${UPS_SETUP", upsget_envstr },
  { "${UPS_COMPILE", upsget_compile },
  { "${UPS_ORIGIN", upsget_origin },
  { "${UPS_SOURCE", upsget_source },
  { "${UPS_UPS_DIR", upsget_ups_dir },
  { "${PRODUCTS", upsget_database },
  {  0, 0 }
} ;


/*
 * Definition of public functions.
 */
void upsget_remall(const FILE * const stream, 
                    const t_upsugo_command * const command_line,
		    const char * const prefix )
{
  char *pdefault;

  if (prefix)
  {
    pdefault = (char *)prefix;
  } else {
    pdefault = "";
  }

  if (command_line->ugo_shell == e_INVALID_SHELL)
  { upserr_add(UPS_NOSHELL, UPS_WARNING);
  } else {
    if (command_line->ugo_shell == e_BOURNE )
    { fprintf((FILE *)stream,"%sunset UPS_PROD_NAME\n", pdefault);
      fprintf((FILE *)stream,"%sunset UPS_PROD_VERSION\n", pdefault);
      fprintf((FILE *)stream,"%sunset UPS_PROD_DIR\n", pdefault);
      fprintf((FILE *)stream,"%sunset UPS_VERBOSE\n", pdefault);
      fprintf((FILE *)stream,"%sunset UPS_EXTENDED\n", pdefault);
      fprintf((FILE *)stream,"%sunset UPS_THIS_DB\n", pdefault);
      fprintf((FILE *)stream,"%sunset UPS_OS_FLAVOR\n", pdefault);
      fprintf((FILE *)stream,"%sunset UPS_PROD_FLAVOR\n", pdefault);
      fprintf((FILE *)stream,"%sunset UPS_PROD_QUALIFIERS\n", pdefault);
/*      fprintf((FILE *)stream,"%sunset UPS_SHELL\n", pdefault); */
      fprintf((FILE *)stream,"%sunset UPS_OPTIONS\n", pdefault);
    } else { 
      fprintf((FILE *)stream,"%sunsetenv UPS_PROD_NAME\n", pdefault);
      fprintf((FILE *)stream,"%sunsetenv UPS_PROD_VERSION\n", pdefault);
      fprintf((FILE *)stream,"%sunsetenv UPS_PROD_DIR\n", pdefault);
      fprintf((FILE *)stream,"%sunsetenv UPS_VERBOSE\n", pdefault);
      fprintf((FILE *)stream,"%sunsetenv UPS_EXTENDED\n", pdefault);
      fprintf((FILE *)stream,"%sunsetenv UPS_THIS_DB\n", pdefault);
      fprintf((FILE *)stream,"%sunsetenv UPS_OS_FLAVOR\n", pdefault);
      fprintf((FILE *)stream,"%sunsetenv UPS_PROD_FLAVOR\n", pdefault);
      fprintf((FILE *)stream,"%sunsetenv UPS_PROD_QUALIFIERS\n", pdefault);
/*      fprintf((FILE *)stream,"%sunsetenv UPS_SHELL\n", pdefault); */
      fprintf((FILE *)stream,"%sunsetenv UPS_OPTIONS\n", pdefault);
    }
  } 
}

void upsget_envout(const FILE * const stream, 
                    const t_upstyp_db * const db,
                    const t_upstyp_matched_instance * const instance,
                    const t_upsugo_command * const command_line )
{ char *name;
  get_element(name,product);
  name = upsutl_upcase( name );
  if (command_line->ugo_shell == e_BOURNE )
  { fprintf((FILE *)stream,"SETUP_%s=\"%s\";export SETUP_%s\n#\n",
    name,upsget_envstr(db,instance,command_line),name);
  } else {
    fprintf((FILE *)stream,"setenv SETUP_%s \"%s\"\n#\n",
    name,upsget_envstr(db,instance,command_line));
  }
}
 
void upsget_allout(const FILE * const stream, 
                    const t_upstyp_db * const db,
                    const t_upstyp_matched_instance * const instance,
                    const t_upsugo_command * const command_line,
		    const char * const prefix )
{ char *addr;
  char *name;
  char *pdefault;
  if (prefix)
  { 
    pdefault = (char *)prefix;
  } else {
    pdefault = "";
  }

  if (command_line->ugo_shell == e_INVALID_SHELL)
  { upserr_add(UPS_NOSHELL, UPS_WARNING);
  } else {
    get_element(name,product);
    if (!name)
    { name = " ";      /* no name in file, set to space */      
    }
    if (command_line->ugo_shell == e_BOURNE )
    { fprintf((FILE *)stream,"%sUPS_PROD_NAME=%s;export UPS_PROD_NAME\n",
	       pdefault, name);
      fprintf((FILE *)stream,"%sUPS_PROD_VERSION=\"%s\";export UPS_PROD_VERSION\n",
               pdefault, upsget_version(db,instance,command_line));
      fprintf((FILE *)stream,"%sUPS_PROD_DIR=\"%s\";export UPS_PROD_DIR\n",
               pdefault, upsget_prod_dir(db,instance,command_line));
      addr=upsget_verbose(db,instance,command_line);
      if (strlen(addr)) 
      { fprintf((FILE *)stream,"%sUPS_VERBOSE=%s;export UPS_VERBOSE\n",
		 pdefault, addr);
      } else { 
        fprintf((FILE *)stream,"%sunset UPS_VERBOSE\n", pdefault);
      }
      addr=upsget_extended(db,instance,command_line);
      if (strlen(addr))
      { fprintf((FILE *)stream,"%sUPS_EXTENDED=%s;export UPS_EXTENDED\n",
		 pdefault, addr); 
      } else { 
        fprintf((FILE *)stream,"%sunset UPS_EXTENDED\n", pdefault);
      } 
      fprintf((FILE *)stream,"%sUPS_THIS_DB=%s;export UPS_THIS_DB\n",pdefault,
               upsget_this_db(db,instance,command_line));
      fprintf((FILE *)stream,"%sUPS_OS_FLAVOR=%s;export UPS_OS_FLAVOR\n",
	       pdefault, upsget_OS_flavor(db,instance,command_line));
      fprintf((FILE *)stream,"%sUPS_PROD_FLAVOR=%s;export UPS_PROD_FLAVOR\n",
               pdefault, upsget_flavor(db,instance,command_line));
      fprintf((FILE *)stream,"%sUPS_PROD_QUALIFIERS=%s;export UPS_PROD_QUALIFIERS\n",
               pdefault,upsget_qualifiers(db,instance,command_line));
/*      fprintf((FILE *)stream,"%sUPS_SHELL=%s;export UPS_SHELL\n",
               pdefault,upsget_shell(db,instance,command_line)); */
      addr=upsget_options(db,instance,command_line);
      if (addr) 
      { fprintf((FILE *)stream,"%sUPS_OPTIONS=\"%s\";export UPS_OPTIONS\n",
		 pdefault,addr); } 
    } else { 
      fprintf((FILE *)stream,"%ssetenv UPS_PROD_NAME %s\n",pdefault,name);
      fprintf((FILE *)stream,"%ssetenv UPS_PROD_VERSION \"%s\"\n",pdefault,
               upsget_version(db,instance,command_line));
      fprintf((FILE *)stream,"%ssetenv UPS_PROD_DIR \"%s\"\n",pdefault,
               upsget_prod_dir(db,instance,command_line));
      addr=upsget_verbose(db,instance,command_line);
      if (strlen(addr))
      { fprintf((FILE *)stream,"%ssetenv UPS_VERBOSE %s\n",pdefault,addr);
      } else { 
        fprintf((FILE *)stream,"%sunsetenv UPS_VERBOSE\n",pdefault);
      }
      addr=upsget_extended(db,instance,command_line);
      if (strlen(addr))
      { fprintf((FILE *)stream,"%ssetenv UPS_EXTENDED %s\n",pdefault,addr); 
      } else { 
        fprintf((FILE *)stream,"%sunsetenv UPS_EXTENDED\n",pdefault);
      }
      fprintf((FILE *)stream,"%ssetenv UPS_THIS_DB %s\n",pdefault,
               upsget_this_db(db,instance,command_line));
      fprintf((FILE *)stream,"%ssetenv UPS_OS_FLAVOR %s\n",pdefault,
               upsget_OS_flavor(db,instance,command_line));
      fprintf((FILE *)stream,"%ssetenv UPS_PROD_FLAVOR %s\n",pdefault,
               upsget_flavor(db,instance,command_line));
      fprintf((FILE *)stream,"%ssetenv UPS_PROD_QUALIFIERS %s\n",pdefault,
               upsget_qualifiers(db,instance,command_line));
/*      fprintf((FILE *)stream,"%ssetenv UPS_SHELL %s\n",pdefault,
               upsget_shell(db,instance,command_line)); */
      addr=upsget_options(db,instance,command_line);
      if (addr) 
      { fprintf((FILE *)stream,"%ssetenv UPS_OPTIONS \"%s\"\n",pdefault,
		 addr); }
    }
  } 
}

char *upsget_translation_env( char * const oldstr )
{
  /* it will translate env.var in the passed string. if any translation
     is done, it will return a pointer to a static string containing the 
     translated string. if no translation is done, it will return (null).

     Note: right now it quit dumb, it will only recognize ${var},
     but should probaly also recognize ${ var }, $var, maybe even
     ${var1_${var2}} ... */

  static char buf[MAX_LINE_LEN];
  static char *buf2;
  static char env[48];
  static char error[51];
  static char *s_tok = "${";
  static char *e_tok = "}";  
  int clear_flag=0;                     /* This makes Eli happy... */
  char *s_loc = oldstr;
  char *e_loc = 0;
  char *tr_env = 0;
  
  buf[0] = 0; /* reset buf[0] */

  while ( s_loc && *s_loc && (e_loc = strstr( s_loc, s_tok )) != 0 ) 
  { if(!clear_flag)                   
    { memset( buf, 0, sizeof( buf ) );  /* clear ONLY if I need too... */
      clear_flag++;
    }
    memset( env, 0, sizeof( env ) );
    strncat( buf, s_loc, (unsigned int )(e_loc - s_loc) );  /* copy everything upto ${     */
    if (!(s_loc = strstr( e_loc, e_tok )))   /* set s_loc to end (finding })*/
    { upserr_add(UPS_NO_TRANSLATION, UPS_FATAL, e_loc);
      return 0;                              /* NO matching } */
    }
    e_loc += 2;                              /* Skip over the ${            */
    strncpy( env, e_loc, (unsigned int )(s_loc - e_loc) );    /* copy from there to } in env */
    if ( (tr_env = (char *)getenv( env )) )
    { strcat( buf, tr_env );
    } else {
      sprintf(error,"${%s}",env);
      upserr_add(UPS_NO_TRANSLATION, UPS_INFORMATIONAL, error);
    }
    /* move pass the '}' */
    ++s_loc;    
  }
  if ( buf[0] ) 
  { if ( s_loc )
    { strcat( buf, s_loc ); /* Something left after translation tack on */
    }
    if ((buf2=upsget_translation_tilde(buf))==0)
    { return buf;
    } else {
      return buf2;
    }
  } else {
    if ((buf2=upsget_translation_tilde(oldstr))==0)
    { return 0;
    } else {
      return buf2;
    }
  }
}
char *upsget_translation_tilde( char * const oldstr )
{
  static char buf[MAX_LINE_LEN];
  static char env[48];
  static char error[49];
  static char *e_tok = "/";  
  int clear_flag=0;                     /* This makes Eli happy... */
  char *s_loc = oldstr;
  char *e_loc = 0;
  char *tr_env = 0;
  
  buf[0] = 0; /* reset buf[0] */

  while ( s_loc && *s_loc && (e_loc = strstr( s_loc, TILDE)) != 0 ) 
  { if(!clear_flag)                   
    { memset( buf, 0, sizeof( buf ) );  /* clear ONLY if I need too... */
      clear_flag++;
    }
    memset( env, 0, sizeof( env ) );
    strncat( buf, s_loc, (unsigned int )(e_loc - s_loc) );    /* copy everything upto ~      */
    if (!(s_loc = strstr( e_loc, e_tok )))   /* set s_loc to end (finding /)*/
    { s_loc = strstr( e_loc, " ");           /* set s_loc to end space */
    }
/*    e_loc++;  oops tilde_dir does that... Skip over the ~             */
    if (s_loc)
    { strncpy( env, e_loc, (unsigned int )(s_loc - e_loc) );  /* copy from there to / in env */
    } else {
      strcpy(env,e_loc);
    }
    if ( (tr_env = (char *)upsget_tilde_dir( env )) )
    { strcat( buf, tr_env );
    } else {
      sprintf(error,"~%s",env);
      upserr_add(UPS_NO_TRANSLATION, UPS_INFORMATIONAL, error);
    }
  }
  if ( buf[0] ) 
  { if ( s_loc )
    { strcat( buf, s_loc ); /* Something left after translation tack on */
    }
    return buf;
  } else {
    return 0;
  }
}
char *upsget_translation( const t_upstyp_matched_instance * const minstance,
			  const t_upstyp_db *const db_info_ptr,
			  const t_upsugo_command * const command_line,
			  char * const oldstr )
{
  static char newstr[4096];
  static char holdstr[4096];
  char * loc;
  char * upto;
  char * eaddr;             /* end addr of } */
  int count=0;
  int found=0;
  int idx=0;
  int any=0;
  char * value;
  char * work;
  newstr[0] = '\0';
  if (!minstance) { return(oldstr); }
  /* work = (char *) malloc((size_t)(strlen(oldstr) +1)); */
  work=holdstr;
  strcpy(work,oldstr);
  upto = work;
  while ((loc = strstr(upto,UPSPRE))!= 0 ) 
  { count = ( loc - upto );
    strncat(newstr,upto,(unsigned int )count);
    upto += count;
    eaddr =strchr(upto,'}');
    *eaddr = '\0';
    found=0;
    for ( idx=0; g_var_subs[idx].string!=0; idx++) 
    { if (!strcmp(g_var_subs[idx].string,upto)) 
      { if (g_var_subs[idx].func)
        {  value=g_var_subs[idx].func(db_info_ptr,minstance,command_line);
           if(value) { strcat(newstr,value); }
        }
        found=1;
        any++;
      }
    }
    if (!found) { strcat(newstr,upto); strcat(newstr,"}"); } 
    *eaddr = '}';
    upto =strchr(upto,'}') + 1;
  }
  if (any)
  { strcat(newstr,upto);
  } else {
    strcpy(newstr,work);
    /* free(work); now static */
  }
  /* work = (char *) malloc((size_t)(strlen(newstr) +1)); */
  work=holdstr;
  strcpy(work,newstr);
  upto = work;
  newstr[0] = '\0';
  any=0;
  while ((loc = strstr(upto,"${PRODUCTS}"))!=0) 
  { count = ( loc - upto );
    strncat(newstr,upto,(unsigned int )count);
    upto += count;
    eaddr =strchr(upto,'}');
    *eaddr = '\0';
    found=0;
    if (!strcmp("${PRODUCTS",upto)) 
    { value=upsget_database(db_info_ptr,minstance,command_line);
      if(value) { strcat(newstr,value); }
      found=1;
      any++;
    } else { 
      strcat(newstr,upto); 
      strcat(newstr,"}");
    } 
    *eaddr = '}';
    upto = strchr(upto,'}') + 1;
  }
  if (any)
  { strcat(newstr,upto);
  } else {
    strcpy(newstr,work);
    /* free(work); no static */
  }
  /* work = (char *) malloc((size_t)(strlen(newstr) +1)); */
  work=holdstr;
  strcpy(work,newstr);
  upto = work;
  newstr[0] = '\0';
  any=0;
  while ((loc = strstr(upto,TILDE))!=0) 
  { count = ( loc - upto );
    strncat(newstr,upto,(unsigned int )count);
    upto += count;
    eaddr = strchr(upto,'/'); 
    *eaddr = '\0';
    found=0;
    if (strchr(upto,'~')) 
    { value=upsget_tilde_dir(upto);
      if(value) 
      { strcat(newstr,value); 
        strcat(newstr,"/");
      }
      found=1;
      any++;
    } else { 
      strcat(newstr,upto); 
      strcat(newstr,"/");
    } 
    *eaddr = '/';
    upto=eaddr+1;
    /* upto = strchr(upto,'/') + 1; */
  }
  if (any)
  { strcat(newstr,upto);
  } else {
    strcpy(newstr,work);
  }
  return newstr;
}

char *upsget_envstr(const t_upstyp_db * const db_info_ptr,
                    const t_upstyp_matched_instance * const instance,
                    const t_upsugo_command * const command_line )
{
  static char newstr[4096];
  static char *string = 0;
  get_element(string,product);
  strcpy(newstr,string);
  strcat(newstr," ");
  get_element(string,version);
  if (string) 
  { strcat(newstr,string);
  }
  strcat(newstr," -f ");
  get_element(string,flavor);
  strcat(newstr,string);
  if ( db_info_ptr )
  { strcat(newstr," -z ");
    strcat(newstr,db_info_ptr->name);
  }
  get_element(string,qualifiers);
  if (string && *string != '\0')
  { strcat(newstr," -q ");
    strcat(newstr,string);
  }
  if ( command_line->ugo_v)
  { strcat(newstr," -v "); }
  if ( command_line->ugo_j)
  { strcat(newstr," -j "); }
  if ( command_line->ugo_r)
  { strcat(newstr," -r ");
    strcat(newstr,command_line->ugo_productdir);
  }
  if ( command_line->ugo_m)
  { strcat(newstr," -m ");
    strcat(newstr,command_line->ugo_tablefile);
  }
  if ( command_line->ugo_M)
  { strcat(newstr," -M ");
    strcat(newstr,command_line->ugo_tablefiledir);
  }
  if ( command_line->ugo_O)
  { strcat(newstr," -O \"");
    strcat(newstr,command_line->ugo_options);
    strcat(newstr,"\"");
  }
  if ( command_line->ugo_e)
  { strcat(newstr," -e "); }
  return newstr;
}

char *upsget_archive_file(const t_upstyp_db * const db_info_ptr,
                          const t_upstyp_matched_instance * const instance,
                          const t_upsugo_command * const command_line,
                          const int strip )
{ static char *string;
  static char *env_string;
  static char buffer[1];
  static char *nostring=buffer;
  buffer[0]='\0';
  if (command_line && command_line->ugo_archivefile)
  { string=command_line->ugo_archivefile;
  } else { 
    get_element(string,archive_file);
  }
  if (!string)
  { string=buffer;
  } else {
    if((env_string=upsget_translation_env(string))!=0)
    { string=env_string;
    }
    if (strip)
    { if (!upsutl_strincmp(string,"ftp://",6))
      { strncpy(string,"123456",6); /* space ftp:// */
        if (strchr(string,'/'))
        { string=upsutl_str_create(strchr(string,'/'),'p');
        } else { 
          string=upsutl_str_create(string,' ');
        }
      }
    } else {
      string=upsutl_str_create(string,' ');
    }
  }
  return ( string ? string : nostring ) ;
}

char *upsget_prod_dir(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char *string;
  static char *prefix_string;
  static char buffer[1];
  static char *nostring=buffer;
  static char *env_string;
  buffer[0]='\0';
  if (command_line && command_line->ugo_r)
  { string=command_line->ugo_productdir;
  } else { 
    get_element(string,prod_dir);
  }
  if (!string)             /* The only why this could happen is if you */
  { string=buffer;         /* declare a product with no -r */
  } else {
    if((env_string=upsget_translation_env(string))!=0)
    { string=env_string;
    }
  }
  if (UPSRELATIVE(string))
  { if (db_info_ptr->config)
    { if (db_info_ptr->config->prod_dir_prefix)
      { prefix_string = upsutl_str_create(db_info_ptr->config->prod_dir_prefix,' '); 
        prefix_string = upsutl_str_crecat(prefix_string,"/"); 
        prefix_string = upsutl_str_crecat(prefix_string,string); 
        return prefix_string;
      } else { 
        return ( string ? string : nostring ) ;
      }
    } else { 
      return ( string ? string : nostring ) ;
    }
  } else {
    return ( string ? string : nostring ) ;
  }
}

char *upsget_ups_dir(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char *string;
  static char *prefix_string;
  static char *env_string;
  if (command_line && command_line->ugo_U)
  { string=command_line->ugo_upsdir;
  } else { 
    get_element(string,ups_dir);
  }
  if (string)            
  { if((env_string=upsget_translation_env(string))!=0)
    { string=env_string;
    }
  }
  if (UPSRELATIVE(string))
  { prefix_string = upsget_prod_dir(db_info_ptr, instance, command_line );
    prefix_string = upsutl_str_crecat(prefix_string,"/"); 
    prefix_string = upsutl_str_crecat(prefix_string,string); 
    return prefix_string;
  } else { 
    return string;
  }
} 

char *upsget_compile(const t_upstyp_db * const db_info_ptr,
                     const t_upstyp_matched_instance * const instance,
                     const t_upsugo_command * const command_line )
{ 
  static char newstr[4096];
  static char *string = 0;
  static char *env_string;
  *newstr='\0';
  if (!command_line->ugo_b) /* did they specify a compile file */
  {  get_element(string,compile_dir);
     if (string) 
     { if((env_string=upsget_translation_env(string))!=0)
       { string=env_string;
       }
       strcpy(newstr,string);
       strcat(newstr,"/");
     }
     get_element(string,compile_file);
     if (string) strcat(newstr,string);
  } else {
    if (command_line->ugo_u)
    { string=command_line->ugo_compile_dir;
      if((env_string=upsget_translation_env(string))!=0)
      { string=env_string;
      }
      strcpy(newstr,string); 
      strcpy(newstr,"/"); 
    }
    if (command_line->ugo_b)
    { strcpy(newstr,command_line->ugo_compile_file); 
    }
  }
  return newstr;
}
char *upsget_origin(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char *string;
  get_element(string,origin);
  return string;
}
char *upsget_product(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char *string;
  get_element(string,product);
  return string;
}
char *upsget_version(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char *string;
  static char *nostring='\0';
  get_element(string,version);
  return ( string ? string : nostring ) ;
}
char *upsget_flavor(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char *string;
  get_element(string,flavor);
  return string;
}
char *upsget_qualifiers(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char *string;
  get_element(string,qualifiers);
  return string;
}
char *upsget_shell(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char SH[]="sh";
  static char CSH[]="csh";
  if (command_line->ugo_shell != e_INVALID_SHELL )
  { if (!command_line->ugo_shell) 
    { return (SH);
    } else {
      return (CSH);
    }
  } else {
    return (0);
  }
}
char *upsget_source(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char SH[]=".";
  static char CSH[]="source";
  if (command_line->ugo_shell != e_INVALID_SHELL )
  { if (!command_line->ugo_shell) 
    { return (SH);
    } else {
      return (CSH);
    }
  } else {
    return (0);
  }
}
char *upsget_verbose(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char string[2];
  static char NOT[]="";
  if (command_line->ugo_v)
  { sprintf(string,"%.1d",command_line->ugo_v);
    return (string);
  } else {
    return(NOT);
  } 
}
char *upsget_extended(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char EXTENDED[]="1";
  static char NOT[]="";
  if (command_line->ugo_e)
  { return(EXTENDED);
  } else {
  } return(NOT);
}
char *upsget_options(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char NOT[]="";
  static char *string;
  if (command_line->ugo_O)
  { string=command_line->ugo_options;
  } else {
    string=NOT;
  }
  return(string);
}
char *upsget_database(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ t_upslst_item *db_list;
  t_upstyp_db *db;
  static char buffer[2028];
  static char *string=buffer;
  int count=0;
  buffer[0]='\0';
  for ( db_list=command_line->ugo_db; db_list; 
        db_list = db_list->next, count++ )
  { db=db_list->data;
    if(count) strcpy(string,":");
    strcpy(string,db->name);
  } return(string);     
} 
char *upsget_tilde_dir(char * const addr)
{
  struct passwd *pdp;
  static char buffer[FILENAME_MAX+1];
  char *name;
  buffer[0]='\0';
  if(strlen(addr)==1)
  { pdp = getpwuid(getuid());
    strcpy(buffer,pdp->pw_dir);
  } else { 
    name=addr+1;
    pdp = getpwnam(name);
    if (pdp)
    { strcpy(buffer,pdp->pw_dir);
    }
  }
  return(buffer);
}

char *upsget_this_db(const t_upstyp_db * const db_info_ptr,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ static char NOT[]="";
  static char *string;
/*  if (command_line->ugo_z) 
  { t_upslst_item *db_list=upslst_first(command_line->ugo_db);
    string=db_list->data-;
  } else {
*/
/* If there is no db_info_ptr they must have specified a -M table_file
   on the command line - or match is broken..;)
*/
    if ( db_info_ptr )
    { string=db_info_ptr->name;
    } else {
      string=NOT;
    }
  return(string);
}
char *upsget_OS_flavor(const t_upstyp_db * const db_info_ptr,
                       const t_upstyp_matched_instance * const instance,
                       const t_upsugo_command * const command_line )
{  static char uname_flavor[80];
   struct utsname baseuname;              /* returned from uname */
   static char *flavor;                /* flavor ptr */
   flavor = uname_flavor;                       /* init pointer */
   if (uname(&baseuname) == -1) return(0);     /* do uname */
   (void) strcpy (flavor,baseuname.sysname);    /* get sysname */
   if (!strncmp(flavor,"IRIX",4))               /* Slam all IRIXanything */
   { strcpy(flavor,"IRIX");
   }
   (void) strcat (flavor,"+");                  /* add plus */
   if (strncmp(baseuname.sysname,"AIX",3) == 0) /* because AIX is different */
   { (void) strcat(flavor,baseuname.version);  /* add in version */
     (void) strcat(flavor,".");
   }
   (void) strcat(flavor,baseuname.release);     /* add in release */
   return flavor;
}

/*  upsget_chain_file
 *
 * Read the specified chain file.
 *
 * Input : a database
 *         a product name
 *         a chain name
 * Output: pointer to the buffer with the file path in it
 * Return: a product structure read from the file
 */
t_upstyp_product *upsget_chain_file(const char * const a_db,
                                    const char * const a_prod,
                                    const char * const a_chain,
                                    char ** const a_buffer)
{
  int file_chars = 0;
  static char buffer[FILENAME_MAX+1];
  t_upstyp_product *read_product = NULL;

  file_chars = (int )(strlen(a_chain) + strlen(a_prod) + strlen(a_db) + 
               sizeof(CHAIN_SUFFIX) + 4);
  if (file_chars <= FILENAME_MAX) {
    sprintf(buffer, "%s/%s/%s%s", a_db, a_prod, a_chain, CHAIN_SUFFIX);
    read_product = upsfil_read_file(&buffer[0]);
    *a_buffer = buffer;
  } else {
    upserr_vplace();
    upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, file_chars);
    *a_buffer = 0;
  }

  return(read_product);
}
/*  upsget_version_file
 *
 * Read the specified version file.
 *
 * Input : a database
 *         a product name
 *         a version 
 * Output: pointer to the buffer with the file path in it
 * Return: a product structure read from the file
 */
t_upstyp_product *upsget_version_file(const char * const a_db,
                                      const char * const a_prod,
                                      const char * const a_version,
                                      char ** const a_buffer)
{
  int file_chars = 0;
  static char buffer[FILENAME_MAX+1];
  t_upstyp_product *read_product = NULL;

  file_chars = (int )(strlen(a_version) + strlen(a_prod) + strlen(a_db) + 
               sizeof(VERSION_SUFFIX) + 4);
  if (file_chars <= FILENAME_MAX) {
    sprintf(buffer, "%s/%s/%s%s", a_db, a_prod, a_version, VERSION_SUFFIX);
    read_product = upsfil_read_file(&buffer[0]);
    *a_buffer = buffer;
  } else {
    upserr_vplace();
    upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, file_chars);
    *a_buffer = 0;
  }

  return(read_product);
}

#define HASHATOM( k ) ((unsigned int)(k)>>2)

int upsget_key(const t_upstyp_instance * const instance)
{ const char *key=0;
  t_upstbl *g_ft = 0;
  static char buffer[MAX_LINE_LEN];
  char *skey=buffer;
  int i;
  static char *nostring = "";
  sprintf(skey,"%s%s%s%s",(instance->product ? instance->product : nostring),
                          (instance->version ? instance->version : nostring),
                          instance->flavor,
                          instance->qualifiers);
  if ( !g_ft )
  { g_ft = upstbl_new( 300 ); }
  key = upstbl_atom_string( skey );
  i = (int)HASHATOM( key )%g_ft->size;
  return(i);
}

/*-----------------------------------------------------------------------
 * upsget_man_source_dir
 *
 * Return a string containing the location of the products' man pages
 *
 * Input : an instance, and db configuration info
 * Output: none
 * Return: static string containing the location of the products' man pages
 */
char *upsget_man_source_dir( const t_upstyp_matched_instance * const a_inst,
                             const t_upstyp_db * const a_db_info,
                             const t_upsugo_command * const uc)
{
  t_upstyp_instance *vinst;
  t_upstyp_instance *tinst;
  char *man_source;                        /* man_source_dir translated      */
  char *prod_dir;                          /* full path to product           */

  g_buffer[0] = '\0';                      /* we want to fill this anew      */
 
  if ((tinst = a_inst->table))             /* See if we have a table file    */
  { if (tinst->man_source_dir)             /* Is the source in the table file*/
    { man_source =                         /* will return value translated   */
         upsget_translation(a_inst, a_db_info,               /* if necessary */
                            0, tinst->man_source_dir);
      if (UPSRELATIVE(man_source))         /* Is it a relative path?         */
      { prod_dir=upsget_prod_dir(a_db_info, a_inst, uc);   
	strcat(g_buffer, prod_dir);
	strcat(g_buffer, "/");
      } 
      strcat(g_buffer,man_source);
      strcat(g_buffer, "/");               /* safe then sorry...             */
      return ((char *)(&g_buffer[0]));     /* Bail out path from table       */
    }
  }

  /* the information we need is in the version file match */
  if ((vinst = a_inst->version))           /* Verify our version is there    */
  { if (vinst->ups_dir)                    /* Do we have a ups dir           */
    { if (UPSRELATIVE(vinst->ups_dir))     /* is ups_dir a relative path     */
      { prod_dir=upsget_prod_dir(a_db_info, a_inst, uc); 
        strcat(g_buffer, prod_dir);        /* full path to product           */
        strcat(g_buffer, "/");             /* add the /                      */
        strcat(g_buffer, vinst->ups_dir);  /* tack on the ups_dir specified  */
      } else { 
        strcat(g_buffer,vinst->ups_dir);   /* this is the whole path         */
      }
      strcat(g_buffer, "/toman/");         /* Add the default location       */
    } else {                               /* no UPS dir, PROD_DIR/ups/toman */
      prod_dir=upsget_prod_dir(a_db_info, a_inst, uc); 
      strcat(g_buffer, prod_dir);          /* full path to product           */
      strcat(g_buffer, "/ups/toman/");     /* add the /ups/toman default     */
    }
  }
  
  return ((char *)(&g_buffer[0]));
}
char *upsget_info_source_dir( const t_upstyp_matched_instance * const a_inst,
                              const t_upstyp_db * const a_db_info,
                              const t_upsugo_command * const uc)
{
  t_upstyp_instance *vinst;
  t_upstyp_instance *tinst;
  char *info_source;                       /* info_source_dir translated     */
  char *prod_dir;                          /* full path to product           */

  g_buffer[0] = '\0';                      /* we want to fill this anew      */
 
  if ((tinst = a_inst->table))             /* See if we have a table file    */
  { if (tinst->info_source_dir)            /* Is the source in the table file*/
    { info_source =                        /* will return value translated   */
         upsget_translation(a_inst, a_db_info,               /* if necessary */
                            0, tinst->info_source_dir);
      if (UPSRELATIVE(info_source))        /* Is it a relative path?         */
      { prod_dir=upsget_prod_dir(a_db_info, a_inst, uc); 
	strcat(g_buffer, prod_dir);
        strcat(g_buffer, "/");
      } 
      strcat(g_buffer,info_source);
      strcat(g_buffer, "/");               /* safe then sorry...             */
      return ((char *)(&g_buffer[0]));     /* Bail out path from table       */
    }
  }

  /* the information we need is in the version file match */
  if ((vinst = a_inst->version))           /* Verify our version is there    */
  { if (vinst->ups_dir)                    /* Do we have a ups dir           */
    { if (UPSRELATIVE(vinst->ups_dir))     /* is ups_dir a relative path     */
      { prod_dir=upsget_prod_dir(a_db_info, a_inst, uc); 
        strcat(g_buffer, prod_dir);        /* full path to product           */
        strcat(g_buffer, "/");             /* add the /                      */
        strcat(g_buffer, vinst->ups_dir);  /* tack on the ups_dir specified  */
      } else { 
        strcat(g_buffer,vinst->ups_dir);   /* this is the whole path         */
      }
      strcat(g_buffer, "/toinfo/");        /* Add the default location       */
    } else {                               /* no UPS dir, PROD_DIR/ups/toman */
      prod_dir=upsget_prod_dir(a_db_info, a_inst, uc); 
      strcat(g_buffer, prod_dir);          /* full path to product           */
      strcat(g_buffer, "/ups/toinfo/");    /* add the /ups/toinfo default    */
    }
  }
  
  return ((char *)(&g_buffer[0]));
}

/*-----------------------------------------------------------------------
 * upsget_table_file
 *
 * Given table file and the directories, find the absolute path
 * to the file.  This depends on the following hierarchy for places to look
 * for the existence of the table file  -
 *
 *    Look in each of the following successively till the file is found.  If
 *    the file is not found, it is an error.  If one of the pieces is missing,
 *    say - ups_dir - then that step is skipped.  NOTE: there is no default for
 *    the table file name. if the prod_dir_prefix is missing that step is
 *    still done with the prefix just left off.  if tablefiledir exists then
 *    only locations relative to it are checked.
 *
 *         tablefiledir/tablefile
 *         prod_dir_prefix/prod_dir/tablefiledir/tablefile
 *         ups_dir/tablefile
 *         prod_dir_prefix/prod_dir/ups_dir/tablefile
 *         db/prodname/tablefile
 *         tablefile
 * 
 *
 * Input : product name
 *         table file name
 *         table file directory
 *         ups directory information
 *         product dir
 *         ups database directory
 *         a flag stating if the directory returned must already contain the
 *             table file
 * Output: none
 * Return: table file location
 */
#define LOOK_FOR_FILE()   \
   if ((! a_exist_flag) || (upsutl_is_a_file(buffer)) == UPS_SUCCESS) {  \
     path_ptr = buffer;                                                  \
     found = 1;                                                          \
     upsver_mes(1, "%sFound table file in '%s'\n", g_UPSGET, buffer);    \
   } else {                                                              \
     upsver_mes(1, "%sCould not find table file in '%s'\n", g_UPSGET,    \
                buffer);                                                 \
   }

#define TRANSLATE_ENV(dst_var, source_var)  \
   dst_var = upsget_translation_env((char *)source_var);                    \
   dst_var = dst_var ? upsutl_str_create(dst_var, ' ') : (char *)source_var;

char *upsget_table_file( const char * const a_prodname,
                         const char * const a_tablefile,
                         const char * const a_tablefiledir,
                         const char * const a_upsdir,
                         const char * const a_productdir,
                         const t_upstyp_db * const a_db_info,
                         const int a_exist_flag)
{
  static char buffer[FILENAME_MAX+1];   /* max size of file name and path 
					   on system */
  char *path_ptr = NULL;
  int file_chars = 0, total_chars = 0;
  int found = 0;
  char *tmp_tfd = NULL, *tmp_ud = NULL, *tmp_pd = NULL;

  if (a_tablefile != NULL) {
    file_chars = (int )strlen(a_tablefile) + 2;  /* length plus trailing null 
						    and leading '/' */
    /* try tablefiledir/tablefile */
    if (a_tablefiledir != NULL) {
      /* first translate any environmental variables */
      TRANSLATE_ENV(tmp_tfd, a_tablefiledir);

      if (!UPSRELATIVE(tmp_tfd)) {
	if ((total_chars = file_chars + (int )strlen(tmp_tfd))
	    <= FILENAME_MAX) {
	  sprintf(buffer, "%s/%s", tmp_tfd, a_tablefile);
	  LOOK_FOR_FILE();
	} else {
	  upserr_vplace();
	  upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	}
      }
      /* try prod_dir/table_file_dir/tablefile */
      if ((found == 0) && (a_productdir != NULL) && UPSRELATIVE(tmp_tfd)) {
	/* first translate any environmental variables */
	TRANSLATE_ENV(tmp_pd, a_productdir);

	if (a_db_info->config && a_db_info->config->prod_dir_prefix &&
	    UPSRELATIVE(tmp_pd)) {
	  if ((total_chars += (int )strlen(tmp_pd) + 
	       (int )strlen(a_db_info->config->prod_dir_prefix) +
	       1) <= FILENAME_MAX) {
	    sprintf(buffer, "%s/%s/%s/%s", a_db_info->config->prod_dir_prefix,
		    tmp_pd, tmp_tfd, a_tablefile);
	    LOOK_FOR_FILE();
	  } else {
	    upserr_vplace();
	    upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	  }
	} else {
	  if ((total_chars += (int )strlen(tmp_pd) + 1)
	      <= FILENAME_MAX) {
	    sprintf(buffer, "%s/%s/%s", tmp_pd, tmp_tfd, a_tablefile);
	    LOOK_FOR_FILE();
	  } else {
	    upserr_vplace();
	    upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	  }
	}
      }
    } else { /* if (a_tablefiledir != NULL) */
      /* first translate any environmental variables */
      TRANSLATE_ENV(tmp_ud, a_upsdir);
      TRANSLATE_ENV(tmp_pd, a_productdir);

      /* try db/prod_name/tablefile */
      if ((a_db_info != NULL) && (a_prodname != NULL)) {
	if ((total_chars = file_chars + (int )(strlen(a_prodname) +
					       strlen(a_db_info->name)) + 1)
	    <= FILENAME_MAX) {
	  sprintf(buffer, "%s/%s/%s", a_db_info->name, a_prodname,
		  a_tablefile);
	  LOOK_FOR_FILE();
	} else {
	  upserr_vplace();
	  upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	}
      }
      /* try ups_dir/tablefile */
      if ((found == 0) && (tmp_ud != NULL) && (!UPSRELATIVE(tmp_ud))) {
	if ((total_chars = file_chars + (int )strlen(tmp_ud))
	    <= FILENAME_MAX) {
	  sprintf(buffer, "%s/%s", tmp_ud, a_tablefile);
	  LOOK_FOR_FILE();
	} else {
	  upserr_vplace();
	  upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	}
      }
      /* try prod_dir/ups_dir/tablefile */
      if ((found == 0) && (tmp_ud != NULL) && (tmp_pd != NULL) &&
	  UPSRELATIVE(tmp_ud)) {
	if (a_db_info->config && a_db_info->config->prod_dir_prefix &&
	    UPSRELATIVE(tmp_pd)) {
	  if ((total_chars = file_chars + (int )strlen(tmp_ud) + 
	       (int )strlen(tmp_pd) + 
	       (int )strlen(a_db_info->config->prod_dir_prefix) +
	       3) <= FILENAME_MAX) {
	    sprintf(buffer, "%s/%s/%s/%s", a_db_info->config->prod_dir_prefix,
		    tmp_pd, tmp_ud, a_tablefile);
	    LOOK_FOR_FILE();
	  } else {
	    upserr_vplace();
	    upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	  }
	} else {
	  if ((total_chars = file_chars + (int )strlen(tmp_ud) +
	       (int )strlen(tmp_pd) + 2) <= FILENAME_MAX) {
	    sprintf(buffer, "%s/%s/%s", tmp_pd, tmp_ud, a_tablefile);
	    LOOK_FOR_FILE();
	  } else {
	    upserr_vplace();
	    upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
	  }
	}
      }
      /* try tablefile */
      if (found == 0) {
        if ((total_chars = file_chars) <= FILENAME_MAX) {
          sprintf(buffer, "%s", a_tablefile);
          LOOK_FOR_FILE();
        } else {
          upserr_vplace();
          upserr_add(UPS_FILENAME_TOO_LONG, UPS_FATAL, total_chars);
        }
      }
    }
  }

  /* cleanup */
  if (tmp_pd && (tmp_pd != a_productdir)) {
    upsmem_free(tmp_pd);
  }
  if (tmp_ud && (tmp_ud != a_upsdir)) {
    upsmem_free(tmp_ud);
  }
  if (tmp_tfd && (tmp_tfd != a_tablefiledir)) {
    upsmem_free(tmp_tfd);
  }
return path_ptr;
}
