SHELL = /bin/sh
# 
#  1) This file is designed to be invoked by a shell script called premake
#  which selects supported options.
#
#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all :
	@echo all target does nothing

#
# Now standard targets
#
clean :
	@ upsClean

install :
	@if [ "$(UPS_INSTALL_DIR)" = "" ]; then \
		echo You have not specified a destination directory >&2; \
		exit 1; \
	fi
	@ rm -rf $(UPS_INSTALL_DIR)/lib
	@ mkdir $(UPS_INSTALL_DIR)/lib
	cp -r Makefile $(UPS_INSTALL_DIR)/lib
	cp -r * $(UPS_INSTALL_DIR)/lib
	@( \
		cd $(UPS_INSTALL_DIR)/lib ;\
		find . \( -type d -name CVS -prune -o \
			-name Makefile -name \*~ \) -exec rm -rf {} \; \
	)
