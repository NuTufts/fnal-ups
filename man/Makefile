SHELL = /bin/sh
# 
#  1) This file is designed to be invoked by a shell script called premake
#  which selects supported options.
#
#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using upspremake. >&2
	@exit 1

all :
	@rm catman/*.1
	@../bin/ups -? > catman/ups.1
	@../bin/ups configure -? > catman/ups_configure.1
	@../bin/ups copy -? > catman/ups_copy.1
	@../bin/ups create -? > catman/ups_create.1
	@../bin/ups declare -? > catman/ups_declare.1
	@../bin/ups depend -? > catman/ups_depend.1
	@../bin/ups exist -? > catman/ups_exist.1
	@../bin/ups flavor -? > catman/ups_flavor.1
	@../bin/ups get -? > catman/ups_get.1
	@../bin/ups list -? > catman/ups_list.1
	@../bin/ups modify -? > catman/ups_modify.1
	@../bin/ups setup -? > catman/ups_setup.1
	@../bin/ups start -? > catman/ups_start.1
	@../bin/ups stop -? > catman/ups_stop.1
	@../bin/ups tailor -? > catman/ups_tailor.1
	@../bin/ups touch -? > catman/ups_touch.1
	@../bin/ups unconfigure -? > catman/ups_unconfigure.1
	@../bin/ups undeclare -? > catman/ups_undeclare.1
	@../bin/ups unsetup -? > catman/ups_unsetup.1
	@../bin/ups verify -? > catman/ups_verify.1

#
# Now standard targets
#
clean :
	@upsClean

spotless :
	@upsClean
	@/bin/rm -f up


