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
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $*.c

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
UPS_TEST_OBJECTS = upstst_cmdline.o upstst_cmdloop.o upstst_cmdproc.o \
	upstst_misc.o upstst_parse.o upstst_error.o upstst_files.o
#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all : unit_tests integration_tests

unit_tests : test_ups_error test_ups_list test_ups_memory test_ups_utils test_upsugo test_ups_match

integration_tests : ups_test

#
# Main programs.
#

test_ups_error : test_ups_error.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_files : test_ups_files.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_list : test_ups_list.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_memory : test_ups_memory.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_utils : test_ups_utils.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_match : test_ups_match.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_upsugo: test_upsugo.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

ups_test : ups_test.o $(UPS_TEST_OBJECTS)
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(UPS_TEST_OBJECTS) $(LDFLAGS)

upstst_cmdline.o : upstst_cmdline.c
	$(CC) -c -o $@ -fullwarn -wlint,-f $< 

upstst_cmdloop.o : upstst_cmdloop.c
	$(CC) -c -o $@ -fullwarn -wlint,-f $< 

upstst_error.o : upstst_error.c
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $< $(LDFLAGS)

upstst_files.o : upstst_files.c
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $< $(LDFLAGS)

#
# Now standard targets
#
clean :
	@ upsClean
	@ rm -f test_ups_error test_ups_memory
	@ rm -f test_ups_list test_ups_utils 
	@ rm -f ups_test libups_test.a
	@ rm -f test_upsugo

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
test_ups_files.o:	test_ups_files.c
test_ups_files.o:	$(ERUPT_DIR)/inc/ups_files.h
test_ups_files.o:	$(ERUPT_DIR)/inc/ups_types.h
test_ups_files.o:	$(ERUPT_DIR)/inc/ups_list.h
test_ups_list.o:	test_ups_list.c
test_ups_list.o:	$(ERUPT_DIR)/inc/ups_list.h
test_ups_list.o:	$(ERUPT_DIR)/inc/ups_memory.h
test_ups_match.o:	test_ups_match.c
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_match.h
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_types.h
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_list.h
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_error.h
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_list.h
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_types.h
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_files.h
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_types.h
test_ups_match.o:	$(ERUPT_DIR)/inc/ups_memory.h
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
test_upsugo.o:	test_upsugo.c
test_upsugo.o:	$(ERUPT_DIR)/inc/ups_types.h
test_upsugo.o:	$(ERUPT_DIR)/inc/ups_list.h
upstst_cmdline.o:	upstst_cmdline.c
upstst_cmdline.o:	./upstst_cmdline.h
upstst_cmdloop.o:	upstst_cmdloop.c
upstst_cmdloop.o:	./upstst_cmdline.h
upstst_cmdloop.o:	./upstst_parse.h
upstst_cmdloop.o:	./upstst_cmdtable.h
upstst_cmdproc.o:	upstst_cmdproc.c
upstst_cmdproc.o:	./upstst_cmdline.h
upstst_cmdproc.o:	./upstst_parse.h
upstst_cmdproc.o:	./upstst_cmdtable.h
upstst_misc.o:	upstst_misc.c
upstst_misc.o:	./upstst_parse.h
upstst_misc.o:	./upstst_macros.h
upstst_parse.o:	upstst_parse.c
upstst_parse.o:	./upstst_parse.h
