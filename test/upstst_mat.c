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

static void upstst_match_dump (const t_upslst_item * const );
/* ==========================================================================

    upstst_mat_instance - tests upsmat_instace

   ==========================================================================*/

int upstst_mat_instance (int argc, char ** const argv)
{
static char     	*myfunc = "upsmat_instance";
int             	status;                         /* status of parse */
int             	estatus = UPS_SUCCESS;          /* expected status */
t_upsugo_command	*uc = 0;			/* ups command */
t_upslst_item		*mp = 0;			/* match product */
char			diffcmd[132];			/* diff command */

static char     	*estatus_str;                   /*expected status str */
static int		unique;				/* unique flag name */
static char		*outfile;			/* filename to output */
static char		*difffile;			/* file to diff */
FILE			*ofd;				/* outfile fd */
int			stdout_dup;			/* dup of stdout */

upstst_argt     	argt[] = 
   {{"-unique",	  UPSTST_ARGV_CONSTANT,(char *)TRUE,	&unique},
    {"-out",	  UPSTST_ARGV_STRING,NULL,		&outfile},
    {"-diff",     UPSTST_ARGV_STRING,NULL,		&difffile},
    {"-status",   UPSTST_ARGV_STRING,NULL,   		&estatus_str},
    {NULL,        UPSTST_ARGV_END,NULL,			NULL}};


/* parse command line
   ------------------ */

estatus_str = NULL; 
unique = NULL; outfile = NULL; difffile = NULL;
status = upstst_parse (&argc, argv, argt, UPSTST_PARSE_EXACTMATCH);
UPSTST_CHECK_PARSE(status,argt,argv[0]);
UPSTST_CHECK_ESTATUS (estatus_str, estatus);

/* let's get our output file descriptor setup
   ------------------------------------------ */

if (outfile)                                    /* don't use stdout */
   {
   if (!(ofd = fopen(outfile,"w")))
      { perror(outfile); return (1); }
   }
else
   {ofd = stdout;}
stdout_dup = dup(STDOUT_FILENO);                /* dup stdout */
fflush(stdout);                                 /* clear output buffer */
status = dup2(fileno(ofd),STDOUT_FILENO);       /* reset it to output file */

/* call the real routine
   --------------------- */

UPS_ERROR = UPS_SUCCESS;
while (uc = upsugo_next(argc,argv,UPSTST_ALLOPTS))	/* for all commands */
   {
   if (UPS_ERROR != UPS_SUCCESS)			/* error on ugo_next */
       {
       fprintf(stderr,"upsugo_next failed: %s\n", g_error_ascii[UPS_ERROR]);  
       upserr_output(); upserr_clear();
       return (0);
       }
   mp = upsmat_instance(uc,unique);
   if (UPS_ERROR != estatus)                    	/* error? */
       {
       fprintf(stderr,"%s: %s, %s: %s\n","actual status",
          g_error_ascii[UPS_ERROR],"expected status", g_error_ascii[estatus]);
       if (UPS_ERROR) { upserr_output(); upserr_clear(); }
       }
   upstst_match_dump(mp);
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
   system(diffcmd);
   }

return (0);

}

/* ==========================================================================

    upstst_match_dump - static function to display contents of match buffer

   ==========================================================================*/

static void upstst_match_dump (const t_upslst_item * const mp)
{
t_upslst_item 			*prod_ptr;		/* product ptr */
t_upslst_item 			*inst_ptr;		/* instance ptr */
t_upstyp_matched_product 	*prod;			/* product match */
t_upstyp_matched_instance	*inst;			/* instance match */
t_upstyp_db			*db;			/* database */


if(!mp) return;
for (prod_ptr = (t_upslst_item *)mp; prod_ptr; prod_ptr = prod_ptr->next)
   {
   prod = (t_upstyp_matched_product *) prod_ptr->data;
   db = (t_upstyp_db *) prod->db_info;
   if (db) printf("D:NAME=%s\n", db->name);
   for (inst_ptr = prod->minst_list; inst_ptr; inst_ptr = inst_ptr->next)
      {
      inst = (t_upstyp_matched_instance *) inst_ptr->data;
      if (inst->chain)
         {
         printf("C:PRODUCT=%s, CHAIN=%s, VERSION=%s, ", inst->chain->product,
             inst->chain->chain, inst->chain->version);
         printf("FLAVOR=%s, QUALIFIERS=%s\n", inst->chain->flavor,
             inst->chain->qualifiers);
         }
      if (inst->version)
         {
         printf("V:PRODUCT=%s, VERSION=%s, ", inst->version->product,
             inst->version->version);
         printf("FLAVOR=%s, QUALIFIERS=%s\n", inst->version->flavor,
             inst->version->qualifiers);
         }
      if (inst->table)
         {
         printf("T:PRODUCT=%s, VERSION=%s, ", inst->table->product,
             inst->table->version);
         printf("FLAVOR=%s, QUALIFIERS=%s\n", inst->table->flavor,
             inst->table->qualifiers);
         }
      }
   }
}
