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
 *       23-Jun-1997, LR, first.
 *       31-Jul-1997, LR, Added use of upsmem.
 *       12-Aug-1997, LR, Added upsfil_write.
 *
 ***********************************************************************/

/* standard include files */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* ups specific include files */
#include "ups_utils.h"
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
static t_upslst_item  *read_comments( void );

static int            write_version_file( void );
static int            write_chain_file( void );
static int            write_table_file( void );
static int            write_action( t_ups_action *act );


/* Line parsing */
static int            get_key( void );
static int            next_key( void );
static int            is_stop_key( void );
static int            is_start_key( void );
static int            put_key( const char * const key, const char * const val );

/* Utils */
static int            trim_qualifiers( char * const str );
static char           *str_create( char * const str );
static int            is_space( const char c );
static int            ckeyi( void );
static int            cfilei( void );

/* Print stuff */
static void           print_instance( t_ups_instance * const inst_ptr );
static void           print_action( t_ups_action * const act_ptr );
/* print_product has gone semi public */

/*
 * Definition of global variables
 */

/* enum of known keys (changes here should be reflected in ckeyi) */
enum e_ups_key {
  e_key_eol = -2,
  e_key_eof = -1,
  e_key_file = 0,
  e_key_product,
  e_key_version,
  e_key_chain,
  e_key_ups_db_version,

  e_key_flavor,
  e_key_qualifiers,
  e_key_declarer,
  e_key_declared,
  e_key_prod_dir,
  e_key_ups_dir,
  e_key_table_dir,
  e_key_table_file,
  e_key_archive_file,
  e_key_authorized_nodes,
  e_key_description,
  e_key_statistics,

  e_key_action,
  
  e_key_group,
  e_key_common,
  e_key_end,

  e_key_unknown,
  
  e_key_count
};

/* enum of known file types (changes here should be reflected in cfilei) */
enum e_ups_file {
  e_file_version = 0,
  e_file_table,
  e_file_chain,
  e_file_dbconfig,
  e_file_unknown,
  e_file_count
};

#define CHAR_REMOVE " \t\n\r\f\""

static t_ups_product  *g_pd = 0; /* current product to fill */
static FILE           *g_fh = 0; /* file handle */

static char           g_line[MAX_LINE_LEN] = "";  /* current line */
static char           g_key[MAX_LINE_LEN] = "";   /* current key */
static char           g_val[MAX_LINE_LEN] = "";   /* current value */
static int            g_ikey = e_key_unknown;        /* current key as enum */  
static int            g_ifile = e_file_unknown;      /* current file type as enum */

static int            g_imargin = 0;

/*
 * Definition of public functions
 */

/*-----------------------------------------------------------------------
 * upsfil_read_file
 *
 * Will read all instances in passed file
 *
 * Input : char *, path to a ups file
 * Output: none
 * Return: t_ups_product *, a pointer to a product
 */
t_ups_product *upsfil_read_file( const char * const ups_file )
{
  g_fh = fopen ( ups_file, "r" );
  if ( ! g_fh ) { fprintf( stderr, "Error opening file %s\n", ups_file ); return 0; }
  
  g_pd = ups_new_product();
  if ( !g_pd ) { fprintf( stderr, "Buy more memory !!!\n"); return 0; }
       
  read_file();
  
  fclose( g_fh );  
  return g_pd;
}

/*-----------------------------------------------------------------------
 * upsfil_write_file
 *
 * Will write a product to a ups file
 *
 * Input : t_ups_product *, a pointer to a product
 *         char *, path to a ups file
 * Output: none
 * Return: int, 1 just fine, 0 somthing went wrong
 */
