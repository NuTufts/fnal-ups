/****************************Copyright Notice ******************************
 *                                                                            *
 * Copyright (c) 1991 Universities Research Association, Inc.                 *
 *               All Rights Reserved.                                         *
 *                                                                            *
 ******************************************************************************/
/* ==========================================================================
**                                                                           
** DESCRIPTION
**
**	ups ugo (ups get opts) 
**           upsugo_getarg
**                                                                           
** DEVELOPERS                                                                
**
**       Eileen Berman
**       David Fagan
**       Lars Rasmussen
**                                                                           
**      Batavia, Il 60510, U.S.A.                                            
**                                                                           
** ENTRY POINTS                 SCOPE                                        
**      +++                     +++                                          
**                                                                           
** MODIFICATIONS                                                             
** 	Jul 24 1997, DjF, First
**
** HEADER STATEMENTS                                                         
*/                                                                           

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/param.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <bstring.h>
#include <sys/utsname.h>

#include "upstyp.h"
#include "upsugo.h"
#include "upserr.h"
#include "upsmem.h"
#include "upsutl.h"

#ifdef UPS_ID
	char	UPS_UGO_ID[] = "@(#)upsugo.c	1.00";
#endif
#define FREE( X )	{			\
			free( X );		\
			X = 0;		\
			}

   int	errflg = 0;
   t_upslst_item *ugo_commands = 0;
   t_upslst_item *last_command = 0;
   int argindx;
/* flag to know if I added current because no chain was specified 
** when this is done the version is not yet know to exist yet so
** I can't check and I have to remove if version is then specified 
** kind of ugly, but otherwise the ifornota would have to be called
** after checking if last command existed or the version was previously
** parsed etc and would add a lot of code.
*/
   int added_current=0;	

/* ===========================================================================
 * Private function declarations
 */
char * upsugo_getarg( const int argc, char *argv[], char ** const argbuf);
int upsugo_bldfvr(struct ups_command * const uc);
int upsugo_ifornota(struct ups_command * const uc);
int upsugo_bldqual(struct ups_command * const uc, char * const inaddr);
int upsugo_dump (struct ups_command * const uc);
void upsugo_liststart(struct ups_command * const a_command_line);


/* ===========================================================================
** ROUTINE	upsugo_bldfvr()
**
*/
int upsugo_bldfvr(struct ups_command * const uc)
{
   char   * addr;
   char   * loc;
   struct utsname *baseuname;
   baseuname=(struct utsname *) malloc( sizeof(struct utsname));
   if ( (uname(baseuname)) == -1) return(-1);

/* Silicon Graphics IRIX machines
   The difference between 64 and 32 bit machines is ignored
   The real old machines return R2300 - But I don't care...
*/
   if ((strncmp(baseuname->sysname,"IRIX",4)) == 0)
   { (void) strcpy(baseuname->machine,"IRIX+");
     (void) strcat(baseuname->machine,baseuname->release);
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'.');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'+');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     addr=upsutl_str_create("NULL",' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     return(0);
   }
/* Linux - which I know very little about...
   It has been suggested to drop down on odd version I don't know
   if this is a correct thing to do???
*/
   if ((strncmp(baseuname->sysname,"Linux",5)) == 0)
   { (void) strcpy(baseuname->machine,"Linux+");
     (void) strcat(baseuname->machine,baseuname->release);
     loc=strchr(baseuname->machine,'.');
     *loc='-';
     loc=strchr(baseuname->machine,'.');
     *loc=0;				/* second dot not first */
     loc=strchr(baseuname->machine,'-');
     *loc='.';				/* return dot */
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'.');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'+');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     addr=upsutl_str_create("NULL",' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     return(0);
   }
/* Sun Microsystems SunOS - Solaris */
   if ((strncmp(baseuname->sysname,"SunOS",5)) == 0)
   { (void) strcpy(baseuname->machine,"SunOS+");
     (void) strcat(baseuname->machine,baseuname->release);
     loc=strchr(baseuname->machine,'.');
     *loc='-';
     loc=strchr(baseuname->machine,'.');
     *loc=0;				/* second dot not first */
     loc=strchr(baseuname->machine,'-');
     *loc='.';				/* return dot */
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'.');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'+');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     addr=upsutl_str_create("NULL",' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     return(0);
   }
/* International Buisness Machines - AIX */
   if ((strncmp(baseuname->sysname,"AIX",3)) == 0)
   { (void) strcpy(baseuname->machine,"AIX+");
     (void) strcat(baseuname->machine,baseuname->version);
     (void) strcat(baseuname->machine,".");
     (void) strcat(baseuname->machine,baseuname->release);
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'.');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'+');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     addr=upsutl_str_create("NULL",' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     return(0);
   }
