#!/usr/bin/env perl

$^W=1;

# debugging -- off for production, should do something with -v[vv...]
$debug = 0;
$| = 1;

print 1, "\% completed.  \r";
make_hflavorlist();
print int(30*100/$entries), "\% completed.  \r";
@rootlist = parseargs();
if ($#rootlist != -1) {

    # build a fresh tree each time, since we only put in thingss
    # that involve us in a ups depend.

    for $root (@rootlist) {
	@tree = ();
	@visited = ();
	find_parents($root);
	dump_tree() if $debug > 2;
        recurse_tree($root, '');
    }
    exit(0);
} else {
    exit(1);
}

#--------------------------------------------------

#
# list of all toplevel ups flavor -3 values we might see.
# if we get a -2-ish flavor, (i.e. IRIX+6) we slap a ".x" 
# on the end.  If it's a -1-ish flavor we don't care, 'cause
# "-H IRIX" is sufficient if there are no IRIX+x flavors...
#
sub make_hflavorlist {

    $ourflavor = `ups flavor -3`;
    %hflavorlist = ();
    $entries = 30;
    open(FLAVORS,"ups list -aK flavor 2>/dev/null |");
    while (<FLAVORS>) {
       $entries++;
       s/\A\s*"//o;
       s/"\s*\Z//o;
       next unless m/\+/o;
       s/\Z/.x/ unless m/\./o;
       $hflavorlist{$_} = 1;
    }
    close(FLAVORS);
    if (! $hflavorlist{$ourflavor}) {
	$hflavorlist{$ourflavor} = 1;
    }
}
#
# Walk the tree and pretty-print it.
# if we've been to this node before, print it in parenthesis, and don't
# do it's kids again...
#
sub recurse_tree {
    my ($node, $prefix) = @_;
    my ($n, @kids, $pre, $k, $i);

    ($pre = $prefix) =~ s/.  $/|__/;
     
    if ( ! $visited{$node} ) {
	@kids = sort(keys(%{$tree{$node}}));
	print $pre, $node, "\n";
	$visited{$node} = 1;
	$k = $#kids;
	$i = 0;
	foreach $n (@kids) {
	    # visit it..
	    if ( $i == $k ) {
		recurse_tree( $n, $prefix . "   " );
	    } else {
		recurse_tree( $n, $prefix . "|  " );
	    }
	    $i++;
	}
    } else {
	print $pre, "(", $node, ")\n";
    }
}

#
# Debug dump of the whole tree graph data structure
#
sub dump_tree {
    print "Tree:\n";
    foreach $k (keys(%tree)) {
        @kids = keys(%{$tree{$k}});
	foreach $n (@kids) {
	    print "$k -> $n\n";
	}
    }
    print "End of Tree:\n";
}

#
# pick on a few arguments, and run ups list...
#
sub parseargs {
    my ($a, $root, @stuff, @rest, @res);

    @res = ();
    $a = join(' ', @ARGV);
    $dashz=0;

    #
    # if they gave us a -z, set $PRODUCTS, and whine if they used -K
    #
    for $f (@ARGV) {
        if ($dashz) {
 	   $::ENV{'PRODUCTS'} = $f;
	   $dashz = 0
        }
	if (/^-.*z/) {
	   $dashz = 1
        }
	if (/^-.*K/) {
	    print STDERR "ERROR: -K not supported\n";
	    return ();
        }
    }

    # don't redirect stderr, this is how they find out about 
    # command line errors, etc.
    $cmd = "ups list -K+:database $a |";
    print "cmd is $cmd\n" if $debug > 1;
    open(LIST, $cmd);

    while (<LIST>) {
	print "got $_" if $debug > 3;
	@stuff = m/"(.*?)"/g;
	@rest = <LIST>;
	push(@res, makenode(@stuff));
    }

    # close will be false if the ups list had errors...
    if (close(LIST)) {
        return @res;
    } else {
	return ();
    }
}

