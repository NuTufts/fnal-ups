#!/bin/sh


# dropit -- shell function to remove items from a path-style variable
#     -c	-- use csh $path defaults instead of $PATH defaults
#     -i char	-- input delimiter (and default output one)
#     -n xx	-- specify null path component replacement xx
#     -d char	-- output delimiter
#     -p path	-- path to edit
#     -e	-- exact matching of things to delete (anchor at both ends)
#     -a	-- anchor at front
#     -s	-- (suffix) reattach item at end after cleaning out
#     -f	-- reattach item at front after cleaning out
#     string	-- string to match against path components, items that match
#		   are dropped and optionally re-added (if -f or -s )
# dash options must precede strings.

dropit() {

    # defaults
    d_p="$PATH"		# -p path argument 
    d_pi=":"		# what to set d_i to if -p is used (-c silliness)
    d_i=":"		# input delimiter
    d_d=":"		# output delimiter
    d_n="."		# null path replacement
    d_f=false		# -f re-attach first
    d_s=false		# -s re-attach suffix
    d_a=false		# -a anchor at front
    d_e=false		# -e exact match


    # parse arguments

    while [ $# -gt 1 ]
    do
	case "x$1" in
	x-c) d_d=" "; d_pi=" "; shift 1;;
	x-i) d_i="$2"; d_pi="$2"; shift 2;;
	x-n) d_n="$2"; shift 2;;
	x-d) d_d="$2"; shift 2;;
	x-p) d_p="$2"; d_i="$d_pi"; shift 2;;
	x-e) d_a=true; d_e=true; shift 1;;
	x-a) d_a=true; shift 1;;
	x-s) d_s=true; shift 1;;
	x-f) d_f=true; shift 1;;
	*) break;;
	esac
    done

    # at this point remaining arguments are strings to match.

    #make regexp pattern scraps out of -a and -e values
    #  basically, if we anchor at the front/end, they're blank, but if we
    #  don't anchor they're a pattern matching a sequence of non-delimiters.
    if $d_a
    then
	d_a=""
    else
	d_a="[^$d_d]*"
    fi

    if $d_e
    then
	d_e=""
    else
	d_e="[^$d_d]*"
    fi

    # replace input delimiters and null path
    #    First we convert input delimiters to output ones
    #    Then we find doubled delimiters, and stuff a null path
    # 	 replacement in there.

    d_p="`echo $d_p | 
	sed -e \"s|$d_i|$d_d|g\" \
	    -e \"s|$d_d$d_d|$d_d$d_n$d_d|g\"`"

    # remove/replace substrings one at a time

    for d_substring in "$@"
    do
	# escape wildcards, etc. in substring (e.g. SunOS+5)
	d_substringp="`echo $d_substring |
	    sed -e 's|[^a-zA-Z_0-9()]|\\\\&|g'`"

	# Now we actually remove the substring occurances from the string.
	# We splice delimiters on both ends first (we'lltake them back off
	# 	when we're done.)
	# we double all the delimiters, (so we match adjacent components)
	# remove the matches
	# un-double the delimiters
	# and clean the extras off the ends.
	# It looks like a lot, but it's only one sed call...

	d_match="${d_d}${d_a}${d_substringp}${d_e}${d_d}"
	d_p="`echo $d_p | 
	    sed -e \"s|.*|${d_d}&${d_d}|\" \
		-e \"s|${d_d}|${d_d}${d_d}|g\" \
		-e \"s|${d_match}||g\" \
		-e \"s|${d_d}${d_d}|${d_d}|g\" \
		-e \"s|${d_d}\(.*\)${d_d}|\1|\"`"

	# add the thing back onto the path if -f or -s is specified.

	if $d_f
	then
	    d_p="${d_substring}${d_d}${d_p}"
	fi
	if $d_s
	then
	    d_p="${d_p}${d_d}${d_substring}"
	fi
    done

    echo $d_p
     
}

dropit "$@"