/* Digital Equipment Corporation OSF1
*/
   if ((strncmp(baseuname->sysname,"OSF1",4)) == 0)
   { (void) strcpy(baseuname->machine,"OSF1+");
     (void) strcat(baseuname->machine,baseuname->release);
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'.');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     loc=strchr(baseuname->machine,'+');
     *loc = 0;
     addr=upsutl_str_create(baseuname->machine,' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     addr=upsutl_str_create("NULL",' ');
     uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     return(0);
   }
   return(-1);
}
/* ===========================================================================
** ROUTINE	upsugo_ifornota()
**
*/
/* I could use the same address for the "*" string but I don't think the
** extra code would justify it.
*/
int upsugo_ifornota(struct ups_command * const uc)
{
   char   * addr;
   char   * PRODUCTS;                           /* PRODUCTS value */
   char   * loc;
 
   added_current=0;
   if (uc->ugo_a)                           /* did the user specify -a */
   { if (!uc->ugo_chain && !uc->ugo_version)    /* If no chain all chains  */
     { addr=upsutl_str_create("*",' ');
       uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
     }
     if (!uc->ugo_qualifiers) 
     { addr=upsutl_str_create("*",' ');
       uc->ugo_qualifiers = upslst_add(uc->ugo_qualifiers,addr);
     }
     if (!uc->ugo_version)          /* unallocated if later specified */
     { addr=upsutl_str_create("*",' ');  
       uc->ugo_version = addr;      /* at this point I may not know... */
     }
     if (!uc->ugo_flavor)
     { addr=upsutl_str_create("*",' ');  
       uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
     }
   } else {                         /* not -a but give defaults */
/* oops... This is called before version may be set!!! */
/*     if (!uc->ugo_chain || !uc->ugo_version) */ /* If no chain current      */
     if (!uc->ugo_chain )
     { addr=upsutl_str_create("current",' ');
       uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
/* ug */
       added_current=1;
     }
     if (!uc->ugo_qualifiers)       /* no qualifiers = ""       */
     { addr=upsutl_str_create("",' ');
     uc->ugo_qualifiers = upslst_add(uc->ugo_qualifiers,addr);
     }
     if (!uc->ugo_flavor) upsugo_bldfvr(uc);
   }
/* If they didn't specify a database pull the environment variable */
   if (!uc->ugo_db) {
     if((PRODUCTS = (char *)getenv("PRODUCTS")) == 0)
     { upserr_add(UPS_NO_DATABASE, UPS_FATAL); 
     } else {
        addr = (char *) malloc((size_t)(strlen(PRODUCTS) +1));
        strcpy(addr,PRODUCTS);
        while ((loc=strchr(addr,' '))!=0) {
                *loc = 0;
                PRODUCTS = upsutl_str_create(addr,' ');
                addr=loc+1; 
                while(*addr==' ') { addr=addr+1; }
                uc->ugo_db = upslst_add(uc->ugo_db,PRODUCTS);
        }
        PRODUCTS = upsutl_str_create(addr,' ');
        uc->ugo_db = upslst_add(uc->ugo_db,PRODUCTS);
     }
   } 
   return(0);
}
/* ===========================================================================
** ROUTINE	upsugo_bldqual()
**
*/
int upsugo_bldqual(struct ups_command * const uc, char * const inaddr)
{
 char * addr;                          /* required qualifier string */
 char * oaddr;                         /* optional qualifier string */
 char * naddr;                         /* address for build red,opt */
 int test;
 char * waddr;                         /* work address string */
 char * loc;
 int onq=0;                            /* parsing a ? element */
 int onc=0;                            /* parsing a , element */
 int done=0;
 int qcount=0;                         /* number of optional qualifiers */
 int i,j;
 int opinit=0;
 char * optionals[10]; /* OH NO!! artifical limit of 10 optionals */

 if ( strchr(inaddr,'?') == 0) {       /* no optional qualifiers */
  addr=upsutl_str_create(inaddr,'p');
  test=upsutl_str_sort(addr,',');
  uc->ugo_qualifiers = upslst_add(uc->ugo_qualifiers,addr);
 } else {
  addr=upsutl_str_create(inaddr,'p');
/* remove all ?qualifiers from required string */
  waddr=addr;				/* work address */
  while (*waddr)   
  { if (*waddr==',')
    { onq=0;
    } else {
      if (*waddr=='?'||onq)
      { onq=1;
        *waddr=' ';
      }
    }
    ++waddr;
  }
  waddr=addr;
  while (*waddr&&!done) 
  { if (*waddr!=' '&&*waddr!=',')
    { done=1;
    } else { 
      if (*waddr==',') 
      { *waddr=' ';
        done=1;
      }
    }
    ++waddr;
  }
  waddr=upsutl_str_create(addr,'p');   /* new list trimed */
  upsmem_free(addr);                   /* disgard work string */
  addr=waddr;                          /* required string in addr */
/* give it for the moment */
/*  uc->ugo_qualifiers = upslst_add(uc->ugo_qualifiers,addr); */
/* remove all required from the optional string */
  oaddr=upsutl_str_create(inaddr,'p');
  onc=1;                               /* must assume first no spec is , */
  waddr=oaddr;				/* work address */
  while (*waddr)   
  { if (*waddr=='?')
    { onc=0;
    } else {
      if (*waddr==','||onc)
      { onc=1;
        *waddr=' ';
      }
    }
    ++waddr;
  }
  waddr=oaddr;
  done=0;
  while (*waddr&&!done) 
  { if (*waddr!=' '&&*waddr!='?')
    { done=1;
    } else { 
      if (*waddr=='?') 
      { *waddr=' ';
        done=1;
      }
    }
    ++waddr;
  }
  waddr=upsutl_str_create(oaddr,'p');   /* new list trimed */
  upsmem_free(oaddr);                   /* disgard work string */
  oaddr=waddr;                          /* optional string in oaddr */
  test=upsutl_str_sort(oaddr,'?');
  optionals[0]=oaddr;
  ++qcount;
  waddr=oaddr;
  while((loc=strchr(waddr,'?'))!=0)
  { optionals[qcount]=loc+1;
    ++qcount;
    *loc=0;
    waddr=loc+1;
  }
/* changed to reverse count, this will allow it to build and return the
** highest number of matches AND in alphabetical significance (since they
** are sorted as well) to least for atleast up to 2 optional qualifiers
** without resorting the list based on number of elements. upslst_insert 
** instead of add MAY have worked as well?  Beyond optional elements it
** will require a bubble sort based ONLY on the number of elements.
*/
    for ( i=(1<<(qcount))-1; i >=0; --i) 
    { opinit=0;
      waddr=0;
      for ( j=0; j < qcount; ++j)
         {  if ( i & (1<<(qcount-j-1)) )
          { 
             if(!opinit)
            { waddr=upsutl_str_create(optionals[j],' '); 
              opinit=1;
            } else { 
              waddr=upsutl_str_crecat(waddr,","); 
              waddr=upsutl_str_crecat(waddr,optionals[j]); 
            }
          }
      }
      if ( *addr != 0 ) /* required as well as optional */
      { naddr=upsutl_str_crecat(addr,",");
        naddr=upsutl_str_crecat(naddr,waddr);
      } else { 
/*        naddr=waddr; 
      }
*/
/* if there is optionals with no required last one but be "" */
        if ( waddr != 0 ) 
          { naddr=waddr;
          } else { 
/*            naddr=upsutl_str_create(" ",'p'); */
            naddr=addr; /* should be a null string yes? */
          }
      }
      test=upsutl_str_sort(naddr,','); 
      uc->ugo_qualifiers = upslst_add(uc->ugo_qualifiers,naddr);
    }
 }
 return(0);
}
/* ===========================================================================
** ROUTINE	upsugo_liststart(t_upsugo_command * const a_command_line)
**
*/
void upsugo_liststart(t_upsugo_command * const a_command_line)
{
  /* move all lists in the command line to their first element.  there
     may be a better way to do this, maybe dave can take a look at it */
  a_command_line->ugo_auth = upslst_first(a_command_line->ugo_auth);
  a_command_line->ugo_flavor = upslst_first(a_command_line->ugo_flavor);
  a_command_line->ugo_host = upslst_first(a_command_line->ugo_host);
  a_command_line->ugo_key = upslst_first(a_command_line->ugo_key);
  a_command_line->ugo_qualifiers = 
                         upslst_first(a_command_line->ugo_qualifiers);
  a_command_line->ugo_db = upslst_first(a_command_line->ugo_db);
  a_command_line->ugo_chain = upslst_first(a_command_line->ugo_chain);
}

