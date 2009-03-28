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

CLEANFILES =

bin_PROGRAMS = blink

blink_multiboot_x86_SOURCES =			\
	multiboot/x86/crt0.S			\
	multiboot/x86/crt1.c			\
	multiboot/x86/asm.c			\
	multiboot/x86/asm.h

blink_multiboot_SOURCES =			\
	$(blink_multiboot_x86_SOURCES)		\
	multiboot/multiboot.c			\
	multiboot/multiboot.h

blink_SOURCES =					\
	$(blink_multiboot_SOURCES)		\
	heap.c					\
	heap.h					\
	panic.c					\
	main.c

LDSCRIPT = $(top_builddir)/src/multiboot/$(architecture)/linker.ld
$(top_builddir)/src/multiboot/$(architecture)/linker.ld: Makefile $(top_srcdir)/src/multiboot/$(architecture)/linker.ld.in
	( rm -f $@ $@.tmp && \
	  $(SED) -e 's,[@]CONFIG_LMA[@],'$(CONFIG_LMA)',g' \
		 -e 's,[@]CONFIG_VMA[@],'$(CONFIG_VMA)',g' \
	  < $(top_srcdir)/src/multiboot/$(architecture)/linker.ld.in > $@.tmp && \
	  chmod -x $@.tmp && \
	  chmod a-w $@.tmp && \
	  mv -f $@.tmp $@ ) || { \
	        rm -f $@ $@.tmp ; \
	        exit 1 ; \
	}
CLEANFILES += $(LDSCRIPT)

blink_DEPENDENCIES = $(LDSCRIPT)
blink_CCASFLAGS    =				\
	$(ELKLIB_CCASFLAGS)			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src
blink_CPPFLAGS     =				\
	$(ELKLIB_CPPFLAGS)			\
	-I$(top_srcdir)/src			\
	-I$(top_builddir)/src
blink_CFLAGS       = $(ELKLIB_CFLAGS)
blink_CXXFLAGS     = $(ELKLIB_CXXFLAGS)
blink_LDADD        = $(ELKLIB_LDADD)
blink_LDFLAGS      =				\
	$(ELKLIB_LDFLAGS)			\
	-static					\
	-T $(LDSCRIPT)				\
        -nostartfiles                           \
        -nostdlib                               \
        -nodefaultlibs                          \
        --warn-common                           \
        --warn-section-align                    \
        --warn-constructors                     \
        --no-gc-sections                        \
        --fatal-warnings                        \
        -Wl,--as-needed	

##
## Maintainer related targets
##
update-local: Makefile

all_SRCS =					\
	$(blink_SOURCES)

maintainer-check-local: Makefile
	$(WHITESPACE_DETECT)  $(all_SRCS)
