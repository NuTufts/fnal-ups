SHELL = /bin/sh
# 
#  1) This file is designed to be invoked by a shell script called premake
#  which selects supported options.
#
#  2) This file re-builds the erupt.
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

CFLAGS	  = $(ERUPT_CFLAGS) $(INCDIRS)
CFLAGScia = $(INCDIRS)

#
#
OBJECTS = 

CIAOBJECTS = 

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

library : $(LIB)/liberupt.a

cia :	$(CIAOBJECTS)
	cia $(CIAOBJECTS)

#
# Compile ERUPT library source
#
$(LIB)/liberupt.a : $(OBJECTS)
	@echo "=== ERUPT BUILD : Building Unix library (liberupt.a) ==="
	$(AR) $(ARFLAGS) $(LIB)/liberupt.a $?
	- if [ "$(RANLIB)" != "" -a -x "$(RANLIB)" ]; then \
		$(RANLIB) $(LIB)/liberupt.a; \
	fi
	@echo "=== ERUPT BUILD : Unix library (liberupt.a) built ==="
	@echo

#
# Main programs.
#

$(BIN)/erSetup :  setupMain.c $(LIB)/liberupt.a
	$(CC) -o $@ $(CFLAGS) $(CCCHK) setupMain.c $(LDFLAGS)

$(BIN)/erUnsetup :  unsetupMain.c $(LIB)/liberupt.a
	$(CC) -o $@ $(CFLAGS) $(CCCHK) unsetupMain.c $(LDFLAGS)

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
	@ rm -rf $(ERUPT_INSTALL_DIR)/src
	@ mkdir $(ERUPT_INSTALL_DIR)/src
	cp -r Makefile $(ERUPT_INSTALL_DIR)/src
	cp -r *.c $(ERUPT_INSTALL_DIR)/src
	@chmod 644 $(ERUPT_INSTALL_DIR)/src/*.c
	@( \
		cd $(ERUPT_INSTALL_DIR)/src ;\
		find . \( -type d -name CVS -prune -o \
			-name Makefile -name \*~ \) -exec rm -rf {} \; \
	)
