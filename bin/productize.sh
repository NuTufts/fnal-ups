#! /bin/sh
#  This file (ups-productize.sh) was created by Ron Rechenmacher <ron@fnal.gov> on
#  Jul  4, 2009. "TERMS AND CONDITIONS" governing this file are in the README
#  or COPYING file. If you do not have such a file, one can be obtained by
#  contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
#  $RCSfile$
#  rev='$Revision$$Date$'

set -u

USAGE="\
   usage: `basename $0`
examples: `basename $0` --prod=ups --ver=4.7.4x
          `basename $0` --deps='prodA v1, prodB v2'
          `basename $0` --deps='prodA v1 -qdebug, prodB v2 -qdebug' -qdebug
          `basename $0` --deps='prodA vX, prodB vZ' -qcms
          `basename $0` --deps='prodA vX -qdebug, prodB vZ -qdebug' -qcms:debug

If PWD is not in the form prod-ver, then --prod and --ver options must be
supplied.
Options:
--prod=
--ver=
--prods-root=          or 1st \"db\" in PRODUCTS env.var.
--deps=
--q=                    understand: debug,; others just used in declare
--productization-lib=
--stages=              dflt: build:install:declare
--configure=           *B configure options (for prods that use \"configure\")
--clean                *B
--no-src               *I do not copy src (not implemented yet)
--redo                 will redo some operation that appear to be done
--quiet                output from the stages is only in <stage-flv>.out
-v

*B - build  stage option
*I - install stage option
"
while opt=`expr "${1-}" : '-\([^=]*\)'`;do
    if opt_val=`expr "${1-}" : '[^=]*=\(.*\)'`;then shift;set - "$opt_val" "$@"
    else                                            shift;  fi
    case "$opt" in
    \?|h|-help)  echo "$USAGE";exit;;
    -prod)               opt_prod=$1;               shift;;
    -ver)                opt_ver=$1;                shift;;
    -prods-root)         opt_prods_root=$1;         shift;;
    -productization-lib) opt_productization_lib=$1; shift;;
    -stage|-stages)      opt_stages=$1;             shift;;
    -deps)               opt_deps=$1;               shift;;
    -configure)          opt_configure=$1;          shift;;
    -q)                  opt_q=$1;                  shift;;
    -clean)              opt_clean=1;;
    -no-src)             opt_no_src=1;;
    -redo)               opt_redo=1;;
    -quiet)              opt_quiet=1;;
    v)                   opt_v=1;;
    *)           echo "Unknown option: $opt"; echo "$USAGE";exit;;
    esac
