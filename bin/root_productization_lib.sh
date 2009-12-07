#! /bin/sh
#  This file (root_productization_lib.sh) was created by Ron Rechenmacher <ron@fnal.gov> on
#  Jul 14, 2009. "TERMS AND CONDITIONS" governing this file are in the README
#  or COPYING file. If you do not have such a file, one can be obtained by
#  contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
#  $RCSfile$
#  rev='$Revision$$Date$'


generic_build()
{
    echov "root build $@; PREFIX=$PREFIX"
    q_bld_dir=build-`basename $PREFIX`
    export ROOTSYS; ROOTSYS=$PREFIX
    if [ "${1-}" = --status ];then
        if [ -d $q_bld_dir ];then return 0
        else                      return 1;fi
    else
        setup_deps
        mkdir -p $q_bld_dir
        cd $q_bld_dir
        cmd lndir .. .
        flags=`qual_to_flags`
        # eval for flags and $PREFIX in $opt_configure
        cmd "$flags ./configure ${opt_configure-}"
        qual_unO_Makefile
        test -f /proc/cpuinfo && j_opt=-j`grep processor /proc/cpuinfo | wc -l`
        make ${j_opt-} ${opt_make-}
    fi
}
generic_declare()
{
    echov "generic declare $@; PREFIX=$PREFIX"
    if [ "${1-}" = --status ];then
        if [ -f $PRODS_RT/$PROD_NAM/$PROD_VER.version ];then return 0
        else                                                 return 1;fi
    else
        if [ -d $PREFIX ];then
            # only declare if this generic (qualified) inst dir exists
            if [ ! -f $PRODS_RT/$PROD_NAM/$PROD_VER/ups/$PROD_NAM.table \
                -o "${opt_redo-}" ];then
                echo mkdir -p $PRODS_RT/$PROD_NAM/$PROD_VER/ups
                mkdir -p $PRODS_RT/$PROD_NAM/$PROD_VER/ups
                ups_table.sh add_fq --envvar='ROOTSYS=\${UPS_PROD_DIR}/\$\${UPS_PROD_NAME_UC}_FQ'\
                    -f ${PROD_FLV} ${opt_qual-}\
                   >$PRODS_RT/$PROD_NAM/$PROD_VER/ups/$PROD_NAM.table
            fi
        
            cmd "ups declare -c -z$PRODS_RT -r$PROD_NAM/$PROD_VER -Mups \
                -m$PROD_NAM.table -f$PROD_FLV ${opt_qual+-q$opt_qual} \
                $PROD_NAM $PROD_VER"
        else
            echo 'Error - generic (qualified) inst dir does not exists'
            exit 1
        fi
    fi
}   # generic_declare