/* ===========================================================================
** ROUTINE	upsugo_getarg( int argc, char * argv[])
**
** DESCRIPTION
**	Ups_ugo_getarg replaces getopt. It will always return
**	the next argument/option from the argc and argv pointers.
**	It does not modify either of its arguments.
**
** Returns
**	Ups_ugo_getarg returns either of the following:
**
**	The pointer to the next argument, or
**	0 if no more arguments exist.
** ==========================================================================
*/

char * upsugo_getarg( const int argc, char *argv[], char ** const argbuf)
{

    static int	arg_end;
    static char ** argpt = 0;
    static char * buff = 0;
    char * c;
    static char d[3];

    if( argc < 2 )
	{
	argpt = 0;
	argindx = 0;
	return  0;
	}

    if( argv == 0 )
	{
	argpt = 0;
	argindx = 0;
	return 0;
	}


    if( argv != argpt)
	{
	if( buff )
	    FREE(buff);
	argpt = argv;
	argindx = 0;
	arg_end = 0;
	}
    if( (*argbuf == 0) && (buff != 0))
	FREE(buff);

    if( argv[argindx] == 0 )
	{
	++arg_end;
	return  0;
	}

    if(*argbuf == 0)
	{

	if( ++argindx < argc )
	    {
	    if((*argv[argindx] == '-') && (strlen(argv[argindx]) >2))
		{
		buff = (char *) malloc((size_t)(strlen(argv[argindx]) +1));
		strcpy(buff, argv[argindx]);
		*argbuf = buff + 2;
		}
	    return argv[argindx];
	    }

	return  0;
    	}	
/* else if argbuf != 0
   ---------------------- */

	d[0] = '-';
	d[1] = **argbuf;
	d[2] = '\0';
	c = *argbuf;
	*argbuf = c+1;
	if(**argbuf == '\0')
	    {
	    FREE(buff);
	    *argbuf = 0;
	    }
	return &d[0];

}

