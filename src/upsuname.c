#include <unistd.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <string.h>
#include <ctype.h>
#include "upsuname.h"

#ifdef __sgi
extern char *strdup();
exterh char *getenv();
#endif

#define HOST_BUF_SIZE 1024

char **
ups_have_flavor_override() {
    static char **res = 0;
    static char *parts[3] = {0,0,0};
    char *override;
    char *scan;
    extern char *getenv();

    if (res) { 		
	/* we've been here before... */
        return res;
    }
    if ( 0 != (override = getenv("UPS_OVERRIDE")) ) {

	res = parts;

	scan = parts[0] = strdup(override);
	while( isspace(*scan) && *scan ) {
	    scan++;
	}
        while( *scan ) {
	    if (0 == strncmp("-H ", scan, 3)) {
                scan += 3;
		while( isspace(*scan) && *scan ) {
		    scan++;
		}
		parts[0] = scan;
		while( !isspace(*scan) && '+' != *scan && *scan) {
		    scan++;
		}
		if ( '+' == *scan ) {
		    *scan = 0;
		    scan++;
		    parts[1] = scan;
		    while( !isspace(*scan) && *scan) {
			scan++;
		    }
		}
		if (*scan) {
		   *scan = 0;
		    scan++;
		}
	    } else if (0 == strncmp("-q ", scan, 3)) {
                scan += 3;
		while( isspace(*scan) && *scan ) {
		    scan++;
		}
		parts[2] = scan;
		while( !isspace(*scan) && *scan ) {
		    scan++;
		}
		if (*scan) {
		   *scan = 0;
		    scan++;
		}
            } else {
		while( !isspace(*scan) && *scan ) {
		    scan++;
		}
            }
	    while( isspace(*scan) && *scan ) {
		scan++;
	    }
        }
    }
    return res;
}

void
ups_append_OS(char *buf) 
{  
   struct utsname baseuname;                    /* returned from uname */
   char **p;

   buf = buf + strlen(buf);     		/* init pointer */
   if (uname(&baseuname) == -1) 
	return;      				/* do uname */

   if (0 != (p = ups_have_flavor_override())) {
       (void)strcpy(buf, p[0]);
       return;
   }

   (void) strcpy (buf,baseuname.sysname);	/* get sysname */
   if (!strncmp(buf,"IRIX",4))  		/* Slam all IRIXanything */
   { 
        (void) strcpy(buf,"IRIX");
   }
   if (!strncmp(buf,"CYGWIN",6))  		/* Slam all IRIXanything */
   { 
        (void) strcpy(buf,"CYGWIN32_NT");
   }
}

void
ups_append_release(char *buf) 
{
   struct utsname baseuname;              	/* returned from uname */
   static char dstr[80];
   char **p, *pc;

   if (uname(&baseuname) == -1) 
       return;     				/* do uname */

   if (0 != (p = ups_have_flavor_override()) && 0 != p[1]) {
	(void)strcat(buf, p[1]);
	return;
   }

   if (0 != (pc = strchr(baseuname.release, '('))) {
	/* releases with parens in them break stuff, so cut it off there */
	*pc = 0;
   }
   if (strncmp(baseuname.sysname,"AIX",3) == 0) /* because AIX is different */
   { 
       (void) strcat(buf,baseuname.version); 	/* add in version */
       (void) strcat(buf,".");
   }
   if(strncmp(baseuname.sysname, "HP-UX", 5) == 0)
   {
     char * tpoint;
     (void)strcpy(dstr, baseuname.release);
     tpoint = strchr(dstr, '.');
     if(0 != tpoint)
       (void)strcpy(baseuname.release, tpoint+1);
   }
   (void) strcat(buf,baseuname.release);     /* add in release */
}

/*
** just thinking about this for now...
*/
void
ups_append_default_quals(char *buf) {
   char **p;

   if (strlen(buf) > 0) {
	(void)strcat(buf,":");
   }
   if (0 != (p = ups_have_flavor_override()) && p[2] ) {
	(void)strcat(buf, p[2]);
	return;
   }
}
