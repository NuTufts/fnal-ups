#
# remove the aliases
#
unalias setup
unalias unsetup
unalias ups

# Remove the bin from the path
#
set path=(`dropit -d' ' /ups`)
