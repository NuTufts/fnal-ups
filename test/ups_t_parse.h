/*
 * ftcl_ParseArgv.h - Generic ftcl command-line parser. 
 *                    Heavily inspired by the Tk command line parser.
 *
 */

#ifndef _UPS_T_PARSE_H
#define _UPS_T_PARSE_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
/*
 * Structure to specify how to handle command line options
 */
typedef struct {
    char *key;          /* The key string that flags the option in the
                         * argv array. */
    int type;           /* Indicates option type;  see below. */
    void *src;          /* Value to be used in setting dst;  usage
                         * depends on type. */
    void *dst;          /* Address of value to be modified;  usage
                         * depends on type. */
} ups_t_argt;

int ups_t_parse(int * const, char ** const , ups_t_argt * const);
int ups_t_split(char *, int * const, char *** const);

void ups_t_print_usage (const ups_t_argt * const argTable,const char *cmd_name);
void ups_t_print_help (ups_t_argt *argTable, char *cmd_name);
void ups_t_print_arg (ups_t_argt *argTable, char *cmd_name);

/* defines for return value of ftcl_ParseArgv */

#define UPS_T_SUCCESS   0 
#define UPS_T_BADSYNTAX	1
#define UPS_T_USAGE	2

/*
 * Legal values for the type field of a ups_t_argt: see the user
 * documentation for details.
 */
 
#define UPS_T_ARGV_CONSTANT                0x00000064
#define UPS_T_ARGV_INT                     0x00000065
#define UPS_T_ARGV_STRING                  0x00000066
#define UPS_T_ARGV_DOUBLE                  0x00000068
#define UPS_T_ARGV_END                     0x0000006B

#define UPS_T_USAGE_BUFSIZE    1025
#define UPS_T_ARGINFO_BUFSIZE  8193

#endif