/* ==========================================================================
**                                                                           
** ROUTINE: upsugo_free
**                                                                           
** DESCRIPTION - free a ups command structure.
** 
** NOTE: This free's the data within the linked list of ugo_command structures
**       it does NOT (yet? and how?) remove the structure from the linked list.
**
**                                                                           
** VALUES RETURNED                                                           
**      +++                                                                  
**                                                                           
** ==========================================================================
*/                                                                           
int upsugo_free (struct ups_command * const uc)
{
    if(uc)
    { if ( uc->ugo_product ) 
         upsmem_free(uc->ugo_product); 
      if ( uc->ugo_version ) 
         upsmem_free(uc->ugo_version); 
      if ( uc->ugo_auth ) 
         upslst_free(uc->ugo_auth,'d'); 
      if ( uc->ugo_flavor ) 
         upslst_free(uc->ugo_flavor,'d'); 
      if ( uc->ugo_host ) 
         upslst_free(uc->ugo_host,'d'); 
      if ( uc->ugo_key ) 
         upslst_free(uc->ugo_key,'d'); 
      if ( uc->ugo_tablefiledir ) 
         upsmem_free(uc->ugo_tablefiledir); 
      if ( uc->ugo_tablefile ) 
         upsmem_free(uc->ugo_tablefile); 
      if ( uc->ugo_anyfile ) 
         upsmem_free(uc->ugo_anyfile); 
      if ( uc->ugo_options ) 
         upsmem_free(uc->ugo_options); 
      if ( uc->ugo_description ) 
         upsmem_free(uc->ugo_description); 
      if ( uc->ugo_override ) 
         upsmem_free(uc->ugo_override); 
      if ( uc->ugo_qualifiers ) 
         upslst_free(uc->ugo_qualifiers,'d'); 
      if ( uc->ugo_productdir ) 
         upsmem_free(uc->ugo_productdir); 
      if ( uc->ugo_archivefile ) 
         upsmem_free(uc->ugo_archivefile); 
      if ( uc->ugo_upsdir ) 
         upsmem_free(uc->ugo_upsdir); 
      if ( uc->ugo_db ) 
         upslst_free(uc->ugo_db,'d'); 
      if ( uc->ugo_chain ) 
         upslst_free(uc->ugo_chain,'d'); 
      upsmem_free(uc);
    } return (0);
}

/* ==========================================================================
**                                                                           
** ROUTINE: upsugo_dump
**                                                                           
** DESCRIPTION                                                               
**
**                                                                           
** VALUES RETURNED                                                           
**      +++                                                                  
**                                                                           
** ==========================================================================
*/                                                                           
int upsugo_dump (struct ups_command * const uc)
{
    if(uc)
    { if ( uc->ugo_product ) 
         fprintf(stdout,"Product: %s\n",uc->ugo_product); 
      if ( uc->ugo_version ) 
         fprintf(stdout,"Version: %s\n",uc->ugo_version); 
      if ( uc->ugo_auth ) 
         upsugo_prtlst(uc->ugo_auth,"Auth"); 
      if ( uc->ugo_flavor ) 
         upsugo_prtlst(uc->ugo_flavor,"Flavor"); 
      if ( uc->ugo_host ) 
         upsugo_prtlst(uc->ugo_host,"Host"); 
      if ( uc->ugo_key ) 
         upsugo_prtlst(uc->ugo_key,"Key"); 
      if ( uc->ugo_tablefiledir ) 
         fprintf(stdout,"Tablefiledir: %s\n",uc->ugo_tablefiledir); 
      if ( uc->ugo_tablefile ) 
         fprintf(stdout,"Tablefile: %s\n",uc->ugo_tablefile); 
      if ( uc->ugo_anyfile ) 
         fprintf(stdout,"Anyfile: %s\n",uc->ugo_anyfile); 
      if ( uc->ugo_options ) 
         fprintf(stdout,"Options: %s\n",uc->ugo_options); 
      if ( uc->ugo_description ) 
         fprintf(stdout,"Description: %s\n",uc->ugo_description); 
      if ( uc->ugo_override ) 
         fprintf(stdout,"Override: %s\n",uc->ugo_override); 
      if ( uc->ugo_qualifiers ) 
         upsugo_prtlst(uc->ugo_qualifiers,"Qualifiers"); 
      if ( uc->ugo_productdir ) 
         fprintf(stdout,"Productdir: %s\n",uc->ugo_productdir); 
      if ( uc->ugo_archivefile ) 
         fprintf(stdout,"Archivefile: %s\n",uc->ugo_archivefile); 
      if ( uc->ugo_upsdir ) 
         fprintf(stdout,"Upsdir: %s\n",uc->ugo_upsdir); 
      if ( uc->ugo_db ) 
         upsugo_prtlst(uc->ugo_db,"DB"); 
      if ( uc->ugo_chain ) 
         upsugo_prtlst(uc->ugo_chain,"Chains"); 
      fprintf(stdout,"ugo_v %d and UPS_VERBOSE %d\n",uc->ugo_v,UPS_VERBOSE); 
    } return (0);
}

