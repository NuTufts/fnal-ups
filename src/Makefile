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
OBJECTS = ups_error.o upslst.o ups_memory.o upsutl.o upstyp.o \
	  upsfil.o upsugo.o ups_match.o upskey.o \
	  ups_help.o

COMMAND_OBJECTS = ups_c_list.o

CIAOBJECTS = ups_error.a  upslst.a ups_memory.o upsutl.a upstyp.a \
	     upsfil.a upsugo.a ups_match.a ups_c_list.a \
             upskey.a ups_help.a

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
	$(CC) -o $@ $(CCCHK) $(CFLAGS) ups_main.o $(LDFLAGS)

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
clean :
	@ upsClean

install :
	@if [ "$(UPS_INSTALL_DIR)" = "" ]; then \
		echo You have not specified a destination directory >&2; \
		exit 1; \
	fi
	@ rm -rf $(UPS_INSTALL_DIR)/src
	@ mkdir $(UPS_INSTALL_DIR)/src
	cp -r Makefile $(UPS_INSTALL_DIR)/src
	cp -r *.c $(UPS_INSTALL_DIR)/src
	@chmod 644 $(UPS_INSTALL_DIR)/src/*.c
	@( \
		cd $(UPS_INSTALL_DIR)/src ;\
		find . \( -type d -name CVS -prune -o \
			-name Makefile -name \*~ \) -exec rm -rf {} \; \
	)

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
ups_c_list.o:	ups_c_list.c
ups_c_list.o:	$(ERUPT_DIR)/inc/upslst.h
ups_c_list.o:	$(ERUPT_DIR)/inc/ups_memory.h
ups_c_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_c_list.o:	$(ERUPT_DIR)/inc/upslst.h
ups_c_list.o:	$(ERUPT_DIR)/inc/ups_error.h
ups_c_list.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_c_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_c_list.o:	$(ERUPT_DIR)/inc/upslst.h
ups_c_list.o:	$(ERUPT_DIR)/inc/ups_match.h
ups_c_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_c_list.o:	$(ERUPT_DIR)/inc/upsfil.h
ups_c_list.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_c_list.o:	$(ERUPT_DIR)/inc/ups_commands.h
ups_error.o:	ups_error.c
ups_error.o:	$(ERUPT_DIR)/inc/ups_error.h
upsfil.o:	upsfil.c
upsfil.o:	$(ERUPT_DIR)/inc/upsutl.h
upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
upsfil.o:	$(ERUPT_DIR)/inc/upslst.h
upsfil.o:	$(ERUPT_DIR)/inc/upslst.h
upsfil.o:	$(ERUPT_DIR)/inc/upsfil.h
upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
upsfil.o:	$(ERUPT_DIR)/inc/upslst.h
upsfil.o:	$(ERUPT_DIR)/inc/upsmem.h
upsfil.o:	$(ERUPT_DIR)/inc/upserr.h
upskey.o:     upskey.c
upskey.o:     $(ERUPT_DIR)/inc/upskey.h
upskey.o:     $(ERUPT_DIR)/inc/upstyp.h
ups_key.o:     $(ERUPT_DIR)/inc/upsmem.h
upslst.o:	upslst.c
upslst.o:	$(ERUPT_DIR)/inc/upslst.h
upslst.o:	$(ERUPT_DIR)/inc/upsmem.h
ups_main.o:	ups_main.c
ups_main.o:	$(ERUPT_DIR)/inc/upslst.h
ups_main.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_main.o:	$(ERUPT_DIR)/inc/upslst.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_commands.h
ups_main.o:	$(ERUPT_DIR)/inc/ups_error.h
ups_match.o:	ups_match.c
ups_match.o:	$(ERUPT_DIR)/inc/ups_match.h
ups_match.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_match.o:	$(ERUPT_DIR)/inc/upslst.h
ups_match.o:	$(ERUPT_DIR)/inc/ups_error.h
ups_match.o:	$(ERUPT_DIR)/inc/upslst.h
ups_match.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_match.o:	$(ERUPT_DIR)/inc/ups_memory.h
ups_match.o:	$(ERUPT_DIR)/inc/upsfil.h
ups_match.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_match.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_match.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_match.o:	$(ERUPT_DIR)/inc/upslst.h
ups_memory.o:	ups_memory.c
ups_memory.o:	$(ERUPT_DIR)/inc/ups_error.h
ups_memory.o:	$(ERUPT_DIR)/inc/ups_memory.h
upstyp.o:	upstyp.c
upstyp.o:	$(ERUPT_DIR)/inc/upstyp.h
upstyp.o:	$(ERUPT_DIR)/inc/upslst.h
upstyp.o:	$(ERUPT_DIR)/inc/upsmem.h
upstyp.o:	$(ERUPT_DIR)/inc/upslst.h
upstyp.o:	$(ERUPT_DIR)/inc/upserr.h
upsugo.o:	upsugo.c
upsugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upsugo.o:	$(ERUPT_DIR)/inc/upslst.h
upsugo.o:	$(ERUPT_DIR)/inc/ups_error.h
upsugo.o:	$(ERUPT_DIR)/inc/ups_memory.h
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
