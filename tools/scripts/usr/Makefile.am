##
## Input file for automake
##
## Copyright (C) 2008 Francesco Salvestrini
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

scripts =					\
	bootcheck				\
	clhandler				\
	getdefine				\
	grub1-config				\
	grub2-config				\
	mkiso-grub1				\
	mkiso-grub2				\
	mergesyms				\
	mkbuild					\
	rules2syms				\
	sh2h

noinst_SCRIPTS = $(scripts)

awk_scripts_SRCS =				\
	bootcheck.awk.in			\
	clhandler.awk.in			\
	getdefine.awk.in			\
	mergesyms.awk.in			\
	mkbuild.awk.in				\
	rules2syms.awk.in			\
	sh2h.awk.in

sh_scripts_SRCS =				\
	grub1-config.sh.in			\
	grub2-config.sh.in			\
	mkiso-grub1.sh.in			\
	mkiso-grub2.sh.in

pl_scripts_SRCS =

CLEANFILES =
EXTRA_DIST =

EXTRA_DIST +=					\
	$(awk_scripts_SRCS)			\
	$(sh_scripts_SRCS)			\
	$(pl_scripts_SRCS)

#
# Transformation related variables
#

awk_lib_SRC  = $(top_builddir)/tools/scripts/libs/awk/lib.awk
sh_lib_SRC   = $(top_builddir)/tools/scripts/libs/sh/lib.sh
perl_lib_SRC = $(top_builddir)/tools/scripts/libs/perl/lib.pl

protect_sh = { cat ; }
header_sh  = { echo "\#! @SHELL@" ; echo "" ; }
footer_sh  = { echo "" ; }

transform_sh_in_2_sh =				\
	{ { $(header_sh) &&			\
	    cat $(sh_lib_SRC) &&		\
	    cat $(srcdir)/$$INFILE &&		\
	    $(footer_sh) } |			\
	    $(protect_sh) > $$OUTFILE ; } 

transform_sh_2_script =					\
	{ cat $$INFILE | $(do_subst) > $$OUTFILE &&	\
	  chmod +x $$OUTFILE ; }

header_awk  = { echo "@AWK@ '" ; }
footer_awk  = { echo "' ""$$""@" ; }
protect_awk = { $(SED) -e "s/'/\'/g" -e 's/,[ \t]*$$/, \\/' ; }

transform_awk_in_2_awk =			\
	{ { $(header_awk) &&			\
	    cat $(awk_lib_SRC) &&		\
	    cat $(srcdir)/$$INFILE &&		\
	    $(footer_awk) } |			\
	    $(protect_awk) > $$OUTFILE ; }

transform_awk_2_sh =				\
	{ { $(header_sh) &&			\
	    cat $(sh_lib_SRC) &&		\
	    cat $$INFILE &&			\
	    $(footer_sh) } |			\
	    $(protect_sh) > $$OUTFILE ; }

header_perl  = { echo "@PERL@ -e '" ; }
footer_perl  = { echo "' ""$$""@" ; }
protect_perl = { $(SED) -e "s/'/\'/g" -e 's/,[ \t]*$$/, \\/' ; }

transform_pl_in_2_pl =				\
	{ { $(header_perl) &&			\
	    cat $(perl_lib_SRC) &&		\
	    cat $(srcdir)/$$INFILE &&		\
	    $(footer_perl) } |			\
	    $(protect_perl) > $$OUTFILE ; }

transform_pl_2_sh =				\
	{ { $(header_sh) &&			\
	    cat $(sh_lib_SRC) &&		\
	    cat $$INFILE &&			\
	    $(footer_sh) } |			\
	    $(protect_sh) > $$OUTFILE ; }