/* ==========================================================================
**                                                                           
** ROUTINE: upsugo_prtlst
**                                                                           
** DESCRIPTION                                                               
**
**                                                                           
** VALUES RETURNED                                                           
**      +++                                                                  
**                                                                           
** ==========================================================================
*/
void upsugo_prtlst( t_upslst_item * const list_ptr , char * const title )
{
  t_upslst_item *l_ptr;
  int count = 0;

  /*
   * Note use of upslst_first(), to be sure to start from first item
   */
  fprintf(stdout,"%s\n",title);
  for ( l_ptr = upslst_first( list_ptr ); l_ptr; l_ptr = l_ptr->next, count++ )
  { fprintf(stdout,"ref count %d\n",upsmem_get_refctr(l_ptr->data));
    fprintf(stdout, "%03d: p=%08x, i=%08x, n=%08x, data=%s\n",
            count, (int)l_ptr->prev, (int)l_ptr,
            (int)l_ptr->next, (char *)l_ptr->data );
  }
}
                                                     

/* ==========================================================================
**                                                                           
** ROUTINE: upsugo_env
**                                                                           
** DESCRIPTION                                                               
**
**                                                                           
** VALUES RETURNED                                                           
**      +++                                                                  
**                                                                           
** ==========================================================================
*/                                                                           
t_upsugo_command *upsugo_env(char * const product,char * const validopts)
{
     char * setup_prod;                          /* SETUP_PROD name */
     char * setup_env;                           /* SETUP_PROD value */
     char * waddr;                               /* work address */
     struct ups_command * uc=0;
     int argc=0;
     int    count=0;
     int    length=0;
     char ** argv;
     t_upslst_item *hold = 0;
     
     setup_prod = (char *) malloc((size_t)(strlen(product) +7));
     (void) strcpy(setup_prod,"SETUP_");
     (void) strcat(setup_prod,product);
     if((setup_env = (char *)getenv(setup_prod)) == 0)
     { return (uc);
     } else {
/* I'm going to count the number of spaces in the environment variable
** there cannot be more arguments than spaces...
*/
       waddr=setup_env;
       while ((waddr != 0) && (strlen(waddr) > 0))
             { if ((waddr = strchr(waddr,' ')) != 0) 
                  { for( ; (*waddr == ' ') ; waddr++ ) ; }
                count++;
             }
       count++;  /* add one more for the program who called */
       argv = (char **) malloc((size_t)count*sizeof(char *));
       argv[1] = (char *) malloc((size_t)(strlen("upsugo_env") +1));
       (void) strcpy(argv[1],"upsugo_env");
       waddr=setup_env;
       for (argc = 1;argc < count;argc++)
           { length = (int)strcspn(waddr," ");
             argv[argc] = (char *) malloc((size_t)(length + 1));
             strncpy(argv[argc],waddr,(size_t)length);
             argv[argc][length] = '\0';
             if ((waddr = strchr(waddr, ' ')) != 0) 
                { waddr++;
                  for( ; (*waddr == ' ') ; waddr++ ) ;
                }
           }
     hold=ugo_commands;
     ugo_commands=0;
     uc=upsugo_next(argc,argv,validopts);
     ugo_commands=hold;
     return(uc);
     }
}

/* ==========================================================================
**                                                                           
** ROUTINE: upsugo_bldcmd
**                                                                           
** DESCRIPTION                                                               
**
**                                                                           
** VALUES RETURNED                                                           
**      +++                                                                  
**                                                                           
** ==========================================================================
*/                                                                           
t_upsugo_command *upsugo_bldcmd(char * const cmdstr,char * const validopts)
{
     char * waddr;                               /* work address */
     struct ups_command * uc=0;
     int argc=0;
     int    count=0;
     int    length=0;
     char ** argv;
     t_upslst_item *hold = 0;
     
       waddr=cmdstr;
       while ((waddr != 0) && (strlen(waddr) > 0))
             { if ((waddr = strchr(waddr,' ')) != 0) 
                  { for( ; (*waddr == ' ') ; waddr++ ) ; }
                count++;
             }
       count++;  /* add one more for the program who called */
       argv = (char **) malloc((size_t)count*sizeof(char *));
       argv[1] = (char *) malloc((size_t)(strlen("upsugo_env") +1));
       (void) strcpy(argv[1],"upsugo_bldcmd");
       waddr=cmdstr;
       for (argc = 1;argc < count;argc++)
           { length = (int)strcspn(waddr," ");
             argv[argc] = (char *) malloc((size_t)(length + 1));
             strncpy(argv[argc],waddr,(size_t)length);
             argv[argc][length] = '\0';
             if ((waddr = strchr(waddr, ' ')) != 0) 
                { waddr++;
                  for( ; (*waddr == ' ') ; waddr++ ) ;
                }
           }
     hold=ugo_commands;
     ugo_commands=0;
     uc=upsugo_next(argc,argv,validopts);
     ugo_commands=hold;
     return(uc);
}

