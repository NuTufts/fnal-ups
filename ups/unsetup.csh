#
#  Unsetup the reconstructed ups
#

# Remove the bin from the path
#
set path=(`dropit -d' ' /erupt`)

# Restore the old value of PRODUCTS
#
setenv PRODUCTS ${PRODUCTS_SAVE}
unsetenv PRODUCTS_SAVE

# Put back the old setup, unsetup and ups definitions
#
source ${UPS_DIR}/ups/setup.csh

