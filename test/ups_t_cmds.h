#ifndef UPSTCMDTABLE
#define UPSTCMDTABLE
/*****************************************************************************
Authors:        Margaret Votava
e-mail:         "votava@fnal.gov"
 
Header file for command line editing and entry

Revision history:-
=================
17-Oct-1995 MEV created 

*/

typedef struct {
   char	*cmdname;
   int	(*func)(int argc, char **argv);
   } ups_t_cmd_table_t;

#endif

