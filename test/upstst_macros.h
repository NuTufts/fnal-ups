#ifndef UPSTSTMACROS
#define UPSTSTMACROS
/*****************************************************************************
Authors:        Margaret Votava
e-mail:         "votava@fnal.gov"
 
Header file with useful things for wrapper routines

*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "upserr.h"

/* Global variables
   ================ */

int strcasecmp		(const char *s1, const char *s2);
extern int              upstst_debug;            /* debug flag */
#define UPSTST_ERROR		1
#define UPSTST_SUCCESS		0
#define UPSTST_NONZEROSUCCESS	10
#define UPSTST_ZEROSUCCESS	20
#define UPSTST_ALLOPTS	"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"


/* Macros:-
   ======== */


#define UPSTST_CHECK_PARSE(status,argt,cmdname){\
   if (status == UPSTST_USAGE)			\
      {						\
      upstst_print_usage (argt,cmdname);	\
      return (UPSTST_SUCCESS);			\
      }						\
   else if (status)				\
      {						\
      upstst_print_usage (argt,cmdname);	\
      return (UPSTST_ERROR);			\
      }						\
   }

#define UPSTST_CHECK_CALL(type,returnval,estatus) {	\
   int macstatus = 0;				\
   if (type == UPSTST_ZEROSUCCESS) 		\
      macstatus = (int)(returnval);		\
   else if (type == UPSTST_NONZEROSUCCESS)	\
      {						\
      if (returnval) macstatus = UPS_SUCCESS;	\
      else macstatus = UPS_ERROR;		\
      }						\
   if (macstatus != estatus)			\
      {						\
      fprintf (stderr, "function: %s\n",myfunc);\
      fprintf (stderr, "%s: %s, %s: %s\n",	\
         "actual status",g_error_ascii[macstatus],\
	 "expected status", g_error_ascii[estatus]);\
      if (macstatus)				\
         {					\
         upserr_output();			\
         upserr_clear();			\
         }					\
      return(UPSTST_ERROR);			\
      }						\
   }

#define UPSTST_CHECK_ESTATUS(estring,estatus) {		\
   if (estring)						\
      {							\
      int i;						\
      for (i = 0; g_error_ascii[i]; i++)		\
         if (!strcasecmp(g_error_ascii[i],estring))	\
	    {						\
	    estatus = i;				\
	    break;					\
	    }						\
      if (!g_error_ascii[i])				\
         {						\
         fprintf (stderr, "function: %s\n",myfunc);	\
         fprintf (stderr,				\
            "Invalid error code specified: %s\n",	\
            estring);					\
         return (UPSTST_ERROR);				\
         }						\
      }							\
   }

#endif
