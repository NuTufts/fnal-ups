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

static void upstst_match_dump (const t_ups_match_product * const , FILE * const);
/* ==========================================================================

    upstst_mat_match_instance - tests upsmat_match_instace

   ==========================================================================*/

int upstst_mat_match_instance (int argc, char ** const argv)
{

int             	status;                         /* status of parse */
int             	estatus = UPS_SUCCESS;          /* expected status */
t_ups_command		*uc = 0;			/* ups command */
t_ups_match_product 	*mp = 0;			/* match product */
char			diffcmd[132];			/* diff command */

static char     	*estatus_str;                   /*expected status str */
static char     	*product;			/* product name */
static char		*database;			/* database dir */
#if 0
static char		*chain;				/* chain name */
static char		*version;			/* version name */
#endif
static int		unique;				/* unique flag name */
static char		*outfile;			/* filename to output */
static char		*difffile;			/* file to diff */
FILE			*ofd;				/* outfile fd */

upstst_argt     	argt[] = 
   {{"-database", UPSTST_ARGV_STRING,NULL,		&database},
    {"-product",  UPSTST_ARGV_STRING,NULL,		&product},
#if 0
    {"-chain",    UPSTST_ARGV_STRING,NULL,		&chain},	
    {"-version",  UPSTST_ARGV_STRING,NULL,		&version},
#endif
    {"-unique",	  UPSTST_ARGV_CONSTANT,(char *)TRUE,	&unique},
    {"-out",	  UPSTST_ARGV_STRING,NULL,		&outfile},
    {"-diff",     UPSTST_ARGV_STRING,NULL,		&difffile},
    {NULL,        UPSTST_ARGV_END,NULL,			NULL}};


/* parse command line
   ------------------ */

estatus_str = NULL; product = NULL; database = NULL; 
#if 0
chain = NULL;
version = NULL; 
#endif
unique = NULL; outfile = NULL; difffile = NULL;
status = upstst_parse (&argc, argv, argt, UPSTST_PARSE_EXACTMATCH);
UPSTST_CHECK_PARSE(status,argt,argv[0]);
UPSTST_CHECK_ESTATUS (estatus_str, estatus);
if (outfile)                                    /* don't use stdout */
   {
   ofd = fopen(outfile,"w");
   if (!ofd)
      { perror(outfile); return (1); }
   }
else
   {ofd = stdout;}



/* call the real routine
   --------------------- */

while (uc = upsugo_next(argc,argv,UPSTST_ALLOPTS))	/* for all commands */
   {
   if (UPS_ERROR != UPS_SUCCESS)			/* error on ugo_next */
       {
       fprintf(stderr,"upsugo_next failed: %s\n", g_error_ascii[UPS_ERROR]);  
       upserr_output(); upserr_clear();
       return (0);
       }
   mp = upsmat_match_instance(uc,database,product,NULL,NULL,unique);
   if (UPS_ERROR != estatus)                    	/* error? */
       {
       fprintf(stderr,"%s: %s, %s: %s\n","actual status",
          g_error_ascii[UPS_ERROR],"expected status", g_error_ascii[estatus]);
       if (UPS_ERROR) { upserr_output(); upserr_clear(); }
       return 0;
       }
   upstst_match_dump(mp,ofd);
   }

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

    upstst_match_dump - static function to display contents of match buffer

   ==========================================================================*/

static void upstst_match_dump (const t_ups_match_product * const mp, 
   FILE * const fd)
{
#define upstst_match_dumplist(title,ptr) 			\
   {								\
   t_upslst_item *l_ptr;					\
   fprintf(fd,"%s",title);					\
   for (l_ptr = upslst_first(ptr); l_ptr; l_ptr = l_ptr->next)	\
      {								\
      if (l_ptr == upslst_first(ptr))				\
         fprintf (fd,"%s\n", (char *)l_ptr->data);		\
      else							\
         fprintf (fd,"          %s\n",  (char *)l_ptr->data);	\
      }								\
   }

if(!mp) return;
if (mp->db) 
   fprintf(fd,"Database: %s\n",mp->db);
if (mp->chain_list)
   upstst_match_dumplist("Chain:    " ,mp->chain_list);
if (mp->version_list)
   upstst_match_dumplist("Version:  " ,mp->version_list);
if (mp->table_list)
   upstst_match_dumplist("Table:    " ,mp->table_list);
}