grub1-config.sh: Makefile $(sh_lib_SRC) grub1-config.sh.in
	INFILE=grub1-config.sh.in ;		\
	OUTFILE=grub1-config.sh ;		\
	$(transform_sh_in_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += grub1-config.sh

grub1-config: Makefile grub1-config.sh
	INFILE=grub1-config.sh ;		\
	OUTFILE=grub1-config ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += grub1-config

grub2-config.sh: Makefile $(sh_lib_SRC) grub2-config.sh.in
	INFILE=grub2-config.sh.in ;		\
	OUTFILE=grub2-config.sh ;		\
	$(transform_sh_in_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += grub2-config.sh

grub2-config: Makefile grub2-config.sh
	INFILE=grub2-config.sh ;		\
	OUTFILE=grub2-config ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += grub2-config

mkiso-grub1.sh: Makefile $(sh_lib_SRC) mkiso-grub1.sh.in
	INFILE=mkiso-grub1.sh.in ;		\
	OUTFILE=mkiso-grub1.sh ;		\
	$(transform_sh_in_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mkiso-grub1.sh

mkiso-grub1: Makefile mkiso-grub1.sh
	INFILE=mkiso-grub1.sh ;			\
	OUTFILE=mkiso-grub1 ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mkiso-grub1

mkiso-grub2.sh: Makefile $(sh_lib_SRC) mkiso-grub2.sh.in
	INFILE=mkiso-grub2.sh.in ;		\
	OUTFILE=mkiso-grub2.sh ;		\
	$(transform_sh_in_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mkiso-grub2.sh

mkiso-grub2: Makefile mkiso-grub2.sh
	INFILE=mkiso-grub2.sh ;			\
	OUTFILE=mkiso-grub2 ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mkiso-grub2

## sh2h
sh2h.awk: Makefile $(awk_lib_SRC) sh2h.awk.in
	INFILE=sh2h.awk.in ;			\
	OUTFILE=sh2h.awk ;			\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += sh2h.awk

sh2h.sh: Makefile $(sh_lib_SRC) sh2h.awk
	INFILE=sh2h.awk ;			\
	OUTFILE=sh2h.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += sh2h.sh

sh2h: Makefile sh2h.sh
	INFILE=sh2h.sh ;			\
	OUTFILE=sh2h ;				\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += sh2h

## mergesyms
mergesyms.awk: Makefile $(awk_lib_SRC) mergesyms.awk.in
	INFILE=mergesyms.awk.in ;		\
	OUTFILE=mergesyms.awk ;			\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mergesyms.awk

mergesyms.sh: Makefile $(sh_lib_SRC) mergesyms.awk
	INFILE=mergesyms.awk ;			\
	OUTFILE=mergesyms.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mergesyms.sh

mergesyms: Makefile mergesyms.sh
	INFILE=mergesyms.sh ;			\
	OUTFILE=mergesyms ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mergesyms

## mkbuild
mkbuild.awk: Makefile $(awk_lib_SRC) mkbuild.awk.in
	INFILE=mkbuild.awk.in ;			\
	OUTFILE=mkbuild.awk ;			\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mkbuild.awk

mkbuild.sh: Makefile $(sh_lib_SRC) mkbuild.awk
	INFILE=mkbuild.awk ;			\
	OUTFILE=mkbuild.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mkbuild.sh

mkbuild: Makefile mkbuild.sh
	INFILE=mkbuild.sh ;			\
	OUTFILE=mkbuild ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mkbuild

## getdefine
getdefine.awk: Makefile $(awk_lib_SRC) getdefine.awk.in
	INFILE=getdefine.awk.in ;		\
	OUTFILE=getdefine.awk ;			\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += getdefine.awk

getdefine.sh: Makefile $(sh_lib_SRC) getdefine.awk
	INFILE=getdefine.awk ;			\
	OUTFILE=getdefine.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += getdefine.sh

getdefine: Makefile getdefine.sh
	INFILE=getdefine.sh ;			\
	OUTFILE=getdefine ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += getdefine

## rules2syms
rules2syms.awk: Makefile $(awk_lib_SRC) rules2syms.awk.in
	INFILE=rules2syms.awk.in ;		\
	OUTFILE=rules2syms.awk ;		\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += rules2syms.awk

rules2syms.sh: Makefile $(sh_lib_SRC) rules2syms.awk
	INFILE=rules2syms.awk ;			\
	OUTFILE=rules2syms.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += rules2syms.sh

rules2syms: Makefile rules2syms.sh
	INFILE=rules2syms.sh ;			\
	OUTFILE=rules2syms ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += rules2syms

## bootcheck
bootcheck.awk: Makefile $(awk_lib_SRC) bootcheck.awk.in
	INFILE=bootcheck.awk.in ;		\
	OUTFILE=bootcheck.awk ;			\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += bootcheck.awk

bootcheck.sh: Makefile $(sh_lib_SRC) bootcheck.awk
	INFILE=bootcheck.awk ;			\
	OUTFILE=bootcheck.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += bootcheck.sh

bootcheck: Makefile bootcheck.sh
	INFILE=bootcheck.sh ;			\
	OUTFILE=bootcheck ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += bootcheck

## clhandler
clhandler.awk: Makefile $(awk_lib_SRC) clhandler.awk.in
	INFILE=clhandler.awk.in ;		\
	OUTFILE=clhandler.awk ;			\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += clhandler.awk

clhandler.sh: Makefile $(sh_lib_SRC) clhandler.awk
	INFILE=clhandler.awk ;			\
	OUTFILE=clhandler.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += clhandler.sh

clhandler: Makefile clhandler.sh
	INFILE=clhandler.sh ;			\
	OUTFILE=clhandler ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += clhandler


##
## Checks
##
check_PASS_SCRPTS =				\
	test-clhandler-1			\
	test-clhandler-2			\
	test-rules2syms-1			\
	test-mergesyms-1			\
	test-getdefine-1

check_FAIL_SCRPTS =				\
	test-clhandler-3

check_SCRIPTS =					\
	$(check_PASS_SCRPTS)			\
	$(check_FAIL_SCRPTS)

XFAIL_TESTS = $(check_FAIL_SCRPTS)
TESTS	    =	$(check_PROGRAMS) $(check_SCRIPTS) 

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
HOST = "http://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob_plain;hb=HEAD;f="

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