int upsfil_write_file( t_ups_product * const prod_ptr,
		       const char * const ups_file )
{
  t_upslst_item *l_ptr = 0;
  
  if ( ! prod_ptr )
    return 0;
  g_pd = prod_ptr;

  if ( strlen( ups_file ) <= 0 )
    return 0;
  g_fh = fopen ( ups_file, "w" );
  if ( ! g_fh ) { fprintf( stderr, "Error opening file %s\n", ups_file ); return 0; }

  g_imargin = 0;
  
  /* write comments */

  l_ptr = upslst_first( g_pd->comment_list );
  for( ; l_ptr; l_ptr = l_ptr->next ) {
    fprintf( g_fh, "%s\n", (char *)l_ptr->data );
  }    

  /* write file type */
  
  if ( g_pd->file ) {
    put_key( "FILE", g_pd->file );
    cfilei();
    /* check for valid file type !!! */
  }
  else {
    fprintf( stderr, "No file type specified\n" );
    return 0;
  }
    
  /* make groups !!! */
  
  /* write instances */

  switch ( g_ifile ) {

  case e_file_version:
    write_version_file();
    break;
	
  case e_file_table:
    write_table_file();
    break;
	
  case e_file_chain:
    write_chain_file();
    break;
  }

  
  fclose( g_fh );

  return 1;
}
     
/*
 * Definition of private functions
 */

int write_version_file( void )
{
  t_upslst_item *l_ptr = 0;
  t_ups_instance *inst_ptr = 0;

  /* write file descriptor */
  
  put_key( "PRODUCT", g_pd->product );
  put_key( "VERSION", g_pd->chaver );
  put_key( "UPS_DB_VERSION", g_pd->ups_db_version );

  /* write instances */
  
  l_ptr = upslst_first( g_pd->instance_list );
  for( ; l_ptr; l_ptr = l_ptr->next ) {
    inst_ptr = (t_ups_instance *)l_ptr->data;
    if ( !inst_ptr || !inst_ptr->flavor ) {
      /* handle error !!! */
      return 0;
    }

    put_key( 0, "" );
    put_key( "FLAVOR", inst_ptr->flavor );
    put_key( "QUALIFIERS", inst_ptr->qualifiers );
    
    g_imargin += 2;    
    put_key( "DECLARED", inst_ptr->declared );
    put_key( "DECLARER", inst_ptr->declarer );
    put_key( "PROD_DIR", inst_ptr->prod_dir );
    put_key( "UPS_DIR", inst_ptr->ups_dir );
    put_key( "TABLE_DIR", inst_ptr->table_dir );
    put_key( "TABLE_FILE", inst_ptr->table_file );
    put_key( "ARCHIVE_FILE", inst_ptr->archive_file );
    put_key( "AUTHORIZED_NODES", inst_ptr->authorized_nodes );
    if ( inst_ptr->statistics )
      put_key( 0, "STATISTICS" );    
    g_imargin -= 2;
  }  

  return 1;
} 

int write_chain_file( void )
{
  t_upslst_item *l_ptr = 0;
  t_ups_instance *inst_ptr = 0;

  /* write file descriptor */
  
  put_key( "PRODUCT", g_pd->product );
  put_key( "CHAIN", g_pd->chaver );
  put_key( "UPS_DB_VERSION", g_pd->ups_db_version );

  /* write instances */
  
  l_ptr = upslst_first( g_pd->instance_list );
  for( ; l_ptr; l_ptr = l_ptr->next ) {
    inst_ptr = (t_ups_instance *)l_ptr->data;
    if ( !inst_ptr || !inst_ptr->flavor ) {
      /* handle error !!! */
      return 0;
    }


    put_key( 0, "" );
    put_key( "FLAVOR", inst_ptr->flavor );
    put_key( "QUALIFIERS", inst_ptr->qualifiers );
    
    g_imargin += 2;
    put_key( "VERSION", inst_ptr->version );
    put_key( "DECLARED", inst_ptr->chain_declared );
    put_key( "DECLARER", inst_ptr->chain_declarer );
    g_imargin -= 2;
  }  

  return 1;
} 

