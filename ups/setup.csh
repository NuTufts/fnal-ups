#
#  Setup for the reconstructed ups
#

# First append the bin directory to the path.  Append it so that the command
# 'ups' will be taken from here and not from the old declared ups product.
#
if ("${?ERUPT_DIR}" == "0") then
    setenv ERUPT_DIR $UPS_DIR
    setenv set_erupt_dir 1
endif

set path=(${ERUPT_DIR}/bin $path)
rehash

# Save the old value of $PRODUCTS so it can be restored later.  Only save it
# if it does not already exist
#
if (! ${?PRODUCTS_SAVE}) then
    setenv PRODUCTS_SAVE "${PRODUCTS}"
endif

# Define a new $PRODUCTS
# 
setenv PRODUCTS "`cat ${ERUPT_DIR}/ups/new_database`"

# Redefine 'setup', 'unsetup', and 'ups'
#
alias   ups             $ERUPT_DIR/bin/ups
set MACH_OS = `uname -s`
if ($MACH_OS == "ULTRIX") then
     alias setup   'set ups_x = `$ERUPT_DIR/bin/ups setup \!*`; source $ups_x && unset ups_x'
     alias unsetup 'set ups_x = `$ERUPT_DIR/bin/ups unsetup \!*`; source $ups_x && unset ups_x'
else   
    alias setup         source \` `echo $ERUPT_DIR`/bin/ups setup '\!*' \` 
    alias unsetup       source \` `echo $ERUPT_DIR`/bin/ups unsetup '\!*' \` 
endif

if (${?set_erupt_dir}) then
    unsetenv set_erupt_dir
endif
