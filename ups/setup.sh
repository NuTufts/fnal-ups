#
#  Setup for the reconstructed ups
#

# First append the bin directory to the path.  Append it so that the command
# 'ups' will be taken from here and not from the old declared ups product.
#
if [ "${ERUPT_DIR:-1}" = "1" ]; then
    ERUPT_DIR=$UPS_DIR;export ERUPT_DIR
    set_erupt_dir=1
fi

PATH="${ERUPT_DIR}/bin:${PATH}"; export PATH

# Save the old value of $PRODUCTS so it can be restored later.  Only save it
# if it does not already exist
#
if [ "${PRODUCTS_SAVE:-1}" != "1" ]; then
    PRODUCTS_SAVE="${PRODUCTS}"; export PRODUCTS_SAVE
fi

# Define a new $PRODUCTS
# 
PRODUCTS="`cat ${ERUPT_DIR}/ups/new_database`";
export PRODUCTS

# Redefine 'setup', 'unsetup', and 'ups'
#
ups()
{
   $ERUPT_DIR/bin/ups "$@"
}
setup()
{
   . `$ERUPT_DIR/bin/ups setup "$@"`
}
unsetup()
{
   . `$ERUPT_DIR/bin/ups unsetup "$@"`
}

if [ "${set_erupt_dir:-2}" = "1" ]; then
    unset set_erupt_dir
fi



