#include <unistd.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <string.h>
#include <ctype.h>
#include "upsuname.h"

#ifdef __sgi
extern char *strdup(const char *);
extern char *getenv(const char *);
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


	scan = strdup(override);
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
	        res = parts;
		while( !isspace(*scan) && *scan) {
		    scan++;
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
		parts[1] = scan;
	        res = parts;
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

static
char *default_machines[6][2] = {
  {"Linux", "86"},
  {"Darwin", "86"},
  {"SunOS", "sun"},
  {"IRIX", "sgi"},
  {"OSF",  "alpha"},
  { 0, 0 }
};

void
ups_append_OS(char *buf) 
{  
   struct utsname baseuname;                    /* returned from uname */
   int i;
   char *p;

   buf = buf + strlen(buf);     		/* init pointer */
   if (uname(&baseuname) == -1) 
	return;      				/* do uname */

   (void) strcpy (buf,baseuname.sysname);	/* get sysname */
   if (!strncmp(buf,"IRIX",4))  		/* Slam all IRIXanything */
   { 
        (void) strcpy(buf,"IRIX");
   }
   if (!strncmp(buf,"CYGWIN",6))  		/* Slam all IRIXanything */
   { 
        (void) strcpy(buf,"CYGWIN32_NT");
   }
   i = 0;

   /* this tries to find our uname -s, uname -m in the table, above */

   while(default_machines[i][0]) {
       if ( !strncmp(default_machines[i][0],buf,strlen(default_machines[i][0])) ) {
          p = strrchr(baseuname.machine,default_machines[i][1][0]);
          if ( p && !strncmp(p, default_machines[i][1],strlen(default_machines[i][1]))) {
              /* looks like our default machine type, so leave it off */
              return;
          }
       }
       i++;
   }
   /* if it's not a default host, add the machine name on the end */
   strcat(buf,baseuname.machine);
}


void
ups_append_MACHINE(char *buf) 
{  
   struct utsname baseuname;                    /* returned from uname */

   buf = buf + strlen(buf);     		/* init pointer */
   if (uname(&baseuname) == -1) 
	return;      				/* do uname */

   (void) strcpy (buf,baseuname.machine);	/* get sysname */

   /* 64 bit aliases...  rather than the whole machine we return
   **  just a "64" suffix...
   */
   if (0 == strcmp(buf,"x86_64")) {
       strcpy(buf,"64bit");
   }
   if (0 == strncmp(buf,"sun",3)) {
       strcpy(buf,"64bit");		
   }
   if (0 == strncmp(buf,"ppc64",5)) {
       strcpy(buf,"64bit");		
   }
   return;
}

/* cheap kluge for now... */

int 
ups_64bit_check() {
  char buf[512];
  buf[0] = 0;
  ups_append_MACHINE(buf);
  return (buf[0] == '6' && buf[1] == '4');
}
void
ups_append_release(char *buf) 
{
   struct utsname baseuname;              	/* returned from uname */
   static char dstr[80];
   char *pc;

   if (uname(&baseuname) == -1) 
       return;     				/* do uname */

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
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
#if __GLIBC__ * 10 + __GLIBC_MINOR__ > 20
    {
      char *tpoint;
      extern char *gnu_get_libc_version(void);
      char *glibc_version = gnu_get_libc_version();

      tpoint = strchr(baseuname.release,'.');
      if (tpoint) 
	  tpoint = strchr(tpoint+1,'.');  /* after kernel verision */
      if (tpoint) {
	  *tpoint = '-';
	  strcpy(tpoint+1, glibc_version);
      }
    }
#endif
#endif
   (void) strcat(buf,baseuname.release);     /* add in release */
}

/*
** just thinking about this for now...
*/
char *
ups_get_default_quals() {
   char **p;

   if (0 != (p = ups_have_flavor_override()) && p[1] ) {
	return p[1];
   } else {
	return 0;
   }
}

void
ups_make_default_quals_optional() {
   char **p, *s;
   int offset = 0;
   if (0 != (p = ups_have_flavor_override()) && p[1] ) {
	s =  p[1];
        while (*s) {
           *(s-offset) = *(s);
           if (*s == '+') {
              offset++;
           }
           s++;
        }
        *(s-offset) = *(s);
     }
}

char *
ups_get_default_host() {
   char **p;

   if (0 != (p = ups_have_flavor_override()) && p[0]) {
       /* KLUGE put flavor back together */
       return p[0];
   } else {
	return 0;
   }
}
