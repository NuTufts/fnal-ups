void ups_append_OS(char *flavor);
void ups_append_release(char *flavor);

ups_append_OS(char *buf) 
{  
   struct utsname baseuname;                    /* returned from uname */
   buf = buf + strlen(buf);            /* init pointer */
   if (uname(&baseuname) == -1) return(0);      /* do uname */
   (void) strcpy (buf,baseuname.sysname);    /* get sysname */
   if (!strncmp(buf,"IRIX",4))               /* Slam all IRIXanything */
   { (void) strcpy(buf,"IRIX");
   }
}

void
ups_append_release(char *buf) 
{
   struct utsname baseuname;              	/* returned from uname */
   static char dstr[80];

   if (uname(&baseuname) == -1) return(0);     	/* do uname */
   if (strncmp(baseuname.sysname,"AIX",3) == 0) /* because AIX is different */
   { (void) strcat(buf,baseuname.version); 	/* add in version */
     (void) strcat(buf,".");
   }
   if(strncmp(baseuname->sysname, "HP-UX", 5) == 0)
   {
     (void)strcpy(dstr, baseuname->release);
     tpoint = strchr(dstr, '.') ;
     if(tpoint != NULL)
       (void)strcpy(baseuname->release, tpoint+1);
   }
   (void) strcat(buf,baseuname.release);     /* add in release */
}
