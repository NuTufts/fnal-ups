/************************************************************************
 *
 * FILE:
 *       ups_files.c
 * 
 * DESCRIPTION: 
 *       Will read an ups file, and fill corresponding data structures.
 *
 * AUTHORS:
 *       Eileen Berman
 *       David Fagan
 *       Lars Rasmussen
 *
 *       Fermilab Computing Division
 *       Batavia, Il 60510, U.S.A.
 *
 * MODIFICATIONS:
 *       23-jun-1997, LR, first.
 *       31-jul-1997, LR, Added use of upsmem.
 *
 ***********************************************************************/

/* standard include files */
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* ups specific include files */
#include "ups_files.h"
#include "ups_types.h"
#include "ups_list.h"
#include "ups_memory.h"

/*
 * Definition of public variables
 */

/*
 * Declaration of private functions
 */
static int            read_file( void );
static int            read_file_desc( void );
static t_ups_instance *read_instance( void );
static t_upslst_item  *read_instances( void );
static t_ups_action   *read_action( void );
static t_upslst_item  *read_actions( void );
static t_upslst_item  *read_group( void );
static t_upslst_item  *read_groups( void );

/* Line parsing*/
static char*          get_key( void );
static char*          next_key( void );
static size_t         trim_line( void );
static int            is_stop_key( void );
static int            is_start_key( void );

/* Utils */
static char           *str_create( char * const str );
static int            is_space( const char c );

/* Print stuff */
static void           print_instance( t_ups_instance * const inst_ptr );
static void           print_action( t_ups_action * const act_ptr );
/* print_product has gone semi public */

/*
 * Definition of global variables
 */
#define MAX_LINE_LENGTH 1024 /* max length of a line */

static FILE           *g_fh = NULL; /* passed file handle */
static t_ups_product  *g_pd = NULL; /* current product to fill */

static char           g_line[MAX_LINE_LENGTH] = "";  /* current line */
static char           g_key[MAX_LINE_LENGTH] = "";   /* current key */
static char           g_val[MAX_LINE_LENGTH] = "";   /* current value */
  
/*
 * Definition of public functions
 */

/*-----------------------------------------------------------------------
 * upsfil_read_file
 *
 * Will read all instances in passed file
 *
 * Input : FILE*, file handle
 * Output: none
 * Return: t_ups_product *, a pointer to a product
 */
t_ups_product *upsfil_read_file( FILE * const fh )
{
  g_fh = fh;
  g_pd = ups_new_product();

  if ( !g_pd ) { fprintf( stderr, "Buy more memory !!!\n"); return 0; }
       
  g_line[0] = 0;
  g_key[0] = 0;
  g_val[0] = 0;
  
  read_file();

  return g_pd;
}

/*
 * Definition of private functions
 */

/*-----------------------------------------------------------------------
 * read_file
 *
 * Will read all instances in an ups file
 *
 * Input : none
 * Output: none
 * Return: int, number of instances read
 */
int read_file( void )
{
  t_upslst_item *l_ptr = NULL;
  
  /* advance to something useful */
  
  next_key();

  /* read file descriptor */

  if ( ! read_file_desc() ) {
    fprintf( stderr, "Could not read file desciptor\n" );
    return 0;
  }

  /* from here on, we expect only to see FLAVOR or GROUP: */
  
  while ( !strcmp( g_key, "FLAVOR") || !strcmp( g_key, "GROUP:" ) ) {
    l_ptr = NULL;
    
    if ( !strcmp( g_key, "FLAVOR" ) )
      l_ptr =  read_instances();
    
    else if ( !strcmp( g_key, "GROUP:" ) )
      l_ptr = read_groups();
    
    if ( l_ptr ) 
      g_pd->instance_list = upslst_merge( g_pd->instance_list, l_ptr );
  }
	  
  return 1;
}

/*-----------------------------------------------------------------------
 * read_file_desc
 *
 * Will read file descriptor: FILE, PRODUCT, VERSION/CHAIN.
 *
 * Input:  none
 * Output: none
 * Return: int,  1 if file descriptor was read, else 0
 */
int read_file_desc( void )
{
  if ( strcmp( g_key, "FILE" ) )
    return 0;
  
  g_pd->file = str_create( g_val );
  
  while ( next_key() ) {
    if ( is_stop_key() ) break;

    if ( !strcmp( g_key, "PRODUCT" ) )
      g_pd->product = str_create( g_val );
    
    else if ( !strcmp( g_key, "VERSION" ) )
      g_pd->chaver = str_create( g_val );
		 
    else if ( !strcmp( g_key, "CHAIN" ) )
      g_pd->chaver = str_create( g_val );
		 
    }

    return 1;
}

