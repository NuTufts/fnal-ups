# Append the bin directory to the path
#
PATH="${UPS_DIR}/bin:${PATH}"; export PATH

#
# Define 'setup', 'unsetup', and 'ups'
#
ups()
{
   ${UPS_DIR}/bin/ups "$@"
}
setup()
{
   . `${UPS_DIR}/bin/ups setup "$@"`
}
unsetup()
{
   . `${UPS_DIR}/bin/ups unsetup "$@"`
}

