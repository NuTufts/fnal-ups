#
#  Setup for the reconstructed ups
#

# First append the bin directory to the path.  Append it so that the command
# 'ups' will be taken from here and not from the old declared ups product.
#
PATH=${ERUPT_DIR}/bin:${PATH}; export PATH

# Save the old value of $PRODUCTS so it can be restored later.  Only save it
# if it does not already exist
#
if [ ! ${PRODUCTS_SAVE-} ]; then
    PRODUCTS_SAVE=${PRODUCTS}; export PRODUCTS_SAVE
fi

# Define a new $PRODUCTS
# 
PRODUCTS=`cat ${ERUPT_DIR}/ups/new_database`;
export PRODUCTS

# Redefine 'setup', 'unsetup', and 'ups'
#
ups()
{
   $ERUPT_DIR/bin/ups "$@"
}
setup()
{
   . `$ERUPT_DIR/bin/ups setup $@`
}
unsetup()
{
   . `$ERUPT_DIR/bin/ups unsetup $@`
}
