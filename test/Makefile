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
	upstst_misc.o upstst_parse.o upstst_err.o upstst_fil.o	\
	upstst_ugo.o upstst_mat.o
#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all : unit_tests integration_tests

unit_tests : test_upserr test_upslst test_ups_memory test_ups_utils test_ups_ugo test_ups_match

integration_tests : ups_test

#
# Main programs.
#

test_ups_error : test_ups_error.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_upsfil : test_upsfil.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_upslst : test_upslst.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_memory : test_ups_memory.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_utils : test_ups_utils.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_match : test_ups_match.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.o $(LDFLAGS)

test_ups_ugo: test_ups_ugo.o $(LIB)/libups.a
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

upstst_ugo.o : upstst_ugo.c
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $< $(LDFLAGS)

upstst_match.o : upstst_match.c
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $< $(LDFLAGS)

#
# Now standard targets
#
clean :
	@ upsClean
	@ rm -f test_ups_error test_ups_memory
	@ rm -f test_upslst test_ups_utils 
	@ rm -f ups_test libups_test.a
	@ rm -f test_ups_ugo

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
test_upserr.o:	test_upserr.c
test_upserr.o:	$(ERUPT_DIR)/inc/upserr.h
test_upsfil.o:	test_upsfil.c
test_upsfil.o:	$(ERUPT_DIR)/inc/upserr.h
test_upsfil.o:	$(ERUPT_DIR)/inc/upsfil.h
test_upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upsfil.o:	$(ERUPT_DIR)/inc/upslst.h
test_upslst.o:	test_upslst.c
test_upslst.o:	$(ERUPT_DIR)/inc/upslst.h
test_upslst.o:	$(ERUPT_DIR)/inc/upsmem.h
test_upsmat.o:	test_upsmat.c
test_upsmat.o:	$(ERUPT_DIR)/inc/upsmat.h
test_upsmat.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upsmat.o:	$(ERUPT_DIR)/inc/upslst.h
test_upsmat.o:	$(ERUPT_DIR)/inc/upserr.h
test_upsmat.o:	$(ERUPT_DIR)/inc/upslst.h
test_upsmat.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upsmat.o:	$(ERUPT_DIR)/inc/upsfil.h
test_upsmat.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upsmat.o:	$(ERUPT_DIR)/inc/upsmem.h
test_upsmem.o:	test_upsmem.c
test_upsmem.o:	$(ERUPT_DIR)/inc/upsmem.h
test_upsmem.o:	$(ERUPT_DIR)/inc/upsutl.h
test_upsmem.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upsmem.o:	$(ERUPT_DIR)/inc/upslst.h
test_upsmem.o:	$(ERUPT_DIR)/inc/upslst.h
test_upsmem.o:	$(ERUPT_DIR)/inc/upserr.h
ups_test.o:	ups_test.c
ups_test.o:	./upstst_cmdtable.h
ups_test.o:	$(ERUPT_DIR)/inc/ups.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upslst.h
ups_test.o:	$(ERUPT_DIR)/inc/upserr.h
ups_test.o:	$(ERUPT_DIR)/inc/ups_commands.h
ups_test.o:	$(ERUPT_DIR)/inc/upsfil.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upskey.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upslst.h
ups_test.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upsmem.h
ups_test.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upslst.h
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
upstst_err.o:	upstst_err.c
upstst_err.o:	./upstst_parse.h
upstst_err.o:	./upstst_macros.h
upstst_err.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_fil.o:	upstst_fil.c
upstst_fil.o:	$(ERUPT_DIR)/inc/ups.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_fil.o:	$(ERUPT_DIR)/inc/ups_commands.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upsfil.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upskey.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upsmat.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upsmem.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upsutl.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_fil.o:	./upstst_parse.h
upstst_fil.o:	./upstst_macros.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_mat.o:	upstst_mat.c
upstst_mat.o:	$(ERUPT_DIR)/inc/ups.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_mat.o:	$(ERUPT_DIR)/inc/ups_commands.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upsfil.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upskey.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upsmat.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upsmem.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upsutl.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_mat.o:	./upstst_parse.h
upstst_mat.o:	./upstst_macros.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_misc.o:	upstst_misc.c
upstst_misc.o:	./upstst_parse.h
upstst_misc.o:	./upstst_macros.h
upstst_misc.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_parse.o:	upstst_parse.c
upstst_parse.o:	./upstst_parse.h
upstst_ugo.o:	upstst_ugo.c
upstst_ugo.o:	$(ERUPT_DIR)/inc/ups.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/ups_commands.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upsfil.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upskey.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upsmat.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upsmem.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upsutl.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_ugo.o:	./upstst_parse.h
upstst_ugo.o:	./upstst_macros.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upserr.h
