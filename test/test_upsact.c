/************************************************************************
 *
 * FILE:
 *       test_ups_action.c
 * 
 * DESCRIPTION: 
 *       Test ups_action routines.
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
 *       02-Sept-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>
#include <string.h>

/* ups specific include files */
#include "upsfil.h"
#include "upserr.h"
#include "upsact.h"
#include "upslst.h"
#include "upsact.h"
#include "upsugo.h"
#include "upsmat.h"
#include "ups_list.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */
static void test_upsact_parse(void);
static void test_upsact_params(void);
static void print_action( const char * const a_action_line,
		   const char * const a_params, const int a_action_val);

/*
 * Definition of global variables.
 */

#ifndef NULL
#define NULL 0
#endif

void print_cmd( t_upsact_cmd *cmd )
{
  int i = 0;
  if ( !cmd )
    return;

  printf( "\nicmd = %d\n", cmd->icmd );
  for ( i=0; i<cmd->argc; i++ ) {
    printf( "   %d = %s\n", i, cmd->argv[i] );
  }
}

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * main
 *
 * test ups_action routines
 *
 */
int  main( const int argc, char * const argv[] )
{
  /*
  int i = 0;
  t_ups_product *prod_ptr = NULL;
  t_upslst_item *inst_ptr = NULL;
  t_upslst_item *act_ptr = NULL;
  t_ups_instance *inst;
  t_ups_action *act;


  if ( argc <= 1 ) {
    printf( "Usage: test_upsact table_file\n" );
    exit( 1 );
  }

  for ( i=1; i<argc; i++ ) {
    
    prod_ptr = upsfil_read_file( argv[i] );  

    if ( prod_ptr ) {
      inst_ptr = upslst_first( prod_ptr->instance_list );
      for( ; inst_ptr; inst_ptr = inst_ptr->next ) {
	inst = (t_ups_instance *)inst_ptr->data;
	act_ptr = upslst_first( inst->action_list );
	for( ; act_ptr; act_ptr = act_ptr->next ) {
	  act = (t_ups_action *)act_ptr->data;
	  upsact_translate( inst, act->action );
	}
      }
    }
  }
  */


  /*  test_upsact_parse();
  upserr_output();
  */
  test_upsact_parse();

  upserr_output();
  
  return 0;
}

/*
 * Definition of private functions.
 */

/*-----------------------------------------------------------------------
 * test_upsact_parse
 *
 * test the routine that parses the actions. for each supported action, call
 * the parsing routine and examine the output.
 *
 * Input : none
 * Output: none
 * Return: none
 */
static void test_upsact_parse(void)
{
  char *params = NULL;
  int action_val = -1, i;
  char *action_line[] = {
    "FILETEST(myfile  ,\"-w  \",  \"can't touch this\"  )",
    "SETUPOPTIONAL(\"-d -f purina -q siamese kitty \")",
    "SETUPREQUIRED(\"-d -f purina -q \"tabby, tiger\" kitty \")",
    "UNSETUPOPTIONAL(\"-d -f $OS_FLAVOR -q siamese kitty \")",
    " UNSETUPREQUIRED(\" kitty \")  ",
    "ENVAPPEND(envVar, \"moses supposes\", \"@\")",
    "ENVREMOVE(envVar, \"his toeses are roses\", \"@\")",
    "ENVPREPEND(envVar, \"but moses supposes erroneously\")",
    "ENVSET(envVar, \"for moses he knowses\")",
    "ENVUNSET(envVar)",
    "NOPRODDIR()",
    "EXEACCESS(dothis, setthis)",
    "SOURCE(myscript.$UPS_SHELL)",
    "SOURCECHECK(myscriptcheck.$UPS_SHELL)",
    "EXECUTE(onlythis)",
    "COPYHTML()",
    "COPYINFO($PROD_DIR/info)",
    "COPYMAN($PROD_DIR/man)",
    "COPYNEWS()",
    "DODEFAULTS()",
    "NODEFAULTS()",
    "NOSETUPENV()",
    "NOBOYO()",
    "COPYNEWS",
    NULL
};
t_upsact_cmd *cmd;
t_upsugo_command *ugo_cmd = 0;
t_upslst_item *mproduct_list;
t_upstyp_action *act;
t_upslst_item *l_dep;
  
  for ( i = 0; action_line[i]; i++ ) {  
    if ( !(cmd = upsact_parse_cmd( action_line[i] )) ) {
      printf("\nInvalid action - %s\n", action_line[i]);
    }
    else {
      /* print_cmd( cmd ); */
    }
  }
  ugo_cmd = upsugo_bldcmd( "-c -f IRIX  exmh",
			   "zAacCdfghKtmMNoOPqrTuUv?" );
  
  upsugo_dump( ugo_cmd, 1);
  mproduct_list = upsmat_instance( ugo_cmd, NULL, 1 );
  list_output( upslst_first( mproduct_list ), ugo_cmd);

  l_dep = upsact_get_cmd( ugo_cmd, 0, "setup" );
  l_dep = upslst_first( l_dep );
  for ( ; l_dep; l_dep = l_dep->next ) {
    upsact_print_item( l_dep->data );
  }
}

/*-----------------------------------------------------------------------
 * test_upsact_params
 *
 * test the routine that parses the parameters.  keep taking string input from
 * the command line until a null line is received.
 *
 * Input : none
 * Output: none
 * Return: none
 */
/*
static void test_upsact_params(void)
{
  char cmd_line[500];
  char *cmd_line_ptr = NULL;
  t_upslst_item *param_list = NULL, *tmp_item;

  while (1) {
    cmd_line_ptr = gets(&cmd_line[0]);
    if (strlen(cmd_line_ptr) > 0) {
      param_list = upsact_params( cmd_line_ptr );
      param_list = upslst_first( param_list );
      printf("\nFor string = %s  params are:\n", cmd_line_ptr);
      for (tmp_item = param_list ; tmp_item ; tmp_item = tmp_item->next) {
	printf("%s\n", (char *)tmp_item->data);
      }
      param_list = upslst_free(param_list, 'd');
    } else {
      break;
    }
  }

}
*/
/*-----------------------------------------------------------------------

 * print_action_info
 *
 * print the passed in action information
 *
 * Input : action line, parameters, value
 * Output: none
 * Return: none
 */
static void print_action( const char * const a_action_line,
			  const char * const a_params, const int a_action_val)
{

  if (a_action_line) {
    printf("\nACTION LINE = %s\n", a_action_line);
    if (a_params) {
      printf("PARAMS = %s   ", a_params);
    }
    if (a_action_val >= 0) {
      printf("VALUE = %d\n", a_action_val);
    }
  }
}





