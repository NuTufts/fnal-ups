# Source this file.
#  This file (ups_productization_lib.sh) was created by Ron Rechenmacher <ron@fnal.gov> on
#  Jul  6, 2009. "TERMS AND CONDITIONS" governing this file are in the README
#  or COPYING file. If you do not have such a file, one can be obtained by
#  contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
#  $RCSfile$
#  rev='$Revision$$Date$'



echo "hello from ups_productization_lib"

build()
{
    echo build
    # look for a build function
    if type ${PROD_NAM}_build 2>/dev/null | grep 'is a function' >/dev/null;then
        echo build function found...
        ${PROD_NAM}_build "$@"
    else
        echo "no build function found; trying generic build function"
        generic_build "$@"
    fi
}   # build

install()
{
    echo install
    if type ${PROD_NAM}_install 2>/dev/null | grep 'is a function' >/dev/null;then
        echo install function found...
        ${PROD_NAM}_install "$@"
    else
        echo "no install function found; trying generic install function"
        generic_install "$@"
    fi
}   # install

declare()
{
    echo delare
    if type ${PROD_NAM}_declare 2>/dev/null | grep 'is a function' >/dev/null;then
        echo declare function found...
        ${PROD_NAM}_declare "$@"
    else
        echo "no declare function found; trying generic declare function"
        generic_declare "$@"
    fi
}   # declare

#------------------------------------------------------------------------------

generic_build()
{
    echo hello from generic build q_inst_dir=$q_inst_dir
    q_bld_dir=build-`basename $q_inst_dir`
    if [ "${1-}" = --status ];then
        if [ -d $q_bld_dir ];then return 0
        else                           return 1;fi
    elif [ -f configure ];then
        echo 'generic "./configure" build'
        setup_deps
        mkdir -p $q_bld_dir
        cd $q_bld_dir
        if [ -f config.log ];then
            echo config.log exists
        fi
        if echo :${opt_q-}: | grep :debug: >/dev/null;then
            CFLAGS="-g -O0" CXXFLAGS="-g -O0" \
            ../configure --prefix=$q_inst_dir ${opt_configure-}
            if [ ! -f Makefile.orig ];then cp -p Makefile Makefile.orig;fi
            # use 2 sed express so as to not filter out -O from -O0
            sed -i -e '/[ "]-O[s1-3 "]/s/-O[s1-3 ]//' \
                   -e '/[ "]-O$/s/-O//'        Makefile 
            CFLAGS="-g -O0" CXXFLAGS="-g -O0" \
            make
        else
            ../configure --prefix=$q_inst_dir ${opt_configure-}
            make
        fi
        cd ..
    fi
}
generic_install()
{
    echo hello from generic install
    q_bld_dir=build-`basename $q_inst_dir`
    if [ "${1-}" = --status ];then
        if [ -d $q_inst_dir ];then return 0
        else                       return 1;fi
    elif [ -f configure ];then
        if [ -d $q_bld_dir ];then
            dir_sav=$PWD
            cd $q_bld_dir
            if [ -f Makefile ];then
                prefix=`sed -n '/^ *prefix *=/{s/^[^=]*= *//;p;q}' Makefile`
                if [ "$prefix" != $q_inst_dir ];then
                    echo 'Error - build/install mismatch'
                    exit
                fi
            else
                echo 'Error '
                exit
            fi

            make install

            echov now install fix

            cd $q_inst_dir
            re="^#! *$q_inst_dir/bin/"
            ff_l=`find . -type f | xargs grep -l "$re"`
            for ff in $ff_l;do
                if grep "${re}[^ ]*  *-" $ff >/dev/null;then
                    echo 'Trouble -- interp with option(s)'
                else
                    # what about permissions???
                    sed -i "s|$re|#!/usr/bin/env |" $ff
                fi
            done
            re="$q_inst_dir"
            ff_l=`find . -type f | xargs grep -l "$re"`
            for ff in $ff_l;do
                ere='script|ELF|English|ASCII text|archive'
                file_t=`file $ff | egrep -o "$ere" | sed 's/ //'`
                ff_basename=`basename $ff`
                ext=`expr "$ff_basename" : '.*\.\([^.]*\)'`
                echov "$ff: >$ext $file_t<"
                case "$ext $file_t" in
                'rb English')  # ruby file
                    # let flavor[_qual] stay
                    sed -i "s|$PRODS_RT/$PROD_NAM/$PROD_VER|#{ENV[\"RUBY_DIR\"]}|" $ff
                    ;;
                esac
            done

            cd $dir_sav
        else
            echo 'Error - generic'
            exit
            fi
    fi
}   # generic_install
generic_declare()
{
    echo hello from generic declare
    if [ "${1-}" = --status ];then
        if [ -f $PRODS_RT/$PROD_NAM/$PROD_VER.version ];then return 0
        else                                                 return 1;fi
    else
        if [ ! -f $PRODS_RT/$PROD_NAM/$PROD_VER/ups/$PROD_NAM.table \
            -o "${opt_redo-}" ];then
            echo mkdir -p $PRODS_RT/$PROD_NAM/$PROD_VER/ups
            mkdir -p $PRODS_RT/$PROD_NAM/$PROD_VER/ups
            cp_with_deps $UPS_DIR/ups/generic.table \
                $PRODS_RT/$PROD_NAM/$PROD_VER/ups/$PROD_NAM.table
        fi
        
        ups declare -c -z$PRODS_RT -r$PROD_NAM/$PROD_VER -Mups \
            -m$PROD_NAM.table -f$PROD_FLV ${opt_q+-q$opt_q} \
            $PROD_NAM $PROD_VER
    fi
}

