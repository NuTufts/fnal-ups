#! /bin/sh
#  This file (ups-productize.sh) was created by Ron Rechenmacher <ron@fnal.gov> on
#  Jul  4, 2009. "TERMS AND CONDITIONS" governing this file are in the README
#  or COPYING file. If you do not have such a file, one can be obtained by
#  contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
#  $RCSfile$
#  rev='$Revision$$Date$'
if [ "$1" = -x ];then set -x;shift;fi
set -u

USAGE="\
   usage: `basename $0`
examples: cd some/dir
          `basename $0` --prod=ups --ver=4.7.4x
          `basename $0` --deps='prodA v1, prodB v2'
          `basename $0` --deps='prodA v1 -qdebug, prodB v2 -qdebug' -qdebug
          `basename $0` --deps='prodA vX, prodB vZ' -qcms
          `basename $0` --deps='prodA vX -qdebug, prodB vZ -qdebug' -qcms:debug

If PWD is not in the form prod-ver, then --prod and --ver options must be
supplied.
Options:
--prod=
--ver=
--prods-root=          or 1st \"db\" in PRODUCTS env.var. (IF \"db\" is a
                        combined db/prod_root)
--deps=
--var=
--q=                    understand: debug,cxxcheck; others just used in declare
--productization-lib=
--stages=              dflt: build:install:declare
--configure=           *B configure options (for prods that use \"configure\")
--clean                *B
#--no-src               *I do not copy src (not implemented yet)
#--redo                 will redo some operation that appear to be done
#--quiet                output from the stages is only in <stage-flv>.out
#--out
-v

*B - build  stage option
*I - install stage option
"
opts_w_args='prod|ver|prods-root|productization-lib|stages|deps|var|configure|q'
opts_wo_args='clean|no-src|redo|quiet|v|out'
do_opt="\
  case \$opt in
  \\?|h|help)    echo \"\$USAGE\"; exit 0;;
  $opts_wo_args) eval opt_\`echo \$opt |sed -e 's/-/_/g'\`=1;;
  $opts_w_args)  if   [ \"\${rest-}\" != '' ];then arg=\$rest; rest=
                 elif [      $#      -ge 1  ];then arg=\$1; shift
                 else  echo \"option \$opt requires argument\"; exit 1; fi
                 eval opt_\`echo \$opt|sed -e 's/-/_/g'\`=\"'\$arg'\"
                 opts=\"\$opts '\$arg'\";;
  *) echo \"invalid option: \$opt\" 2>&1;echo \"\$USAGE\" 2>&1; exit 1;;
  esac"
while op=`expr "${1-}" : '-\(.*\)'`;do
    shift
    if xx=`expr "$op" : '-\(.*\)'`;then
        if   opt=`expr     "$xx" : '\([^=]*\)='`;then
            set ${-+-$-} -- "`expr "$xx" : '[^=]*=\(.*\)'`" "$@"
        else opt=$xx; fi
        opts="${opts-} '--$opt'"
        eval "$do_opt"
    else
        while opt=`expr "$op" : '\(.\)'`;do
            opts="${opts-} '-$opt'"
            rest=`expr "$op" : '.\(.*\)'`
            eval "$do_opt"
            op=$rest
        done
    fi
done
if [ $# -ne 0 ];then echo "no arguments ($@) expected";echo "$USAGE";exit;fi

#-----------------------------------------------------------------------

redo_stage=99
all_stages=build:install:install_fix:declare
if [ "${opt_stages-}" ];then
    stage_idx=0
    stages=
    for ss in `echo $all_stages | tr : ' '`;do
        stage_idx=`expr $stage_idx + 1`
        if xx=`expr ":${opt_stages}:" : ".*:\(${ss}\):"`;then
            stages="${stages:+$stages:}$xx"
            if [ $redo_stage -eq 99 ];then redo_stage=$stage_idx;fi
        fi
    done
else
    stages=$all_stages
fi

exec 3>&1 4>&2    # dup so as to enable restore
orestore='1>&3 2>&4'
if [ "${opt_out-}" ];then
    ospec='>>productize-`basename $PREFIX`${opt_q+_`echo $opt_q | tr : _`}.out 2>&1'
    # expect error status to be enough if error before exec after PROD_*
    exec >/dev/null 2>&1
else
    ospec='1>&3 2>&4'
fi

#-----------------------------------------------------------------------

echov() { if [ "${opt_v-}" ];then echo "`date`: $@";fi; }
cmd() { eval "echov \"$@\""; eval "$@"; }   # used in ups_productization_lib.sh

#-----------------------------------------------------------------------

set_PRODS_RT()    # aka PRODS_DB (I combine them)
{
    if   [ "${opt_prods_root-}" ];then
        PRODS_RT="$opt_prods_root"
    elif [ "${PRODUCTS-}"       ];then
        DB=`expr "$PRODUCTS" : '\([^:]*\)'`
        if [ -f $DB/.upsfiles/dbconfig ];then
            # check for PROD_DIR_PREFIX
            PROD_DIR_PREFIX=`sed -n '/^[ \t]*PROD_DIR_PREFIX/{s/.*= *//;s/[ \t]*$//;p}' $DB/.upsfiles/dbconfig`
            if [ "$PROD_DIR_PREFIX" = "$DB" -o "$PROD_DIR_PREFIX" = '${UPS_THIS_DB}' ];then
                PRODS_RT=$DB
            else
                echo "Incompatible DB $DB != $PROD_DIR_PREFIX"
            fi
        else
            echo 'Products DB, $DB, has not been initialized - .upsfile/dbconfig'
            ehco 'file not found.'
        fi
    fi
    if [ "${PRODS_RT-}" = '' ];then
        echo 'Error - cannot determine "products root" - the place to where'
        echo 'product will be installed'
        echo "$USAGE"; exit
    fi
    echov PRODS_RT=$PRODS_RT
}   # set_PRODS_RT

set_NAM_and_VER()   # $1=nam-ver
{
    if nam_ver=`expr "$PWD" : "$PRODS_RT/\([^/][^/]*/[^/][^/]*\)\$"`;then
        echo "name/ver=$nam_ver"
        prod_=`expr "$nam_ver" : '\([^/][^/]*\)'`
         ver_=`expr "$nam_ver" : '[^/][^/]*/\([^/][^/]*\)'`
        stages=`echo :$stages:|sed 's/:build:/:/;s/:install:/:/;s/^://;s/:$//'`
        echo "set stages=$stages"
    else
        # tests:
        #   gcc-4.1.2-20080102  -> gcc       v4_1_2_20080102
        #   root-v5-18-00f.svn  -> root      v5_18_00f_svn
        #   boost_1_34_1        -> boost     v1_34_1
        #   libsigc++-2.2.3     -> libsigcxx v2_2_3
        nam_ver=`basename $PWD`
        prod_=`echo "$nam_ver" | sed 's/[-_]v*[0-9][-0-9.a-zA-Z]*$//'`
         ver_=`expr "$nam_ver" : "${prod_}[-_]\(.*\)"`
        prod_=`echo "$prod_" | sed 's/++/xx/'`    # libsigc++ -> libsigcxx
    fi

    if   [ "$prod_" = '' -a "${opt_prod-}" = '' ];then
        echo can not determine prod; return 1
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
        echo can not determine ver; return 1
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
    if expr "$PROD_VER" : '[0-9]' >/dev/null;then
        PROD_VER=v`echo "$PROD_VER" | tr '.-' '__'`
    else
        PROD_VER=`echo "$PROD_VER" | tr '.-' '__'`
    fi
}   # set_NAM_and_VER

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
        # Now, additionally, there may be an  ${opt_productization_lib-} or
        # an {PROD_NAM}_productization_lib.sh
        for ff in ${opt_productization_lib+$opt_productization_lib $opt_productization_lib.sh} \
            ${PROD_NAM}_productization_lib \
            ${PROD_NAM}_productization_lib.sh;do
            for dd in '' .. $UPS_DIR/bin;do
              echov "checking for ${dd:+$dd/}$ff"
              if [ -f ${dd:+$dd/}$ff ];then . ${dd:+$dd/}$ff; break 2; fi
            done
        done
    else
        echo Error - ups must be installed 1st into products root at $PRODS_RT
    fi
}   # get_productization_lib

#------------------------------------------------------------------------------

set_PRODS_RT

if set_NAM_and_VER;then
    : OK
else
    exit 1
fi

# check write permissions...
# 

get_productization_lib

#--

set_FLV

PREFIX=`qualified_inst_dir`  # important var
if [ "${opt_out-}" ];then
    >productize-`basename $PREFIX`${opt_q+_`echo $opt_q | tr : _`}.out
    eval "exec ${ospec-}"
fi
echov "`basename $0` starting in .../`basename $PWD`"

if   declare     --status;then  last_stage_done=4
elif install_fix --status;then  last_stage_done=3
elif install     --status;then  last_stage_done=2
elif build       --status;then  last_stage_done=1
else                            last_stage_done=0
fi
if [ "${opt_quiet-}" ];then outspec='>/dev/null 2>&1';else outspec=; fi
echov last_stage_done=$last_stage_done redo_stage=$redo_stage stages=$stages
for ss in `echo $stages | tr : ' '`;do
    #if echo $stages | grep $ss >/dev/null;then :;else continue; fi
    # all stages are run in a pipeline to collect output AND to run in
    # sub-shell (so the cwd in this parent shell is preserved).
    pid=
    ofile=$ss-`basename $PREFIX`${opt_q+_`echo $opt_q | tr : _`}.out
    case $ss in
    build)
        if [ $last_stage_done -lt 1 -o $redo_stage -le 1 ];then
            echov "output in $ofile"
            exec >$ofile 2>&1
            build & pid=$!
        fi;;
    install)
        if [ $last_stage_done -lt 2 -o $redo_stage -le 2 ];then
            echov "output in $ofile"
            exec >$ofile 2>&1
            install & pid=$!
        fi;;
    install_fix)
        if [ $last_stage_done -lt 3 -o $redo_stage -le 3 ];then
            echov "output in $ofile"
            exec >$ofile 2>&1
            install_fix & pid=$!
        fi;;
    declare)
        if [ $last_stage_done -lt 4 -o $redo_stage -le 4 ];then
            echov "output in $ofile"
            exec >$ofile 2>&1
            declare & pid=$!
        fi;;
    esac
    if [ "$pid" ];then
        eval "exec ${ospec-}"  # restore normal output
        if [ "${opt_quiet-}" ];then
            wait $pid
        else
            tail --pid=$pid -f $ss-`basename $PREFIX`.out
            wait $pid
        fi
        if [ $? -eq 0 ];then
            echov "stage $ss completed OK"
        else
            echo "Error in $ss stage for $PROD_NAM $PROD_VER ${opt_q-}"
            exit 1            
        fi
    fi
done

echov "Done in .../`basename $PWD`"
