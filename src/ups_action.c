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

static char *dquote = "\"";
static char *comma = ",";
static char *wspace = " \t\n\r\f";

/* These actions are listed in order of use.  Hopefully the more used
   actions are at the front of the list. Also the ones most used by setup
   and unsetup are at the front of the array.  The actions in this array
   MUST appear in the same order in the following enumeration */
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
  /* 20 */     "nodefaults",
  /* 21 */     "nosetupenv",
	       NULL };


enum action {
  e_setupoptional,
  e_setuprequired,
  e_unsetupoptional,
  e_unsetuprequired,
  e_envappend,
  e_envremove,
  e_envprepend,
  e_envset,
  e_envunset,
  e_noproddir,
  e_source,
  e_sourcecheck,
  e_exeaccess,
  e_execute,
  e_filetest,
  e_copyhtml,
  e_copyinfo,
  e_copyman,
  e_copynews,
  e_dodefaults,
  e_nodefaults,
  e_nosetup
} ;


/*
 * Declaration of private functions.
 */

int upsact_params(const char * const a_params,
			 t_upslst_item ** const a_param_list);

/*
 * Definition of global variables.
 */



/*
 * Definition of public functions.
 */

/*-----------------------------------------------------------------------
 * upsact_code
 *
 * Given an action, it's parameters and a stream pointer, write the shell 
 * or cshell code corresponding to the passed action to the stream.
 *
 * Input : action value, action parameters, stream pointer
 * Output: none
 * Return: error value or UPS_SUCCESS
 */
int upsact_code()
{
  

  /*  switch (a_action_val) {
  case e_setupoptional:
    break;
  case e_setuprequired:
    break;
  case e_unsetupoptional:
    break;
  case e_unsetuprequired:
    break;
  case e_envappend:
    break;
  case e_envremove:
    break;
  case e_envprepend:
    break;
  case e_envset:
    break;
  case e_envunset:
    break;
  case e_noproddir:
    break;
  case e_source:
    break;
  case e_sourcecheck:
    break;
  case e_exeaccess:
    break;
  case e_execute:
    break;
  case e_filetest:
    break;
  case e_copyhtml:
    break;
  case e_copyman:
    break;
  case e_copynews:
    break;
  case e_dodefaults:
    break;
  case e_nodefaults:
    break;
  case e_nosetup:
    break;
  default:
    break;
  }*/
}

/*-----------------------------------------------------------------------
 * upsact_params
 *
 * Given an action's parameters split them up into separate params and
 * return a linked list of the separate parameters.  The parameters are
 * separated by commas, but ignore commas within quotes.
 *
 * Input : parameters
 * Output: linked list of parameters
 * Return: error code
 */
int upsact_params(const char * const a_params,
		  t_upslst_item ** const a_param_list)
{
  int rstatus = UPS_SUCCESS;
  char *ptr, *new_string, *saved_ptr = NULL, *new_ptr;

  ptr = (char *)a_params;

  while (*ptr) {
    if (! strncmp(ptr, dquote, 1)) {
      /* this may be the beginning of the line, saved_ptr is not set yet so do
	 it now. */
      if (! saved_ptr) {
	saved_ptr = ptr;           /* the beginning of a new parameter */
      }
      /* found a double quote, skip to next double quote */
      if ((new_ptr = strchr(++ptr, (int)(*dquote))) == NULL) {
	/* did not find another quote  - take the end of the line as end of
	   string and end of param list */
	if (new_ptr != ptr) {
	  /* only do this if we actually have a param */
	  new_string = upsutl_str_create(ptr, STR_TRIM_DEFAULT);
	  upsutl_str_remove_edges(new_string, wspace);
	  *a_param_list = upslst_add(*a_param_list, new_string);
	  saved_ptr = NULL;         /* no longer valid, we got the param */
	  break;                    /* all done */
	}
      } else {
	/* point string just past double quote */
	ptr = ++new_ptr;
      }
    } else if (! strncmp(ptr, comma, 1)) {
      /* found a comma */
      if (saved_ptr) {
	/* we have a param, add it to the list */
	*ptr = NULL;                   /* temporary so only take param */
	new_string = upsutl_str_create(saved_ptr, STR_TRIM_DEFAULT);
	upsutl_str_remove_edges(new_string, wspace);
	*a_param_list = upslst_add(*a_param_list, new_string);
	*ptr = *comma;                 /* restore */
      }
      ++ptr;                       /* go past the comma */
      saved_ptr = ptr;             /* start of param */
    } else {
      if (! saved_ptr) {
	saved_ptr = ptr;           /* the beginning of a new parameter */
      }
      ++ptr;                       /* go to the next character */
    }
  }

  if (saved_ptr) {
    /* Get the last one too */
    new_string = upsutl_str_create(saved_ptr, STR_TRIM_DEFAULT);
    upsutl_str_remove_edges(new_string, wspace);
    *a_param_list = upslst_add(*a_param_list, new_string);
  }
  return(rstatus);
}

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
  static char trim_chars[] = "\t\n\r\f)";

  /* First split the line to separate the action from the parameters. Locate
     the first '(' as that will divide the action from it's parameters */
  if ((strPtr = strchr(a_action_line, OPEN_PAREN)) != NULL) {
    len = strPtr - a_action_line;       /* get length of action name */

    /* get a new string */
    *strPtr = '\0';
    if ((tmpBuf = upsutl_str_create(a_action_line, STR_TRIM_DEFAULT))
	!= NULL) {
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
	  if ((tmpBuf2 = upsutl_str_create((char *)&a_action_line[len+1],
					   STR_TRIM_DEFAULT)) != NULL) {
	    /* trim off beginning & ending whitespace & the ending ")" */
	    upsutl_str_remove_edges(tmpBuf2, trim_chars);
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



