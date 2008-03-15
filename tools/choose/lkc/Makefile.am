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

AUTOMAKE_OPTIONS = subdir-objects

COMMONCFLAGS   = 
COMMONCPPFLAGS =				\
	-Wall -Werror				\
	-I$(top_srcdir)/include			\
	-I$(top_builddir)/include		\
	-I$(top_srcdir)				\
	-I$(top_builddir)
COMMONCXXFLAGS = 
COMMONLDADD    =

AM_LFLAGS = $(LEX_FLAGS)
AM_YFLAGS = $(YACC_FLAGS) -d

noinst_PROGRAMS = conf mconf

CLEANFILES =

EXTRA_DIST =					\
	confdata.c				\
	expr.c					\
	expr.h					\
	menu.c					\
	symbol.c				\
	util.c					\
	util.h					\
	lkc.h					\
	lkc_proto.h				\
	Kconfig-language.txt			\
	lxdialog/BIG.FAT.WARNING

noinst_LIBRARIES = libzconf.a lxdialog/liblxdialog.a
lxdialog_liblxdialog_a_SOURCES =		\
	lxdialog/checklist.c			\
	lxdialog/menubox.c			\
	lxdialog/textbox.c			\
	lxdialog/yesno.c			\
	lxdialog/inputbox.c			\
	lxdialog/util.c				\
	lxdialog/msgbox.c			\
	lxdialog/colors.h			\
	lxdialog/dialog.h
lxdialog_liblxdialog_a_CPPFLAGS =		\
	$(COMMONCPPFLAGS)
lxdialog_liblxdialog_a_CFLAGS   =		\
	$(COMMONCFLAGS)				\
	$(CURSES_CFLAGS)

BUILT_SOURCES =					\
	zconf-parser.h

## Including zconf-parser.h into the libzconf_a_SOURCES variable solves
## the chicken-egg problem in the 'distcheck' target (generation of
## zconf-parser.h file)
libzconf_a_SOURCES  =				\
	lkc_defs.h				\
	zconf-parser.y				\
	zconf-scanner.l
libzconf_a_CPPFLAGS =				\
	$(COMMONCPPFLAGS)
libzconf_a_CFLAGS =				\
	$(COMMONCFLAGS)

conf_SOURCES  =					\
	conf.c
conf_CPPFLAGS = $(COMMONCPPFLAGS)
conf_CFLAGS   = $(COMMONCFLAGS)
conf_LDADD    =					\
	$(COMMONLDADD)				\
	libzconf.a 

mconf_SOURCES  =				\
	mconf.c
mconf_CPPFLAGS = $(COMMONCPPFLAGS)
mconf_CFLAGS   = $(COMMONCFLAGS)
mconf_LDADD    =				\
	$(COMMONLDADD)				\
	libzconf.a				\
	lxdialog/liblxdialog.a			\
	$(CURSES_LIBS)

lkc_defs.h: Makefile $(srcdir)/lkc_proto.h
	$(SED) < $(srcdir)/lkc_proto.h > lkc_defs.h 's/P(\([^,]*\),.*/#define \1 (\*\1_p)/'

CLEANFILES += lkc_defs.h
