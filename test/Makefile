SHELL = /bin/sh
# 
#  1) This file is designed to be invoked by a shell script called premake
#  which selects supported options.
#
#  2) This file re-builds the ups tests.
#
#  3) Environment:
# Environmental variables -
#
# Expected variables on the command line:
# CC=        The name of the appropriate compiler, with suitable flags
# CCCHK=     A set of compiler flags which check ANSI C strictly
#

.c.o :
	$(CC) -c $(CCCHK) $(CFLAGS) $*.c

#
SRC	= ../src
INC	= ../inc
LIB	= ../lib
BIN	= ../bin
DOC	= ../doc

CFLAGS	  = $(UPS_CFLAGS) $(INCDIRS)

#
#
OBJECTS = 

LDFLAGS = $(LLIBS)

#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all : test_ups_error test_ups_list test_ups_memory test_ups_utils

#
# Main programs.
#

test_ups_error : test_ups_error.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

test_ups_list : test_ups_list.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

test_ups_memory : test_ups_memory.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

test_ups_utils : test_ups_utils.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

#
# Now standard targets
#
clean :
	@ upsClean
	@ rm -f test_ups_error test_ups_memory
	@ rm -f test_ups_list test_ups_utils

install :
	@if [ "$(UPS_INSTALL_DIR)" = "" ]; then \
		echo You have not specified a destination directory >&2; \
		exit 1; \
	fi
	@ rm -rf $(UPS_INSTALL_DIR)/test
	@ mkdir $(UPS_INSTALL_DIR)/test
	cp -r Makefile $(UPS_INSTALL_DIR)/test
	cp -r * $(UPS_INSTALL_DIR)/test
	@( \
		cd $(UPS_INSTALL_DIR)/test ;\
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
#START_DEPEND
test_ups_error.o:	test_ups_error.c
test_ups_error.o:	$(ERUPT_DIR)/inc/ups_error.h
test_ups_list.o:	test_ups_list.c
test_ups_list.o:	$(ERUPT_DIR)/inc/ups_list.h
test_ups_memory.o:	test_ups_memory.c
test_ups_memory.o:	$(ERUPT_DIR)/inc/ups_memory.h
test_ups_memory.o:	$(ERUPT_DIR)/inc/ups_utils.h
test_ups_memory.o:	$(ERUPT_DIR)/inc/ups_types.h
test_ups_memory.o:	$(ERUPT_DIR)/inc/ups_list.h
test_ups_memory.o:	$(ERUPT_DIR)/inc/ups_error.h
test_ups_utils.o:	test_ups_utils.c
test_ups_utils.o:	$(ERUPT_DIR)/inc/ups_utils.h
test_ups_utils.o:	$(ERUPT_DIR)/inc/ups_types.h
test_ups_utils.o:	$(ERUPT_DIR)/inc/ups_list.h
test_ups_utils.o:	$(ERUPT_DIR)/inc/ups_error.h
test_upsugo_utils.o:	test_upsugo_utils.c
test_upsugo_utils.o:	./../inc/ups_types.h
test_upsugo_utils.o:	./../inc/ups_list.h
