# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#  CVS/Build standard makefile template
#  $Id$
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

# run commands with bourne shell!
SHELL=/bin/sh

#------------------------------------------------------------------
# DIR is a "proper" name for the current directory (e.g. the name visible
# all over the cluster)  We try to undo common automount name munging here,
# and correct afs paths to read-only volume paths, etc.   This way we get a
# name we can use in ups declares, and when rsh-ing to another node
# to do "cmd addproduct", etc.
DIR=`pwd | sed	-e 's|^/tmp_mnt||' \
	   	-e 's|^/export||' \
	   	-e 's|^/afs/\.fnal\.gov|/afs/fnal.gov|' \
		-e 's|^/products|/usr&|'`

#------------------------------------------------------------------
# Variables for ups declaration and setup of product
#
# product name, PRODUCT_DIR environment variable, VERSion, 
# DEPENDency flags for ups, CHAIN name for ups, ups subdirectory name,
# Flavor for ups declares, which is built up from the Operating System,
# Qualifiers, and customization information, which is also used for 
# "cmd addproduct", and finally the distribution file name.
# this section may change in later ups|addproduct incarnations
            PROD=erupt
     PRODUCT_DIR=ERUPT_DIR
            VERS=devel
#          DEPEND=-b "< gcc current $(OS)$(CUST)"
           CHAIN=development
      UPS_SUBDIR=ups
 ADDPRODUCT_HOST=fndaub
DISTRIBUTIONFILE="$(DIR)/../$(FLAVOR).tar"

# for Flavored products
          FLAVOR=$(OS)$(CUST)$(QUALS)
              OS=`funame -s`
           QUALS=
            CUST=+`funame -r | sed -e 's|\..*||'`

#------------------------------------------------------------------
# Files to include in Distribution
#
# ADDDIRS is dirs to run "find" in to get a complete list
# ADDFILE is files to pick out by pattern anywhere in the product
# ADDEMPTY is empty directories to include in distribution which would
#	   otherwise be missed
# ADDCMD is a command to run to generate a list of files to include 
#	(one per line) (e.g. 'cd fnal/module; make distriblist')
# LOCAL is destination for local: and install: target
# DOCROOT is destination for html documentation targets
ADDDIRS =
ADDFILES= ! -name '*.out'
ADDEMPTY=
  ADDCMD=
   LOCAL=/usr/products/$(OS)/$(PROD)/$(VERS)$(QUALS)
 DOCROOT=/afs/fnal/files/docs/products/$(PROD)

#
#------------------------------------------------------------------
# Files that have the version string in them
# this is used by "make setversion" to know what files need
# the version string replaced
VERSIONFILES=Makefile README $(UPS_SUBDIR)/INSTALL_NOTE $(UPS_SUBDIR)/Version

#------------------------------------------------------------------
# this is an *example* of a mythical product with a GNU Configure
# based module "gnuproduct", an X11R6 imake based module "xproduct", 
# and a locally developed product that uses "premake"
# "all" should probably depend on the product directory being set.

ERUPTDBG=

all: 	proddir_is_set
	cd src; premake $(ERUPTDBG)
	cd test; premake $(ERUPTDBG)

debug: 	
	make ERUPTDBG=-debug all

clean:
	for subdir in src inc test doc bin lib; do \
	   ( cd $$subdir; echo "cleaning $$subdir"; premake clean ); \
	done

spotless:
	for subdir in src inc test doc bin lib; do \
	   ( cd $$subdir; echo "cleaning $$subdir"; premake spotless ); \
	done


#clean: 				# clean up unneeded files	#
#	-cd imports/gnuproduct ; make clean; rm config.status
#	-cd imports/xproduct ; make clean; rm Makefile
#	-cd fnal/premakedir ; premake -f $(FLAVOR) clean

