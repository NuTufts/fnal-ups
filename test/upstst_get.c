/*

Authors:        David J. Fagan stolen from Margaret Votava
e-mail:         "votava@fnal.gov"

Include files:-
===============
*/

/* ups specific include files */
#include "ups.h"
#include "upstst_parse.h"
#include "upstst_macros.h"

static void upstst_trans_dump (const t_upslst_item * const , FILE * const,
                               const t_upsugo_command * const, char * const);
/* ==========================================================================

    upstst_get_translation - tests upsget_translation

   ==========================================================================*/

int upstst_get_translation (int argc, char ** const argv)
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
static char             *transstring;                   /* string totranslate */
FILE			*ofd;				/* outfile fd */

upstst_argt     	argt[] = 
   {{"-unique",	  UPSTST_ARGV_CONSTANT,(char *)TRUE,	&unique},
    {"-out",	  UPSTST_ARGV_STRING,NULL,		&outfile},
    {"-diff",     UPSTST_ARGV_STRING,NULL,		&difffile},
    {"-string",   UPSTST_ARGV_STRING,NULL,		&transstring},
    {NULL,        UPSTST_ARGV_END,NULL,			NULL}};


/* parse command line
   ------------------ */

estatus_str = NULL; 
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
   mp = upsmat_instance(uc,unique);
   if (UPS_ERROR != estatus)                    	/* error? */
       {
       fprintf(stderr,"%s: %s, %s: %s\n","actual status",
          g_error_ascii[UPS_ERROR],"expected status", g_error_ascii[estatus]);
       if (UPS_ERROR) { upserr_output(); upserr_clear(); }
       }
   upstst_trans_dump(mp,ofd,uc,transstring);
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

    upstst_trans_dump - static function to display contents of match buffer

   ==========================================================================*/

static void upstst_trans_dump (const t_upslst_item * const mp, 
   FILE * const fd, const t_upsugo_command * const uc,char * const string)
{
t_upslst_item 			*prod_ptr;		/* product ptr */
/* t_upslst_item 			*inst_ptr;	 instance ptr */
t_upstyp_matched_product 	*prod;			/* product match */
/* t_upstyp_matched_instance	*inst;			 instance match */
static char *tostring;


if(!mp) return;
for (prod_ptr = (t_upslst_item *)mp; prod_ptr; prod_ptr = prod_ptr->next)
   {
   prod = (t_upstyp_matched_product *) prod_ptr->data;
/*   for (inst_ptr = prod->minst_list; inst_ptr; inst_ptr = inst_ptr->next)
      { inst = (t_upstyp_matched_instance *) inst_ptr->data; */
        tostring=upsget_translation(prod,uc,string);
        if (tostring)
        { fprintf(fd,"Converted to >%s<\n",tostring);
        } else {
          fprintf(fd,"String >%s< NOT Converted\n",string);
        }
/*
      }
*/
   }
}