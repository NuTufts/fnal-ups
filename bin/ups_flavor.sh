#! /bin/sh
#  This file (ups_flavor.sh) was created by Ron Rechenmacher <ron@fnal.gov> on
#  Aug 21, 2009. "TERMS AND CONDITIONS" governing this file are in the README
#  or COPYING file. If you do not have such a file, one can be obtained by
#  contacting Ron or Fermi Lab in Batavia IL, 60510, phone: 630-840-3000.
#  $RCSfile$
#  $Revision$
#  $Date$
if [ "$1" = -x ];then set -x; shift; fi
set -u

APP=`basename $0`
USAGE="\
Attempt to tell the "flavor" of the bin and lib sub directories based on
the info from the files in those directories.
  usage: $APP [-?]
example: cd some/dir
         $APP
"
while opt=`expr "${1-}" : '-\([^=]*\)'`;do
    case "$opt" in
    \?|h|-help)  echo "$USAGE";exit;;
    *)           echo "Unknown option -$opt";echo "$USAGE";exit;;
    esac
done
echov() { echo "$APP: $@"; }

dirsOfInterest=`find . -type d \( -name bin -o -name 'bin.*' \)`
dirsOfInterest="$dirsOfInterest `find . -type d \( -name lib -o -name 'lib.*' \)`"

if [ "$dirsOfInterest" = '' ];then
    echo NULL
    exit 0;
fi

adjust_os()
{
    echo $1 | sed 's|GNU/||'
}

dot2dec()
{  ee=`echo $1.0.0 | sed \
   's/\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\).*/\1 \\\* 65536 + \2 \\\* 256 + \3/'`
   eval "expr $ee"
}
dec2dot()
{   d1=`expr $1 / 65536`
    d2=`expr \( $1 - \( $d1 \* 65536 \) \) / 256`
    d3=`expr $1 - $d1 \* 65536 - $d2 \* 256`
    echo $d1.$d2.$d3
}

awkvers()
{
    awk "/$1[1-9]/{
  xx=\$0;while(ii=match(xx,/$1([0-9.]*)/,aa)){
   sub(/$1[0-9.]*/,\"\",xx);print aa[1];
   }
 }"
}
awkhilo()
{
    awk 'BEGIN{lo=0xffffff;hi=0;}
  { xx=$0 ".0.0";
    x1=gensub(/([^.]*).*/,"\\1",1,xx);
    x2=gensub(/[^.]*\.([^.]*).*/,"\\1",1,xx);
    x3=gensub(/[^.]*\.[^.]*\.([^.]*).*/,"\\1",1,xx);
    xo=x1*65536+x2*256+x3;
    if (xo < lo) lo=xo;
    if (xo > hi) hi=xo;
    #printf("%d 0x%x %s %s %s\n",xo,xo,x1,x2,x3);
  }
  END{
    #l1=lo/65536;l2=and(lo,0xff00)/256;l3=and(lo,0xff);
    #h1=hi/65536;h2=and(hi,0xff00)/256;h3=and(hi,0xff);
    #printf("%d   %d.%d.%d  %d  %d.%d.%d\n",lo,l1,l2,l3,hi,h1,h2,h3);
    printf("%d %d\n",lo,hi);
  }'
}

