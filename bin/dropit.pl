#!/usr/local/bin/perl 
#
#  Remove a string from an environment variable which can be split into
#  smaller pieces separated by a divider.  For example: a string that 
#  contains substrings and looks like the following -
#
#       string:string:string:string:
#
#  where ":" is the divider.  A piece of a string or the entire string
#  may be entered.  All substrings which match the entered string piece
#  will be removed.  The altered string will be returned by the script.
#  If there is an error or the environment variable does not exist, TBD.
#

$usage = "dropit.pl environmentVariable subString [delimiter]";
$defaultD = ":";
$subsMade = 0;
$rtn = 0;                             # assume success

#
# Get the command line arguments
#
if ($#ARGV < 1){ 
  print STDERR "ERROR: Need at least 2 parameters -\n\t$usage\n";
  $rtn = 1;
}
else {
  $sourceVar = $ARGV[0];              # environment variable
  $stringToRem = $ARGV[1];            # substring to remove (or piece of it)

# If we were passed a delimeter, get it - else use the default
  if ($#ARGV == 2) {
    $delimeter = $ARGV[2];
  } else {
    $delimiter = $defaultD;
  }

# Get the value of the environment variable
  $sourceVarValue = $ENV{"$sourceVar"};

# Make sure the environment variable exists first
  if ($sourceVarValue eq "") {
    print STDERR "ERROR: Environment Variable - $sourceVar, does not exist\n";
    $rtn = 1;
  } else {
# Split into substrings using the $delimiter
    @sourceVarArray = split(/$delimiter/o, $sourceVarValue);

# see if each substring contains the string to remove. if it doesn't add it to
# a new array
    foreach $subString (@sourceVarArray) {
      if (($pos = index($subString, $stringToRem)) == -1) {
	if ($subsMade == 0) {
#       This is our first one so there is no $newString yet
	  $newString = join "$delimiter", $subString;
	} else {
	  $newString = join "$delimiter", $newString, $subString;
	}
	++$subsMade;
      }
    }
    print $newString;
  }
}

exit $rtn ;
