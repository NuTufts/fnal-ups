SHELL = /bin/sh
# 
#  1) This file is designed to be invoked by a shell script called premake
#  which selects supported options.
#
#  2) This file re-builds ups.
#
#  3) Environment:
# Environmental variables -
#
# Expected variables on the command line:
# CC=        The name of the appropriate compiler, with suitable flags
# CCCHK=     A set of compiler flags which check ANSI C strictly
#

.SUFFIXES :	.A

.c.o :
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $*.c

.c.A :
	@ echo '        $(CIA_DIR)/bin/cia -c $(CFLAGScia) $*.c'
	@               $(CIA_DIR)/bin/cia -c $(CFLAGScia) $*.c

#
SRC	= ../src
INC	= ../inc
LIB	= ../lib
BIN	= ../bin
DOC	= ../doc

CFLAGS	  = $(UPS_CFLAGS) $(INCDIRS)
CFLAGScia = $(INCDIRS)

#
#

OBJECTS = upserr.o upslst.o upsmem.o upsutl.o upstyp.o \
	  upsfil.o upsugo.o upsmat.o upskey.o upshlp.o \
	  upsver.o upsact.o upsget.o upstbl.o upscpy.o

COMMAND_OBJECTS = ups_list.o ups_setup.o ups_start.o ups_stop.o \
	          ups_configure.o ups_unconfigure.o ups_unsetup.o \
	          ups_tailor.o ups_create.o ups_unk.o ups_declare.o \
	          ups_depend.o ups_get.o ups_undeclare.o ups_flavor.o \
	          ups_copy.o ups_modify.o ups_touch.o ups_verify.o

CIAOBJECTS = upserr.A upslst.A upsmem.A upsutl.A upstyp.A \
	     upsfil.A upsugo.A upsmat.A ups_list.A  \
	     upskey.A upshlp.A ups_unk.A ups_setup.A \
	     upsver.A upsact.A upsget.A ups_start.A \
	     ups_stop.A ups_configure.A ups_unconfigure.A \
	     ups_unsetup.A ups_tailor.A ups_create.A upstbl.A \
             ups_list.A ups_depend.A ups_get.A ups_declare.A \
	     ups_undeclare.A ups_flavor.A ups_copy.A ups_modify.A \
             ups_unk.A ups_touch.A ups_verify.A upscpy.A

LDFLAGS = $(LLIBS)

#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all : library main old