/*-----------------------------------------------------------------------
 * read_instances
 *
 * Will build a list of instances. Reading instances will continue
 * until next 'stop' key is not "FLAVOR".
 *
 * Input : none
 * Output: none
 * Return: t_upslst_item *, pointer to a list of instances
 */
t_upslst_item *read_instances( void )
{
    t_upslst_item *l_ptr = NULL;
    t_ups_instance *inst_ptr = NULL;
    
    while  ( !strcmp( g_key, "FLAVOR" ) ) {
	inst_ptr = read_instance();
	
	if ( inst_ptr ) {
	  l_ptr = upslst_add( l_ptr, inst_ptr );
	}
	else {
	  break;
	}
    }

    return upslst_first( l_ptr );
}
  
/*-----------------------------------------------------------------------
 * read_instance
 *
 * Will read a single instance
 *
 * Input : none
 * Output: none
 * Return: pointer to an instance
 */
t_ups_instance *read_instance( void )
{
  t_ups_instance *inst_ptr = NULL;
  
  if ( strcmp( g_key, "FLAVOR" ) )
    return 0;    

  inst_ptr = ups_new_instance();

  /* fill information from file descriptor */
  
  inst_ptr->product = str_create( g_pd->product );  
  if ( !strcmp( g_pd->file, "CHAIN" ) ) {
    inst_ptr->chain = str_create( g_pd->chaver );
  }
  else {
    inst_ptr->version = str_create( g_pd->chaver );
  }
    
  /* fill information from found key words */
  
  inst_ptr->flavor = str_create( g_val );
    
  while ( next_key() ) {
    if ( !strcmp( g_key, "ACTION" ) ) {
      inst_ptr->action_list = read_actions();
    }

    if ( is_stop_key() ) break;

    if ( !strcmp( g_key, "QUALIFIERS" ) ) {
      inst_ptr->qualifiers = str_create( g_val );
    }
    
    else if ( !strcmp( g_key, "VERSION" ) ) {
      inst_ptr->version = str_create( g_val );
    }
    
    else if ( !strcmp( g_key, "DECLARER" ) ) {
      if ( !strcmp( g_pd->file, "CHAIN" ) ) 
	inst_ptr->chain_declarer = str_create( g_val );
      else 
	inst_ptr->declarer = str_create( g_val );
    }
		 
    else if ( !strcmp( g_key, "DECLARED" ) ) {
      if ( !strcmp( g_pd->file, "CHAIN" ) ) 
	inst_ptr->chain_declared = str_create( g_val );
      else 
	inst_ptr->declared = str_create( g_val );
    }
		 
    else if ( !strcmp( g_key, "PROD_DIR" ) ) {
      inst_ptr->prod_dir = str_create( g_val );
    }
		 
    else if ( !strcmp( g_key, "UPS_DIR" ) ) {
      inst_ptr->ups_dir = str_create( g_val );
    }
		 
    else if ( !strcmp( g_key, "TABLE_DIR" ) ) {
      inst_ptr->table_dir = str_create( g_val );
    }
		 
    else if ( !strcmp( g_key, "TABLE_FILE" ) ) {
      inst_ptr->table_file = str_create( g_val );
    }
		 
    else if ( !strcmp( g_key, "ARCHIVE_FILE" ) ) {
      inst_ptr->archive_file = str_create( g_val );
    }
    
    else if ( !strcmp( g_key, "AUTHORIZED_NODES" ) ) {
      inst_ptr->authorized_nodes = str_create( g_val );
    }
      
    else if ( !strcmp( g_key, "DESCRIPTION" ) ) {
      inst_ptr->description = str_create( g_val );
    }
    
    else {
      /* add_unknown( g_key, g_val ) */; 
    }
		 
  }

  return inst_ptr;
}

/*-----------------------------------------------------------------------
 * read_actions
 *
 * Will build a list of actions. Reading actions will continue
 * until next 'stop' key is not "ACTION".
 *
 * Input : none
 * Output: none
 * Return: pointer to a list of actions
 */
t_upslst_item *read_actions( void )
{
    t_upslst_item *l_ptr = NULL;
    t_ups_action *act_ptr = NULL;
    
    while  ( !strcmp( g_key, "ACTION" ) ) {
	act_ptr = read_action();
	
	if ( act_ptr ) {
	  l_ptr = upslst_add( l_ptr, act_ptr );
	}
	else {
	  break;
	}
    }

    return upslst_first( l_ptr );
}

/*-----------------------------------------------------------------------
 * read_action
 *
 * Will read a single action
 *
 * Input : none
 * Output: none
 * Return: pointer to an action
 */