#------------------------------------------------------------------------------

ups_build()
{
    echo hello from ups build
    if [ "${1-}" = --status ];then
        ups_flv=`get_flv_64_to_32`
        if [ -d build-$ups_flv ];then return 0
        else                          return 1;fi
    else

        dir_sav=$PWD

        # if Linux64bit -- try stripping 64bit and using CFLAGS=-m32
        if ups_flv=`get_flv_64_to_32`;then cflags="-cflags -m32"; fi

        if [ ! -d build-$ups_flv ];then mkdir build-$ups_flv;fi
        cd build-$ups_flv
        ln -s ../inc .
        for dd in bin lib src man doc ups;do
            mkdir $dd
            cd $dd
            for ff in ../../$dd/*;do ln -s $ff .;done

            # TABLE FILE ADJUST SHOULD GO INTO CVS!!!
            if [ -f ups.table ];then
                if grep PRODUCTS ups.table >/dev/null;then
                    :
                else
                    sed '/Action=current/,/^$/d
/End:/i\
\     envRemove( PRODUCTS, ${UPS_THIS_DB}, : )\
\     envPrepend( PRODUCTS, ${UPS_THIS_DB}, : )\
\     Execute("echo added ${UPS_THIS_DB} to PRODUCTS",UPS_ENV)
' ups.table >ups.table.$$
                    /bin/mv -f ups.table.$$ ups.table
                fi
            elif [ -f Makefile ];then
                # MAKEFILE ADJUST SHOULD GO INTO CVS!!!
                if grep 'funame.*(OLD_LIBS)$' Makefile >/dev/null;then
                # fix for SunOS - tack  " $(LDFLAGS)" on the end of the funame
                #  compile command.  This is only really needed in the src
                #  directory where LDFLAGS is defined in the Makefile
                    sed '/funame.*OLD_LIBS/s/$/ $(LDFLAGS)/' Makefile \
                        >Makefile.$$
                    /bin/mv -f Makefile.$$ Makefile
                fi
                if [ "${opt_clean-}" ];then
                    UPS_DIR=$dir_sav ../../bin/upspremake clean
                fi
                UPS_DIR=$dir_sav ../../bin/upspremake ${cflags-}
            fi

            cd ..
        done
        /bin/rm -fr src inc        # remove these "lndirs"

        cd $dir_sav
    fi
}

ups_install()
{
    echo hello from ups install
    if [ "${1-}" = --status ];then
        if [ -d $PRODS_RT/$PROD_NAM/$PROD_VER/$PROD_FLV ];then return 0
        else                                                   return 1;fi
    else
        # b/c I'm not using 
        ups_flv=`get_flv_64_to_32`
        cd build-$ups_flv
        inst_=$PRODS_RT/$PROD_NAM/$PROD_VER/$ups_flv
        mkdir -p $inst_
        /bin/cp -Lr bin lib man doc ups  $inst_
        unset inst_
        cd ..
    fi
}

ups_declare()
{
    echo hello from ups declare
    if [ "${1-}" = --status ];then
        if [ -f $PRODS_RT/$PROD_NAM/$PROD_VER.version ];then return 0
        else                                                 return 1;fi
    else

        ups_flv=`get_flv_64_to_32`

        # ups is special -- see if db is initializaed
        mkdir -p $PRODS_RT/.upsfiles
        if [ ! -f $PRODS_RT/.upsfiles/dbconfig ];then
            cat >$PRODS_RT/.upsfiles/dbconfig <<"EOF"
FILE = DBCONFIG
AUTHORIZED_NODES = *
PROD_DIR_PREFIX = ${UPS_THIS_DB}
#MAN_TARGET_DIR = /usr/local/man
#CATMAN_TARGET_DIR = /usr/local/catman
#SETUPS_DIR = /usr/local/etc
UPD_USERCODE_DIR = ${UPS_THIS_DB}/.updfiles
EOF
        fi

        cd build-$ups_flv
        bin/ups declare -c -z$PRODS_RT -r$PROD_NAM/$PROD_VER/$ups_flv -Mups \
            -m$PROD_NAM.table -f$ups_flv $PROD_NAM $PROD_VER

        # now make sure the famous "setup" file is copied to $PRODS_RT
        cp ups/setup $PRODS_RT
        . $PRODS_RT/setup
    fi
}

#------------------------------------------------------------------------------

qualified_inst_dir()
{
    if [ "${opt_q-}" ];then
        qq=`echo $opt_q | sed 's/:/_/g'`
        echo $PRODS_RT/$PROD_NAM/$PROD_VER/${PROD_FLV}_$qq
    else
        echo $PRODS_RT/$PROD_NAM/$PROD_VER/$PROD_FLV
    fi
}

setup_deps()
{
    if [ "${opt_deps-}" ];then
        IFSsav=$IFS IFS=,; for dep in $opt_deps;do IFS=$IFSsav
            setup $dep
        done
    fi
}

cp_with_deps()
{
    tblf_i=$1
    tblf_o=$2
    if [ "${opt_deps-}" ];then
        sed_up=''
        sed_dn=''
        IFSsav=$IFS IFS=,; for dep in $opt_deps;do IFS=$IFSsav
            dep=`echo $dep`
            echov "dep=>$dep<"
            sed_up="$sed_up\\        setupRequired( \"$dep\" )\\n"
            # unsetup in opposite order
            sed_dn="\\        unsetupRequired( \"$dep\" )\\n$sed_dn"
        done
        if xx=`grep -io '^[  ]*action[       ]*=[    ]unsetup' $tblf_i`;then
            : echo table file has unsetup
        else
            echo 'Error - missing unsetup in table file'
            exit
        fi

        sed "\
/$xx/i$sed_up\n
/$xx/a$sed_dn
" $tblf_i >$tblf_o
    else
        cp $tblf_i $tblf_o
    fi
}   # cp_with_deps

flavor()
{
    os=`uname -s` os_rev=`uname -r | cut -d. -f1-2` mach=`uname -m` libc=
    if [ $os = Linux ];then
        libc=-`/bin/ls /lib/libc-* | sed '{s/^[^0-9]*//;s/[^0-9]*$//;q}'`
    fi
    case $mach in
    x86_64|sun*) b64=64bit;;
    *)           b64=;;
    esac
    fl1=$os$b64+$os_rev$libc
    fl2=$os+$os_rev$libc
    fl3=$os+`expr "$os_rev" : '\([0-9]*\)'`
    case "${1-}" in
    ''|-4)      echo $fl1;;
    -[1-3])  nn=`expr "$1" : '-\(.\)'`;expr "$fl1" : "\(\([^-+.]*[-+.]\)\{$nn\}\)" | sed 's/.$//';;
    *)       echo $fl3;;
    esac
}

