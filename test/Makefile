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
UPS_TEST_OBJECTS = ups_t_cmdline.o ups_t_cmdloop.o ups_t_cmdproc.o \
	ups_t_misc.o ups_t_parse.o
#=============================================================================
# TARGETS
#=============================================================================

default:	
	@echo Please invoke this makefile using premake. >&2
	@exit 1

all : test_ups_error test_ups_list test_ups_memory test_ups_utils ups_test

#
# Main programs.
#

test_ups_error : test_ups_error.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

test_ups_files : test_ups_files.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

test_ups_list : test_ups_list.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

test_ups_memory : test_ups_memory.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

test_ups_utils : test_ups_utils.o $(LIB)/libups.a
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(LDFLAGS)

ups_test : ups_test.o $(UPS_TEST_OBJECTS)
	$(CC) -o $@ $(CCCHK) $(CFLAGS) $@.c $(UPS_TEST_OBJECTS) $(LDFLAGS)

ups_t_cmdline.o : ups_t_cmdline.c
	$(CC) -c -o $@ -fullwarn -wlint,-f $< 

ups_t_cmdloop.o : ups_t_cmdloop.c
	$(CC) -c -o $@ -fullwarn -wlint,-f $< 

#
# Now standard targets
#
clean :
	@ upsClean
	@ rm -f test_ups_error test_ups_memory
	@ rm -f test_ups_list test_ups_utils 
	@ rm -f ups_test libups_test.a

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
test_upsugo.o:	./../inc/ups_types.h
test_upsugo.o:	./../inc/ups_list.h
ups_t_cmdline.o:	ups_t_cmdline.c
ups_t_cmdline.o:	./ups_t_cmdline.h
ups_t_cmdloop.o:	ups_t_cmdloop.c
ups_t_cmdloop.o:	./ups_t_cmdline.h
ups_t_cmdloop.o:	./ups_t_parse.h
ups_t_cmdloop.o:	./ups_t_cmdtable.h
ups_t_cmdproc.o:	ups_t_cmdproc.c
ups_t_cmdproc.o:	./ups_t_cmdline.h
ups_t_cmdproc.o:	./ups_t_parse.h
ups_t_cmdproc.o:	./ups_t_cmdtable.h
ups_t_misc.o:	ups_t_misc.c
ups_t_misc.o:	./ups_t_parse.h
ups_t_misc.o:	./ups_t_macros.h
ups_t_parse.o:	ups_t_parse.c
ups_t_parse.o:	./ups_t_parse.h
ups_test.o:	ups_test.c
ups_test.o:	./ups_t_cmdtable.h
