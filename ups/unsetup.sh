#
#  Unsetup the reconstructed ups
#

# Remove the bin from the path
#
PATH=`dropit /erupt/`; export PATH;

# Restore the old value of PRODUCTS
#
PRODUCTS=${PRODUCTS_SAVE}; export PRODUCTS
unset PRODUCTS_SAVE

# Put back the old setup, unsetup and ups definitions
#
. ${UPS_DIR}/ups/setup.sh
