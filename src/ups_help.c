/************************************************************************
 *
 * FILE:
 *       ups_help.c
 * 
 * DESCRIPTION: 
 *       Will read help file and output help...
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
 *       Sep 17 1997, DjF, First.
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* UPS includes */
#include "ups_types.h"
#include "ups_utils.h"
/*
 * Definition of public variables
 */

/*
 * Declaration of private functions
 */

/*
 * Definition of global variables
 */
/*
 * Definition of public functions
 */

/*-----------------------------------------------------------------------
 * upshlp_command
 *
 */

int upshlp_command(const char * const what) 
{
    FILE    *fh = 0;                  /* file handle */
    char    line[MAX_LINE_LEN] = "";  /* current line */
    int     oncommands=0;             /* on commands */
    int     onoptions=0;              /* on options */
    struct  upslst_item * command_options=0;
    struct  upslst_item * command_description=0;
    struct  upslst_item * option_list=0;
    struct  upslst_item * l_ptr=0;
    char    * addr=0;
    char    * data;
    char    * command;
    int     count=0;
    char    * option;
    char    * last ;

/* test
    char    *what = "compile";
*/

  last =  (char *) malloc(13);

  fh = fopen ( "/home/t2/fagan/erupt/src/ups_help.dat", "r" );
  while ( fgets( line, MAX_LINE_LEN, fh ) ) {
    if ( line[0] != '#' ) {
       if ( !strncmp(line,"COMMAND",7)) {
          oncommands=1;
       } else { 
         if ( !strncmp(line,"OPTIONS",7)) {
            onoptions=1;
            oncommands=0;
         } else { 
           if (oncommands) {
              if ( line[0] != ':' ) {        /* new command */
                 line[12] = '\0';
                 command = upsutl_str_create(line,' ');
                 data = upsutl_str_create(line,' ');
                 if ((addr = strchr(&line[13],':'))!=0 ) *addr=0;
                 data = upsutl_str_crecat(command,&line[13]);
                 command_options=upslst_add(command_options,data);
              } else { 
                  data = upsutl_str_crecat(command,line);
                  command_description=upslst_add(command_description,data);
              }
           } else { 
             if (onoptions) {
                if ( line[0] == '-' ) {        /* new command */
                   line[2] = '\0';
                   option = upsutl_str_create(line,' ');
                   data = upsutl_str_crecat(option,&line[3]);
                   option_list=upslst_add(option_list,data);
                } else { 
                   data = upsutl_str_crecat(option,line);
                   option_list=upslst_add(option_list,data);
                }

             } else {
                fprintf(stdout,"Unrecognized data in UPS help file\n");
                fprintf(stdout,"%s\n",line);
                return 1;
             }
           }
         }
       }
    }
  }
  if ( !what || strlen( what ) <= 0 ) 
  { fprintf(stdout,"UPS commands:\n\n");
    fprintf(stdout,"for specific command options\n");
    fprintf(stdout,"use ups help COMMAND, or COMMAND -?\n\n");
    for ( l_ptr = upslst_first(command_description); 
          l_ptr; l_ptr = l_ptr->next, count++ )
    { if (strncmp(last,l_ptr->data,12)) {
         fprintf(stdout,"          %s",(char *)l_ptr->data ); 
      } else {
         fprintf(stdout,"                       %s",(char *)l_ptr->data+13 ); 
      } strncpy(last,l_ptr->data,12);
    }
  } else { 
    fprintf(stdout,"UPS help command: %s\n\n",what);
    for ( l_ptr = upslst_first(command_description); 
          l_ptr; l_ptr = l_ptr->next, count++ )
    { if (!strncmp(l_ptr->data,what,strlen(what))) {
         fprintf(stdout,"          %s",(char *)l_ptr->data+13 );
      }
    }
    fprintf(stdout,"\n     Valid Options:\n");
    for ( l_ptr = upslst_first(command_options); 
          l_ptr; l_ptr = l_ptr->next, count++ )
    { if (!strncmp(l_ptr->data,what,strlen(what))) {
/*         fprintf(stdout,"          %s",(char *)l_ptr->data+13 ); */
         addr=l_ptr->data;
         option=addr+13;
      }
    }
    for ( l_ptr = upslst_first(option_list); 
          l_ptr; l_ptr = l_ptr->next, count++ )
    { addr=l_ptr->data;  
      *(addr+2)=' ';
        if (strchr(option,(int)*(addr+1))) {
           if (*(addr+1)==*last) { 
              *addr=' ';
              *(addr+1)=' ';
           } else { 
             *last=*(addr+1);
           }
           fprintf(stdout,"          %s",(char *)l_ptr->data);
        }
    }
  }
/* test dump...
  upsugo_prtlst(command_options,"options");
  upsugo_prtlst(command_description,"description");
  upsugo_prtlst(option_list,"options");
*/
 return 0;
}
