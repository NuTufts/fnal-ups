/*


Authors:        Margaret Votava
e-mail:         "votava@fnal.gov"
 
Include files:-
===============
*/

#include <time.h>
#include "ups_t_parse.h"
#include "ups_t_macros.h"


/* ============================================================================

ROUTINE:
	ups_t_date
 	
	print date to stderr
==============================================================================*/
int	ups_t_date		(int argc, char ** const argv)
{

time_t	stime;			/* time */
int 		status;
ups_t_argt	argt[] = {{NULL, UPS_T_ARGV_END, NULL, NULL}};

/* parse command line
   ------------------ */

status = ups_t_parse (&argc, argv, argt);
UPS_T_CHECK_PARSE(status,argt,argv[0]);

/* echo date
   --------- */

stime = time(NULL);					/* display time */
printf (asctime(localtime(&stime)));
return 0;						/* success */
}


/* ============================================================================

ROUTINE:
	ups_t_echo
 	
	print string to stderr
==============================================================================*/
int	ups_t_echo		(int argc, char ** const argv)
{
int 		status;				/* status */
static char	*mystring;			/* string */
ups_t_argt	argt[] = {
	{"<mystring>",	UPS_T_ARGV_STRING,	NULL,		&mystring},
	{NULL,		UPS_T_ARGV_END,		NULL,		NULL}};

/* parse command line
   ------------------ */

mystring = NULL;
status = ups_t_parse (&argc, argv, argt);
UPS_T_CHECK_PARSE (status, argt, argv[0]);

/* echo string to stderr
   --------------------- */

printf ("%s\n",mystring);
return 0;
}

/* ============================================================================

ROUTINE:
	ups_t_debug_level
 	
	set/display the debug level

==============================================================================*/
int	ups_t_debug_level	(int argc, char ** const argv)
{
int 		status;			/* status */
static int	testflag;		/* test flag */
static int	level;			/* string */
ups_t_argt	argt[] = {
	{"[level]",	UPS_T_ARGV_INT,		NULL,		&level},
	{"-test",	UPS_T_ARGV_CONSTANT,	(char *)TRUE,	&testflag},
	{NULL,		UPS_T_ARGV_END,		NULL,		NULL}};

/* parse command line
   ------------------ */

level = -1; testflag = FALSE;
status = ups_t_parse (&argc, argv, argt);
UPS_T_CHECK_PARSE (status, argt, argv[0]);

/* either set or show debug level
   ------------------------------ */

if (level == -1)					/* show current level */
   {
   printf ("Current test debug level is: %d\n",ups_t_debug);
#if 0
   printf ("Current ups debug level is: %d\n",ups_debug);
#endif
   }
else							/* change level */
   {
   if (testflag)
      ups_t_debug = level;
#if 0
   else
      ups_debug = level;
#endif
   }

return 0;
}
