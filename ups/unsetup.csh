#
#  Unsetup the reconstructed ups
#

# Remove the bin from the path
#
set path=(`dropit -d' ' /erupt`)
set path=(`dropit -d' ' /ups`)

# Restore the old value of PRODUCTS
#
if ("${?PRODUCTS_SAVE}" == "1") then
    setenv PRODUCTS "${PRODUCTS_SAVE}"
    unsetenv PRODUCTS_SAVE
endif

# Put back the old setup, unsetup and ups definitions
#
if ("${?ERUPT_DIR}" == 1) then
    source ${UPS_DIR}/ups/setup.csh
endif