t_ups_action *read_action( void )
{
  t_ups_action *act_ptr = NULL;
  t_upslst_item *l_cmd = NULL;
  char *cmd_ptr = NULL;

  if ( strcmp( g_key, "ACTION" ) )
    return 0;

  act_ptr = ups_new_action();
  act_ptr->action = str_create( g_val );

  while ( next_key() ) {
    if ( is_stop_key() ) break;

    if ( strlen( g_line ) > 0 ) {
      cmd_ptr = str_create( g_line );
      if ( cmd_ptr )
	l_cmd = upslst_add( l_cmd, cmd_ptr );
    }
  }

  act_ptr->command_list = upslst_first( l_cmd );
  
  return act_ptr;
}

/*-----------------------------------------------------------------------
 * read_groups()
 *
 * Will read a list of groups. Reading groups will
 * continue until 'stop' key is not "GROUP:" 
 *
 * Input : none.
 * Output: none.
 * Return: t_upslst_item *, pointer to a list of instances
 */
t_upslst_item *read_groups( void )
{    
    t_upslst_item *l_ptr = NULL;
    t_upslst_item *l_tmp_ptr = NULL;
    
    while  ( !strcmp( g_key, "GROUP:" ) ) {
	l_tmp_ptr = read_group();

	if ( l_tmp_ptr ) {
	  l_ptr = upslst_merge( l_ptr, l_tmp_ptr );
	}
	else {
	  break;
	}
	
    }

    return upslst_first( l_ptr );
}

/*-----------------------------------------------------------------------
 * read_group
 *
 * Will read a single group.
 *
 * Input : none.
 * Output: none.
 * Return: t_upslst_item *, pointer to a list of instances
 */
t_upslst_item *read_group( void )
{
    t_upslst_item *l_ptr = NULL;
    t_upslst_item *l_inst_ptr = NULL;
    t_upslst_item *l_act_ptr = NULL;
    t_ups_instance *inst_ptr = NULL;

    if ( strcmp( g_key, "GROUP:" ) )
      return 0;
    
    if ( strcmp( next_key(), "FLAVOR" ) )
      return 0;

    l_inst_ptr = read_instances();

    if ( !l_inst_ptr )
      return 0;

    if ( !strcmp( g_key, "COMMON:" ) ) {
	next_key();
	l_act_ptr = read_actions();
    }

    /* add actions to instances */

    if ( l_act_ptr ) {
      l_ptr = upslst_first( l_inst_ptr );
      inst_ptr = (t_ups_instance *)l_ptr->data;
      inst_ptr->action_list = l_act_ptr;
      l_ptr = l_ptr->next;
      for ( ; l_ptr; l_ptr = l_ptr->next ) {
	inst_ptr = (t_ups_instance *)l_ptr->data;
	inst_ptr->action_list = upslst_copy( l_act_ptr );
      }
    }

    next_key();
    while ( !strcmp( g_key, "END:" ) ) { next_key(); }
    
    return l_inst_ptr;
}

/*
 * Line parsing
 */

/*-----------------------------------------------------------------------
 * next_key
 *
 * Will read file and find next pair of g_key and g_val 
 *
 * Input : none
 * Output: none
 * Return: char*, pointer to current key;
 */
char *next_key( void )
{  
  g_key[0] = 0;
  g_val[0] = 0;
  g_line[0] = 0;

  while ( fgets( g_line, MAX_LINE_LENGTH, g_fh ) ) {

    if ( strlen( g_line ) < 1 ) continue;
    if ( g_line[0] == '#' ) continue;
    if ( !trim_line() ) continue;

    if ( get_key() ) {
      return g_key;
    }
  }

  return NULL;
}

/*-----------------------------------------------------------------------
 * get_key
 *
 * Will parse the current line (g_line) and fill current key (g_key) and
 * current value (g_val)
 *
 * Input : none
 * Output: none
 * Return: char*, pointer to current key;
 */
char *get_key( void )
{
  char *cp = g_line;
  int count = 0;

  /* check if line is not empty (again) */
  
  if ( strlen( g_line ) < 1 ) return 0;
  if ( g_line[0] == '#' ) return 0;
    
  /* check if line has a key/value pair */
  
  if ( !strchr( g_line, '=' ) ) {
    strcpy( g_key, g_line );
    return g_key;
  }

  /* split line into key/value pair */
  
  count = 0;
  while ( cp && *cp && !is_space( *cp ) && *cp != '=' ) {
    g_key[count] = *cp;
    count++; cp++;
  }
  g_key[count] = 0;
  if ( strlen( g_key ) <= 0 ) return 0;
  
  while( cp && *cp && *cp != '=' ) { cp++; }
  cp++;
  while( cp && *cp && is_space( *cp ) ) { cp++; }
  count = 0;
  while( cp && *cp && *cp != '\n' ) {
    g_val[count] = *cp;
    count++; cp++;
  }
  g_val[count] = 0;

  return g_key;
}

