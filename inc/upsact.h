/************************************************************************
 *
 * FILE:
 *       upsact.h
 * 
 * DESCRIPTION: 
 *       Prototypes etc. needed when handling actions.
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
 *       28-Aug-1997, EB, first
 *
 ***********************************************************************/

#ifndef _UPSACT_H_
#define _UPSACT_H_

/* standard include files, if needed for .h file */
#include <stdio.h>

/* ups specific include files, if needed for .h file */

#include "upstyp.h"
#include "upsugo.h"
#include "upsmat.h"
#include "upslst.h"

/*
 * Constans.
 */
#define UPS_MAX_ARGC 100

/*
 * Types.
 */

typedef struct upsact_cmd {
  int  iact;
  int  icmd;
  int  argc;
  char *argv[UPS_MAX_ARGC];
  char *pmem;
} t_upsact_cmd;

typedef struct upsact_item {
  int                        level;
  t_upsugo_command           *ugo;
  t_upstyp_matched_product   *mat;
  t_upstyp_action            *act;
  t_upsact_cmd               *cmd;
} t_upsact_item;

/*
 * Declaration of public functions.
 */

void upsact_process_commands( const t_upslst_item * const a_cmd_list,
			      const FILE * const a_stream);
t_upslst_item *upsact_get_cmd( t_upsugo_command * const ugo_cmd,
			       t_upstyp_matched_product * const mat_prod,
			       const char * const act_name );
int upsact_print( t_upsugo_command * const ugo_cmd,
		  t_upstyp_matched_product *mat_prod,
		  const char * const act_name,
		  const char * const sopt );
t_upsact_cmd *upsact_parse_cmd( const char * const cmd_str );
void upsact_cleanup( t_upslst_item *dep_list );
void upsact_free_upsact_cmd( t_upsact_cmd * const act_cmd );
void upsact_print_cmd( const t_upsact_cmd * const cmd_cur );
void upsact_print_item( const t_upsact_item *const p_cur, const char * const sopt );

#endif /* _UPSACT_H_ */