spotless: clean
	-rm lib/*.a 
	-rm bin/ups
	-rm test/ups_test test/test_upserr test/upslst test/upsmat
	-rm test/upsugo   test/test_upstul 


# we indirect this a level so we can customize it for bundle products

declare: real_declare

#---------------------------------------------------------------------------
# 		*** Do not change anything below this line: ***
#     it will go away if you # update the makefile from the templates.
# - - - - - - - - - - - - - - cut here - - - - - - - - - - - - - - - - - -
#
#---------------------------------------------------------------------------
# Standard Product Distribution/Declaration Targets
#    $Id$
#---------------------------------------------------------------------------
#

# distribution -- currently makes tarfiles
#
# The invocation looks weird here; we need to make a tarfile from a (long)
# file list on stdin, but something obvious like:
#     $(LISTALL) | xargs tar cvf - 
# doesn't work on all the platforms; tar thinks its done when it hits
# the end of the first one, and "tar rvf" doesn't work right on OSF1, so
# we do the following:
# * make an empty file .header
# * make an initial tarfile by adding .header
# * then go through $(LISTALL) output and add files to the tarfile a few dozen
# 	at a time with "xargs tar uf..."
# * Finally echo the filename and do a table of contents to show it worked
#
distribution: clean .manifest
	@: > .header
	@tar cf $(DISTRIBUTIONFILE) .header
	@$(LISTALL) | xargs tar uf $(DISTRIBUTIONFILE)
	@echo $(DISTRIBUTIONFILE):
	@tar tvf $(DISTRIBUTIONFILE)

kits: addproduct

unkits: delproduct

# addproduct -- add the distribution file to a distribution platform, 
# and clean out the local copy
#
addproduct: dproducts_is_set distribution
	$(ADDPRODUCT)
	rm $(DISTRIBUTIONFILE)

# local --  Make a local copy of the product directly
# we do this by running $(LISTALL) and having cpio make a direct copy
# then we cd over there and do a check_manifest to make sure the copy 
#      worked okay.
#
local: clean .manifest
	$(LISTALL) | cpio -dumpv $(LOCAL)
	cd $(LOCAL); make check_manifest

install: local

# declare -- declares or redeclares the product; first we check
#        if its already declared and if so remove the existing declaration
#	Finally we declare it, and do a ups list so you can see the
#	declaration.
#
real_declare: dproducts_is_set $(UPS_SUBDIR)/Version
	@($(UPS_EXIST) && $(UPS_UNDECLARE)) || true
	@$(UPS_DECLARE)
	@$(UPS_LIST)

undeclare: dproducts_is_set $(UPS_SUBDIR)/Version 
	$(UPS_UNDECLARE)

# test: run regression tests on "clean" product
#      some religious discussions have centered on whether we should
#      require a clean before testing; it is this way because we want
#      to be sure clean doesn't remove anything neccesary to operating
#      the product.
#
test: clean
	sh test/TestScript

delproduct:
	$(DELPRODUCT)

# this is the usual target for manually rebuilding the software if it's just
# been checked out of the repository.  We declare it, set it up, and 
# build and regression test it.  Note that the make test will indirectly
# make clean (see above).
# 
build_n_test:
	set +e						;\
	. /usr/local/etc/setups.sh			;\
	PRODUCTS="$(DPRODUCTS) $$PRODUCTS" 		;\
	export PRODUCTS					;\
	make FLAVOR=$(FLAVOR) declare			;\
	setup -b -f $(FLAVOR) $(PROD) $(VERS)||true	;\
	make all 					;\
	setup -f $(FLAVOR) $(PROD) $(VERS)||true	;\
	make test

#
#---------------------------------------------------------------------------
# utility targets; check for variables, test file list generation
#---------------------------------------------------------------------------
CHECKIT_DEF= checkit() {\
	    test ! -z "$$1"||(echo "$$2 needs to be set";false)\
	}

dproducts_is_set: check_template_vars
	@$(CHECKIT_DEF); checkit "$(DPRODUCTS)" 'DPRODUCTS'

proddir_is_set: check_template_vars
	@$(CHECKIT_DEF); checkit "$$$(PRODUCT_DIR)" '$(PRODUCT_DIR)'

listall:
	$(LISTALL)

check_template_vars:
	@$(CHECKIT_DEF) ;\
	checkit "$(ADDPRODUCT_HOST)" "ADDPRODUCT_HOST"	;\
	checkit "$(CHAIN)" "CHAIN"			;\
	checkit "$(DIR)" "DIR"				;\
	checkit "$(DISTRIBUTIONFILE)" "DISTRIBUTIONFILE";\
	checkit "$(DOCROOT)" "DOCROOT"			;\
	checkit "$(FLAVOR)" "FLAVOR"			;\
	checkit "$(LOCAL)" "LOCAL"			;\
	checkit "$(OS)" "OS"				;\
	checkit "$(PROD)" "PROD"			;\
	checkit "$(PRODUCT_DIR)" "PRODUCT_DIR"		;\
	checkit "$(SHELL)" "SHELL"			;\
	checkit "$(UPS_SUBDIR)" "UPS_SUBDIR"		;\
	checkit "$(VERS)" "VERS"			;\
	checkit "$(VERSIONFILES)" "VERSIONFILES"	

#---------------------------------------------------------------------------
#
$(UPS_SUBDIR)/Version:
	echo $(VERS) > $(UPS_SUBDIR)/Version

$(UPS_SUBDIR)/upd_files.dat:
	$(LISTALL) > $@

#---------------------------------------------------------------------------
# .manifest file support
#---------------------------------------------------------------------------
#
MANIFEST = $(LISTALL) | 				\
		grep -v .manifest |			\
		xargs sum -r | 				\
		sed -e 's/[ 	].*[ 	]/	/' | 	\
		sort +1

.manifest: FORCE
	$(MANIFEST) > $@

check_manifest:
	$(MANIFEST) > /tmp/check$$$$ 	;\
	diff /tmp/check$$$$ .manifest	;\
	rm /tmp/check$$$$

#---------------------------------------------------------------------------
# Version change support
#---------------------------------------------------------------------------
setversion:
	@echo "New version? \c"; read newvers; set -x;			\
	perl -pi.bak -e "s/$(VERS)/$$newvers/go;" $(VERSIONFILES) ;	\
	cvs commit -m "marked as $$newvers";				\
	cvs tag -F $$newvers  .

#
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#  Gory Details section
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#---------------------------------------------------------------------------
# List all files in to be included in distributions
# this is tricky 'cause if you list a directory tar does it *and* its
# contents...
#---------------------------------------------------------------------------
#

# find arguements to prevent CVS directories and manifest file stuff
# from being listed:
PRUNECVS =  '(' -name CVS -prune ')' -o ! -name .manifest ! -name .header

# make a list of all the files to be included in the product.  There are
# several make variables that could be set, so use all of them that aree
# to make the list.  We do the whole thing in a subshell (e.g. between
# parenthesis) so the whole thing can be piped to other programs, etc.
#
LISTALL =  ( \
    test -z "$(ADDDIRS)" || find $(ADDDIRS) $(PRUNECVS) ! -type d -print; \
    test -z "$(ADDFILES)" || find . $(PRUNECVS) $(ADDFILES) ! -type d -print; \
    test -z "$(ADDCMD)" || sh -c "$(ADDCMD)"; \
    for d in $(ADDEMPTY) .manifest; do echo $$d; done )

#---------------------------------------------------------------------------
# Ugly Definitions for ups
#---------------------------------------------------------------------------
#

# These are all basic ups commands with loads of options.
UPS_EXIST= \
	PRODUCTS=$$DPRODUCTS \
	$(UPS_DIR)/bin/ups_exist \
		-f $(FLAVOR) \
		$(PROD) $(VERS)
UPS_UNDECLARE= \
	PRODUCTS=$$DPRODUCTS \
	$(UPS_DIR)/bin/ups_undeclare \
		-f $(FLAVOR) \
		$(PROD) $(VERS)

UPS_DECLARE= \
	PRODUCTS=$$DPRODUCTS \
	$(UPS_DIR)/bin/ups_declare \
		$(DEPEND) \
		-U $(DIR)/$(UPS_SUBDIR) \
		-r $(DIR) \
		-f $(FLAVOR) \
		$(PROD) $(VERS)

UPS_LIST= \
	PRODUCTS=$$DPRODUCTS \
	$(UPS_DIR)/bin/ups_list \
		-la \
		-f $(FLAVOR) \
		$(PROD) $(VERS)

ADDPRODUCT = \
    rsh $(ADDPRODUCT_HOST) /bin/sh -c "'\
	. /usr/local/etc/setpath.sh ; \
	. /usr/local/etc/setups.sh ; \
	cmd addproduct \
		-t $(DISTRIBUTIONFILE) \
		-o $(OS) \
		-c $(CUST)$(QUALS) \
		-m $(CUST)$(QUALS) \
		-p $(PROD)  \
		-C $(CHAIN) \
		-v $(VERS) \
		-y '"

DELPRODUCT = \
    rsh $(ADDPRODUCT_HOST) /bin/sh -c "'\
	. /usr/local/etc/setpath.sh ; \
	. /usr/local/etc/setups.sh ; \
	cmd delproduct \
		-o $(OS) \
		-c $(CUST)$(QUALS) \
		-m $(CUST)$(QUALS) \
		-p $(PROD)  \
		-v $(VERS) \
		-y '"

FORCE:
#
#---------------------------------------------------------------------------
# Documentation Targets
#---------------------------------------------------------------------------
#
# you probably don't need this for local products, but third party
# software tends to stuff unformatted man pages in $PREFIX/man...
#
$(UPS_SUBDIR)/toman:
	mkdir $(UPS_SUBDIR)/toman
	mkdir $(UPS_SUBDIR)/toman/man
	mkdir $(UPS_SUBDIR)/toman/catman
	. /usr/local/etc/setups.sh                                      ;\
	setup groff  || true                                            ;\
	cd man                                                          ;\
	for d in man?                                                   ;\
	do                                                               \
	    (cd $$d                                                 ;\
	    for f in *                                              ;\
	    do                                                       \
		echo $$d/$$f                                    ;\
		cp $$f ../../$(UPS_SUBDIR)/toman/man                      ;\
		nroff -man $$f > ../../$(UPS_SUBDIR)/toman/catman/$$f     ;\
	    done)                                                   ;\
	done


#
# targets to install html manpages,etc.  in afs docs/products area
#
html: html-man html-texi html-html

html-man: $(UPS_SUBDIR)/toman
	. /usr/local/etc/setups.sh					;\
	setup conv2html							;\
	if [ -d $(UPS_SUBDIR)/toman/catman ]; then			;\
	    src=$(UPS_SUBDIR)/toman/catman				;\
	else								;\
	    src=$(UPS_SUBDIR)/toman					;\
	fi								;\
	dest=$(DOCROOT)/man						;\
	mkdir -p $$dest	|| true						;\
	(cd $$src; find . -print) |				 	 \
	    while read f; do 						 \
		if [ -d $$src/$$f ]					;\
		then							;\
		    mkdir -p $$dest/$$f || true				;\
		else							;\
		    man2html < $$src/$$f > $$dest/$$f			;\
		fi							;\
	    done

html-texi:
	. /usr/local/etc/setups.sh					;\
	setup conv2html							;\
	dest=$(DOCROOT)/texi						;\
	mkdir -p $$dest	|| true						;\
	cd $$dest							;\
	(cd $$src; find . -name *.texi -print) |		 	 \
	    while read f; do						 \
		texi2html -split_chapter $$src/$$f 			;\
	    done

html-html:
	dest=$(DOCROOT)/html						;\
	find . -name '*.html' -print |					 \
	    cpio -dumpv $$dest
