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
	  upsver.o upsact.o upsget.o

COMMAND_OBJECTS = ups_list.o ups_setup.o ups_start.o ups_stop.o \
	          ups_configure.o ups_unconfigure.o \
	          ups_unk.o

CIAOBJECTS = upserr.A upslst.A upsmem.A upsutl.A upstyp.A \
	     upsfil.A upsugo.A upsmat.A ups_list.A  \
	     upskey.A upshlp.A ups_unk.A ups_setup.A \
	     upsver.A upsact.A upsget.A ups_start.A \
	     ups_stop.A ups_configure.A ups_unconfigure.A

LDFLAGS = $(LLIBS)

#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all : library main

tag_file : 
	$(EMACS_DIR)/bin/etags -t *.c ../inc/*.h

main : $(BIN)/ups

library : $(LIB)/libups.a

cia :	$(CIAOBJECTS)
	cia $(CIAOBJECTS)

#
# Build main line executable
#
$(BIN)/ups : ups_main.o library
	$(CC) ups_main.o -o $@  $(CFLAGS) $(LDFLAGS)

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
ups_configure.o:	$(ERUPT_DIR)/inc/upserr.h
ups_configure.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_configure.o:	$(ERUPT_DIR)/inc/upslst.h
ups_configure.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_configure.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_configure.o:	$(ERUPT_DIR)/inc/upslst.h
ups_configure.o:	$(ERUPT_DIR)/inc/upslst.h
ups_configure.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_configure.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_configure.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_configure.o:	$(ERUPT_DIR)/inc/upsact.h
ups_configure.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_configure.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_configure.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_configure.o:	$(ERUPT_DIR)/inc/upslst.h
ups_configure.o:	$(ERUPT_DIR)/inc/ups_main.h
ups_exist.o:	ups_exist.c
ups_exist.o:	$(ERUPT_DIR)/inc/upserr.h
ups_exist.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_exist.o:	$(ERUPT_DIR)/inc/upslst.h
ups_exist.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_exist.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_exist.o:	$(ERUPT_DIR)/inc/upslst.h
ups_exist.o:	$(ERUPT_DIR)/inc/upslst.h
ups_exist.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_exist.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_exist.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_exist.o:	$(ERUPT_DIR)/inc/upsact.h
ups_exist.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_exist.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_exist.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_exist.o:	$(ERUPT_DIR)/inc/upslst.h
ups_exist.o:	$(ERUPT_DIR)/inc/ups_main.h
ups_list.o:	ups_list.c
ups_list.o:	$(ERUPT_DIR)/inc/ups.h
ups_list.o:	$(ERUPT_DIR)/inc/upserr.h
ups_list.o:	$(ERUPT_DIR)/inc/upsfil.h
ups_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_list.o:	$(ERUPT_DIR)/inc/upslst.h
ups_list.o:	$(ERUPT_DIR)/inc/upskey.h
ups_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_list.o:	$(ERUPT_DIR)/inc/upslst.h
ups_list.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_list.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_list.o:	$(ERUPT_DIR)/inc/upslst.h
ups_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_list.o:	$(ERUPT_DIR)/inc/upsmem.h
ups_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_list.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_list.o:	$(ERUPT_DIR)/inc/upslst.h
ups_list.o:	$(ERUPT_DIR)/inc/upsver.h
ups_list.o:	$(ERUPT_DIR)/inc/upsget.h
ups_list.o:	$(ERUPT_DIR)/inc/ups_list.h
ups_list.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_main.o:	ups_main.c
ups_main.o:	$(ERUPT_DIR)/inc/ups_main.h
ups_main.o:	$(ERUPT_DIR)/inc/upslst.h
ups_main.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_main.o:	$(ERUPT_DIR)/inc/upslst.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_setup.h
ups_main.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_main.o:	$(ERUPT_DIR)/inc/upslst.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_start.h
ups_main.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_stop.h
ups_main.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_configure.h
ups_main.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_unconfigure.h
ups_main.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_list.h
ups_main.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_unk.h
ups_main.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_main.o:	$(ERUPT_DIR)/inc/upserr.h
ups_main.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_main.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_main.o:	$(ERUPT_DIR)/inc/upslst.h
ups_main.o:	$(ERUPT_DIR)/inc/upshlp.h
ups_setup.o:	ups_setup.c
ups_setup.o:	$(ERUPT_DIR)/inc/upserr.h
ups_setup.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_setup.o:	$(ERUPT_DIR)/inc/upslst.h
ups_setup.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_setup.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_setup.o:	$(ERUPT_DIR)/inc/upslst.h
ups_setup.o:	$(ERUPT_DIR)/inc/upslst.h
ups_setup.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_setup.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_setup.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_setup.o:	$(ERUPT_DIR)/inc/upsact.h
ups_setup.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_setup.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_setup.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_setup.o:	$(ERUPT_DIR)/inc/upslst.h
ups_setup.o:	$(ERUPT_DIR)/inc/ups_main.h
ups_start.o:	ups_start.c
ups_start.o:	$(ERUPT_DIR)/inc/upserr.h
ups_start.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_start.o:	$(ERUPT_DIR)/inc/upslst.h
ups_start.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_start.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_start.o:	$(ERUPT_DIR)/inc/upslst.h
ups_start.o:	$(ERUPT_DIR)/inc/upslst.h
ups_start.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_start.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_start.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_start.o:	$(ERUPT_DIR)/inc/upsact.h
ups_start.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_start.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_start.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_start.o:	$(ERUPT_DIR)/inc/upslst.h
ups_start.o:	$(ERUPT_DIR)/inc/ups_main.h
ups_stop.o:	ups_stop.c
ups_stop.o:	$(ERUPT_DIR)/inc/upserr.h
ups_stop.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_stop.o:	$(ERUPT_DIR)/inc/upslst.h
ups_stop.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_stop.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_stop.o:	$(ERUPT_DIR)/inc/upslst.h
ups_stop.o:	$(ERUPT_DIR)/inc/upslst.h
ups_stop.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_stop.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_stop.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_stop.o:	$(ERUPT_DIR)/inc/upsact.h
ups_stop.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_stop.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_stop.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_stop.o:	$(ERUPT_DIR)/inc/upslst.h
ups_stop.o:	$(ERUPT_DIR)/inc/ups_main.h
ups_unconfigure.o:	ups_unconfigure.c
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upserr.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upslst.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upslst.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upslst.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upsact.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/upslst.h
ups_unconfigure.o:	$(ERUPT_DIR)/inc/ups_main.h
ups_unk.o:	ups_unk.c
ups_unk.o:	$(ERUPT_DIR)/inc/ups_unk.h
ups_unk.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_unk.o:	$(ERUPT_DIR)/inc/upslst.h
ups_unsetup.o:	ups_unsetup.c
ups_unsetup.o:	$(ERUPT_DIR)/inc/upserr.h
ups_unsetup.o:	$(ERUPT_DIR)/inc/upsugo.h
ups_unsetup.o:	$(ERUPT_DIR)/inc/upslst.h
upsact.o:	upsact.c
upsact.o:	$(ERUPT_DIR)/inc/upsact.h
upsact.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact.o:	$(ERUPT_DIR)/inc/upslst.h
upsact.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact.o:	$(ERUPT_DIR)/inc/upslst.h
upsact.o:	$(ERUPT_DIR)/inc/upsmat.h
upsact.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact.o:	$(ERUPT_DIR)/inc/upslst.h
upsact.o:	$(ERUPT_DIR)/inc/upserr.h
upsact.o:	$(ERUPT_DIR)/inc/upslst.h
upsact.o:	$(ERUPT_DIR)/inc/upsutl.h
upsact.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact.o:	$(ERUPT_DIR)/inc/upslst.h
upsact.o:	$(ERUPT_DIR)/inc/upsmem.h
upsact.o:	$(ERUPT_DIR)/inc/upskey.h
upsact.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact.o:	$(ERUPT_DIR)/inc/upsget.h
upsact.o:	$(ERUPT_DIR)/inc/ups_main.h
upsact_premerge.o:	upsact_premerge.c
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsact.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upslst.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upslst.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsmat.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upslst.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upserr.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upslst.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsutl.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upslst.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsmem.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upskey.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsget.h
upsact_premerge.o:	./ups_main.c
upsact_premerge.o:	$(ERUPT_DIR)/inc/ups_main.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upslst.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upstyp.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/ups_setup.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/ups_start.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/ups_stop.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/ups_configure.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/ups_unconfigure.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/ups_list.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/ups_unk.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsugo.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upserr.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upsutl.h
upsact_premerge.o:	$(ERUPT_DIR)/inc/upshlp.h
upserr.o:	upserr.c
upserr.o:	$(ERUPT_DIR)/inc/upserr.h
upsfil.o:	upsfil.c
upsfil.o:	$(ERUPT_DIR)/inc/upsutl.h
upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
upsfil.o:	$(ERUPT_DIR)/inc/upslst.h
upsfil.o:	$(ERUPT_DIR)/inc/upslst.h
upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
upsfil.o:	$(ERUPT_DIR)/inc/upslst.h
upsfil.o:	$(ERUPT_DIR)/inc/upsmem.h
upsfil.o:	$(ERUPT_DIR)/inc/upserr.h
upsfil.o:	$(ERUPT_DIR)/inc/upskey.h
upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
upsfil.o:	$(ERUPT_DIR)/inc/upsfil.h
upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
upsget.o:	upsget.c
upsget.o:	$(ERUPT_DIR)/inc/ups.h
upsget.o:	$(ERUPT_DIR)/inc/upserr.h
upsget.o:	$(ERUPT_DIR)/inc/upsfil.h
upsget.o:	$(ERUPT_DIR)/inc/upstyp.h
upsget.o:	$(ERUPT_DIR)/inc/upslst.h
upsget.o:	$(ERUPT_DIR)/inc/upskey.h
upsget.o:	$(ERUPT_DIR)/inc/upstyp.h
upsget.o:	$(ERUPT_DIR)/inc/upslst.h
upsget.o:	$(ERUPT_DIR)/inc/upsmat.h
upsget.o:	$(ERUPT_DIR)/inc/upsugo.h
upsget.o:	$(ERUPT_DIR)/inc/upslst.h
upsget.o:	$(ERUPT_DIR)/inc/upstyp.h
upsget.o:	$(ERUPT_DIR)/inc/upsmem.h
upsget.o:	$(ERUPT_DIR)/inc/upstyp.h
upsget.o:	$(ERUPT_DIR)/inc/upsutl.h
upsget.o:	$(ERUPT_DIR)/inc/upstyp.h
upsget.o:	$(ERUPT_DIR)/inc/upslst.h
upsget.o:	$(ERUPT_DIR)/inc/upsver.h
upsget.o:	$(ERUPT_DIR)/inc/upsget.h
upsget.o:	$(ERUPT_DIR)/inc/ups_list.h
upsget.o:	$(ERUPT_DIR)/inc/upsugo.h
upshlp.o:	upshlp.c
upshlp.o:	$(ERUPT_DIR)/inc/upstyp.h
upshlp.o:	$(ERUPT_DIR)/inc/upslst.h
upshlp.o:	$(ERUPT_DIR)/inc/upsutl.h
upshlp.o:	$(ERUPT_DIR)/inc/upstyp.h
upshlp.o:	$(ERUPT_DIR)/inc/upslst.h
upskey.o:	upskey.c
upskey.o:	$(ERUPT_DIR)/inc/upstyp.h
upskey.o:	$(ERUPT_DIR)/inc/upslst.h
upskey.o:	$(ERUPT_DIR)/inc/upsmem.h
upskey.o:	$(ERUPT_DIR)/inc/upsutl.h
upskey.o:	$(ERUPT_DIR)/inc/upstyp.h
upskey.o:	$(ERUPT_DIR)/inc/upslst.h
upskey.o:	$(ERUPT_DIR)/inc/upskey.h
upskey.o:	$(ERUPT_DIR)/inc/upstyp.h
upslst.o:	upslst.c
upslst.o:	$(ERUPT_DIR)/inc/upslst.h
upslst.o:	$(ERUPT_DIR)/inc/upsmem.h
upsmat.o:	upsmat.c
upsmat.o:	$(ERUPT_DIR)/inc/upsmat.h
upsmat.o:	$(ERUPT_DIR)/inc/upsugo.h
upsmat.o:	$(ERUPT_DIR)/inc/upslst.h
upsmat.o:	$(ERUPT_DIR)/inc/upstyp.h
upsmat.o:	$(ERUPT_DIR)/inc/upslst.h
upsmat.o:	$(ERUPT_DIR)/inc/upserr.h
upsmat.o:	$(ERUPT_DIR)/inc/upslst.h
upsmat.o:	$(ERUPT_DIR)/inc/upstyp.h
upsmat.o:	$(ERUPT_DIR)/inc/upsmem.h
upsmat.o:	$(ERUPT_DIR)/inc/upsfil.h
upsmat.o:	$(ERUPT_DIR)/inc/upstyp.h
upsmat.o:	$(ERUPT_DIR)/inc/upsutl.h
upsmat.o:	$(ERUPT_DIR)/inc/upstyp.h
upsmat.o:	$(ERUPT_DIR)/inc/upslst.h
upsmem.o:	upsmem.c
upsmem.o:	$(ERUPT_DIR)/inc/upserr.h
upsmem.o:	$(ERUPT_DIR)/inc/upsmem.h
upstyp.o:	upstyp.c
upstyp.o:	$(ERUPT_DIR)/inc/upsutl.h
upstyp.o:	$(ERUPT_DIR)/inc/upstyp.h
upstyp.o:	$(ERUPT_DIR)/inc/upslst.h
upstyp.o:	$(ERUPT_DIR)/inc/upslst.h
upstyp.o:	$(ERUPT_DIR)/inc/upstyp.h
upstyp.o:	$(ERUPT_DIR)/inc/upsmem.h
upstyp.o:	$(ERUPT_DIR)/inc/upslst.h
upstyp.o:	$(ERUPT_DIR)/inc/upserr.h
upsugo.o:	upsugo.c
upsugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upsugo.o:	$(ERUPT_DIR)/inc/upslst.h
upsugo.o:	$(ERUPT_DIR)/inc/upsugo.h
upsugo.o:	$(ERUPT_DIR)/inc/upslst.h
upsugo.o:	$(ERUPT_DIR)/inc/upserr.h
upsugo.o:	$(ERUPT_DIR)/inc/upsmem.h
upsugo.o:	$(ERUPT_DIR)/inc/upsutl.h
upsugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upsugo.o:	$(ERUPT_DIR)/inc/upslst.h
upsutl.o:	upsutl.c
upsutl.o:	$(ERUPT_DIR)/inc/upsutl.h
upsutl.o:	$(ERUPT_DIR)/inc/upstyp.h
upsutl.o:	$(ERUPT_DIR)/inc/upslst.h
upsutl.o:	$(ERUPT_DIR)/inc/upslst.h
upsutl.o:	$(ERUPT_DIR)/inc/upserr.h
upsutl.o:	$(ERUPT_DIR)/inc/upstyp.h
upsutl.o:	$(ERUPT_DIR)/inc/upsmem.h
upsutl.o:	$(ERUPT_DIR)/inc/upsfil.h
upsutl.o:	$(ERUPT_DIR)/inc/upstyp.h
upsver.o:	upsver.c
upsver.o:	$(ERUPT_DIR)/inc/upserr.h
upsver.o:	$(ERUPT_DIR)/inc/upsver.h
