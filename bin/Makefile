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
	@ eruptClean

install :
	@if [ "$(ERUPT_INSTALL_DIR)" = "" ]; then \
		echo You have not specified a destination directory >&2; \
		exit 1; \
	fi
	@ rm -rf $(ERUPT_INSTALL_DIR)/bin
	@ mkdir $(ERUPT_INSTALL_DIR)/bin
	cp -r Makefile $(ERUPT_INSTALL_DIR)/bin
	cp -r * $(ERUPT_INSTALL_DIR)/bin
	@chmod 755 $(ERUPT_INSTALL_DIR)/bin/*
	@( \
		cd $(ERUPT_INSTALL_DIR)/bin ;\
		find . \( -type d -name CVS -prune -o \
			-name Makefile -name \*~ \) -exec rm -rf {} \; \
	)