VERS="GLIBC_ GLIBCXX_ CXXABI_" # no fortran yet
cnt_f=0
for dd in $dirsOfInterest;do
    pushd $dd >/dev/null
    subdirs=`find . -type d \! -name CVS \! -name .svn -print`
    # "." will be the 1st one -- 
    # if "." has a flavor, then we should assume the layout:
    #      flv1/lib
    #          /bin
    #      flv2/lib
    #          /bin
    # if "." does not have a flavor, then we should assume the layout:
    #      lib/flv1
    #         /flv2
    #      bin/flv1
    #          flv2
    subdir_dot_has_flavor=
    for ss in $subdirs;do
        #echov $dd/$ss
        pushd $ss >/dev/null
        files=`find . \! -name . -prune -type f -print`
        if [ "$files" ];then
            if [ 1 -o "$subdir_dot_has_flavor" = '' ];then
                # 16777215      # 0xffffff
                os=ANY           os_file=
                klo=16777215     klo_file=      # kernel version
                khi=0            khi_file=
                prbsl4=          prbsl4_file=   # has 
                prbsl5=          prbsl5_file=
                for vv in $VERS;do
                    eval "${vv}lo=16777215 ${vv}lo_file="
                    eval "${vv}lx=0        ${vv}lx_file="
                    eval "${vv}hi=0        ${vv}hi_file="
                done
            fi
            for ff in $files;do
                cnt_f=`expr $cnt_f + 1`
                finfo=`file -b $ff`
                ftype=`echo "$finfo" | cut -d, -f1`
                case "$ftype" in
                ELF*executable|ELF*shared\ object)
                    ee=$finfo
                    if   kv=`expr "$ee" : '.*for [^ ]* \([0-9.]*\)'`;then
                        kv=`dot2dec $kv`
                        if [ $kv -lt $klo ];then klo=$kv klo_file=$ff;fi
                        if [ $kv -gt $khi ];then khi=$kv khi_file=$ff;fi
                        os=`expr "$ee" : '.*for \([^ ,]*\)'` os_file=$ff
                    elif os_=`expr "$ee" : '.*for \([^ ,]*\)'`;then
                        os=$os_  os_file=$ff
                    fi
                    os=`adjust_os "$os"`
                    for vv in $VERS;do
                        vers=`readelf -V $ff | awkvers $vv`
                        lo_hi=`echo "$vers" | awkhilo`
                        if lo=`expr "$lo_hi" : '\([0-9][0-9]*\)'`;then
                            hi=`expr "$lo_hi" : '[0-9][0-9]* \([0-9][0-9]*\)'`
                            eval "\
                            if [ $lo -lt \$${vv}lo ];then ${vv}lo=$lo ${vv}lo_file=$ff;fi
                            if [ $lo -gt \$${vv}lx ];then ${vv}lx=$lo ${vv}lx_file=$ff;fi
                            if [ $hi -gt \$${vv}hi ];then ${vv}hi=$hi ${vv}hi_file=$ff;fi
                            "
                        fi
                    done
                    ;;
                "current ar archive"|ELF*relocatable)
                    # attempt to look inside the archive for ELF files
                    ee=`readelf -hg $ff`
                    if echo "$ee" | grep 'Class:.*ELF' >/dev/null;then
                        if echo "$ee" | grep 'i686\.get_pc_' >/dev/null;then
                            if [ -z "$prbsl5" ];then prbsl5=Y prbsl5_file=$ff;fi
                        else
                            if [ -z "$prbsl4" ];then prbsl4=Y prbsl4_file=$ff;fi
                        fi
                    fi
                    #echov file=$dd/$ss/$ff aaaaaaaaaaaaaaaaaaaaaaaaaaaaa
                    ;;
                esac
            done
            if [   $klo -eq 16777215 -a $khi -eq 0 \
                -a $GLIBC_lo -eq 16777215 -a $GLIBC_hi -eq 0 \
                -a -z "$prbsl4" -a -z "$prbsl5" ];then
                if [ -z "$subdir_dot_has_flavor" ];then
                    echo "for dir:$dd/$ss: NULL"
                fi
            elif true;then
                echo "for dir:$dd/$ss: os=$os  os_file=$os_file"
                if [ $klo -lt 16777215 ];then
                    echo "        klo=`dec2dot $klo` klo_file=$klo_file"
                fi
                if [ $khi -gt 0 -a $khi -ne $klo ];then
                    echo "        khi=`dec2dot $khi` khi_file=$khi_file"
                fi
                for vv in 4 5;do
                    eval prb=\$prbsl${vv}
                    if [ "$prb" ];then
                        eval "echo \"     prbsl$vv=$prb     prbsl${vv}_file=\$prbsl${vv}_file\""
                    fi
                done
                for vv in $VERS;do
                    eval "lo=\$${vv}lo lo_file=\$${vv}lo_file"
                    eval "lx=\$${vv}lx lx_file=\$${vv}lx_file"
                    eval "hi=\$${vv}hi hi_file=\$${vv}hi_file"
                    if [ $lo -lt 16777215 ];then
                        echo "     ${vv}lo=`dec2dot $lo` ${vv}lo_file=$lo_file"
                    fi
                    if [ $lx -gt 0 -a $lx -ne $lo ];then 
                        echo "     ${vv}lx=`dec2dot $lx` ${vv}lx_file=$lx_file"
                    fi
                    if [ $hi -gt 0 -a $hi -ne $lx ];then 
                        echo "     ${vv}hi=`dec2dot $hi` ${vv}hi_file=$hi_file"
                    fi
                done
                if [ "$ss" = . ];then subdir_dot_has_flavor=1;fi
            elif true;then
                echo "for dir:$dd/$ss: $os+2.6"
            elif true;then
                echo "for dir:$dd/$ss: $os+2.6-2.5"
            else
                echo "for dir:$dd/$ss: $os+$os_v2-$glib_v2"
            fi
        fi
        popd >/dev/null
    done
    popd >/dev/null
done
if [ $cnt_f -gt 0 ];then echo cnt_f=$cnt_f;fi
