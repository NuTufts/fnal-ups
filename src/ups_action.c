/************************************************************************
 *
 * FILE:
 *       ups_action.c
 * 
 * DESCRIPTION: 
 *       This file contains routines to manage ups action lines.
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
 *       28-Aug-1997, EB, first
 *
 ***********************************************************************/

/* standard include files */
#include "string.h"
#include "ctype.h"

/* ups specific include files */
#include "ups_action.h"
#include "ups_error.h"
#include "ups_list.h"
#include "ups_utils.h"
#include "ups_memory.h"

/*
 * Definition of public variables.
 */

/* These actions are listed in order of use.  Hopefully the more used
   actions are at the front of the list. Also the ones most used by setup
   and unsetup are at the front of the array */
char *ups_actions[] = {
  /* 0 */      "setupoptional",
  /* 1 */      "setuprequired",
  /* 2 */      "unsetupoptional",
  /* 3 */      "unsetuprequired",
  /* 4 */      "envappend",
  /* 5 */      "envremove",
  /* 6 */      "envprepend",
  /* 7 */      "envset",
  /* 8 */      "envunset",
  /* 9 */      "noproddir",
  /* 10 */     "source",
  /* 11 */     "sourcecheck",
  /* 12 */     "exeaccess",
  /* 13 */     "execute",
  /* 14 */     "filetest",
  /* 15 */     "copyhtml",
  /* 16 */     "copyinfo",
  /* 17 */     "copyman",
  /* 18 */     "copynews",
  /* 19 */     "dodefaults",
  /* 21 */     "nodefaults",
  /* 22 */     "nosetupenv",
	       NULL };


/*
 * Declaration of private functions.
 */

/*
 * Definition of global variables.
 */



/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upsact_parse
 *
 * Given an action return the parameters as a separate string, and an integer
 * corresponding to the action string.  In later routines more strcmp's can
 * be avoided.
 *
 * Input : action line
 * Output: action parameter string, integer action value
 * Return: error value or UPS_SUCCESS
 */
int upsact_parse( char * const a_action_line, char ** const a_params,
		  int * const a_action_val)
{
  int rstatus = UPS_SUCCESS;          /* assume success */
  char *tmpBuf = NULL, *tmpBuf2 = NULL;
  char *strPtr = NULL;
  int match_found = 0, i, len;

  /* First split the line to separate the action from the parameters. Locate
     the first '(' as that will divide the action from it's parameters */
  if ((strPtr = strchr(a_action_line, OPEN_PAREN)) != NULL) {
    len = strPtr - a_action_line;       /* get length of action name */

    /* get a new string and remove any surrounding whitespace */
    *strPtr = '\0';
    if ((tmpBuf = upsutl_str_create(a_action_line, 't')) != NULL) {
      *strPtr = OPEN_PAREN;         /* restore the string */
      
      /* convert to lowercase for the compare */
      for (i = 0; i < len ; ++i) {
	tmpBuf[i] = (char )tolower((int )tmpBuf[i]);
      }

      /* look for this action in the supported action array */
      for (i = 0; ups_actions[i]; ++i) {
	if (! strcmp(tmpBuf, ups_actions[i])) {
	  /* we found a match.  create a new string with these parameters.
	     note - it does not include an open parenthesis */
	  ++match_found;
	  if ((tmpBuf2 = upsutl_str_create((char *)&a_action_line[len+1], 't'))
	      != NULL) {
	    *a_params = tmpBuf2;
	    *a_action_val = i;         /* save the location in the array */
	  } else {
	    rstatus = UPS_LINE_TOO_LONG;
	  }
	  break;
	}
      }
      upsmem_free(tmpBuf);                 /* clean up from str_create */

      /* see if we found a match, if not - return an error */
      if (! match_found) {
	rstatus = UPS_INV_ACTION;
      }
    } else {
      rstatus = UPS_LINE_TOO_LONG;
    }
  } else {
    /* This is an incorrectly formatted line */
    rstatus = UPS_INV_ACTION;
  }

  return rstatus;
}



