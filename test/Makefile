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

UNIT_TESTS = test_upserr test_upslst test_upsmem test_upsutl test_upsugo \
	test_upsmat test_upsact

UPS_TEST_OBJECTS = upstst_cmdline.o upstst_cmdloop.o upstst_cmdproc.o \
	upstst_misc.o upstst_parse.o upstst_err.o upstst_fil.o	\
	upstst_ugo.o upstst_mat.o upstst_get.o upstst_list.o
#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all : unit_tests integration_tests

unit_tests : $(UNIT_TESTS)

integration_tests : ups_test

#
# Main programs.
#

test_ups_list : test_ups_list.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

test_upsact : test_upsact.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

test_upserr : test_upserr.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

test_upsfil : test_upsfil.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

test_upslst : test_upslst.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

test_upsmem : test_upsmem.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

test_upsutl : test_upsutl.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

test_upsmat : test_upsmat.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

test_upsugo: test_upsugo.o $(LIB)/libups.a
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(LDFLAGS)

ups_test : ups_test.o $(UPS_TEST_OBJECTS)
	$(CC) $@.o -o $@ $(CCCHK) $(CFLAGS) $(UPS_TEST_OBJECTS) $(LDFLAGS)

upstst_cmdline.o : upstst_cmdline.c $(LIB)/libups.a
	$(CC) -c -o $@   $< 

upstst_cmdloop.o : upstst_cmdloop.c $(LIB)/libups.a
	$(CC) -c -o $@   $< 

upstst_error.o : upstst_error.c $(LIB)/libups.a
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $< $(LDFLAGS)

upstst_files.o : upstst_files.c $(LIB)/libups.a
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $< $(LDFLAGS)

upstst_ugo.o : upstst_ugo.c $(LIB)/libups.a
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $< $(LDFLAGS)

upstst_match.o : upstst_match.c $(LIB)/libups.a
	$(CC) -c -o $@ $(CCCHK) $(CFLAGS) $< $(LDFLAGS)

#
# Now standard targets
#
clean :
	@ upsClean

spotless :
	@ upsClean
	@ rm -f $(UNIT_TESTS)
	@ rm -f ups_test libups_test.a

make :
	@if [ "$(CCENV)" = "" ]; then ccenv=`uname`; else ccenv=$(CCENV); fi; \
	echo \
	"make_make -cc '$(CC) $(CFLAGS)' -nostd -file Makefile" \
                                                        "-env $$ccenv *.c"; \
	make_make -cc '$(CC) $(CFLAGS)' -nostd -file Makefile \
                                                         -env $$ccenv *.c
#START_DEPEND
test_upsact.o:	test_upsact.c
test_upsact.o:	$(ERUPT_DIR)/inc/upsact.h
test_upsact.o:	$(ERUPT_DIR)/inc/upserr.h
test_upsact.o:	$(ERUPT_DIR)/inc/upslst.h
test_upserr.o:	test_upserr.c
test_upserr.o:	$(ERUPT_DIR)/inc/upserr.h
test_upsfil.o:	test_upsfil.c
test_upsfil.o:	$(ERUPT_DIR)/inc/upserr.h
test_upsfil.o:	$(ERUPT_DIR)/inc/upsfil.h
test_upsfil.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upsfil.o:	$(ERUPT_DIR)/inc/upslst.h
test_upshlp.o:	test_upshlp.c
test_upshlp.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upshlp.o:	$(ERUPT_DIR)/inc/upslst.h
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
test_upsugo.o:	test_upsugo.c
test_upsugo.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upsugo.o:	$(ERUPT_DIR)/inc/upslst.h
test_upsutl.o:	test_upsutl.c
test_upsutl.o:	$(ERUPT_DIR)/inc/upsutl.h
test_upsutl.o:	$(ERUPT_DIR)/inc/upstyp.h
test_upsutl.o:	$(ERUPT_DIR)/inc/upslst.h
test_upsutl.o:	$(ERUPT_DIR)/inc/upslst.h
test_upsutl.o:	$(ERUPT_DIR)/inc/upserr.h
test_upsutl.o:	$(ERUPT_DIR)/inc/upsmat.h
test_upsutl.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	ups_test.c
ups_test.o:	./upstst_cmdtable.h
ups_test.o:	$(ERUPT_DIR)/inc/ups.h
ups_test.o:	$(ERUPT_DIR)/inc/upserr.h
ups_test.o:	$(ERUPT_DIR)/inc/upsfil.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upslst.h
ups_test.o:	$(ERUPT_DIR)/inc/upskey.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upslst.h
ups_test.o:	$(ERUPT_DIR)/inc/upsmat.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upsmem.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upsutl.h
ups_test.o:	$(ERUPT_DIR)/inc/upstyp.h
ups_test.o:	$(ERUPT_DIR)/inc/upslst.h
ups_test.o:	$(ERUPT_DIR)/inc/ups_list.h
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
upstst_fil.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upsfil.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upskey.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upsmat.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upsmem.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upsutl.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_fil.o:	$(ERUPT_DIR)/inc/ups_list.h
upstst_fil.o:	./upstst_parse.h
upstst_fil.o:	./upstst_macros.h
upstst_fil.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_mat.o:	upstst_mat.c
upstst_mat.o:	$(ERUPT_DIR)/inc/ups.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upsfil.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upskey.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upsmat.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upsmem.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upsutl.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_mat.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_mat.o:	$(ERUPT_DIR)/inc/ups_list.h
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
upstst_ugo.o:	$(ERUPT_DIR)/inc/upserr.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upsfil.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upskey.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upsmat.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upsmem.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upsutl.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upstyp.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upslst.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/ups_list.h
upstst_ugo.o:	./upstst_parse.h
upstst_ugo.o:	./upstst_macros.h
upstst_ugo.o:	$(ERUPT_DIR)/inc/upserr.h
