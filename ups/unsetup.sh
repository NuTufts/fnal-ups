#
#  Unsetup the reconstructed ups
#

# Remove the bin from the path
#
PATH=`dropit /erupt/`; export PATH;
PATH=`dropit /ups/`; export PATH;

# Restore the old value of PRODUCTS
#
if [ "${PRODUCTS_SAVE:-1}" != "1" ]; then
    PRODUCTS="${PRODUCTS_SAVE}"; export PRODUCTS
    unset PRODUCTS_SAVE
fi

# Put back the old setup, unsetup and ups definitions
#
if [ "${ERUPT_DIR:-1}" != "1" ]; then
    . ${UPS_DIR}/ups/setup.sh
fi