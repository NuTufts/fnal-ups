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
	$(CC) -c $(CCCHK) $(CFLAGS) $*.c

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
OBJECTS = ups_error.o ups_list.o ups_memory.o ups_utils.o ups_types.c

CIAOBJECTS = ups_error.a  ups_list.a ups_memory.o ups_utils.a ups_types.a

LDFLAGS = $(LLIBS)

#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all : library

tag_file : 
	$(EMACS_DIR)/bin/etags -t *.c ../inc/*.h

library : $(LIB)/libups.a

cia :	$(CIAOBJECTS)
	cia $(CIAOBJECTS)

#
# Compile UPS library source
#
$(LIB)/libups.a : $(OBJECTS)
	@echo "=== UPS BUILD : Building Unix library (libups.a) ==="
	$(AR) $(ARFLAGS) $(LIB)/libups.a $?
	- if [ "$(RANLIB)" != "" -a -x "$(RANLIB)" ]; then \
		$(RANLIB) $(LIB)/libups.a; \
	fi
	@echo "=== UPS BUILD : Unix library (libups.a) built ==="
	@echo

#
# Main programs.
#

$(BIN)/Setup :  setupMain.c $(LIB)/libups.a
	$(CC) -o $@ $(CFLAGS) $(CCCHK) setupMain.c $(LDFLAGS)

$(BIN)/Unsetup :  unsetupMain.c $(LIB)/libups.a
	$(CC) -o $@ $(CFLAGS) $(CCCHK) unsetupMain.c $(LDFLAGS)

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
ups_error.o:	ups_error.c
ups_error.o:	$(ERUPT_DIR)/inc/ups_error.h
ups_list.o:	ups_list.c
ups_list.o:	./../inc/ups_list.h
ups_match.o:	ups_match.c
ups_match.o:	$(ERUPT_DIR)/inc/ups_error.h
ups_memory.o:	ups_memory.c
ups_memory.o:	$(ERUPT_DIR)/inc/ups_error.h
ups_memory.o:	$(ERUPT_DIR)/inc/ups_list.h
ups_memory.o:	$(ERUPT_DIR)/inc/ups_memory.h
ups_types.o:    $(ERUPT_DIR)/inc/ups_types.h
ups_types.o:    $(ERUPT_DIR)/inc/ups_list.h
ups_types.o:    $(ERUPT_DIR)/inc/ups_memory.h
ups_utils.o:	ups_utils.c
ups_utils.o:	$(ERUPT_DIR)/inc/ups_utils.h
ups_utils.o:	$(ERUPT_DIR)/inc/ups_error.h
upsugo_utils.o:	upsugo_utils.c
upsugo_utils.o:	./../inc/ups_types.h
upsugo_utils.o:	./../inc/ups_list.h
