#!/usr/bin/env perl

$| = 1;
$debug = 0;
@tree = ();
@visited = ();
@rootlist = parseargs();

if ($#rootlist != -1) {
    find_parents();
    dump_tree() if $debug > 2;
    for $root (@rootlist) {
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
    for ($i = 0; $i < 4; $i++) {
        $cmd = "ups list -a -K+:database -$i |";
        print "cmd is $cmd\n" if $debug;
	open(LIST, $cmd);

	while( <LIST> ) {
           print "got $_" if $debug;
	   @words = m/"(.*?)"/g;
	   dodeps(@words);
	}
	close(LIST);
    }
}

sub dodeps {
   my (@words, $parent);

   $parent = makenode(@_);
   ($sp = $parent) =~ s/ -z .*//;
   print "doing dependencies for $parent\n" if $debug;
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
