#++
# FACILITY:
#
# ABSTRACT:	Overall make file for building a product.  Each subdirectory of
#		the product's root directory is built.
#
# ENVIRONMENT:	make file.  Bourne shell.
#		Makefile
#--
#*******************************************************************************
#
# Define the environment.
#

SHELL		= /bin/sh

#
# Define some macros:
#
#   PROD	The product name used only for cosmetics (so case it properly).
#   PRODDIRNAME	The name of the root directory for the product (and the version
#		in use).
#   PRODDIR	The value of the environment value name $(PRODDIRNAME).
#

PROD		=   UPS
PRODDIRNAME	=   $UPS_INSTALL_DIR
PRODDIR		= $(UPS_INSTALL_DIR)

#*******************************************************************************

.DEFAULT:
	@ echo ''
	@ echo '*** Finished updating target $@ ***'
	@ echo ''

#*******************************************************************************
#
# Build all targets in all subdirectories.
#
#   o	Check whether we're building on the same platform.
#

all :
	@ if [ ! -f ".uname~" ]; then \
		uname > .uname~; \
	else \
		if [ `uname` != `cat .uname~` ]; then \
			echo ""; \
			echo "This copy of $(PROD) was built for the `cat .uname~` platform.";\
			echo "Execute \"make clean\" first if you want to build on a `uname` platform."; \
			echo ""; \
			exit 1; \
		fi; \
	fi
	@ echo ""
	@ echo "Updating $(PROD) libraries and subdirectories."
	@ for subdir in src inc lib bin test doc; do \
		if [ -d $$subdir ]; then \
			(cd $$subdir; echo ""; echo "make all in ./$$subdir"; echo ""; $(MAKE) $(MFLAGS) all); \
		else \
			echo ""; \
			echo "Subdirectory does not exist: ./$$subdir"; \
			echo ""; \
		fi; \
	done


#*******************************************************************************
#
# Install things from the current directories to their proper places in
# $(PRODDIR).
#
#   o	A "decision" period is provided ("I'll give you n seconds ...").  The
#	length of the echo string (all spaces) is the maximum time that can be
#	waited, since the following sed does replacements on that input string.
#

install :
	@ echo ""
	@ echo "Make sure the current $(PROD) directories under"
	@ echo ""
	@ echo "     `pwd`"
	@ echo ""
	@ echo "have the latest versions of the files.  These will be copied to the"
	@ echo "the destination during the install of $(PROD)."
	@ echo ""
	@ if [ "$(PRODDIR)" = "" ]; then \
		echo "The destination directory has not been specified.  Set the environment"	>&2; \
		echo "variable $(PRODDIRNAME)"							>&2; \
		echo ""; \
		exit 1; \
	fi
	@ if [ `(cd $(PRODDIR); pwd)` = `pwd` ]; then \
		echo "The destination directory is the same as the current directory."		>&2; \
		echo "The install will be aborted."						>&2; \
		echo ""; \
		exit 1; \
	fi
	@ echo "You will be installing in"
	@ echo ""
	@ echo "   $(PRODDIRNAME) = $(PRODDIR)"
	@ echo ""
	@ echo "I'll give you 5 seconds to think about it (control-C to abort) ..."
	@ for pos in          5 4 3 2 1; do \
	   echo "       $$pos\n; \
	   sleep 1; \
	done
	@ for subdir in bin doc inc lib src test; do \
		if [ -d $$subdir ]; then \
			if [ ! -r $(PRODDIR)/$$subdir ]; then \
				mkdir $(PRODDIR)/$$subdir; \
			fi; \
			(cd $$subdir; echo ""; echo "make install in ./$$subdir"; echo ""; $(MAKE) $(MFLAGS) install); \
		else \
			echo "Subdirectory does not exist: ./$$subdir"; \
		fi; \
	done
	cp Makefile $(PRODDIR)

#*******************************************************************************
#
# Clean up.
#
#   o	.uname~ is removed last.  This protects against future improper builds
#	(wrong platform) in case the clean fails for some reason.
#

clean :
	@ echo ""
	@ echo "Cleaning in . = `pwd`"
	@ echo ""
	@ upsClean
	@ for subdir in bin doc inc lib src test; do \
		if [ -d $$subdir ]; then \
			(cd $$subdir; echo ""; echo "Cleaning in ./$$subdir"; echo ""; $(MAKE) $(MFLAGS) clean); \
		else \
			echo "Subdirectory does not exist: ./$$subdir"; \
		fi; \
	done
	- rm -f .uname~

#*******************************************************************************