int write_table_file( void )
{
  t_upslst_item *l_inst = 0;
  t_upslst_item *l_act = 0;
  t_ups_instance *inst_ptr = 0;
  t_ups_action *act_ptr = 0;

  /* write file descriptor */
  
  put_key( "PRODUCT", g_pd->product );
  put_key( "VERSION", g_pd->chaver );
  put_key( "UPS_DB_VERSION", g_pd->ups_db_version );

  /* write instances */
  
  l_inst = upslst_first( g_pd->instance_list );
  for( ; l_inst; l_inst = l_inst->next ) {
    inst_ptr = (t_ups_instance *)l_inst->data;
    if ( !inst_ptr || !inst_ptr->flavor ) {
      /* handle error !!! */
      return 0;
    }


    put_key( 0, "" );
    put_key( "FLAVOR", inst_ptr->flavor );
    put_key( "QUALIFIERS", inst_ptr->qualifiers );
    
    g_imargin += 2;
    put_key( "DESCRIPTION", inst_ptr->description );
    l_act = upslst_first( inst_ptr->action_list );
    for( ; l_act; l_act = l_act->next ) {
      act_ptr = (t_ups_action *)l_act->data;
      write_action( act_ptr );
    }
    g_imargin -= 2;
  }  

  return 1;
} 

int write_action( t_ups_action* act_ptr )
{
  t_upslst_item *l_com = 0;
  char *com;

  if ( !act_ptr ) return 0;

  put_key( "ACTION", act_ptr->action );
  
  g_imargin += 2;
  l_com = upslst_first( act_ptr->command_list );
  for( ; l_com; l_com = l_com->next ) {
    com = (char * )l_com->data;
    put_key( 0, com );
  }
  g_imargin -= 2;

  return 1;
}

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
  t_upslst_item *l_ptr = 0;
  
  /* read comments */

  g_pd->comment_list = read_comments();
  
  /* read file descriptor */

  if ( ! read_file_desc() ) {
    fprintf( stderr, "Could not read file desciptor\n" );
    return 0;
  }

  /* here, we expect only to see FLAVOR or GROUP: */

  while ( g_ikey != e_key_eof ) {
    l_ptr = 0;

    switch( g_ikey ) {

    case e_key_flavor:
      l_ptr = read_instances();
      break;
    
    case e_key_group:
      l_ptr = read_groups();
      break;

    default:
      next_key();
    }
    
    if ( l_ptr ) 
      g_pd->instance_list = upslst_merge( g_pd->instance_list, l_ptr );
  }
	  
  return 1;
}

/*-----------------------------------------------------------------------
 * read_comments
 *
 * Will read a ups files comments (lines at the top of the file
 * starting with a '#').
 *
 * Input:  none
 * Output: none
 * Return: t_upslst_item *, pointer to a list of strings
 */