tag_file : 
	$(EMACS_DIR)/bin/etags -t *.c ../inc/*.h

main : $(BIN)/ups

# Old stuff people just expect to work that really isn't part of ups
old: $(BIN)/funame \
	$(BIN)/dropit

library : $(LIB)/libups.a

cia :	$(CIAOBJECTS)
	cia $(CIAOBJECTS)

#
# Build main line executable
#
$(BIN)/ups : ups_main.o library
	$(CC) ups_main.o -o $@  $(CFLAGS) $(LDFLAGS)

#
# Build the old products people expect that arn't really dependencies
$(BIN)/funame : 
	$(CC) funame.c -o $@ $(OLD_LIBS)

$(BIN)/dropit :
	$(CC) dropit.c -o $@ 

#
# Compile UPS library source
#
$(LIB)/libups.a : $(OBJECTS) $(COMMAND_OBJECTS)
	@echo "=== UPS BUILD : Building Unix library (libups.a) ==="
	$(AR) $(ARFLAGS) $(LIB)/libups.a $?
	- if [ "$(RANLIB)" != "" -a -x "$(RANLIB)" ]; then \
		$(RANLIB) $(LIB)/libups.a; \
	fi
	@echo "=== UPS BUILD : Unix library (libups.a) built ==="
	@echo

#
# Now standard targets
#
clean spotless :
	@ upsClean

make :
	@if [ "$(CCENV)" = "" ]; then ccenv=`uname`; else ccenv=$(CCENV); fi; \
	echo \
	"make_make -cc '$(CC) $(CFLAGS)' -nostd -file Makefile" \
                                                        "-env $$ccenv *.c"; \
	make_make -cc '$(CC) $(CFLAGS)' -nostd -file Makefile \
                                                         -env $$ccenv *.c

# DO NOT HAND EDIT BELOW THE START_DEPEND LINE. USE "premake make" IN THE
# src DIRECTORY TO GENERATE IT
#START_DEPEND
ups_configure.o:	ups_configure.c
ups_configure.o:	$(UPS_DIR)/inc/upserr.h
ups_configure.o:	$(UPS_DIR)/inc/upsugo.h
ups_configure.o:	$(UPS_DIR)/inc/upslst.h
ups_configure.o:	$(UPS_DIR)/inc/upsutl.h
ups_configure.o:	$(UPS_DIR)/inc/upstyp.h
ups_configure.o:	$(UPS_DIR)/inc/upslst.h
ups_configure.o:	$(UPS_DIR)/inc/upslst.h
ups_configure.o:	$(UPS_DIR)/inc/upsugo.h
ups_configure.o:	$(UPS_DIR)/inc/upsmat.h
ups_configure.o:	$(UPS_DIR)/inc/upsugo.h
ups_configure.o:	$(UPS_DIR)/inc/upstyp.h
ups_configure.o:	$(UPS_DIR)/inc/upsact.h
ups_configure.o:	$(UPS_DIR)/inc/upstyp.h
ups_configure.o:	$(UPS_DIR)/inc/upsugo.h
ups_configure.o:	$(UPS_DIR)/inc/upsmat.h
ups_configure.o:	$(UPS_DIR)/inc/upslst.h
ups_configure.o:	$(UPS_DIR)/inc/upstyp.h
ups_configure.o:	$(UPS_DIR)/inc/ups_main.h
ups_copy.o:	ups_copy.c
ups_copy.o:	$(UPS_DIR)/inc/upserr.h
ups_copy.o:	$(UPS_DIR)/inc/upsugo.h
ups_copy.o:	$(UPS_DIR)/inc/upslst.h
ups_copy.o:	$(UPS_DIR)/inc/upsutl.h
ups_copy.o:	$(UPS_DIR)/inc/upstyp.h
ups_copy.o:	$(UPS_DIR)/inc/upslst.h
ups_copy.o:	$(UPS_DIR)/inc/upslst.h
ups_copy.o:	$(UPS_DIR)/inc/upsugo.h
ups_copy.o:	$(UPS_DIR)/inc/upsmat.h
ups_copy.o:	$(UPS_DIR)/inc/upsugo.h
ups_copy.o:	$(UPS_DIR)/inc/upstyp.h
ups_copy.o:	$(UPS_DIR)/inc/upsact.h
ups_copy.o:	$(UPS_DIR)/inc/upstyp.h
ups_copy.o:	$(UPS_DIR)/inc/upsugo.h
ups_copy.o:	$(UPS_DIR)/inc/upsmat.h
ups_copy.o:	$(UPS_DIR)/inc/upslst.h
ups_copy.o:	$(UPS_DIR)/inc/upstyp.h
ups_copy.o:	$(UPS_DIR)/inc/upsmem.h
ups_copy.o:	$(UPS_DIR)/inc/upslst.h
ups_copy.o:	$(UPS_DIR)/inc/upstyp.h
ups_copy.o:	$(UPS_DIR)/inc/upsfil.h
ups_copy.o:	$(UPS_DIR)/inc/upstyp.h
ups_copy.o:	$(UPS_DIR)/inc/ups_main.h
ups_copy.o:	$(UPS_DIR)/inc/ups_declare.h
ups_copy.o:	$(UPS_DIR)/inc/upsugo.h
ups_copy.o:	$(UPS_DIR)/inc/ups_copy.h
ups_copy.o:	$(UPS_DIR)/inc/upsugo.h
ups_create.o:	ups_create.c
ups_create.o:	$(UPS_DIR)/inc/upserr.h
ups_create.o:	$(UPS_DIR)/inc/upsugo.h
ups_create.o:	$(UPS_DIR)/inc/upslst.h
ups_create.o:	$(UPS_DIR)/inc/upsutl.h
ups_create.o:	$(UPS_DIR)/inc/upstyp.h
ups_create.o:	$(UPS_DIR)/inc/upslst.h
ups_create.o:	$(UPS_DIR)/inc/upslst.h
ups_create.o:	$(UPS_DIR)/inc/upsugo.h
ups_create.o:	$(UPS_DIR)/inc/upslst.h
ups_create.o:	$(UPS_DIR)/inc/upstyp.h
ups_create.o:	$(UPS_DIR)/inc/upsfil.h
ups_create.o:	$(UPS_DIR)/inc/upstyp.h
ups_declare.o:	ups_declare.c
ups_declare.o:	$(UPS_DIR)/inc/ups.h
ups_declare.o:	$(UPS_DIR)/inc/upsact.h
ups_declare.o:	$(UPS_DIR)/inc/upstyp.h
ups_declare.o:	$(UPS_DIR)/inc/upslst.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/upslst.h
ups_declare.o:	$(UPS_DIR)/inc/upsmat.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/upstyp.h
ups_declare.o:	$(UPS_DIR)/inc/upslst.h
ups_declare.o:	$(UPS_DIR)/inc/upstyp.h
ups_declare.o:	$(UPS_DIR)/inc/upserr.h
ups_declare.o:	$(UPS_DIR)/inc/upsfil.h
ups_declare.o:	$(UPS_DIR)/inc/upstyp.h
ups_declare.o:	$(UPS_DIR)/inc/upsget.h
ups_declare.o:	$(UPS_DIR)/inc/upshlp.h
ups_declare.o:	$(UPS_DIR)/inc/upskey.h
ups_declare.o:	$(UPS_DIR)/inc/upstyp.h
ups_declare.o:	$(UPS_DIR)/inc/upslst.h
ups_declare.o:	$(UPS_DIR)/inc/upsmat.h
ups_declare.o:	$(UPS_DIR)/inc/upsmem.h
ups_declare.o:	$(UPS_DIR)/inc/upstbl.h
ups_declare.o:	$(UPS_DIR)/inc/upstyp.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/upsutl.h
ups_declare.o:	$(UPS_DIR)/inc/upstyp.h
ups_declare.o:	$(UPS_DIR)/inc/upslst.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/upsver.h
ups_declare.o:	$(UPS_DIR)/inc/ups_list.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_configure.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_copy.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_create.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_declare.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_depend.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_get.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_list.h
ups_declare.o:	$(UPS_DIR)/inc/ups_main.h
ups_declare.o:	$(UPS_DIR)/inc/ups_modify.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_setup.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_start.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_stop.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_tailor.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_unconfigure.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_unk.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_unsetup.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_undeclare.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_declare.o:	$(UPS_DIR)/inc/ups_flavor.h
ups_declare.o:	$(UPS_DIR)/inc/upsugo.h
ups_depend.o:	ups_depend.c
ups_depend.o:	$(UPS_DIR)/inc/ups_depend.h
ups_depend.o:	$(UPS_DIR)/inc/upsugo.h
ups_depend.o:	$(UPS_DIR)/inc/upslst.h
ups_depend.o:	$(UPS_DIR)/inc/upserr.h
ups_depend.o:	$(UPS_DIR)/inc/upsugo.h
ups_depend.o:	$(UPS_DIR)/inc/upsact.h
ups_depend.o:	$(UPS_DIR)/inc/upstyp.h
ups_depend.o:	$(UPS_DIR)/inc/upslst.h
ups_depend.o:	$(UPS_DIR)/inc/upsugo.h
ups_depend.o:	$(UPS_DIR)/inc/upsmat.h
ups_depend.o:	$(UPS_DIR)/inc/upsugo.h
ups_depend.o:	$(UPS_DIR)/inc/upstyp.h
ups_depend.o:	$(UPS_DIR)/inc/upslst.h
ups_depend.o:	$(UPS_DIR)/inc/upstyp.h
ups_depend.o:	$(UPS_DIR)/inc/ups_main.h
ups_flavor.o:	ups_flavor.c
ups_flavor.o:	$(UPS_DIR)/inc/ups.h
ups_flavor.o:	$(UPS_DIR)/inc/upsact.h
ups_flavor.o:	$(UPS_DIR)/inc/upstyp.h
ups_flavor.o:	$(UPS_DIR)/inc/upslst.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/upslst.h
ups_flavor.o:	$(UPS_DIR)/inc/upsmat.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/upstyp.h
ups_flavor.o:	$(UPS_DIR)/inc/upslst.h
ups_flavor.o:	$(UPS_DIR)/inc/upstyp.h
ups_flavor.o:	$(UPS_DIR)/inc/upserr.h
ups_flavor.o:	$(UPS_DIR)/inc/upsfil.h
ups_flavor.o:	$(UPS_DIR)/inc/upstyp.h
ups_flavor.o:	$(UPS_DIR)/inc/upsget.h
ups_flavor.o:	$(UPS_DIR)/inc/upshlp.h
ups_flavor.o:	$(UPS_DIR)/inc/upskey.h
ups_flavor.o:	$(UPS_DIR)/inc/upstyp.h
ups_flavor.o:	$(UPS_DIR)/inc/upslst.h
ups_flavor.o:	$(UPS_DIR)/inc/upsmat.h
ups_flavor.o:	$(UPS_DIR)/inc/upsmem.h
ups_flavor.o:	$(UPS_DIR)/inc/upstbl.h
ups_flavor.o:	$(UPS_DIR)/inc/upstyp.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/upsutl.h
ups_flavor.o:	$(UPS_DIR)/inc/upstyp.h
ups_flavor.o:	$(UPS_DIR)/inc/upslst.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/upsver.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_list.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_configure.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_copy.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_create.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_declare.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_depend.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_get.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_list.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_main.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_modify.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_setup.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_start.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_stop.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_tailor.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_unconfigure.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_unk.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_unsetup.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_undeclare.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_flavor.o:	$(UPS_DIR)/inc/ups_flavor.h
ups_flavor.o:	$(UPS_DIR)/inc/upsugo.h
ups_get.o:	ups_get.c
ups_get.o:	$(UPS_DIR)/inc/upserr.h
ups_get.o:	$(UPS_DIR)/inc/upsugo.h
ups_get.o:	$(UPS_DIR)/inc/upslst.h
ups_get.o:	$(UPS_DIR)/inc/upsutl.h
ups_get.o:	$(UPS_DIR)/inc/upstyp.h
ups_get.o:	$(UPS_DIR)/inc/upslst.h
ups_get.o:	$(UPS_DIR)/inc/upslst.h
ups_get.o:	$(UPS_DIR)/inc/upsugo.h
ups_get.o:	$(UPS_DIR)/inc/upsmat.h
ups_get.o:	$(UPS_DIR)/inc/upsugo.h
ups_get.o:	$(UPS_DIR)/inc/upstyp.h
ups_get.o:	$(UPS_DIR)/inc/upsact.h
ups_get.o:	$(UPS_DIR)/inc/upstyp.h
ups_get.o:	$(UPS_DIR)/inc/upsugo.h
ups_get.o:	$(UPS_DIR)/inc/upsmat.h
ups_get.o:	$(UPS_DIR)/inc/upslst.h
ups_get.o:	$(UPS_DIR)/inc/upstyp.h
ups_get.o:	$(UPS_DIR)/inc/upsmem.h
ups_get.o:	$(UPS_DIR)/inc/upsget.h
ups_get.o:	$(UPS_DIR)/inc/ups_main.h
ups_list.o:	ups_list.c
ups_list.o:	$(UPS_DIR)/inc/ups.h
ups_list.o:	$(UPS_DIR)/inc/upsact.h
ups_list.o:	$(UPS_DIR)/inc/upstyp.h
ups_list.o:	$(UPS_DIR)/inc/upslst.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/upslst.h
ups_list.o:	$(UPS_DIR)/inc/upsmat.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/upstyp.h
ups_list.o:	$(UPS_DIR)/inc/upslst.h
ups_list.o:	$(UPS_DIR)/inc/upstyp.h
ups_list.o:	$(UPS_DIR)/inc/upserr.h
ups_list.o:	$(UPS_DIR)/inc/upsfil.h
ups_list.o:	$(UPS_DIR)/inc/upstyp.h
ups_list.o:	$(UPS_DIR)/inc/upsget.h
ups_list.o:	$(UPS_DIR)/inc/upshlp.h
ups_list.o:	$(UPS_DIR)/inc/upskey.h
ups_list.o:	$(UPS_DIR)/inc/upstyp.h
ups_list.o:	$(UPS_DIR)/inc/upslst.h
ups_list.o:	$(UPS_DIR)/inc/upsmat.h
ups_list.o:	$(UPS_DIR)/inc/upsmem.h
ups_list.o:	$(UPS_DIR)/inc/upstbl.h
ups_list.o:	$(UPS_DIR)/inc/upstyp.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/upsutl.h
ups_list.o:	$(UPS_DIR)/inc/upstyp.h
ups_list.o:	$(UPS_DIR)/inc/upslst.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/upsver.h
ups_list.o:	$(UPS_DIR)/inc/ups_list.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_configure.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_copy.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_create.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_declare.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_depend.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_get.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_list.h
ups_list.o:	$(UPS_DIR)/inc/ups_main.h
ups_list.o:	$(UPS_DIR)/inc/ups_modify.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_setup.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_start.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_stop.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_tailor.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_unconfigure.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_unk.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_unsetup.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_undeclare.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_list.o:	$(UPS_DIR)/inc/ups_flavor.h
ups_list.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	ups_main.c
ups_main.o:	$(UPS_DIR)/inc/ups_main.h
ups_main.o:	$(UPS_DIR)/inc/upslst.h
ups_main.o:	$(UPS_DIR)/inc/upstyp.h
ups_main.o:	$(UPS_DIR)/inc/upslst.h
ups_main.o:	$(UPS_DIR)/inc/ups_setup.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/upslst.h
ups_main.o:	$(UPS_DIR)/inc/ups_unsetup.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_start.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_stop.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_configure.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_unconfigure.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_list.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_create.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_tailor.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_unk.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_depend.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/upserr.h
ups_main.o:	$(UPS_DIR)/inc/upsutl.h
ups_main.o:	$(UPS_DIR)/inc/upstyp.h
ups_main.o:	$(UPS_DIR)/inc/upslst.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/upshlp.h
ups_main.o:	$(UPS_DIR)/inc/upsget.h
ups_main.o:	$(UPS_DIR)/inc/upsact.h
ups_main.o:	$(UPS_DIR)/inc/upstyp.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/upsmat.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/upstyp.h
ups_main.o:	$(UPS_DIR)/inc/upslst.h
ups_main.o:	$(UPS_DIR)/inc/upstyp.h
ups_main.o:	$(UPS_DIR)/inc/upsver.h
ups_main.o:	$(UPS_DIR)/inc/ups_declare.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_undeclare.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_flavor.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_get.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_copy.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_main.o:	$(UPS_DIR)/inc/ups_modify.h
ups_main.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	ups_modify.c
ups_modify.o:	$(UPS_DIR)/inc/ups.h
ups_modify.o:	$(UPS_DIR)/inc/upsact.h
ups_modify.o:	$(UPS_DIR)/inc/upstyp.h
ups_modify.o:	$(UPS_DIR)/inc/upslst.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/upslst.h
ups_modify.o:	$(UPS_DIR)/inc/upsmat.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/upstyp.h
ups_modify.o:	$(UPS_DIR)/inc/upslst.h
ups_modify.o:	$(UPS_DIR)/inc/upstyp.h
ups_modify.o:	$(UPS_DIR)/inc/upserr.h
ups_modify.o:	$(UPS_DIR)/inc/upsfil.h
ups_modify.o:	$(UPS_DIR)/inc/upstyp.h
ups_modify.o:	$(UPS_DIR)/inc/upsget.h
ups_modify.o:	$(UPS_DIR)/inc/upshlp.h
ups_modify.o:	$(UPS_DIR)/inc/upskey.h
ups_modify.o:	$(UPS_DIR)/inc/upstyp.h
ups_modify.o:	$(UPS_DIR)/inc/upslst.h
ups_modify.o:	$(UPS_DIR)/inc/upsmat.h
ups_modify.o:	$(UPS_DIR)/inc/upsmem.h
ups_modify.o:	$(UPS_DIR)/inc/upstbl.h
ups_modify.o:	$(UPS_DIR)/inc/upstyp.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/upsutl.h
ups_modify.o:	$(UPS_DIR)/inc/upstyp.h
ups_modify.o:	$(UPS_DIR)/inc/upslst.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/upsver.h
ups_modify.o:	$(UPS_DIR)/inc/ups_list.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_configure.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_copy.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_create.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_declare.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_depend.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_get.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_list.h
ups_modify.o:	$(UPS_DIR)/inc/ups_main.h
ups_modify.o:	$(UPS_DIR)/inc/ups_modify.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_setup.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_start.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_stop.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_tailor.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_unconfigure.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_unk.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_unsetup.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_undeclare.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_modify.o:	$(UPS_DIR)/inc/ups_flavor.h
ups_modify.o:	$(UPS_DIR)/inc/upsugo.h
ups_setup.o:	ups_setup.c
ups_setup.o:	$(UPS_DIR)/inc/upserr.h
ups_setup.o:	$(UPS_DIR)/inc/upsugo.h
ups_setup.o:	$(UPS_DIR)/inc/upslst.h
ups_setup.o:	$(UPS_DIR)/inc/upsutl.h
ups_setup.o:	$(UPS_DIR)/inc/upstyp.h
ups_setup.o:	$(UPS_DIR)/inc/upslst.h
ups_setup.o:	$(UPS_DIR)/inc/upslst.h
ups_setup.o:	$(UPS_DIR)/inc/upsugo.h
ups_setup.o:	$(UPS_DIR)/inc/upsmat.h
ups_setup.o:	$(UPS_DIR)/inc/upsugo.h
ups_setup.o:	$(UPS_DIR)/inc/upstyp.h
ups_setup.o:	$(UPS_DIR)/inc/upsact.h
ups_setup.o:	$(UPS_DIR)/inc/upstyp.h
ups_setup.o:	$(UPS_DIR)/inc/upsugo.h
ups_setup.o:	$(UPS_DIR)/inc/upsmat.h
ups_setup.o:	$(UPS_DIR)/inc/upslst.h
ups_setup.o:	$(UPS_DIR)/inc/upstyp.h
ups_setup.o:	$(UPS_DIR)/inc/ups_main.h
ups_start.o:	ups_start.c
ups_start.o:	$(UPS_DIR)/inc/upserr.h
ups_start.o:	$(UPS_DIR)/inc/upsugo.h
ups_start.o:	$(UPS_DIR)/inc/upslst.h
ups_start.o:	$(UPS_DIR)/inc/upsutl.h
ups_start.o:	$(UPS_DIR)/inc/upstyp.h
ups_start.o:	$(UPS_DIR)/inc/upslst.h
ups_start.o:	$(UPS_DIR)/inc/upslst.h
ups_start.o:	$(UPS_DIR)/inc/upsugo.h
ups_start.o:	$(UPS_DIR)/inc/upsmat.h
ups_start.o:	$(UPS_DIR)/inc/upsugo.h
ups_start.o:	$(UPS_DIR)/inc/upstyp.h
ups_start.o:	$(UPS_DIR)/inc/upsact.h
ups_start.o:	$(UPS_DIR)/inc/upstyp.h
ups_start.o:	$(UPS_DIR)/inc/upsugo.h
ups_start.o:	$(UPS_DIR)/inc/upsmat.h
ups_start.o:	$(UPS_DIR)/inc/upslst.h
ups_start.o:	$(UPS_DIR)/inc/upstyp.h
ups_start.o:	$(UPS_DIR)/inc/ups_main.h
ups_stop.o:	ups_stop.c
ups_stop.o:	$(UPS_DIR)/inc/upserr.h
ups_stop.o:	$(UPS_DIR)/inc/upsugo.h
ups_stop.o:	$(UPS_DIR)/inc/upslst.h
ups_stop.o:	$(UPS_DIR)/inc/upsutl.h
ups_stop.o:	$(UPS_DIR)/inc/upstyp.h
ups_stop.o:	$(UPS_DIR)/inc/upslst.h
ups_stop.o:	$(UPS_DIR)/inc/upslst.h
ups_stop.o:	$(UPS_DIR)/inc/upsugo.h
ups_stop.o:	$(UPS_DIR)/inc/upsmat.h
ups_stop.o:	$(UPS_DIR)/inc/upsugo.h
ups_stop.o:	$(UPS_DIR)/inc/upstyp.h
ups_stop.o:	$(UPS_DIR)/inc/upsact.h
ups_stop.o:	$(UPS_DIR)/inc/upstyp.h
ups_stop.o:	$(UPS_DIR)/inc/upsugo.h
ups_stop.o:	$(UPS_DIR)/inc/upsmat.h
ups_stop.o:	$(UPS_DIR)/inc/upslst.h
ups_stop.o:	$(UPS_DIR)/inc/upstyp.h
ups_stop.o:	$(UPS_DIR)/inc/ups_main.h
ups_tailor.o:	ups_tailor.c
ups_tailor.o:	$(UPS_DIR)/inc/upserr.h
ups_tailor.o:	$(UPS_DIR)/inc/upsfil.h
ups_tailor.o:	$(UPS_DIR)/inc/upstyp.h
ups_tailor.o:	$(UPS_DIR)/inc/upslst.h
ups_tailor.o:	$(UPS_DIR)/inc/upsugo.h
ups_tailor.o:	$(UPS_DIR)/inc/upslst.h
ups_tailor.o:	$(UPS_DIR)/inc/upsutl.h
ups_tailor.o:	$(UPS_DIR)/inc/upstyp.h
ups_tailor.o:	$(UPS_DIR)/inc/upslst.h
ups_tailor.o:	$(UPS_DIR)/inc/upsugo.h
ups_tailor.o:	$(UPS_DIR)/inc/upsmat.h
ups_tailor.o:	$(UPS_DIR)/inc/upsugo.h
ups_tailor.o:	$(UPS_DIR)/inc/upstyp.h
ups_tailor.o:	$(UPS_DIR)/inc/upsmem.h
ups_tailor.o:	$(UPS_DIR)/inc/upsact.h
ups_tailor.o:	$(UPS_DIR)/inc/upstyp.h
ups_tailor.o:	$(UPS_DIR)/inc/upsugo.h
ups_tailor.o:	$(UPS_DIR)/inc/upsmat.h
ups_tailor.o:	$(UPS_DIR)/inc/upslst.h
ups_tailor.o:	$(UPS_DIR)/inc/upstyp.h
ups_tailor.o:	$(UPS_DIR)/inc/ups_main.h
ups_unconfigure.o:	ups_unconfigure.c
ups_unconfigure.o:	$(UPS_DIR)/inc/upserr.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upsugo.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upslst.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upsutl.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upstyp.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upslst.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upslst.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upsugo.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upsmat.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upsugo.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upstyp.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upsact.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upstyp.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upsugo.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upsmat.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upslst.h
ups_unconfigure.o:	$(UPS_DIR)/inc/upstyp.h
ups_unconfigure.o:	$(UPS_DIR)/inc/ups_main.h
ups_undeclare.o:	ups_undeclare.c
ups_undeclare.o:	$(UPS_DIR)/inc/ups.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsact.h
ups_undeclare.o:	$(UPS_DIR)/inc/upstyp.h
ups_undeclare.o:	$(UPS_DIR)/inc/upslst.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/upslst.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsmat.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/upstyp.h
ups_undeclare.o:	$(UPS_DIR)/inc/upslst.h
ups_undeclare.o:	$(UPS_DIR)/inc/upstyp.h
ups_undeclare.o:	$(UPS_DIR)/inc/upserr.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsfil.h
ups_undeclare.o:	$(UPS_DIR)/inc/upstyp.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsget.h
ups_undeclare.o:	$(UPS_DIR)/inc/upshlp.h
ups_undeclare.o:	$(UPS_DIR)/inc/upskey.h
ups_undeclare.o:	$(UPS_DIR)/inc/upstyp.h
ups_undeclare.o:	$(UPS_DIR)/inc/upslst.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsmat.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsmem.h
ups_undeclare.o:	$(UPS_DIR)/inc/upstbl.h
ups_undeclare.o:	$(UPS_DIR)/inc/upstyp.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsutl.h
ups_undeclare.o:	$(UPS_DIR)/inc/upstyp.h
ups_undeclare.o:	$(UPS_DIR)/inc/upslst.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsver.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_list.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_configure.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_copy.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_create.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_declare.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_depend.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_get.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_list.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_main.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_modify.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_setup.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_start.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_stop.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_tailor.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_unconfigure.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_unk.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_unsetup.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_undeclare.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_undeclare.o:	$(UPS_DIR)/inc/ups_flavor.h
ups_undeclare.o:	$(UPS_DIR)/inc/upsugo.h
ups_unk.o:	ups_unk.c
ups_unk.o:	$(UPS_DIR)/inc/ups_unk.h
ups_unk.o:	$(UPS_DIR)/inc/upsugo.h
ups_unk.o:	$(UPS_DIR)/inc/upslst.h
ups_unk.o:	$(UPS_DIR)/inc/upslst.h
ups_unk.o:	$(UPS_DIR)/inc/upstyp.h
ups_unk.o:	$(UPS_DIR)/inc/upslst.h
ups_unk.o:	$(UPS_DIR)/inc/upserr.h
ups_unk.o:	$(UPS_DIR)/inc/upsutl.h
ups_unk.o:	$(UPS_DIR)/inc/upstyp.h
ups_unk.o:	$(UPS_DIR)/inc/upslst.h
ups_unk.o:	$(UPS_DIR)/inc/upsugo.h
ups_unk.o:	$(UPS_DIR)/inc/upsmat.h
ups_unk.o:	$(UPS_DIR)/inc/upsugo.h
ups_unk.o:	$(UPS_DIR)/inc/upstyp.h
ups_unk.o:	$(UPS_DIR)/inc/upsact.h
ups_unk.o:	$(UPS_DIR)/inc/upstyp.h
ups_unk.o:	$(UPS_DIR)/inc/upsugo.h
ups_unk.o:	$(UPS_DIR)/inc/upsmat.h
ups_unk.o:	$(UPS_DIR)/inc/upslst.h
ups_unk.o:	$(UPS_DIR)/inc/upstyp.h
ups_unsetup.o:	ups_unsetup.c
ups_unsetup.o:	$(UPS_DIR)/inc/upserr.h
ups_unsetup.o:	$(UPS_DIR)/inc/upsugo.h
ups_unsetup.o:	$(UPS_DIR)/inc/upslst.h
ups_unsetup.o:	$(UPS_DIR)/inc/upsutl.h
ups_unsetup.o:	$(UPS_DIR)/inc/upstyp.h
ups_unsetup.o:	$(UPS_DIR)/inc/upslst.h
ups_unsetup.o:	$(UPS_DIR)/inc/upslst.h
ups_unsetup.o:	$(UPS_DIR)/inc/upsugo.h
ups_unsetup.o:	$(UPS_DIR)/inc/upsmat.h
ups_unsetup.o:	$(UPS_DIR)/inc/upsugo.h
ups_unsetup.o:	$(UPS_DIR)/inc/upstyp.h
ups_unsetup.o:	$(UPS_DIR)/inc/upsact.h
ups_unsetup.o:	$(UPS_DIR)/inc/upstyp.h
ups_unsetup.o:	$(UPS_DIR)/inc/upsugo.h
ups_unsetup.o:	$(UPS_DIR)/inc/upsmat.h
ups_unsetup.o:	$(UPS_DIR)/inc/upslst.h
ups_unsetup.o:	$(UPS_DIR)/inc/upstyp.h
ups_unsetup.o:	$(UPS_DIR)/inc/ups_main.h
upsact.o:	upsact.c
upsact.o:	$(UPS_DIR)/inc/upsact.h
upsact.o:	$(UPS_DIR)/inc/upstyp.h
upsact.o:	$(UPS_DIR)/inc/upslst.h
upsact.o:	$(UPS_DIR)/inc/upsugo.h
upsact.o:	$(UPS_DIR)/inc/upslst.h
upsact.o:	$(UPS_DIR)/inc/upsmat.h
upsact.o:	$(UPS_DIR)/inc/upsugo.h
upsact.o:	$(UPS_DIR)/inc/upstyp.h
upsact.o:	$(UPS_DIR)/inc/upslst.h
upsact.o:	$(UPS_DIR)/inc/upstyp.h
upsact.o:	$(UPS_DIR)/inc/upserr.h
upsact.o:	$(UPS_DIR)/inc/upsver.h
upsact.o:	$(UPS_DIR)/inc/upslst.h
upsact.o:	$(UPS_DIR)/inc/upsutl.h
upsact.o:	$(UPS_DIR)/inc/upstyp.h
upsact.o:	$(UPS_DIR)/inc/upslst.h
upsact.o:	$(UPS_DIR)/inc/upsugo.h
upsact.o:	$(UPS_DIR)/inc/upsmem.h
upsact.o:	$(UPS_DIR)/inc/upskey.h
upsact.o:	$(UPS_DIR)/inc/upstyp.h
upsact.o:	$(UPS_DIR)/inc/upsugo.h
upsact.o:	$(UPS_DIR)/inc/upsget.h
upserr.o:	upserr.c
upserr.o:	$(UPS_DIR)/inc/upserr.h
upsfil.o:	upsfil.c
upsfil.o:	$(UPS_DIR)/inc/upsutl.h
upsfil.o:	$(UPS_DIR)/inc/upstyp.h
upsfil.o:	$(UPS_DIR)/inc/upslst.h
upsfil.o:	$(UPS_DIR)/inc/upslst.h
upsfil.o:	$(UPS_DIR)/inc/upsugo.h
upsfil.o:	$(UPS_DIR)/inc/upslst.h
upsfil.o:	$(UPS_DIR)/inc/upstyp.h
upsfil.o:	$(UPS_DIR)/inc/upslst.h
upsfil.o:	$(UPS_DIR)/inc/upstbl.h
upsfil.o:	$(UPS_DIR)/inc/upsmem.h
upsfil.o:	$(UPS_DIR)/inc/upserr.h
upsfil.o:	$(UPS_DIR)/inc/upskey.h
upsfil.o:	$(UPS_DIR)/inc/upstyp.h
upsfil.o:	$(UPS_DIR)/inc/upsfil.h
upsfil.o:	$(UPS_DIR)/inc/upstyp.h
upsfil.o:	$(UPS_DIR)/inc/upsver.h
upsget.o:	upsget.c
upsget.o:	$(UPS_DIR)/inc/ups.h
upsget.o:	$(UPS_DIR)/inc/upsact.h
upsget.o:	$(UPS_DIR)/inc/upstyp.h
upsget.o:	$(UPS_DIR)/inc/upslst.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/upslst.h
upsget.o:	$(UPS_DIR)/inc/upsmat.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/upstyp.h
upsget.o:	$(UPS_DIR)/inc/upslst.h
upsget.o:	$(UPS_DIR)/inc/upstyp.h
upsget.o:	$(UPS_DIR)/inc/upserr.h
upsget.o:	$(UPS_DIR)/inc/upsfil.h
upsget.o:	$(UPS_DIR)/inc/upstyp.h
upsget.o:	$(UPS_DIR)/inc/upsget.h
upsget.o:	$(UPS_DIR)/inc/upshlp.h
upsget.o:	$(UPS_DIR)/inc/upskey.h
upsget.o:	$(UPS_DIR)/inc/upstyp.h
upsget.o:	$(UPS_DIR)/inc/upslst.h
upsget.o:	$(UPS_DIR)/inc/upsmat.h
upsget.o:	$(UPS_DIR)/inc/upsmem.h
upsget.o:	$(UPS_DIR)/inc/upstbl.h
upsget.o:	$(UPS_DIR)/inc/upstyp.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/upsutl.h
upsget.o:	$(UPS_DIR)/inc/upstyp.h
upsget.o:	$(UPS_DIR)/inc/upslst.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/upsver.h
upsget.o:	$(UPS_DIR)/inc/ups_list.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_configure.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_copy.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_create.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_declare.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_depend.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_get.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_list.h
upsget.o:	$(UPS_DIR)/inc/ups_main.h
upsget.o:	$(UPS_DIR)/inc/ups_modify.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_setup.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_start.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_stop.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_tailor.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_unconfigure.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_unk.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_unsetup.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_undeclare.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upsget.o:	$(UPS_DIR)/inc/ups_flavor.h
upsget.o:	$(UPS_DIR)/inc/upsugo.h
upshlp.o:	upshlp.c
upshlp.o:	$(UPS_DIR)/inc/upstyp.h
upshlp.o:	$(UPS_DIR)/inc/upslst.h
upshlp.o:	$(UPS_DIR)/inc/upsutl.h
upshlp.o:	$(UPS_DIR)/inc/upstyp.h
upshlp.o:	$(UPS_DIR)/inc/upslst.h
upshlp.o:	$(UPS_DIR)/inc/upsugo.h
upshlp.o:	$(UPS_DIR)/inc/upslst.h
upskey.o:	upskey.c
upskey.o:	$(UPS_DIR)/inc/upstyp.h
upskey.o:	$(UPS_DIR)/inc/upslst.h
upskey.o:	$(UPS_DIR)/inc/upsmem.h
upskey.o:	$(UPS_DIR)/inc/upsutl.h
upskey.o:	$(UPS_DIR)/inc/upstyp.h
upskey.o:	$(UPS_DIR)/inc/upslst.h
upskey.o:	$(UPS_DIR)/inc/upsugo.h
upskey.o:	$(UPS_DIR)/inc/upslst.h
upskey.o:	$(UPS_DIR)/inc/upskey.h
upskey.o:	$(UPS_DIR)/inc/upstyp.h
upslst.o:	upslst.c
upslst.o:	$(UPS_DIR)/inc/upslst.h
upslst.o:	$(UPS_DIR)/inc/upsmem.h
upsmat.o:	upsmat.c
upsmat.o:	$(UPS_DIR)/inc/upsmat.h
upsmat.o:	$(UPS_DIR)/inc/upsugo.h
upsmat.o:	$(UPS_DIR)/inc/upslst.h
upsmat.o:	$(UPS_DIR)/inc/upstyp.h
upsmat.o:	$(UPS_DIR)/inc/upslst.h
upsmat.o:	$(UPS_DIR)/inc/upserr.h
upsmat.o:	$(UPS_DIR)/inc/upslst.h
upsmat.o:	$(UPS_DIR)/inc/upstyp.h
upsmat.o:	$(UPS_DIR)/inc/upsmem.h
upsmat.o:	$(UPS_DIR)/inc/upsfil.h
upsmat.o:	$(UPS_DIR)/inc/upstyp.h
upsmat.o:	$(UPS_DIR)/inc/upsutl.h
upsmat.o:	$(UPS_DIR)/inc/upstyp.h
upsmat.o:	$(UPS_DIR)/inc/upslst.h
upsmat.o:	$(UPS_DIR)/inc/upsugo.h
upsmat.o:	$(UPS_DIR)/inc/upsget.h
upsmat.o:	$(UPS_DIR)/inc/upsver.h
upsmem.o:	upsmem.c
upsmem.o:	$(UPS_DIR)/inc/upserr.h
upsmem.o:	$(UPS_DIR)/inc/upsmem.h
upstbl.o:	upstbl.c
upstbl.o:	$(UPS_DIR)/inc/upstbl.h
upstyp.o:	upstyp.c
upstyp.o:	$(UPS_DIR)/inc/upsutl.h
upstyp.o:	$(UPS_DIR)/inc/upstyp.h
upstyp.o:	$(UPS_DIR)/inc/upslst.h
upstyp.o:	$(UPS_DIR)/inc/upslst.h
upstyp.o:	$(UPS_DIR)/inc/upsugo.h
upstyp.o:	$(UPS_DIR)/inc/upslst.h
upstyp.o:	$(UPS_DIR)/inc/upstyp.h
upstyp.o:	$(UPS_DIR)/inc/upsmem.h
upstyp.o:	$(UPS_DIR)/inc/upslst.h
upstyp.o:	$(UPS_DIR)/inc/upserr.h
upsugo.o:	upsugo.c
upsugo.o:	$(UPS_DIR)/inc/upstyp.h
upsugo.o:	$(UPS_DIR)/inc/upslst.h
upsugo.o:	$(UPS_DIR)/inc/upsugo.h
upsugo.o:	$(UPS_DIR)/inc/upslst.h
upsugo.o:	$(UPS_DIR)/inc/upserr.h
upsugo.o:	$(UPS_DIR)/inc/upsmem.h
upsugo.o:	$(UPS_DIR)/inc/upsutl.h
upsugo.o:	$(UPS_DIR)/inc/upstyp.h
upsugo.o:	$(UPS_DIR)/inc/upslst.h
upsugo.o:	$(UPS_DIR)/inc/upsugo.h
upsutl.o:	upsutl.c
upsutl.o:	$(UPS_DIR)/inc/upsutl.h
upsutl.o:	$(UPS_DIR)/inc/upstyp.h
upsutl.o:	$(UPS_DIR)/inc/upslst.h
upsutl.o:	$(UPS_DIR)/inc/upslst.h
upsutl.o:	$(UPS_DIR)/inc/upsugo.h
upsutl.o:	$(UPS_DIR)/inc/upslst.h
upsutl.o:	$(UPS_DIR)/inc/upserr.h
upsutl.o:	$(UPS_DIR)/inc/upstyp.h
upsutl.o:	$(UPS_DIR)/inc/upsmem.h
upsutl.o:	$(UPS_DIR)/inc/upsfil.h
upsutl.o:	$(UPS_DIR)/inc/upstyp.h
upsutl.o:	$(UPS_DIR)/inc/upsget.h
upsver.o:	upsver.c
upsver.o:	$(UPS_DIR)/inc/upserr.h
upsver.o:	$(UPS_DIR)/inc/upsver.h
