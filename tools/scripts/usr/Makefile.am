##
## Input file for automake
##
## Copyright (C) 2008, 2009 Francesco Salvestrini
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##

include $(top_srcdir)/Makefile.sub
include $(top_srcdir)/tools/scripts/Makefile.sub

scripts =					\
	getdefine				\
	mergesyms				\
	rules2syms				\
	sh2h					\
	packrules

noinst_SCRIPTS = $(scripts)

awk_scripts_SRCS =

sh_scripts_SRCS =				\
	rules2syms.sh.in			\
	sh2h.sh.in				\
	getdefine.sh.in				\
	mergesyms.sh.in

pl_scripts_SRCS =				\
	packrules.pl.in

CLEANFILES =
EXTRA_DIST =

EXTRA_DIST +=					\
	$(awk_scripts_SRCS)			\
	$(sh_scripts_SRCS)			\
	$(pl_scripts_SRCS)


mergesyms.sh: Makefile $(sh_lib_SRC) mergesyms.sh.in
	INFILE=mergesyms.sh.in ;		\
	OUTFILE=mergesyms.sh ;			\
	$(transform_sh_in_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mergesyms.sh

mergesyms: Makefile mergesyms.sh
	INFILE=mergesyms.sh ;			\
	OUTFILE=mergesyms ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mergesyms

getdefine.sh: Makefile $(sh_lib_SRC) getdefine.sh.in
	INFILE=getdefine.sh.in ;		\
	OUTFILE=getdefine.sh ;			\
	$(transform_sh_in_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += getdefine.sh

getdefine: Makefile getdefine.sh
	INFILE=getdefine.sh ;			\
	OUTFILE=getdefine ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += getdefine

sh2h.sh: Makefile $(sh_lib_SRC) sh2h.sh.in
	INFILE=sh2h.sh.in ;			\
	OUTFILE=sh2h.sh ;			\
	$(transform_sh_in_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += sh2h.sh

sh2h: Makefile sh2h.sh
	INFILE=sh2h.sh ;			\
	OUTFILE=sh2h ;				\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += sh2h

rules2syms.sh: Makefile $(sh_lib_SRC) rules2syms.sh.in
	INFILE=rules2syms.sh.in ;		\
	OUTFILE=rules2syms.sh ;			\
	$(transform_sh_in_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += rules2syms.sh

rules2syms: Makefile rules2syms.sh
	INFILE=rules2syms.sh ;			\
	OUTFILE=rules2syms ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += rules2syms

packrules.pl: Makefile $(pl_lib_SRC) packrules.pl.in
	INFILE=packrules.pl.in ;		\
	OUTFILE=packrules.pl ;			\
	$(transform_pl_in_2_pl) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += packrules.pl

packrules: Makefile packrules.pl
	INFILE=packrules.pl ;			\
	OUTFILE=packrules ;			\
	$(transform_pl_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += packrules

##
## Checks
##
check_PASS_SCRPTS =				\
	test-rules2syms-1			\
	test-rules2syms-2			\
	test-getdefine-1			\
	test-getdefine-2			\
	test-getdefine-4			\
	test-sh2h-1				\
	test-sh2h-2				\
	test-sh2h-3				\
	test-sh2h-4				\
	test-mergesyms-1


check_FAIL_SCRPTS =				\
	test-sh2h-5				\
	test-sh2h-6				\
	test-getdefine-3			\
	test-mergesyms-2			\
	test-mergesyms-3			\
	test-mergesyms-4			\
	test-mergesyms-5			\
	test-mergesyms-6

check_SCRIPTS =					\
	$(check_PASS_SCRPTS)			\
	$(check_FAIL_SCRPTS)

XFAIL_TESTS = $(check_FAIL_SCRPTS)
TESTS	    = $(check_PROGRAMS) $(check_SCRIPTS)

EXTRA_DIST += $(TESTS)

##
## XXX FIXME: This hard-wired name is very ugly ... please fix ASAP
##
clean-local:
	for i in $(check_SCRIPTS) ; do										\
		rm -f `$(GREP) TEST_FILENAME $(srcdir)/$$i | $(GREP) = | $(SED) -e 's,^.*=\(.*\),\1,'` ;	\
	done

##
## Maintainer related targets
##
update-local: Makefile

maintainer-check-parameters: Makefile $(scripts)
	for i in $(scripts) ; do \
		echo @ECHO_N@ "Checking standard parameters for $$i... " ; \
		if ( ./$$i -h 2>&1 > /dev/null ) ; then \
			echo "@ECHO_T@ok" ; \
		else \
			echo "@ECHO_T@error" ; \
			exit 1 ; \
		fi ; \
	done

##
## NOTE:
##     All exported library functions must be called by two or more
##     different scripts
##
maintainer-check-exported: Makefile $(awk_lib_SRCS) $(awk_scripts_SRCS)
	for l in $(awk_libs_SRCS) ; do \
		FUNCTIONS=`$(GREP) -e '^[ \t]*function\ .*$$' $(srcdir)/$$l | $(SED) -e 's/^[ \t]*function\ [ \t]*//' -e 's/(.*$$//'` ; \
		for f in $$FUNCTIONS ; do \
			echo @ECHO_N@ "Checking $$f() " ; \
			CNTs=0 ; \
			for x in $(awk_scripts_SRCS) ; do \
				CNTs=$$(( $$CNTs + `cat $(srcdir)/$$x | $(GREP) -m 1 -c "[ \t]*[^\#][ \t]*$$f[ \t]*("` )) ; \
			done ; \
			CNTl=0 ; \
			for x in $(awk_libs_SRCS) ; do \
				CNTl=$$(( $$CNTl + `cat $(srcdir)/$$x | $(GREP) -m 1 -c "[ \t]*[^\#][ \t]*$$f[ \t]*("` )) ; \
			done ; \
			echo @ECHO_N@ "(calls $$CNTl:$$CNTs)... " ; \
			if test $$CNTl -ge 2 ; then \
				echo "@ECHO_T@ok" ; \
			else \
				if test $$CNTs -ge 1 ; then \
					echo "@ECHO_T@ok" ; \
				else \
					echo "@ECHO_T@unused" ; \
					exit 1 ; \
				fi ; \
			fi ; \
		done ; \
	done

##
## NOTE:
##     Don't leave white spaces before the parentheses in user functions
##     calls; GNU awk will reject it (XXX-FIXME: Add check)
##
maintainer-check-whitespaces: Makefile $(awk_scripts_SRCS)

##
## NOTE:
##     Don't use unsupported functions (portability problems)
##
maintainer-check-unallowed-function: Makefile $(awk_scripts_SRCS)
	FUNCTIONS="asort asorti" ; \
	for l in $(awk_scripts_SRCS) ; do \
		echo @ECHO_N@ "Checking $$l for unallowed functions: " ; \
		for f in $$FUNCTIONS ; do \
			echo @ECHO_N@ "$$f " ; \
			if test `$(GREP) -c "$$f[ \t]+(" $(srcdir)/$$l` != 0 ; then \
				echo "... @ECHO_T@no" ; \
				exit 1 ; \
			fi ; \
		done ; \
		echo "...  @ECHO_T@ok" ; \
	done

##
## NOTE:
##     All scripts must call lib_init() and lib_fini() functions once
##
maintainer-check-init-fini-functions: Makefile $(awk_scripts_SRCS)
#	for l in $(awk_scripts_SRCS) ; do \
#		name=`echo $$l | $(SED) -e 's,^.*lib,,' -e 's,\\..*$$,,'` ; \
#		if test "x$$name" = "xcore" ; then \
#			continue ; \
#		fi ; \
#		echo @ECHO_N@ "Checking if $$l calls only one _$${name}_init()... " ; \
#		CNT=`$(GREP) -c _$${name}_init\(\) $(srcdir)/$$l` ; \
#		if test "x$$CNT" = "x1" ; then \
#			echo "@ECHO_T@ok" ; \
#		else \
#			echo "@ECHO_T@no" ; \
#			exit 1 ; \
#		fi ; \
#		echo @ECHO_N@ "Checking if $$l calls only one _$${name}_fini()... " ; \
#		CNT=`$(GREP) -c _$${name}_fini\(\) $(srcdir)/$$l` ; \
#		if test "x$$CNT" = "x1" ; then \
#			echo "@ECHO_T@ok" ; \
#		else \
#			echo "@ECHO_T@no" ; \
#			exit 1 ; \
#		fi ; \
#	done

maintainer-check-local: Makefile maintainer-check-exported maintainer-check-whitespaces maintainer-check-init-fini-functions maintainer-check-unallowed-function maintainer-check-parameters