t_upslst_item *read_comments( void )
{
  t_upslst_item *l_ptr = 0;
  
  while ( fgets( g_line, MAX_LINE_LEN, g_fh ) ) {

    if ( !upsutl_str_remove_edges( g_line, CHAR_REMOVE ) ) continue;   
    if ( g_line[0] == '#' ) {
      l_ptr = upslst_add( l_ptr, str_create( g_line ) );
    }
    else {
      get_key();
      break;
    }
  }
  
  return l_ptr;
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
  if ( g_ikey != e_key_file )
    return 0;
  
  g_pd->file = str_create( g_val );
  cfilei(); /* translate file type to an enum */
  
  while ( next_key() != e_key_eof ) {

    switch( g_ikey ) {

    case e_key_product:
      g_pd->product = str_create( g_val );
      break;

    case e_key_version:
    case e_key_chain:
      g_pd->chaver = str_create( g_val );
      break;
      
    case e_key_ups_db_version:
      g_pd->ups_db_version = str_create( g_val );
      break;
    }
		 
    if ( is_stop_key() ) break;
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
    t_upslst_item *l_ptr = 0;
    t_ups_instance *inst_ptr = 0;
    
    while  ( g_ikey == e_key_flavor ) {
	inst_ptr = read_instance();
	
	if ( inst_ptr )
	  l_ptr = upslst_add( l_ptr, inst_ptr );
	else
	  break;
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
  t_ups_instance *inst_ptr = 0;
  
  if ( g_ikey != e_key_flavor )
    return 0;    

  inst_ptr = ups_new_instance();

  /* fill information from file descriptor */
  
  inst_ptr->product = str_create( g_pd->product );
  if ( g_ifile == e_file_chain )
    inst_ptr->chain = str_create( g_pd->chaver );
  else
    inst_ptr->version = str_create( g_pd->chaver );
    
  /* fill information from file ... we still need a map */
  
  inst_ptr->flavor = str_create( g_val );
    
  while ( next_key() != e_key_eof ) {

    switch( g_ikey ) {

    case e_key_action:
      inst_ptr->action_list = read_actions();
      break;

    case e_key_qualifiers:
      trim_qualifiers( g_val );
      inst_ptr->qualifiers = str_create( g_val );
      break;
    
    case e_key_version:
      inst_ptr->version = str_create( g_val );
      break;
    
    case e_key_declarer: 
      if ( g_ifile == e_file_chain  ) 
	inst_ptr->chain_declarer = str_create( g_val );
      else 
	inst_ptr->declarer = str_create( g_val );
      break;
    		 
    case e_key_declared:
      if ( g_ifile == e_file_chain  ) 
	inst_ptr->chain_declared = str_create( g_val );
      else 
	inst_ptr->declared = str_create( g_val );
      break;
		 
    case e_key_prod_dir:
      inst_ptr->prod_dir = str_create( g_val );
      break;
		 
    case e_key_ups_dir:
      inst_ptr->ups_dir = str_create( g_val );
      break;
		 
    case e_key_table_dir:
      inst_ptr->table_dir = str_create( g_val );
      break;
		 
    case e_key_table_file:
      inst_ptr->table_file = str_create( g_val );
      break;
		 
    case e_key_archive_file:      
      inst_ptr->archive_file = str_create( g_val );
      break;
    
    case e_key_authorized_nodes:
      inst_ptr->authorized_nodes = str_create( g_val );
      break;
      
    case e_key_description:
      inst_ptr->description = str_create( g_val );
      break;
    
    case e_key_statistics:
      inst_ptr->statistics = str_create( "on" );
      break;
    
    case e_key_unknown:
      if ( g_line[0] == '_' ) {	
	inst_ptr->unknown_list = upslst_add( inst_ptr->unknown_list,
					     str_create( g_line ) );
      }
      break;

    }

    if ( is_stop_key() ) break;		 
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
    t_upslst_item *l_ptr = 0;
    t_ups_action *act_ptr = 0;
    
    while  ( g_ikey == e_key_action ) {
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
  t_ups_action *act_ptr = 0;
  t_upslst_item *l_cmd = 0;
  char *cmd_ptr = 0;

  if ( g_ikey != e_key_action ) 
    return 0;

  act_ptr = ups_new_action();
  act_ptr->action = str_create( g_val );

  while ( next_key() != e_key_eof ) {

    if ( g_ikey == e_key_unknown && strlen( g_line ) > 0 ) {
      cmd_ptr = str_create( g_line );
      if ( cmd_ptr )
	l_cmd = upslst_add( l_cmd, cmd_ptr );
    }
    
    if ( is_stop_key() ) break;
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
    t_upslst_item *l_ptr = 0;
    t_upslst_item *l_tmp_ptr = 0;
    
    while  ( g_ikey == e_key_group ) {
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
    t_upslst_item *l_ptr = 0;
    t_upslst_item *l_inst_ptr = 0;
    t_upslst_item *l_act_ptr = 0;
    t_ups_instance *inst_ptr = 0;

    if ( g_ikey != e_key_group ) 
      return 0;
    
    if ( next_key() != e_key_flavor )
      return 0;

    l_inst_ptr = read_instances();

    if ( !l_inst_ptr )
      return 0;

    if ( g_ikey == e_key_common ) {
	next_key();
	l_act_ptr = read_actions();
    }

    /* add actions to instances */

    if ( l_act_ptr ) {
      l_ptr = upslst_first( l_inst_ptr );
      inst_ptr = (t_ups_instance *)l_ptr->data;
      inst_ptr->action_list = l_act_ptr;
      l_ptr = l_ptr->next;

      /* make a reference for all other instances, */
      /* not to the list but to list data elements */
      
      for ( ; l_ptr; l_ptr = l_ptr->next ) {
	inst_ptr = (t_ups_instance *)l_ptr->data;
	inst_ptr->action_list = upslst_copy( l_act_ptr );
      }
    }

    while ( next_key() == e_key_end ) {}
    
    return l_inst_ptr;
}

/*
 * Line parsing
 */

/*-----------------------------------------------------------------------
 * next_key
 *
 * Will read next not empty line from file.
 *
 * Input : none
 * Output: none
 * Return: int, enum of current key.
 */
int next_key( void )
{  
  g_key[0] = 0;
  g_val[0] = 0;
  g_line[0] = 0;
  g_ikey = e_key_eof;

  while ( fgets( g_line, MAX_LINE_LEN, g_fh ) ) {

    if ( strlen( g_line ) < 1 ) continue;
    if ( g_line[0] == '#' ) continue;
    if ( !upsutl_str_remove_edges( g_line, CHAR_REMOVE ) ) continue;

    if ( get_key() != e_key_eol ) {
      return g_ikey;
    }
  }

  return e_key_eof;
}

/*-----------------------------------------------------------------------
 * get_key
 *
 * Will parse the current line (g_line) and fill current key (g_key/g_ikey)
 * and current value (g_val)
 *
 * Input : none
 * Output: none
 * Return: char*, pointer to current key;
 */
int get_key( void )
{
  char *cp = g_line;
  int count = 0;

  /* check if line is not empty (again) */
  
  if ( strlen( g_line ) < 1 ) return e_key_eol;
  if ( g_line[0] == '#' ) return e_key_eol;
    
  /* check if line has a key/value pair */
  
  if ( !strchr( g_line, '=' ) ) {
    strcpy( g_key, g_line );
  }

  /* split line into key/value pair */

  else {
    count = 0;
    while ( cp && *cp && !is_space( *cp ) && *cp != '=' ) {
      g_key[count] = *cp;
      count++; cp++;
    }
    g_key[count] = 0;
    if ( strlen( g_key ) <= 0 ) return e_key_eol;
  
    while( cp && *cp && *cp != '=' ) { cp++; }
    cp++;
    while( cp && *cp && is_space( *cp ) ) { cp++; }
    count = 0;
    while( cp && *cp && *cp != '\n' ) {
      g_val[count] = *cp;
      count++; cp++;
    }
    g_val[count] = 0;
  }

  ckeyi();

  return g_ikey;
}

/*-----------------------------------------------------------------------
 * put_key
 *
 * Will print and format passed key and val.
 * It's using current value of margin (g_imargin) to indent text.
 * It will not print anything if val is empty.
 *
 * Input : char *, key string
 *         char *, value string 
 * Output: none
 * Return: int, 1 fine, 0 not fine
 */
int put_key( const char * const key, const char * const val )
{
  int i = 0;
  
  if ( !val ) return 0;

  if ( strlen( val ) > 0 ) {
    for ( i=0; i<g_imargin; i++ )
      fputc( ' ', g_fh );

    if ( key && strlen( key ) > 0 ) 
      fprintf( g_fh, "%s = ", key );
  
    fprintf( g_fh, "%s", val );
  }

  fputc( '\n', g_fh );
  
  return 1;
}
     
int is_start_key( void )
{
  if ( g_ikey == e_key_flavor ) 
    return 1;
  if ( g_ikey == e_key_group ) 
    return 1;
  if ( g_ikey == e_key_common ) 
    return 1;
  if ( g_ikey == e_key_file )
    return 1;
  if ( g_ikey == e_key_action ) 
    return 1;

  return 0;
}

int is_stop_key( void )
{
  if ( is_start_key() )
    return 1;
  if ( g_ikey == e_key_end ) 
    return 1;

  return 0;
}

int trim_qualifiers( char * const str )
{
  int i, len;
  
  if ( !str || strlen( str ) <= 0 ) return 0;

  len = (int)strlen( str );
  for ( i=0; i<len; i++ )
    str[i] = tolower( str[i] );
  
  upsutl_str_remove( str, CHAR_REMOVE );  
  upsutl_str_sort( str, ',' );

  return strlen( str );
}

/*
 * Utils
 */

char *str_create( char * const str )
{
  char *new_str = 0;
    
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

int ckeyi( void )
{
  /* strings of known keys */
  static char *s_ups_keys[e_key_count] = {
    "FILE",
    "PRODUCT",
    "VERSION",
    "CHAIN",
    "UPS_DB_VERSION",
  
    "FLAVOR",
    "QUALIFIERS",
    "DECLARER",
    "DECLARED",
    "PROD_DIR",
    "UPS_DIR",
    "TABLE_DIR",
    "TABLE_FILE",
    "ARCHIVE_FILE",
    "AUTHORIZED_NODES",
    "DESCRIPTION",
    "STATISTICS",
  
    "ACTION",

    "GROUP:",
    "COMMON:",
    "END:",
  
    ""
  };

  int i;
  g_ikey = e_key_unknown;

  /* for now, just a linear search ... we need a map */

  for( i=0; i<e_key_count; i++ ) {    
    if ( !strcmp( g_key, s_ups_keys[i] ) ) {
      g_ikey = i;
      break;
    }
  }

  return g_ikey;
}

int cfilei( void )
{
  /* strings of known file types ... we need a map */
  static char *s_ups_files[e_file_count] = {
    "VERSION",
    "TABLE",
    "CHAIN",
    "DBCONFIG",
    ""
  };

  int i;
  g_ifile = e_file_unknown;

  /* for now, just a linear search */

  for( i=0; i<e_file_count; i++ ) {    
    if ( !strcmp( g_pd->file, s_ups_files[i] ) ) {
      g_ifile = i;
      break;
    }
  }

  return g_ifile;
}

/*
 * Print stuff
 */

void print_instance( t_ups_instance * const inst_ptr )
{
  t_upslst_item *l_ptr = 0;
  
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
    printf( "Actions = %s\n", (char*)0 );
  }
  
  if ( inst_ptr->unknown_list ) {
    printf( "User Defined = \n" );
    l_ptr = upslst_first( inst_ptr->unknown_list );
    for ( ; l_ptr; l_ptr = l_ptr->next ) {
      printf( "%s\n", (char *) l_ptr->data );
    }
  }
  else {
    printf( "User Defined = %s\n", (char*)0 );
  }
}

void print_action( t_ups_action * const act_ptr )
{
  t_upslst_item *l_ptr = 0;
  
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
    printf( "Command list = %s\n", (char*)0 );
  }
}

void g_print_product( t_ups_product * const prod_ptr )
{
  t_upslst_item *l_ptr = 0;

  l_ptr = upslst_first( prod_ptr->comment_list );
  for ( ; l_ptr; l_ptr = l_ptr->next ) {
    printf( "%s\n", (char *)l_ptr->data ); 
  }
  
  printf( "\nfile    = %s\n", prod_ptr->file );
  printf( "product = %s\n", prod_ptr->product );
  printf( "chaver  = %s\n", prod_ptr->chaver );
  printf( "instance_list:\n" );

  
  l_ptr = upslst_first( prod_ptr->instance_list );
  for ( ; l_ptr; l_ptr = l_ptr->next ) {
    t_ups_instance *inst_ptr = (t_ups_instance *)l_ptr->data;
    print_instance( inst_ptr );
  }     
}


