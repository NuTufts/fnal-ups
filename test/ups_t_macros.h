#ifndef UPST
#define UPST
/*****************************************************************************
Authors:        Margaret Votava
e-mail:         "votava@fnal.gov"
 
Header file with useful things for wrapper routines

*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Global variables
   ================ */

extern int              ups_t_debug;            /* debug flag */
extern int             	ups_t_max_error;	/* max number of errs */
extern int              ups_t_nerror;		/* current error count*/


/* Macros:-
   ======== */


#define UPS_T_INC_NERROR(){					\
ups_t_nerror++;							\
if (ups_t_nerror > ups_t_max_error)				\
   {								\
   fprintf (stderr, "Maximum number of errors (%d) exceeded\n",	\
      ups_t_max_error);						\
   exit(1);							\
   }								\
}

#define UPS_T_CHECK_PARSE(status,argt,cmdname){	\
   if (status == UPS_T_USAGE)			\
      {						\
      ups_t_print_usage (argt,cmdname);		\
      return (0);				\
      }						\
   else if (status)				\
      {						\
      ups_t_print_usage (argt,cmdname);		\
      return (status);				\
      }						\
   }

#define UPS_T_CHECK_CALL(status,estatus) {				\
   if ((int)(status) >= 0)						\
      {									\
      if (estatus != 0) 						\
	 {								\
         fprintf (stderr,"Command successful, but error %s expected\n",	\
	    ups_ascii_error[estatus]);					\
	 return ((int)(status));					\
	 }								\
      }									\
   else									\
      {									\
      int error;							\
      char *errstring;							\
      errstring = ups_get_error(&error);				\
      if ((int)(error) != estatus)					\
         {								\
	 fprintf (stderr, "command failed with %s, expected %s\n",	\
	    ups_ascii_error[error],ups_ascii_error[estatus]); 		\
 	 fprintf (stderr, "%s\n",errstring);				\
	 UPS_T_INC_NERROR();						\
	 return (error);						\
         }								\
      }									\
   }

#define UPS_T_CHECK_ESTATUS(estring,estatus) {				\
   if (estring)								\
      {									\
      int i;								\
      for (i = 0; ups_ascii_error[i]; i++)				\
         if (!strcasecmp(ups_ascii_error[i],estring))			\
	    {								\
	    estatus = i;						\
	    break;							\
	    }								\
      if (!ups_ascii_error[i])						\
         {								\
         fprintf (stderr,"Invalid error code specified: %s\n",estring);	\
         return 1;							\
         }								\
      }									\
   }

#endif
