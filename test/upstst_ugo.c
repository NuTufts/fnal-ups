/*

Authors:        Margaret Votava
e-mail:         "votava@fnal.gov"

Include files:-
===============
*/

/* ups specific include files */
#include "ups.h"
#include "upstst_parse.h"
#include "upstst_macros.h"

static void upstst_ugo_dump (const t_upsugo_command * const uc, FILE * const fd);

/* ==========================================================================

    upstst_ugo_env - tests upsugo_env

   ==========================================================================*/

int upstst_ugo_env (int argc, char ** const argv)
{

int             status;                         /* status of parse */
int             estatus = UPS_SUCCESS;          /* expected status */
static char     *estatus_str;                   /* expected status string */
static char     *product;			/* product name */
static char     *options;			/* options */
upstst_argt     argt[] = {{"<product>", UPSTST_ARGV_STRING,NULL,&product},
			  {"-options",  UPSTST_ARGV_STRING,NULL,&options},
                          {"-status",   UPSTST_ARGV_STRING,NULL,&estatus_str},
                          {NULL,        UPSTST_ARGV_END,   NULL,NULL}};
t_upsugo_command	*uc;				/* ups command */


/* parse command line
   ------------------ */

estatus_str = NULL; options = NULL;
status = upstst_parse (&argc, argv, argt, UPSTST_PARSE_NOLEFTOVERS);
UPSTST_CHECK_PARSE(status,argt,argv[0]);
UPSTST_CHECK_ESTATUS (estatus_str, estatus);
if (!options) options = UPSTST_ALLOPTS;

/* call the real routine
   --------------------- */

uc = upsugo_env(product,options);
UPSTST_CHECK_CALL(UPSTST_NONZEROSUCCESS,uc,estatus);
return (0);
}

/* ==========================================================================

    upstst_ugo_next - tests upsugo_next

   ==========================================================================*/
int upstst_ugo_next (int argc, char ** const argv)
{

int             status;                         /* status of parse */
int             estatus = UPS_SUCCESS;          /* expected status */
static char     *estatus_str;                   /* expected status string */
static char     *options;			/* options */
static int	encmds;				/* expected count */
static char	*outfile;			/* filename to output */
static char     *difffile;			/* file to diff */
FILE		*ofd;				/* outfile file descriptor */
upstst_argt     argt[] = {{"-options",UPSTST_ARGV_STRING,NULL,&options},
			  {"-encmds", UPSTST_ARGV_INT,   NULL,&encmds},
                          {"-status", UPSTST_ARGV_STRING,NULL,&estatus_str},
                          {"-out",    UPSTST_ARGV_STRING,NULL,&outfile},
                          {"-diff",   UPSTST_ARGV_STRING,NULL,&difffile},
                          {NULL,      UPSTST_ARGV_END,   NULL,NULL}};
t_upsugo_command	*uc =0;				/* ups command */
int		ncmds = 0;
char            diffcmd[132];                   /* diff command */



/* parse command line
   ------------------ */

estatus_str = NULL; encmds = 1; outfile = NULL; difffile = NULL;
options = NULL;
status = upstst_parse (&argc, argv, argt, UPSTST_PARSE_EXACTMATCH);
UPSTST_CHECK_PARSE(status,argt,argv[0]);
UPSTST_CHECK_ESTATUS (estatus_str, estatus);
if (!options) options = UPSTST_ALLOPTS;
if (outfile) 					/* don't use stdout */
   {
   ofd = fopen(outfile,"w");
   if (!ofd)
      { perror(outfile); return (1); }
   }
else
   {ofd = stdout;}

/* call the real routine
   --------------------- */

while (uc = upsugo_next(argc,argv,options))	/* for all commands */
   {
   if (UPS_ERROR != estatus)			/* error? */
       {
       fprintf(stderr,"%s: %s, %s: %s\n","actual status",
          g_error_ascii[UPS_ERROR],"expected status", g_error_ascii[estatus]);  
       if (UPS_ERROR)                                       
            { upserr_output(); upserr_clear(); }
        }
   upstst_ugo_dump(uc,ofd);			/* display */
   ncmds++;					/* increment ncommands */
   }

if (ncmds != encmds)
   fprintf(stderr,"%s: %d, %s: %d\n","actual ncmds",ncmds, 
      "expected ncmds", encmds);    

/* dump the output to specified file and compare
   --------------------------------------------- */

if(outfile) fclose(ofd);
if (difffile && outfile)
   {
   sprintf (diffcmd,"diff %s %s",difffile,outfile);
   system(diffcmd);
   }

return (0);
}

/* ==========================================================================

    upstst_ugo_dump - static function to display contents of command buffer

   ==========================================================================*/

static void upstst_ugo_dump (const t_upsugo_command * const uc, FILE * const fd)
{
#define upstst_ugo_dumplist(title,ptr) 				\
   {								\
   t_upslst_item *l_ptr;					\
   fprintf(fd,"%s",title);					\
   for (l_ptr = upslst_first(ptr); l_ptr; l_ptr = l_ptr->next)	\
      {								\
      if (l_ptr == upslst_first(ptr))				\
         fprintf (fd,"%s\n", (char *)l_ptr->data);		\
      else							\
         fprintf (fd,"                  %s\n",  (char *)l_ptr->data);	\
      }								\
   }

if(!uc) return;
if ( uc->ugo_product ) 
   fprintf(fd,"Product:          %s\n",uc->ugo_product);
if ( uc->ugo_version )
   fprintf(fd,"Version:          %s\n",uc->ugo_version);
if ( uc->ugo_auth )
   upstst_ugo_dumplist("Authorized Nodes: " ,uc->ugo_auth);
if ( uc->ugo_flavor )
   upstst_ugo_dumplist("Flavor:           ",uc->ugo_flavor);
if ( uc->ugo_host )
   upstst_ugo_dumplist("Host:             ",uc->ugo_host);
if ( uc->ugo_key )
   upstst_ugo_dumplist("Key:              ",uc->ugo_key);
if ( uc->ugo_tablefiledir )
   fprintf(fd,"Tablefiledir:     %s\n",uc->ugo_tablefiledir);
if ( uc->ugo_tablefile )
   fprintf(fd,"Tablefile:        %s\n",uc->ugo_tablefile);
if ( uc->ugo_anyfile )
   fprintf(fd,"Anyfile:          %s\n",uc->ugo_anyfile);
if ( uc->ugo_options )
   fprintf(fd,"Options:          %s\n",uc->ugo_options);
if ( uc->ugo_description )
   fprintf(fd,"Description:      %s\n",uc->ugo_description);
if ( uc->ugo_override )
   fprintf(fd,"Override:         %s\n",uc->ugo_override);
if ( uc->ugo_qualifiers )
   upstst_ugo_dumplist("Qualifiers:       ",uc->ugo_qualifiers);
if ( uc->ugo_productdir )
   fprintf(fd,"Productdir:       %s\n",uc->ugo_productdir);
if ( uc->ugo_archivefile )
   fprintf(fd,"Archivefile:      %s\n",uc->ugo_archivefile);
if ( uc->ugo_upsdir )
   fprintf(fd,"Upsdir:           %s\n",uc->ugo_upsdir);
if ( uc->ugo_db )
   upstst_ugo_dumplist("DB:               ",uc->ugo_db);
if ( uc->ugo_chain )
   upstst_ugo_dumplist("Chains:           ",uc->ugo_chain);
}
