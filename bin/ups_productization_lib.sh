#! /bin/sh
# Source this file.
#  This file (ups_productization_lib.sh) was created by Ron Rechenmacher <ron@fnal.gov> on
#  Jul  6, 2009. "TERMS AND CONDITIONS" governing this file are in the README
#  or COPYING file. If you do not have such a file, one can be obtained by
#  contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
#  $RCSfile$
#  rev='$Revision$$Date$'



echov "hello from ups_productization_lib"

build()
{
    echov build
    t0=`date +%s`
    # look for a build function
    if type ${PROD_NAM}_build 2>/dev/null | grep 'is a function' >/dev/null;then
        echov prod specific build function found...
        ${PROD_NAM}_build "$@"
    else
        generic_build "$@"
    fi
    rsts=$?
    echov "build $@ took `delta_m` minutes"
    return $rsts
}   # build

install()
{
    echov install
    t0=`date +%s`
    if type ${PROD_NAM}_install 2>/dev/null | grep 'is a function' >/dev/null;then
        echov prod specific install function found...
        ${PROD_NAM}_install "$@"
    else
        generic_install "$@"
    fi
    rsts=$?
    echov "install $@ took `delta_m` minutes"
    return $rsts
}   # install

declare()
{
    echov delare
    t0=`date +%s`
    if type ${PROD_NAM}_declare 2>/dev/null | grep 'is a function' >/dev/null;then
        echov prod specific declare function found...
        ${PROD_NAM}_declare "$@"
    else
        generic_declare "$@"
    fi
    rsts=$?
    echov "declare $@ took `delta_m` minutes"
    return $rsts
}   # declare

#------------------------------------------------------------------------------

generic_build()
{
    echov "generic build $@; PREFIX=$PREFIX"
    q_bld_dir=build-`basename $PREFIX`
    if [ "${1-}" = --status ];then
        if [ -d $q_bld_dir ];then return 0
        else                           return 1;fi
    else
        setup_deps
        generic_build_install_dispatch build
    fi
}
generic_install()
{
    echov "generic install $@; PREFIX=$PREFIX"
    if [ "${1-}" = --status ];then
        if [ -d $PREFIX ];then return 0
        else                       return 1;fi
    else
        generic_build_install_dispatch install
    fi
}   # generic_install
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
                cp_with_deps $UPS_DIR/ups/generic.table \
                    $PRODS_RT/$PROD_NAM/$PROD_VER/ups/$PROD_NAM.table
            fi
        
            cmd "ups declare -c -z$PRODS_RT -r$PROD_NAM/$PROD_VER -Mups \
                -m$PROD_NAM.table -f$PROD_FLV ${opt_q+-q$opt_q} \
                $PROD_NAM $PROD_VER"
        else
            echo 'Error - generic (qualified) inst dir does not exists'
        fi
    fi
}   # generic_declare

#------------------------------------------------------------------------------

ups_build()
{
    echov "ups build $@"
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
                    UPS_DIR=$dir_sav/build-$ups_flv ../../bin/upspremake clean
                fi
                UPS_DIR=$dir_sav/build-$ups_flv ../../bin/upspremake ${cflags-}
            fi

            cd ..
        done
        /bin/rm -fr src inc        # remove these "lndirs"

        cd $dir_sav
    fi
}   # ups_build

ups_install()
{
    echov "ups install $@"
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
}   # ups_install

ups_declare()
{
    echov "ups declare $@"
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
    fi
}   # ups_declare

#------------------------------------------------------------------------------


generic_configure_build()
{
    eval "configure_opts=\"${opt_configure-}\""  # eval $PREFIX
    echov "generic \"./configure\" build (configure_opts=$configure_opts)"
    mkdir -p $q_bld_dir
    cd $q_bld_dir
    if [ -f config.log ];then
        echo config.log exists
    fi
    if echo :${opt_q-}: | grep :debug: >/dev/null;then
        CFLAGS="-g -O0" CXXFLAGS="-g -O0" \
            ../configure --prefix=$PREFIX $configure_opts
        if [ ! -f Makefile.orig ];then cp -p Makefile Makefile.orig;fi
        # use 2 sed express so as to not filter out -O from -O0
        sed -i -e '/[ "]-O[s1-3 "]/s/-O[s1-3 ]//' \
               -e '/[ "]-O$/s/-O//'        Makefile  # " quote for emacs
                                                     #  colorization
        CFLAGS="-g -O0" CXXFLAGS="-g -O0" \
            make
    else
        ../configure --prefix=$PREFIX $configure_opts
        make
    fi
    cd ..
}   # generic_configure_build
generic_configure_install()
{
        q_bld_dir=build-`basename $PREFIX`
        if [ -d $q_bld_dir ];then   # the result of the build
            dir_sav=$PWD
            cd $q_bld_dir
            if [ -f Makefile ];then
                prefix=`sed -n '/^ *prefix *=/{s/^[^=]*=[ 	]*//;p;q}' Makefile`
                if [ "$prefix" != $PREFIX ];then
                    echo "Error - build/install mismatch"
                    echo "   >$prefix<"
                    echo "!= >$PREFIX<"
                    exit
                fi
            else
                echo 'Error '
                exit
            fi

            make install

            echov now install fix

            cd $PREFIX
            re="^#! *$PREFIX/bin/"
            ff_l=`find . -type f | xargs grep -l "$re"`
            for ff in $ff_l;do
                if grep "${re}[^ ]*  *-" $ff >/dev/null;then
                    echo 'Trouble -- interp with option(s)'
                else
                    echov "fixing $ff"
                    # what about permissions???
                    sed -i "s|$re|#!/usr/bin/env |" $ff
                fi
            done
            re="$PREFIX"
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
            echo 'Error - generic (build) install error'
            exit
        fi
}   # generic_configure_install
generic_cmake_build()
{   echov 'generic_cmake_build'
}   # generic_cmake_build
generic_cmake_install()
{   echov 'generic_cmake_install'
}   # generic_cmake_install
generic_bootstrap_build()
{   echov 'generic_bootstrap_build'
}   # generic_bootstrap_build
generic_bootstrap_install()
{   echov 'generic_bootstrap_install'
}   # generic_bootstrap_install
generic_setup_py_build()
{   echov 'generic_setup_py_build'
}   # generic_setup_py_build
generic_setup_py_install()
{   echov 'generic_setup_py_install'
}   # generic_setup_py_install
generic_Makefile_build()
{   echov 'generic_Makefile_build'
}   # generic_Makefile_build
generic_Makefile_install()
{   echov 'generic_Makefile_install'
}   # generic_Makefile_install


#------------------------------------------------------------------------------

calc() { echo "scale=4;$@" | bc; }
delta_m() { t1=`date +%s`; delta_s=`expr $t1 - $t0`; calc "$delta_s / 60"; }

generic_build_install_dispatch()
{
    # of course, the order of checking is important!!!
    if   [ -f bootstrap ];then
        method=bootstrap
    elif [ -f configure ];then
        method=configure
    elif xx=`/bin/ls *.cmake 2>/dev/null`;then
        if [ `echo "$xx"|wc -l` -gt 1 ];then
            echo "Warning - more than 1 cmake" >&2
        fi
        method=cmake
    elif [ -f setup.py ];then
        method=setup_py
    elif [ -f Makefile ];then
        method=Makefile
    else
        echo 'Error - cannot determine "generic" method. You must create a'
        echo '<prod>_productization_lib.sh file for $PROD_NAM'
        exit
    fi
    case $1 in
    build|install)   generic_${method}_$1;;
    *)  echo 'Error - internal';exit;;
    esac
}

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