done
if [ $# -ne 0 ];then echo "no arguments ($@) expected";echo "$USAGE";exit;fi

#-----------------------------------------------------------------------

if [ "${opt_stages-}" ];then stages=${opt_stages-}
else                         stages=build:install:declare; fi

#-----------------------------------------------------------------------

echov() { if [ "${opt_v-}" ];then echo "`date`; $@";fi; }
cmd() { echov "$@"; eval "$@"; }

#-----------------------------------------------------------------------

set_NAM_and_VER()   # $1=nam-ver
{
    # tests:
    #   gcc-4.1.2-20080102  -> gcc  v4_1_2_20080102
    prod_=`echo "$1" | sed 's/-[0-9][-0-9.a-zA-z]*$//'`
     ver_=`expr "$1" : "${prod_}-\(.*\)"`
    if   [ "$prod_" = '' -a "${opt_prod-}" = '' ];then
        echo can not determine prod; exit
    elif [ "$prod_"      -a "${opt_prod-}"      ];then
        if [ "$prod_" = "${opt_prod-}" ];then
            PROD_NAM=$prod_; echov OK - "$prod_" = "${opt_prod-}"
        else
            echov "Warning - $prod_ != ${opt_prod-}. Taking ${opt_prod-}."
            # example: prefer "python" over "Python" 
            PROD_NAM=$opt_prod
        fi
    else
        if [ "$prod_" ];then 
            PROD_NAM=$prod_;    echov "prod is $prod_"
        else
            PROD_NAM=$opt_prod; echov "prod is $opt_prod (from opt)"
        fi
    fi
    if   [ "$ver_" = '' -a "${opt_ver-}" = '' ];then
        echo can not determine ver; exit
    elif [ "$ver_"      -a "${opt_ver-}"      ];then
        if [ "$ver_" = "${opt_ver-}" ];then
            PROD_VER=$ver_; echov OK - "$ver_" = "${opt_ver-}" 
        else
            echov "Warning - $ver_ != ${opt_ver-}. Taking ${opt_ver-}."
            PROD_VER=$opt_ver
        fi
    else
        if [ "$ver_" ];then
            PROD_VER=$ver_;    echov "ver is $ver_"
        else
            PROD_VER=$opt_ver; echov "ver is $opt_ver (from opt)"
        fi
    fi
    if expr "$PROD_VER" : v >/dev/null;then
        : OK
    else
        PROD_VER=v`echo "$PROD_VER" | sed 's/\./_/g'`
    fi
}   # set_NAM_and_VER

set_PRODS_RT()    # aka PRODS_DB (I combine them)
{
    if   [ "${opt_prods_root-}" ];then
        PRODS_RT="$opt_prods_root"
    elif [ "${PRODUCTS-}"       ];then
        PRODS_RT=`expr "$PRODUCTS" : '\([^:]*\)'`
    else
        echo 'Error - cannot determine "products root" - the place to where'
        echo 'product will be installed'
        echo "$USAGE"; exit
    fi
    echov PRODS_RT=$PRODS_RT
}   # set_PRODS_RT

#-----------------------------------------------------------------------

get_productization_lib()
{
    if [ $PROD_NAM = ups ];then
        PATH=$PWD/bin:$PATH
        if [ -f bin/ups_productization_lib.sh ];then
            .   bin/ups_productization_lib.sh
        elif [ "${opt_productization_lib-}" ];then
            if [ -f "${opt_productization_lib-}" ];then
                . "${opt_productization_lib-}"
            else
                echo 'Error: --productization_lib is not a file'
                exit
            fi
        else
            echo "ups_productization_lib.sh not found"
            exit
        fi
    elif [ -d $PRODS_RT/ups ];then
        echov UPS_DIR =${UPS_DIR-}
        echov PRODS_RT=$PRODS_RT
        # I need _full_ ups (i.e. setup function)
        if [ -f $PRODS_RT/setup ];then
            . $PRODS_RT/setup
        else
            for fuefile in /usr/local/etc/fermi.shrc \
                   /usr/local/etc/setups.sh \
                   /fnal/ups/etc/setups.sh; do
                if [ -f $fuefile ];then
                    # I do not want to reset PRODUCTS
                    unset SETUP_UPS UPS_DIR # PRODUCTS
                    set +u  # bad programmers
                    . $fuefile
                    set -u
                    break
                fi
            done
            if type setup | grep 'ups setup' >/dev/null && hash ups 2>/dev/null;then
                : success
            else
                echo Error - can not initial ups support script environment
            fi
        fi
        if [ -f $UPS_DIR/bin/ups_productization_lib.sh ];then
            .   $UPS_DIR/bin/ups_productization_lib.sh
        else
            echo "ups_productization_lib.sh not found"
            exit
        fi
        # Now, additionally, there may be a {PROD_NAM}_productization_lib.sh
        if [ "${opt_productization_lib-}" ];then
            if [ -f "${opt_productization_lib-}" ];then
                . "${opt_productization_lib-}"
            else
                echo 'Error: --productization_lib is not a file'
                exit
            fi
        else
            for dd in . ..;do
                for ff in ${PROD_NAM}_productization_lib \
                    ${PROD_NAM}_productization_lib.sh;do
                    if [ -f $dd/$ff ];then . $dd/$ff; break 2; fi
                done
            done
        fi
    else
        echo Error - ups must be installed 1st into products root at $PRODS_RT
    fi
}   # get_productization_lib

#------------------------------------------------------------------------------

echov "`basename $0` starting in .../`basename $PWD`"

set_NAM_and_VER `basename $PWD`

set_PRODS_RT

get_productization_lib

#--

set_FLV

redo_stage=99
PREFIX=`qualified_inst_dir`  # important var

if   declare --status;then  last_stage_done=3
elif install --status;then  last_stage_done=2
elif build   --status;then  last_stage_done=1
else                        last_stage_done=0
fi
if [ "${opt_quiet-}" ];then outspec='>/dev/null 2>&1';else outspec=; fi
echov last_stage_done=$last_stage_done
for ss in build install declare;do
    if echo $stages | grep $ss >/dev/null;then :;else continue; fi
    case $ss in
    build)
        if [ $last_stage_done -lt 1 -o $redo_stage -le 1 ];then
            cmd "build 2>&1   | tee build-`basename $PREFIX`.out $outspec"
        fi;;
    install)
        if [ $last_stage_done -lt 2 -o $redo_stage -le 2 ];then
            cmd "install 2>&1 | tee install-`basename $PREFIX`.out $outspec"
        fi;;
    declare)
        if [ $last_stage_done -lt 3 -o $redo_stage -le 3 ];then
            cmd "declare 2>&1 | tee declare-`basename $PREFIX`.out $outspec"
        fi;;
    esac
done

echov "Done."
