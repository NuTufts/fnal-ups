#!/usr/bin/env perl

$| = 1;
$debug = 1;
@rootlist = parseargs();

if ($#rootlist != -1) {

    for $root (@rootlist) {
	@tree = ();
	@visited = ();
	find_parents($root);
	dump_tree() if $debug > 2;
        recurse_tree($root, '');
    }

}

sub recurse_tree {
    my ($node, $prefix) = @_;
    my ($n, @kids, $pre, $k, $i);

    ($pre = $prefix) =~ s/.  $/|__/;
     
    if ( ! $visited{$node} ) {
	@kids = keys(%{$tree{$node}});
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

sub parseargs {
    my ($a, $root, @stuff, @rest, @res);
    @res = ();
    $a = join(' ', @ARGV);
    $dashz=0;
    for $f (@ARGV) {
        if ($dashz) {
 	   $::ENV{'PRODUCTS'} = $f;
	   $dashz = 0
        }
	if (/^-.*z/) {
	   $dashz = 1
        }
    }

    $cmd = "ups list -K+:database $a |";

    print "cmd is $cmd\n" if $debug > 1;

    open(LIST, $cmd);

    while (<LIST>) {
	print "got $_" if $debug > 3;
	@stuff = m/"(.*?)"/g;
	@rest = <LIST>;
	push(@res, makenode(@stuff));
    }

    if (close(LIST)) {
        return @res;
    } else {
	return ();
    }
}

sub find_parents {
    my ($root) = @_;
    $cmd = "ups list -a -K+:database |";
    print "cmd is $cmd\n" if $debug;
    open(LIST, $cmd);

    while( <LIST> ) {
       print "got $_" if $debug;
       @words = m/"(.*?)"/g;
       dodeps($root, @words);
    }
    close(LIST);
}

sub dodeps {
   my (@words, $parent, $root);

   $root = shift(@_);
   $parent = makenode(@_);
   $sp = $parent;
   $sp =~ s/ -z .*//;
   $sp =~ s/ -f ([^N]|N[^U]|NU[^L])/ -H $1/;
   print "doing _all_ dependencies for $parent\n" if $debug;
   $cmd = "ups depend -K+:database $sp |";
   open(DEPEND, $cmd);
   $useit = 0;
   while(<DEPEND>) {
       print "got $_" if $debug;
       @words = m/"(.*?)"/g;
       $child = makenode(@words);
       if ( $child eq $root ) {
           $useit = 1;
	   print "found our $root" if $debug;
       }
   }
   close(DEPEND);
 
   return unless $useit;

   print "doing _direct_ dependencies for $parent\n" if $debug;
   $cmd = "ups depend -j -K+:database $sp |";
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
