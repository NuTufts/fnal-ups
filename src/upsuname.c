#include <unistd.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <string.h>
#include <ctype.h>
#include "upsuname.h"

#define HOST_BUF_SIZE 1024
char **
ups_have_flavor_override(char *hostname) {
    static char **res = 0;
    static char *parts[2];
    static char filename[HOST_BUF_SIZE];
    static char buffer[HOST_BUF_SIZE];
    char *setups_dir;
    char *scan;
    extern char *getenv();
    int fd;

    if (res) { 		
	/* we've been here before... */
        return res;
    }
    if ( 0 != (setups_dir = getenv("SETUPS_DIR")) && 
		strlen(setups_dir) + strlen(hostname) + 8 < HOST_BUF_SIZE ) {

	(void)strcpy(filename, setups_dir);
        (void)strcat(filename, "/flavor.");
        (void)strcat(filename, hostname);
        if ( -1 != (fd = open(filename, O_RDONLY))) {
	    read(fd, buffer, HOST_BUF_SIZE);
	    scan = parts[0] = buffer;
            while( !isspace(*scan) && scan < buffer+HOST_BUF_SIZE) {
		scan++;
	    }
            *scan = 0;
            scan++;
            while( isspace(*scan) && scan < buffer+HOST_BUF_SIZE) {
		scan++;
	    }
	    parts[1] = scan;
            while( !isspace(*scan) && scan < buffer+HOST_BUF_SIZE) {
		scan++;
	    }
            *scan = 0;
            res = parts;
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

   if (0 != (p = ups_have_flavor_override(baseuname.nodename))) {
       (void)strcpy(buf, p[0]);
       return;
   }

   (void) strcpy (buf,baseuname.sysname);	/* get sysname */
   if (!strncmp(buf,"IRIX",4))  		/* Slam all IRIXanything */
   { 
        (void) strcpy(buf,"IRIX");
   }
}

void
ups_append_release(char *buf) 
{
   struct utsname baseuname;              	/* returned from uname */
   static char dstr[80];
   char **p;

   if (uname(&baseuname) == -1) 
       return;     				/* do uname */

   if (0 != (p = ups_have_flavor_override(baseuname.nodename))) {
	(void)strcat(buf, p[1]);
	return;
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

