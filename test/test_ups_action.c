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

/* ups specific include files */
#include "ups_error.h"
#include "ups_action.h"

/*
 * Definition of public variables.
 */

/*
 * Declaration of private functions.
 */
static void test_upsact_parse(void);
static void print_action( const char * const a_action_line,
		   const char * const a_params, const int a_action_val);

/*
 * Definition of global variables.
 */

#ifndef NULL
#define NULL 0
#endif

/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * main
 *
 * test ups_action routines
 *
 */
int main (int argc, char *argv[])
{

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
    "SETUPOPTIONAL(\"-d -f purina -q siamese kitty \")",
    "SETUPREQUIRED(\"-d -f purina -q \"tabby, tiger\" kitty \")",
    "UNSETUPOPTIONAL(\"-d -f $OS_FLAVOR -q siamese kitty \")",
    "UNSETUPREQUIRED(\" kitty \")",
    "ENVAPPEND(envVar, \"moses supposes\", \"@\")",
    "ENVREMOVE(envVar, \"his toeses are roses\", \"@\")",
    "ENVPREPEND(envVar, \"but moses supposes erroneously\")",
    "ENVSET(envVar, \"for moses he knowses\")",
    "ENVUNSET(envVar)",
    "NOPRODDIR()",
    "SOURCE(myscript.$SHELL)",
    "SOURCECHECK(myscriptcheck.$SHELL)",
    "EXEACCESS(dothis, setthis)",
    "EXECUTE(onlythis)",
    "FILETEST(myfile, \"-w\", \"can't touch this\")",
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

  for (i = 0; action_line[i]; ++i) {
    if (upsact_parse(action_line[i], &params, &action_val) == UPS_SUCCESS) {
      print_action(action_line[i], params, action_val);
    } else {
      printf("\nInvalid action - %s\n", action_line[i]);
    }
  }
}

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










