/************************************************************************
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
#include <string.h>
#include <ctype.h>

/* ups specific include files */

#include "ups.h"
 
/*
 * Definition of public variables.
 */

/*
 * Private constants
 */

#define UPSPRE "${UPS_"

#define get_element(STRING,ELEMENT) \
{  if(instance->chain)                                 \
    { if (instance->chain->ELEMENT)                     \
      { STRING=instance->chain->ELEMENT;         \
      } else {                                          \
        if(instance->version)                           \
        { if (instance->version->ELEMENT)               \
          { STRING=instance->version->ELEMENT;   \
          } else {                                      \
            if (instance->table)                        \
            { if (instance->table->ELEMENT)             \
              { STRING=instance->table->ELEMENT; \
              } else {                                  \
                STRING=0;                        \
              }                                         \
            } else {                                    \
              STRING=0;                          \
            }                                           \
          }                                             \
        } else {                                        \
          if (instance->table)                          \
          { if (instance->table->ELEMENT)               \
            { STRING=instance->table->ELEMENT;   \
            } else {                                    \
              STRING=0;                          \
            }                                           \
          } else {                                      \
            STRING=0;                            \
          }                                             \
        }                                               \
      }                                                 \
    } else {                                            \
      if(instance->version)                             \
      { if (instance->version->ELEMENT)                 \
        { STRING=instance->version->ELEMENT;     \
        } else {                                        \
          if (instance->table)                          \
          { if (instance->table->ELEMENT)               \
            { STRING=instance->table->ELEMENT;   \
            } else {                                    \
              STRING=0;                          \
            }                                           \
          } else {                                      \
            STRING=0;                            \
          }                                             \
        }                                               \
      } else {                                          \
        if (instance->table)                            \
        { if (instance->table->ELEMENT)                 \
          { STRING=instance->table->ELEMENT;     \
          } else {                                      \
            STRING=0;                            \
          }                                             \
        } else {                                        \
          STRING=0;                              \
        }                                               \
      }                                                 \
    }                                                   \
}


typedef char * (*var_func)(const t_upstyp_matched_product * const product,
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
  { "${UPS_OS_FLAVOR", 0 },
  { "${UPS_PROD_QUALIFIERS", upsget_qualifiers },
  { "${UPS_PROD_DIR", upsget_prod_dir },
  { "${UPS_SHELL", upsget_shell },
  { "${UPS_OPTIONS", upsget_options },
  { "${UPS_VERBOSE", upsget_verbose },
  { "${UPS_EXTENDED", 0 },
  { "${UPS_FLAGS", 0 },
  { "${UPS_FLAGSDEPEND", 0 },
  { "${UPS_THIS_DB", 0 },
  {  0, 0 }
} ;


/*
 * Definition of public functions.
 */

char *upsget_translation( const t_upstyp_matched_product * const product,
                  const t_upsugo_command * const command_line,
                  char * const oldstr )
{
  t_upslst_item *inst_list;
  t_upstyp_matched_instance *instance;
  static char newstr[4096];
  char * loc;
  char * upto;
  char * eaddr;             /* end addr of } */
  int count=0;
  int found=0;
  int idx=0;
  int any=0;
  char * value;
  newstr[0] = '\0';
  upto = oldstr;
  inst_list = product->minst_list;
  instance = (t_upstyp_matched_instance *)(inst_list->data);
  while ((loc = strstr(upto,UPSPRE))!= 0 ) 
  { count = ( loc - upto );
    strncat(newstr,upto,count);
    upto += count;
    eaddr =strchr(upto,'}');
    *eaddr = '\0';
    found=0;
    for ( idx=0; g_var_subs[idx].string!=0; idx++) 
    { if (!strcmp(g_var_subs[idx].string,upto)) 
      { if (g_var_subs[idx].func)
        {  value=g_var_subs[idx].func(product,instance,command_line);
           if(value) strcat(newstr,value);
        }
        found=1;
        any++;
      }
    }
    if (!found) { strcat(newstr,upto); strcat(newstr,"}");} 
    *eaddr = '}';
    upto =strchr(upto,'}') + 1;
  }
  if (any)
  { strcat(newstr,upto);
    return newstr;
  } else {
    return 0;
  }
}

char *upsget_prod_dir(const t_upstyp_matched_product * const product,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ char *string;
  get_element(string,prod_dir);
  return string;
}
char *upsget_product(const t_upstyp_matched_product * const product,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ char *string;
  get_element(string,product);
  return string;
}
char *upsget_version(const t_upstyp_matched_product * const product,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ char *string;
  get_element(string,version);
  return string;
}
char *upsget_flavor(const t_upstyp_matched_product * const product,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ char *string;
  get_element(string,flavor);
  return string;
}
char *upsget_qualifiers(const t_upstyp_matched_product * const product,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ char *string;
  get_element(string,qualifiers);
  return string;
}
char *upsget_shell(const t_upstyp_matched_product * const product,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ 
  if (command_line->ugo_shell != e_INVALID_SHELL )
  { if (!command_line->ugo_shell) 
    { return ("SH");
    } else {
      return ("CSH");
    }
  }
}
char *upsget_verbose(const t_upstyp_matched_product * const product,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ char string[2];
  if (command_line->ugo_v)
  { sprintf(string,"%.1d",command_line->ugo_v);
  } else {
    sprintf(string,"");
  } return string;
}
char *upsget_options(const t_upstyp_matched_product * const product,
                      const t_upstyp_matched_instance * const instance,
                      const t_upsugo_command * const command_line )
{ if (command_line->ugo_O)
  { return command_line->ugo_options;
  } else {
    return 0;
  }
}
