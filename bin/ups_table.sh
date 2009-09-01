#! /bin/sh
#  This file (ups_table.sh) was created by Ron Rechenmacher <ron@fnal.gov> on
#  Jul 16, 2009. "TERMS AND CONDITIONS" governing this file are in the README
#  or COPYING file. If you do not have such a file, one can be obtained by
#  contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
#  $RCSfile$
#  rev='$Revision$$Date$'

USAGE="\
  The purpose of this script is to automatically adjust the generic
table file to add qualified (or build) and non-qualified dependencies
and qualified (or build) and non-qualified env. variables.

  usage: `basename $0` [options] <prod|table> [ver]
options:
  -q
...
"

#if [ $# -lt 1 -o $# -gt 2 ];then echo "$USAGE"; exit; fi




add_fq()
{
    set_opt "$@"
    set_TBLF
    set_FLV
    set_QUAL
    if list_fq | grep "$FLV \"$QUAL\"";then
        echo "flv/qual combination already exists"
        awk_fq='{print;}'
    else
        awk_fq="BEGIN{IGNORECASE=1;}
{ print; }
/^GROUP:/{ printf \"\
FLAVOR = $FLV\nQUALIFIERS = \\\"$QUAL\\\"\n\
    ACTION = setup\n\
        exeActionRequired(\\\"setup__\\\")\n\
\";}"
    fi

    if [ "${opt_deps-}" ];then
        sed_up=''
        sed_dn=''
        IFSsav=$IFS IFS=,; for dep in $opt_deps;do IFS=$IFSsav
            dep=`echo $dep`
            echo "dep=>$dep<"
            sed_up="$sed_up        setupRequired( \\\"$dep\\\" )\\n"
            # unsetup in opposite order
            sed_dn="\\n        unsetupRequired( \"$dep\" )$sed_dn"
        done
        awk_deps="BEGIN{IGNORECASE=1;}
 {print;}
 /^flavor *=/{flv=gensub(/flavor *= */,\"\",1);}
 /^qualifiers *=/{qual=gensub(/qualifiers *= */,\"\",1);
   if (flv == \"$FLV\" && qual == \"\\\"$QUAL\\\"\") {
      do_it=1;
   }
 }
 /^ *action *= *setup/ {
   if (do_it == 1) {printf \"$sed_up\";do_it=0;}
 }
"
    else
        awk_deps='{print;}'
    fi

    if [ "${opt_vars-}" ];then
        vars=''
        IFSsav=$IFS IFS=,; for vv in $opt_vars;do IFS=$IFSsav
            vvar=`expr "$vv" : '\([^=]*\)'`
            vval=`expr "$vv" : '[^=]*=\(.*\)'`
            vars="$vars        envSet( $vvar, $vval )\\n"
        done
        awk_vars="BEGIN{IGNORECASE=1;}
 /^flavor *=/{flv=gensub(/flavor *= */,\"\",1);}
 /^qualifiers *=/{qual=gensub(/qualifiers *= */,\"\",1);
   if (flv == \"$FLV\" && qual == \"\\\"$QUAL\\\"\") {
      do_it=1;
   }
 }
 /^ *exeActionRequired/ {
   if (do_it == 1) {printf \"$vars\";do_it=0;}
 }
 {print;}
"
    else
        awk_vars='{print;}'
    fi


    cat $TBLF | awk "$awk_fq" | awk "$awk_deps" | awk "$awk_vars"
}
list_fq()
{
    set_opt "$@"
    set_TBLF
    awk 'BEGIN{IGNORECASE=1;}
 /^flavor *=/{flv=gensub(/flavor *= */,"",1);}
 /^qualifiers *=/{qual=gensub(/qualifiers *= */,"",1);
   printf "%s %s\n",flv,qual;
 }
' $TBLF
}

list_vars()
{
    set_opt "$@"
    set_TBLF
    set_FLV
    set_QUAL
    awk "BEGIN{IGNORECASE=1;}
 /^flavor *=/{flv=gensub(/flavor *= */,\"\",1);do_it=0;}
 /^qualifiers *=/{qual=gensub(/qualifiers *= */,\"\",1);
   if (flv == \"$FLV\" && qual == \"\\\"$QUAL\\\"\") {
      do_it=1;
   }
 }
 /^common:/{do_it=0;}
 /^ *envset/ {
   if (do_it == 1) {printf \"%s\\n\",gensub(/^ *envset *\\( *([^ ,]*) *, *(.*)\\)$/,\"\\\\1=\\\\2\",1);}
 }
" $TBLF
}

list_deps()
{
    set_opt "$@"
    set_TBLF
    set_FLV
    set_QUAL
    awk "BEGIN{IGNORECASE=1;}
 /^flavor *=/{flv=gensub(/flavor *= */,\"\",1);do_it=0;}
 /^qualifiers *=/{qual=gensub(/qualifiers *= */,\"\",1);
   if (flv == \"$FLV\" && qual == \"\\\"$QUAL\\\"\") {
      do_it=1;
   }
 }
 /^common:/{do_it=0;}
 /^ *setuprequired/ {
   if (do_it == 1) {printf \"%s\\n\",gensub(/^ *setuprequired *\\( *\"([^\"]*)\" *\\).*$/,\"\\\\1\",1);}
 }
" $TBLF
}

#------------------------------------------------------------------------------
set_TBLF()
{   if [ "${TBLF-}" ];then return; fi
    if [ "${opt_tblf-}" ];then TBLF=$opt_tblf
    else                      TBLF=$UPS_DIR/ups/generic.table; fi
}
set_FLV()
{   if [ "${FLV-}" ];then return; fi
    if [ "${opt_flv-}" ];then FLV=$opt_flv
    else                      FLV=`ups flavor`; fi
}
set_QUAL()
{   if [ "${QUAL-}" ];then return; fi
    if [ "${opt_qual-}" ];then QUAL=$opt_qual
    else                       QUAL=; fi
}
set_opt()
{
    opts_w_args='tblf|flv|qual|deps|vars'
    do_opt="\
    case \$opt in
    \\?|h|help)    echo \"\$USAGE\"; exit 0;;
    $opts_w_args)  if   [ \"\${rest-}\" != '' ];then arg=\$rest; rest=
                   elif [      $#      -ge 1  ];then arg=\$1; shift
                   else  echo \"option \$opt requires argument\"; exit 1; fi
                   eval opt_\`echo \$opt|sed -e 's/-/_/g'\`=\"'\$arg'\"
                   opts=\"\$opts '\$arg'\";;      # tricky part Aa
    *) echo \"invalid option: \$opt\" 2>&1;echo \"\$USAGE\" 2>&1; exit 1;;
    esac"
    while op=`expr "${1-}" : '-\(.*\)'`;do
        shift
        if xx=`expr "$op" : '-\(.*\)'`;then
            if   opt=`expr     "$xx" : '\([^=]*\)='`;then
                set ${-+-$-} -- "`expr "$xx" : '[^=]*=\(.*\)'`" "$@"
            else opt=$xx; fi
            opts="${opts-} '--$opt'"      # tricky part Ab
            eval "$do_opt"
        else
            while opt=`expr "$op" : '\(.\)'`;do
                opts="${opts-} '-$opt'"      # tricky part Ac
                rest=`expr "$op" : '.\(.*\)'`
                eval "$do_opt"
                op=$rest
            done
        fi
    done
}
#------------------------------------------------------------------------------

if [ "${1-}" = -x ];then set -x;shift; fi
if [ "${1-}" ];then
    set -u
    USAGE="\
   usage: `basename $0` [opt] <cmd=add_fq>
 example: `basename $0` add_fq
"
    cmd=$1; shift
    case $cmd in
    add_fq)    add_fq "$@";;
    list_fq)   list_fq "$@";;
    list_vars) list_vars "$@";;
    list_deps) list_deps "$@";;
    *)       echo "Unknown cmd $cmd."; echo "$USAGE"; exit 1;;
    esac
fi
