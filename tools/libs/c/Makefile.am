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
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##

noinst_LIBRARIES = liblog.a libhelp.a libmisc.a libfio.a
check_PROGRAMS   =				\
	test-fio				\
	test-misc

CLEANFILES = 

COMMONCFLAGS   =
COMMONCPPFLAGS =				\
	-Wall -Werror				\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src			\
	-I$(top_srcdir)/tools
COMMONCXXFLAGS = 
COMMONLDADD    =				\
	libmisc.a				\
	liblog.a				\
	libhelp.a				\
	libfio.a

EXTRA_DIST =					\
	test.h

liblog_a_SOURCES  =				\
	log.c log.h
liblog_a_CFLAGS   = $(COMMONCFLAGS)
liblog_a_CPPFLAGS = $(COMMONCPPFLAGS)
liblog_a_CXXFLAGS = $(COMMONCXXFLAGS)

libhelp_a_SOURCES  =				\
	help.c help.h
libhelp_a_CFLAGS   = $(COMMONCFLAGS)
libhelp_a_CPPFLAGS = $(COMMONCPPFLAGS)
libhelp_a_CXXFLAGS = $(COMMONCXXFLAGS)

libmisc_a_SOURCES  =				\
	misc.c misc.h
libmisc_a_CFLAGS   = $(COMMONCFLAGS)
libmisc_a_CPPFLAGS = $(COMMONCPPFLAGS)
libmisc_a_CXXFLAGS = $(COMMONCXXFLAGS)

libfio_a_SOURCES  =				\
	fio.c fio.h
libfio_a_CFLAGS   = $(COMMONCFLAGS)
libfio_a_CPPFLAGS = $(COMMONCPPFLAGS)
libfio_a_CXXFLAGS = $(COMMONCXXFLAGS)


TESTS =	$(check_PROGRAMS) $(check_SCRIPTS)

test_fio_SOURCES  =				\
	test-fio.c				\
	fio.c      fio.h			\
	$(test_common_SRCS)
test_fio_CFLAGS   = $(COMMONCFLAGS)
test_fio_CPPFLAGS = $(COMMONCPPFLAGS)
test_fio_CXXFLAGS = $(COMMONCXXFLAGS)
test_fio_LDADD    = $(COMMONLDADD)

test_misc_SOURCES  =				\
	test-misc.c				\
	misc.c      misc.h			\
	$(test_common_SRCS)
test_misc_CFLAGS   = $(COMMONCFLAGS)
test_misc_CPPFLAGS = $(COMMONCPPFLAGS)
test_misc_CXXFLAGS = $(COMMONCXXFLAGS)
test_misc_LDADD    = $(COMMONLDADD)

##
## XXX FIXME: This hard-wired name is very ugly ... please fix ASAP
##
clean-local:
	for i in $(check_SCRIPTS) ; do										\
		rm -f `$(GREP) TEST_FILENAME $(srcdir)/$$i | $(GREP) = | $(SED) -e 's,^.*=\(.*\),\1,'` ;	\
	done
	rm -f `$(GREP) TEST_FILENAME $(top_srcdir)/tools/libs/c/test.h | $(SED) -e 's,^.*"\(.*\)".*,\1,'`