#
# go through the whole database, and look at each product with
# dodeps
#
sub find_parents {
    my ($root) = @_;
    my ($count, $cmd);

    $cmd = "ups list -a -K+:database 2>/dev/null |";
    print "cmd is $cmd\n" if $debug;
    open(LIST, $cmd);

    $count = 30;
    while( <LIST> ) {
       print "got $_" if $debug;
       @words = m/"(.*?)"/g;
       dodeps($root, @words);
       $count++;
       print int($count*100/$entries), "\% completed.  \r";
    }
    close(LIST);
}

#
# look at the full dependency tree for the product
# (turning -f into -H to get a sane dependency list)
# if its full dependencies involve the product in question,
# dump the direct dependencies into the tree (backwards)
#
%didthat = ();
sub dodeps {
   my (@words, $parent, $root);
   my ($cmd,$hflavorpat, $hflavor,$sp);

   $root = shift(@_);
   $parent = makenode(@_);

   #
   # here we guess one (or more) longer flavors to go with
   # any -f option we have to get around problems like where we have
   # both dependency graphs below possible (assuming we are 
   # parent-ing bleem):
   # -------------
   # foo -f IRIX
   # |__bar -f IRIX+6
   #    |__baz -f NULL
   #       |__bleem -f IRIX
   # -------------
   # foo -f IRIX
   # |__bar -f IRIX+5
   #    |__baz -f NULL
   #       |__bleem -f IRIX
   # -------------
   # you actually need to do two (or more) different ups depends of 
   # foo to find out about the two instances of bar...

   # turn the flavor into a regexp
   $hflavorpat = @_[2];
   $hflavorpat =~ s/\W/\\$&/go;
   $hflavorpat =~ s/NULL/.*/o;

   print "doing dependencies for $parent\n" if $debug;
   foreach $hflavor (keys(%hflavorlist)) {
       if ( $hflavor =~ m/$hflavorpat/ ) {

	   $sp = $parent;
	   $sp =~ s/ -z .*//o;
	   $sp =~ s/ -f \S+/ -H $hflavor/;

	   print "doing _all_ dependencies for $sp\n" if $debug;
	   $cmd = "ups depend -K+:database $sp 2>/dev/null |";

	   # weed out duplicates!
           next if $didthat{$cmd};
           $didthat{$cmd} = 1;

	   open(DEPEND, $cmd);
	   $useit = 0;
	   while(<DEPEND>) {
	       print "got $_" if $debug;
	       @words = m/"(.*?)"/g;
	       $child = makenode(@words);
	       if ( $child eq $root ) {
		   # we found our base product in the full dependency tree
		   $useit = 1;
		   print "found our $root" if $debug;
	       }
	   }
	   close(DEPEND);
	 
	   next unless $useit;

	   print "doing _direct_ dependencies for $sp\n" if $debug;
	   $cmd = "ups depend -j -K+:database $sp 2>/dev/null |";
	   print "cmd is $cmd\n" if $debug;
	   open(DEPEND, $cmd);

	   while(<DEPEND>) {
	       print "got $_" if $debug;
	       @words = m/"(.*?)"/g;
	       $child = makenode(@words);
	       addedge($parent, $child);
	   }
	   close(DEPEND);
       }
   }
}

sub addedge {
    my ($parent, $child) = @_;
    return if ($parent eq $child);
    print "adding $child -> $parent\n" if $debug;
    if (!defined $tree{$child}) {
       $tree{$child} = {};
    }
    $tree{$child}->{$parent} = 1;
}

sub makenode {
    my  $res;

    $res = @_[0] . ' ' . @_[1] . ' -f ' . @_[2];
    if (@_[3]) {
 	$res .=  ' -q ' . @_[3] 
    }
    if (@_[5]) {
        $res .= ' -z ' . @_[5];
    }
    return $res;
}