/* ==========================================================================
**                                                                           
** ROUTINE: upsugo_next
**                                                                           
** DESCRIPTION                                                               
**
**                                                                           
** VALUES RETURNED                                                           
**      +++                                                                  
**                                                                           
** ==========================================================================
*/                                                                           
t_upsugo_command *upsugo_next(const int ups_argc,char *ups_argv[],char * const validopts)
{
   char   *arg_str;

   char   * addr;
   char   * loc;
   int add_ver=0;
   int my_argc=0;
   int test=0;
   
   char   **argbuf;		/* String to hold residual argv stuff*/
				/* returned by upsugo_getarg */
				/* if contents are used is reset to */
				/* to 0 before recalling getarg */
/* Initialize those pesky variables
    -------------------------------- */
   struct upslst_item * temp;   /* hold clean current chain if version */
   struct ups_command * uc;
   struct ups_command * luc=0;
   uc=(struct ups_command *)upsmem_malloc( sizeof(struct ups_command));
   bzero (uc,sizeof(struct ups_command));
  if ( ugo_commands ) { /* subsequent call */ 
     /* dealloc your brain out */ 
     upsugo_free(ugo_commands->data);	/* free all lists etc in struct */
     last_command=ugo_commands;      /* need pointer to drop & remove struct */
     if ( ugo_commands=ugo_commands->next ) {
        upslst_delete( last_command, last_command->data, 'd');
        luc = ugo_commands->data;
        UPS_VERBOSE = luc->ugo_v;
	upsugo_liststart(luc);       /* move all lists to first element */
        return (t_upsugo_command *)ugo_commands->data; 
     } else {
        return 0;
     }
  } else { 
/* this is VERY important... to make sure argindx is 0 */
/* if there is a subsequent call to upsugo_next for a WHOLE NEW command
** line to be parsed the index must be reset!!!
*/
   argindx=0;
   argbuf = (char **)upsmem_malloc(sizeof(char *)+1);
   *argbuf = 0;
   while ((arg_str= upsugo_getarg(ups_argc, ups_argv, argbuf)) != 0)
   { my_argc+=1; 
     if(*arg_str == '-')      /* is it an option */
     { if (!strchr(validopts,(int)*(arg_str+1))) { 
          upserr_add(UPS_INVALID_ARGUMENT, UPS_FATAL, arg_str+1);
/*          errflg=1; */
       }
       add_ver=0;
       switch(*(arg_str+1))      /* which flag was specified */
       { case 'a':
              uc->ugo_a = 1;
              break;
         case 'C':
              uc->ugo_C = 1;
              break;
         case 'D':
              uc->ugo_D = 1;
              break;
         case 'e':
              uc->ugo_e = 1;
              break;
         case 'E':
              uc->ugo_E = 1;
              break;
         case 'F':
              uc->ugo_F = 1;
              break;
         case 'j':
              uc->ugo_j = 1;
              break;
         case 'k':
              uc->ugo_k = 1;
              break;
         case 'l':
              uc->ugo_l = 1;
              break;
         case 'S':
              uc->ugo_S = 1;
              break;
         case 'u':
              uc->ugo_u = 1;
              break;
         case 'v':
              uc->ugo_v +=1;
              break;
         case 'V':
              uc->ugo_V = 1;
              break;
         case 'w':
              uc->ugo_w = 1;
              break;
         case 'W':
              uc->ugo_W = 1;
              break;
         case 'x':		/* This command is incomplete */
              uc->ugo_x = 1;
              break;
         case 'y':
              uc->ugo_y = 1;
              break;
         case 'Y':
              uc->ugo_Y = 1;
              break;
         case 'Z':
              uc->ugo_Z = 1;
              break;
         case 'c':
              uc->ugo_c = 1;
              addr=upsutl_str_create("current",' ');
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 'n':
              uc->ugo_n = 1;
              addr=upsutl_str_create("new",' ');
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 'd':
              uc->ugo_d = 1;
              addr=upsutl_str_create("development",' ');
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 't':
              uc->ugo_t = 1;
              addr=upsutl_str_create("test",' ');
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 'o':
              uc->ugo_o = 1;
              addr=upsutl_str_create("old",' ');
              uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
              break;
         case 'g':
              uc->ugo_g = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=upsutl_str_create(addr,'p');
                  uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
                }
                addr=upsutl_str_create(*argbuf,'p');
                uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "g" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=upsutl_str_create(addr,'p');
		    uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
                 }
                 addr=upsutl_str_create(arg_str,'p');
                 uc->ugo_chain = upslst_add(uc->ugo_chain,addr);
                 break;
               }
               errflg = 1;
               break;
         case 'f':
              uc->ugo_f = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=upsutl_str_create(addr,'p');
                  uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
                }
                addr=upsutl_str_create(*argbuf,'p');
                uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "f" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=upsutl_str_create(addr,'p');
		    uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
                 }
                    addr=upsutl_str_create(arg_str,'p');
                 uc->ugo_flavor = upslst_add(uc->ugo_flavor,addr);
                 break;
               }
               errflg = 1;
               break;
         case 'h':
              uc->ugo_h = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=upsutl_str_create(addr,'p');
                  uc->ugo_host = upslst_add(uc->ugo_host,addr);
                }
                addr=upsutl_str_create(*argbuf,'p');
                uc->ugo_host = upslst_add(uc->ugo_host,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "h" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=upsutl_str_create(addr,'p');
		    uc->ugo_host = upslst_add(uc->ugo_host,addr);
                 }
                 addr=upsutl_str_create(arg_str,'p');
                 uc->ugo_host = upslst_add(uc->ugo_host,addr);
                 break;
               }
               errflg = 1;
               break;
         case 'K':
              uc->ugo_K = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=upsutl_str_create(addr,'p');
                  uc->ugo_key = upslst_add(uc->ugo_key,addr);
                }
                addr=upsutl_str_create(*argbuf,'p');
                uc->ugo_key = upslst_add(uc->ugo_key,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "K" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=upsutl_str_create(addr,'p');
		    uc->ugo_key = upslst_add(uc->ugo_key,addr);
                 }
                    addr=upsutl_str_create(arg_str,'p');
                 uc->ugo_key = upslst_add(uc->ugo_key,addr);
                 break;
               }
               errflg = 1;
               break;
         case 'm':
              uc->ugo_m = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'p');
                 uc->ugo_tablefiledir = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "m" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'p');
		uc->ugo_tablefiledir = addr;
                break;
              }
              errflg = 1;
              break;
         case 'M':
              uc->ugo_M = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'p');
                 uc->ugo_tablefile = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "M" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'p');
		uc->ugo_tablefile = addr;
                break;
              }
              errflg = 1;
              break;
         case 'N':
              uc->ugo_N = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'p');
                 uc->ugo_anyfile = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "N" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'p');
		uc->ugo_anyfile = addr;
                break;
              }
              errflg = 1;
              break;
         case 'O':
              uc->ugo_O = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'p');
                 uc->ugo_options = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "O" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'p');
		uc->ugo_options = addr;
                break;
              }
              errflg = 1;
              break;
         case 'p':
              uc->ugo_p = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'t');
                 uc->ugo_description = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "p" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'t');
		uc->ugo_description = addr;
                break;
              }
              errflg = 1;
              break;
         case 'P':
              uc->ugo_P = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'p');
                 uc->ugo_override = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "P" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'p');
		uc->ugo_override = addr;
                break;
              }
              errflg = 1;
              break;
         case 'q':
              uc->ugo_q = 1;
              if ( *argbuf ) 
              { test=upsugo_bldqual(uc,*argbuf); *argbuf=0; break; }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "q" );
                   break;
                 }
                 test=upsugo_bldqual(uc,arg_str); break;
               }
               errflg = 1;
               break;
         case 'r':
              uc->ugo_r = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'p');
                 uc->ugo_productdir = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "r" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'p');
		uc->ugo_productdir = addr;
                break;
              }
              errflg = 1;
              break;
         case 'T':
              uc->ugo_T = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'p');
                 uc->ugo_archivefile = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "T" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'p');
		uc->ugo_archivefile = addr;
                break;
              }
              errflg = 1;
              break;
         case 'U':
              uc->ugo_U = 1;
              if ( *argbuf ) 
              {  addr=upsutl_str_create(*argbuf,'p');
                 uc->ugo_upsdir = addr;
                 *argbuf = 0;
                 break;
              }
              if((arg_str = upsugo_getarg(ups_argc,ups_argv, argbuf)) != 0)
              { if(*arg_str == '-')
                { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "U" );
                  break;
                }
                addr=upsutl_str_create(arg_str,'p');
		uc->ugo_upsdir = addr;
                break;
              }
              errflg = 1;
              break;
         case 'A':
              uc->ugo_A = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=upsutl_str_create(addr,'p');
                  uc->ugo_auth = upslst_add(uc->ugo_auth,addr);
                }
                addr=upsutl_str_create(*argbuf,'p');
                uc->ugo_auth = upslst_add(uc->ugo_auth,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "A" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=upsutl_str_create(addr,'p');
		    uc->ugo_auth = upslst_add(uc->ugo_auth,addr);
                 }
                    addr=upsutl_str_create(arg_str,'p');
                 uc->ugo_auth = upslst_add(uc->ugo_auth,addr);
                 break;
               }
               errflg = 1;
               break;
         case 'z':
              uc->ugo_z = 1;
              if ( *argbuf ) 
              { while((loc=strchr(*argbuf,','))!=0) {
                  addr=*argbuf; 
                  *argbuf=loc+1;
                  *loc = 0;		/* replace "," terminate the string */
                  addr=upsutl_str_create(addr,'p');
                  uc->ugo_db = upslst_add(uc->ugo_db,addr);
                }
                addr=upsutl_str_create(*argbuf,'p');
                uc->ugo_db = upslst_add(uc->ugo_db,addr);
                *argbuf = 0;
                break;
               }
               if((arg_str = upsugo_getarg(ups_argc,ups_argv,argbuf)) != 0)
               { if(*arg_str == '-')
                 { upserr_add(UPS_NOVALUE_ARGUMENT, UPS_FATAL, arg_str, "z" );
                   break;
                 }
                 while((loc=strchr(arg_str,','))!=0) {
                    addr=arg_str;
                    arg_str=loc+1;
                    *loc = 0;
                    addr=upsutl_str_create(addr,'p');
		    uc->ugo_db = upslst_add(uc->ugo_db,addr);
                 }
                    addr=upsutl_str_create(arg_str,'p');
                 uc->ugo_db = upslst_add(uc->ugo_db,addr);
                 break;
               }
               errflg = 1;
               break;
         default:
            errflg = 1;
       }
     } else {
       if ( strchr(arg_str,',') == 0 )
       { addr=upsutl_str_create(arg_str,' ');
         if (add_ver) 
         { if(luc->ugo_version) upsmem_free(luc->ugo_version); /* -a put * */
           luc->ugo_version=addr;
/* may have added the current chain if no chain specified if have version
** remove that current chain I added */
           if (added_current)
           { temp = luc->ugo_chain; 
/*             upsugo_prtlst(temp,"the current chain"); */
             upslst_delete( temp, temp->data, 'd');
             luc->ugo_chain=0;
             added_current=0;
           }
           add_ver=0;
         } else { 
           uc->ugo_product = addr;
/*           if (!uc->ugo_flavor) test=upsugo_bldfvr(uc); */
           test = upsugo_ifornota(uc);
           luc=uc;
           add_ver=1;
           ugo_commands = upslst_add(ugo_commands,uc);
           uc=(struct ups_command *)upsmem_malloc( sizeof(struct ups_command));
         } 
       } else { 
/* was it just a , or a ,something? */
         if(strlen(arg_str)==1 || *arg_str==',' )  /* just a comma all alone */
         { add_ver=0;                              /* just in case... */
           if ( strlen(arg_str)!=1 )               /* a ,something not just , */
           { ups_argv[argindx]=arg_str+1;
             argindx=argindx-1;
             my_argc=my_argc-1;
           }
         } else { 
           loc=strchr(arg_str,',');
           if(loc==arg_str)
           { addr=upsutl_str_create(arg_str+1,' ');
           } else {
             *loc=0;
             addr=upsutl_str_create(arg_str,' ');
            *loc=' '; 
           }
           if (add_ver) 
           { luc->ugo_version=addr;
/* may have added the current chain if no chain specified if have version
** remove that current chain I added */
           if (added_current)
           { temp = luc->ugo_chain; 
/*             upsugo_prtlst(temp,"the current chain"); */
             upslst_delete( temp, temp->data, 'd');
             luc->ugo_chain=0;
             added_current=0;
           }
             add_ver=0;
           } else { 
             uc->ugo_product = addr;
/*             if (!uc->ugo_flavor) test=upsugo_bldfvr(uc); */
             test = upsugo_ifornota(uc);
             ugo_commands = upslst_add(ugo_commands,uc);
             uc=(struct ups_command *)upsmem_malloc( sizeof(struct ups_command));
             luc=uc;
           }
/* prod/version, (space) */
           if (strlen(arg_str) != (strlen(addr)+1))
           { ups_argv[argindx]=loc+1;
             argindx=argindx-1;
             my_argc=my_argc-1;
           }
         }
       }
     }
   }
/*    if ((errflg ))
   {   fprintf(stderr, "Valid options are %s\n",validopts); } */
/* no product specification */
/* this DOES NOT MEAN no specifications on the command line the 
   structure is only pushed (addlst) when the product is specified.  
   Therefore if no product it must be pushed but all other flags 
   were still done.
*/
   if (!ugo_commands) 
   { addr=upsutl_str_create("*",' ');
     uc->ugo_product = addr;
/*     if (!uc->ugo_flavor) test=upsugo_bldfvr(uc); */
     test = upsugo_ifornota(uc);             
/* need more ?? ... */
     ugo_commands = upslst_add(ugo_commands,uc);
   }
   ugo_commands=upslst_first(ugo_commands);
   luc = ugo_commands->data;
   UPS_VERBOSE=luc->ugo_v;
   upsugo_liststart(luc);       /* move all lists to first element */
   return (t_upsugo_command *)ugo_commands->data; 
   } /* not subsequent call ... */
}
