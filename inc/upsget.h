/***********************************************************************
 *
 * FILE:
 *       upsugo.h
 * 
 * DESCRIPTION: 
 *       Prototypes for upsugo
 *
 * AUTHORS:
 *       Eileen Berman
 *       David Fagan
 *       Lars Rasmussen
 *
 *       Fermilab Computing Division
 *       Batavia, Il 60510, U.S.A.                                               *
 * MODIFICATIONS:
 *       06 Oct 1997, DjF First 
 *
 ***********************************************************************/

#ifndef _UPSGET_H_
#define _UPSGET_H_
char *upsget_translation( const t_upstyp_matched_product * const ,
                  const t_upsugo_command * const ,
                  char * const );
char *upsget_prod_dir(const t_upstyp_matched_product * const,
                      const t_upstyp_matched_instance * const ,
                      const t_upsugo_command * const );
char *upsget_product(const t_upstyp_matched_product * const,
                     const t_upstyp_matched_instance * const ,
                     const t_upsugo_command * const );
char *upsget_version(const t_upstyp_matched_product * const,
                     const t_upstyp_matched_instance * const ,
                     const t_upsugo_command * const );
char *upsget_flavor(const t_upstyp_matched_product * const,
                    const t_upstyp_matched_instance * const ,
                    const t_upsugo_command * const );
char *upsget_qualifiers(const t_upstyp_matched_product * const,
                        const t_upstyp_matched_instance * const ,
                        const t_upsugo_command * const );
char *upsget_shell(const t_upstyp_matched_product * const,
                   const t_upstyp_matched_instance * const ,
                   const t_upsugo_command * const );
char *upsget_verbose(const t_upstyp_matched_product * const,
                     const t_upstyp_matched_instance * const ,
                     const t_upsugo_command * const );
char *upsget_options(const t_upstyp_matched_product * const,
                     const t_upstyp_matched_instance * const ,
                     const t_upsugo_command * const );
char *upsget_database(const t_upstyp_matched_product * const,
                      const t_upstyp_matched_instance * const ,
                      const t_upsugo_command * const );
char *upsget_OS_flavor(const t_upstyp_matched_product * const,
                       const t_upstyp_matched_instance * const ,
                       const t_upsugo_command * const );
char *upsget_extended(const t_upstyp_matched_product * const,
                      const t_upstyp_matched_instance * const ,
                      const t_upsugo_command * const );

#endif /* _UPSGET_H_ */
