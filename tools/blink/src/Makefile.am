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

include $(top_srcdir)/Makefile.sub

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = blink

blink_SOURCES =					\
	main.c
#	asm.c
#	multiboot.c

blink_LDADD        =
blink_LDFLAGS      = $(ELKLIB_LDFLAGS)
blink_CCASFLAGS    = $(ELKLIB_CCASFLAGS)
blink_CPPFLAGS     = $(ELKLIB_CPPFLAGS)
blink_CFLAGS       = $(ELKLIB_CFLAGS)
blink_CXXFLAGS     = $(ELKLIB_CXXFLAGS)

##
## Maintainer related targets
##
update-local: Makefile

all_SRCS =					\
	$(blink_SOURCES)

maintainer-check-local: Makefile
	$(WHITESPACE_DETECT)  $(all_SRCS)
