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

    upstst_commands - tests ups all the commands with the generic interface

   ==========================================================================*/


int upstst_command(int argc, char ** const argv, const void * const myfunc(),const char * const funcname, const int calledby)
{
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
   *myfunc(uc,stdout,calledby);
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

int upstst_declare(int argc, char ** const argv)
{ return(upstst_command(argc,argv, 
          (void *)&ups_declare,"ups_declare",e_declare)); }

int upstst_undeclare(int argc, char ** const argv)
{ return(upstst_command(argc,argv, 
          (void *)&ups_undeclare,"ups_undeclare",e_undeclare)); }

int upstst_configure(int argc, char ** const argv)
{ return(upstst_command(argc,argv, 
          (void *)&ups_configure,"ups_configure",e_configure)); }

int upstst_unconfigure(int argc, char ** const argv)
{ return(upstst_command(argc,argv, 
          (void *)&ups_unconfigure,"ups_unconfigure",e_unconfigure)); }

int upstst_tailor(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_tailor,"ups_tailor",e_tailor)); }

int upstst_copy(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_copy,"ups_copy",e_copy)); }

int upstst_start(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_start,"ups_start",e_start)); }

int upstst_stop(int argc, char ** const argv)
{ return(upstst_command(argc,argv, (void *)&ups_stop,"ups_stop",e_stop)); }

int upstst_create(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_create,"ups_create",e_create)); }

int upstst_flavor(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_flavor,"ups_flavor",e_flavor)); }

int upstst_get(int argc, char ** const argv)
{ return (upstst_command(argc,argv,(void *)&ups_get,"ups_get",e_get)); }

int upstst_modify(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_modify,"ups_modify",e_modify)); }

int upstst_setup(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_setup,"ups_setup",e_setup));}

int upstst_unsetup(int argc, char ** const argv)
{ return(upstst_command(argc,argv, 
          (void *)&ups_unsetup,"ups_unsetup",e_unsetup)); }

int upstst_unk(int argc, char ** const argv)
{ return (upstst_command(argc,argv, (void *)&ups_unk,"ups_unk",e_unk)); }

int upstst_depend(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_depend,"ups_depend",e_depend)); }

int upstst_touch(int argc, char ** const argv)
{ return(upstst_command(argc,argv,(void *)&ups_touch,"ups_touch",e_touch)); }