/*-----------------------------------------------------------------------
 * trim_line
 *
 * Will erase trailing and starting white spaces on current line (g_line)
 *
 * Input : none
 * Output: none
 * Return: int, length of trimmed line
 */
size_t trim_line( void )
{
  char *cp = g_line;
  char *cstart = NULL;
  char *cend = NULL;
  int count = 0;
  
  while ( cp && is_space( *cp ) ){ cp++; }
  cstart = cp;
  cp = &g_line[strlen( g_line ) - 1];
  while ( cp && is_space( *cp ) ){ cp--; }
  cend = cp;

  count = 0;
  for ( cp=cstart; cp<=cend; cp++, count++ ) {
    g_line[count] = *cp;
  }
  g_line[count] = 0;

  return strlen( g_line );
}

int is_start_key( void )
{
  if ( strcmp( g_key, "FLAVOR" ) == 0 )
    return 1;
  if ( strcmp( g_key, "GROUP:" ) == 0 )
    return 1;
  if ( strcmp( g_key, "COMMON:" ) == 0 )
    return 1;
  if ( strcmp( g_key, "FILE" ) == 0 )
    return 1;
  if ( strcmp( g_key, "ACTION" ) == 0 )
    return 1;

  return 0;
}

int is_stop_key( void )
{
  if ( is_start_key() )
    return 1;
  if ( !strcmp( g_key, "END:" ) )
    return 1;

  return 0;
}

/*
 * Utils
 */

char *str_create( char * const str )
{
  char *new_str = NULL;
    
  if ( str ) {
    new_str = (char *)upsmem_malloc( (int)strlen( str ) + 1 );
    strcpy( new_str, str );
  }
  
  return new_str;
}

int is_space( const char c )
{
  if ( c == ' ' ) return 1;
  if ( c == '\t' ) return 1;
  if ( c == '\n' ) return 1;
  if ( c == '\r' ) return 1;
  if ( c == '\f' ) return 1;

  return 0;
}


/*
 * Print stuff
 */

void print_instance( t_ups_instance * const inst_ptr )
{
  t_upslst_item *l_ptr = NULL;
  
  if ( !inst_ptr ) return;
  
  printf( "\nproduct = %s\nversion = %s\nflavor = %s\nqualifiers = %s\n",
	  inst_ptr->product, inst_ptr->version, inst_ptr->flavor,
	  inst_ptr->qualifiers );
  
  printf( "chain = %s\ndeclarer = %s\ndeclared = %s\n",
	  inst_ptr->chain, inst_ptr->declarer, inst_ptr->declared );
  
  printf( "chain_declarer = %s\nchain_declared = %s\nprod_dir = %s\nups_dir = %s\n",
	  inst_ptr->chain_declarer, inst_ptr->chain_declared, inst_ptr->prod_dir,
	  inst_ptr->ups_dir );
  
  printf( "table_dir = %s\ntable_file = %s\narchive_file = %s\nauthorized_nodes = %s\n",
	  inst_ptr->table_dir, inst_ptr->table_file, inst_ptr->archive_file,
	  inst_ptr->authorized_nodes );
  
  printf( "description = %s\n",
	  inst_ptr->description );

  if ( inst_ptr->action_list ) {
    printf( "Actions = \n" );
    l_ptr = upslst_first( inst_ptr->action_list );
    for ( ; l_ptr; l_ptr = l_ptr->next ) {
      print_action( l_ptr->data );
    }
  }
  else {
    printf( "Actions = %s\n", (char*)NULL );
  }
}

void print_action( t_ups_action * const act_ptr )
{
  t_upslst_item *l_ptr = NULL;
  
  if ( !act_ptr ) return;

  printf ( "action = %s\n", act_ptr->action );
  
  if ( act_ptr->command_list ) {
    printf( "Command list = \n" );
    l_ptr = upslst_first( act_ptr->command_list );
    for ( ; l_ptr; l_ptr = l_ptr->next ) {
      printf( "   %s\n", (char*)l_ptr->data );
    }
  }
  else {
    printf( "Command list = %s\n", (char*)NULL );
  }
}

void g_print_product( t_ups_product * const prod_ptr )
{
  t_upslst_item *l_ptr = NULL;
  int count = 0;

  printf( "\nfile    = %s\n", prod_ptr->file );
  printf( "product = %s\n", prod_ptr->product );
  printf( "chaver  = %s\n", prod_ptr->chaver );
  printf( "instance_list:\n" );

  
  l_ptr = upslst_first( prod_ptr->instance_list );
  for ( ; l_ptr; l_ptr = l_ptr->next, count++) {
    t_ups_instance *inst_ptr = (t_ups_instance *)l_ptr->data;
    print_instance( inst_ptr );
  }     
}












