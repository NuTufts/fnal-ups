#
# Append the bin directory to the path
#
set path=(${UPS_DIR}/bin $path)
rehash

#
# Redefine 'setup', 'unsetup', and 'ups'
#
alias   ups             ${UPS_DIR}/bin/ups
alias setup         source \` `echo ${UPS_DIR}`/bin/ups setup '\!*' \` 
alias unsetup       source \` `echo ${UPS_DIR}`/bin/ups unsetup '\!*' \` 
