#
# Append the bin directory to the path
#
set path=(${UPS_DIR}/bin $path)
rehash

#
# Redefine 'setup', 'unsetup', and 'ups'
#
alias   ups             ${UPS_DIR}/bin/ups
set MACH_OS = `uname -s`
if ($MACH_OS == "ULTRIX") then
     alias setup   'set ups_x = `${UPS_DIR}/bin/ups setup \!*`; source $ups_x && unset ups_x'
     alias unsetup 'set ups_x = `${UPS_DIR}/bin/ups unsetup \!*`; source $ups_x && unset ups_x'
else   
    alias setup         source \` `echo ${UPS_DIR}`/bin/ups setup '\!*' \` 
    alias unsetup       source \` `echo ${UPS_DIR}`/bin/ups unsetup '\!*' \` 
endif