lndir()
{   src=$1
    dst=$2
    abs_src=`cd $src;pwd`
    abs_dst=`cd $dst;pwd`
    abs_chk_len=`expr "$abs_dst/" : '.*'`
    cd $dst
    src_slash_cnt=`echo $src | sed 's/[^/]//g' | wc -c`
    src_slash_cnt=`expr $src_slash_cnt - 1`
    (cd $src;find . -type d ) | \
    while read dd;do
        dd=`expr "$dd" : '\.\(.*\)'` # strip off leading "."
        if [ `expr "$abs_src$dd/" : "$abs_dst/"` -eq $abs_chk_len ];then
            echo skipping $abs_src$dd >&2
            continue
        fi
        if [ "$dd" ];then  mkdir .$dd; fi

        if expr "$src" : / >/dev/null;then
            extra_up=
        else
            slash_cnt=`echo $dd | sed 's/[^/]//g' | wc -c`
            extra_up=
            slash_cnt=`expr $slash_cnt - $src_slash_cnt`
            while slash_cnt=`expr $slash_cnt - 1`;do
                extra_up="../$extra_up"
            done
        fi

        no_prune=`basename "$src$dd"`
        find "$src$dd" \! -type d -o -type d \! -name "$no_prune" -prune \! -type d | \
        while read ff;do ln -s $extra_up$ff .$dd; done
    done
}   # lndir

set_FLV()
{
    PROD_FLV=`flavor`
}

get_flv_64_to_32()
{
    echo $PROD_FLV | sed 's/64bit//'
    if expr "$PROD_FLV" : '.*64bit' >/dev/null;then return 0
    else                                            return 1; fi
}

#------------------------------------------------------------------------------

case "${1-}" in
flavor)  shift; flavor "$@";;
esac
