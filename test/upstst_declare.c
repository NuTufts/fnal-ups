/*

Authors:        Margaret Votava
e-mail:         "votava@fnal.gov"

Include files:-
===============
*/

#include <unistd.h>

/* ups specific include files */
#include "ups.h"
#include "upstst_parse.h"
#include "upstst_macros.h"

/* ==========================================================================

    upstst_declare - tests ups list 

   ==========================================================================*/

int upstst_declare(int argc, char ** const argv)
{
static char     *myfunc = "upsugo_declare";
static char	*outfile;			/* filename to output */
static char     *difffile;			/* file to diff */
static char	*estatus_str;                   /*expected status str */
int		estatus = UPS_SUCCESS;          /* expected status */
int		status;				/* function status */
FILE		*ofd;				/* outfile file descriptor */
upstst_argt     argt[] = {{"-out",    UPSTST_ARGV_STRING,NULL,&outfile},
                          {"-diff",   UPSTST_ARGV_STRING,NULL,&difffile},
                          {"-status", UPSTST_ARGV_STRING,NULL,&estatus_str},
                          {NULL,      UPSTST_ARGV_END,   NULL,NULL}};
t_upsugo_command	*uc =0;			/* ups command */
char            diffcmd[132];                   /* diff command */
int		stdout_dup;			/* dup of stdout */

/* parse command line
   ------------------ */

outfile = NULL; difffile = NULL; estatus_str = NULL;
status = upstst_parse (&argc, argv, argt, UPSTST_PARSE_EXACTMATCH);
UPSTST_CHECK_ESTATUS (estatus_str, estatus);
UPSTST_CHECK_PARSE(status,argt,argv[0]);
if (outfile) 					/* don't use stdout */
   {
   if (!(ofd = fopen(outfile,"w")))		/* open file */
      { perror(outfile); return (1); }
   }
else
   {ofd = stdout;}

stdout_dup = dup(STDOUT_FILENO);		/* dup stdout */
fflush(stdout);					/* clear output buffer */
dup2(fileno(ofd),STDOUT_FILENO);		/* reset it to output file */

/* call the real routine
   --------------------- */

UPS_ERROR = UPS_SUCCESS;
while (uc = upsugo_next(argc,argv,UPSTST_ALLOPTS))/* for all commands */
   {
   if (UPS_ERROR != UPS_SUCCESS)
      {
      fprintf(stderr,"upsugo_next failed: %s\n", g_error_ascii[UPS_ERROR]);
      upserr_output(); upserr_clear();
      return (0);
      }
   ups_declare(uc,stdout,e_declare);
   UPSTST_CHECK_UPS_ERROR(estatus);		/* check UPS_ERROR */
   }

/* dump the output to specified file and compare
   --------------------------------------------- */

fflush(stdout);                                 /* flush buffer */
dup2(stdout_dup,STDOUT_FILENO);                 /* reset stdout */
close(stdout_dup);                              /* close files*/
if(fileno(ofd) != STDOUT_FILENO) fclose(ofd);

if (difffile && outfile)
   {
   sprintf (diffcmd,"diff %s %s",difffile,outfile);
   if (system(diffcmd)) printf("files %s %s differ\n",difffile,outfile);
   }

return (0);
}
