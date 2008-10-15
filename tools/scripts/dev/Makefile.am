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
include $(top_srcdir)/tools/scripts/Makefile.sub

scripts =					\
	stackcheck				\
	mksyscalls				\
	obj2symstab				\
	symstab2map				\
	sections				\
	includescheck				\
	ifdefscheck

noinst_SCRIPTS = $(scripts)

awk_scripts_SRCS =				\
	stackcheck.awk.in			\
	mksyscalls.awk.in			\
	obj2symstab.awk.in			\
	symstab2map.awk.in			\
	sections.awk.in				\
	includescheck.awk.in			\
	ifdefscheck.awk.in

sh_scripts_SRCS =

pl_scripts_SRCS =

CLEANFILES =
EXTRA_DIST =

EXTRA_DIST +=					\
	$(awk_scripts_SRCS)			\
	$(sh_scripts_SRCS)			\
	$(pl_scripts_SRCS)


## stackcheck
stackcheck.awk: Makefile $(awk_lib_SRC) stackcheck.awk.in
	INFILE=stackcheck.awk.in ;		\
	OUTFILE=stackcheck.awk ;		\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += stackcheck.awk

stackcheck.sh: Makefile $(sh_lib_SRC) stackcheck.awk
	INFILE=stackcheck.awk ;			\
	OUTFILE=stackcheck.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += stackcheck.sh

stackcheck: Makefile stackcheck.sh
	INFILE=stackcheck.sh ;			\
	OUTFILE=stackcheck ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += stackcheck

## mksyscalls
mksyscalls.awk: Makefile $(awk_lib_SRC) mksyscalls.awk.in
	INFILE=mksyscalls.awk.in ;		\
	OUTFILE=mksyscalls.awk ;		\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mksyscalls.awk

mksyscalls.sh: Makefile $(sh_lib_SRC) mksyscalls.awk
	INFILE=mksyscalls.awk ;			\
	OUTFILE=mksyscalls.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mksyscalls.sh

mksyscalls: Makefile mksyscalls.sh
	INFILE=mksyscalls.sh ;			\
	OUTFILE=mksyscalls ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += mksyscalls

## obj2symstab
obj2symstab.awk: Makefile $(awk_lib_SRC) obj2symstab.awk.in
	INFILE=obj2symstab.awk.in ;		\
	OUTFILE=obj2symstab.awk ;		\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += obj2symstab.awk

obj2symstab.sh: Makefile $(sh_lib_SRC) obj2symstab.awk
	INFILE=obj2symstab.awk ;		\
	OUTFILE=obj2symstab.sh ;		\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += obj2symstab.sh

obj2symstab: Makefile obj2symstab.sh
	INFILE=obj2symstab.sh ;			\
	OUTFILE=obj2symstab ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += obj2symstab

## sections
sections.awk: Makefile $(awk_lib_SRC) sections.awk.in
	INFILE=sections.awk.in ;		\
	OUTFILE=sections.awk ;			\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += sections.awk

sections.sh: Makefile $(sh_lib_SRC) sections.awk
	INFILE=sections.awk ;			\
	OUTFILE=sections.sh ;			\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += sections.sh

sections: Makefile sections.sh
	INFILE=sections.sh ;			\
	OUTFILE=sections ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += sections

## includescheck
includescheck.awk: Makefile $(awk_lib_SRC) includescheck.awk.in
	INFILE=includescheck.awk.in ;		\
	OUTFILE=includescheck.awk ;		\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += includescheck.awk

includescheck.sh: Makefile $(sh_lib_SRC) includescheck.awk
	INFILE=includescheck.awk ;		\
	OUTFILE=includescheck.sh ;		\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += includescheck.sh

includescheck: Makefile includescheck.sh
	INFILE=includescheck.sh ;		\
	OUTFILE=includescheck ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += includescheck

## ifdefscheck
ifdefscheck.awk: Makefile $(awk_lib_SRC) ifdefscheck.awk.in
	INFILE=ifdefscheck.awk.in ;		\
	OUTFILE=ifdefscheck.awk ;		\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += ifdefscheck.awk

ifdefscheck.sh: Makefile $(sh_lib_SRC) ifdefscheck.awk
	INFILE=ifdefscheck.awk ;		\
	OUTFILE=ifdefscheck.sh ;		\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += ifdefscheck.sh

ifdefscheck: Makefile ifdefscheck.sh
	INFILE=ifdefscheck.sh ;			\
	OUTFILE=ifdefscheck ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += ifdefscheck

## symstab2map
symstab2map.awk: Makefile $(awk_lib_SRC) symstab2map.awk.in
	INFILE=symstab2map.awk.in ;		\
	OUTFILE=symstab2map.awk ;		\
	$(transform_awk_in_2_awk) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += symstab2map.awk

symstab2map.sh: Makefile $(sh_lib_SRC) symstab2map.awk
	INFILE=symstab2map.awk ;		\
	OUTFILE=symstab2map.sh ;		\
	$(transform_awk_2_sh) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += symstab2map.sh

symstab2map: Makefile symstab2map.sh
	INFILE=symstab2map.sh ;			\
	OUTFILE=symstab2map ;			\
	$(transform_sh_2_script) ||		\
	{ rm -f $$OUTFILE ; exit 1 ; }
CLEANFILES += symstab2map


##
## Checks
##
check_PASS_SCRPTS =

check_FAIL_SCRPTS =

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
