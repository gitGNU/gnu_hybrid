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
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along
## with this program; if not, write to the Free Software Foundation, Inc.,
## 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##

include $(top_srcdir)/Makefile.sub

ACLOCAL_AMFLAGS = -I tools/autotools/m4

SUBDIRS =					\
	tools					\
	blink					\
	kernel

help:
	@echo ""
	@echo "  Targets:"
	@echo ""
	@echo "    image.iso           -- Generate a bootable ISO image"
	@echo ""
	@echo "    *automake*          -- Standard automake targets"
	@echo ""

CLEANFILES =

if GRUB1_SUPPORT
image.iso: Makefile all
	rm -r -f image-fs && \
	mkdir image-fs && \
	cp $(abs_top_builddir)/blink/src/blink   image-fs && \
	cp $(abs_top_builddir)/kernel/src/kernel image-fs && \
	( cd image-fs &&	\
	  $(abs_top_builddir)/tools/scripts/mkiso-grub1 -o $(abs_top_builddir)/image.iso ) || { \
		rm -r -f image-fs ; \
		exit 1 ; \
	} && \
	rm -r -f image-fs

CLEANFILES += image.iso
else
image.iso: all Makefile
endif

if GRUB2_SUPPORT
image.iso: Makefile all
	rm -r -f image-fs && \
	mkdir image-fs && \
	cp $(abs_top_builddir)/blink/src/blink   image-fs && \
	cp $(abs_top_builddir)/kernel/src/kernel image-fs && \
	( cd image-fs &&	\
	  $(abs_top_builddir)/tools/scripts/mkiso-grub2 -o $(abs_top_builddir)/image.iso ) || { \
		rm -r -f image-fs ; \
		exit 1 ; \
	} && \
	rm -r -f image-fs

CLEANFILES += image.iso
else
image.iso: all Makefile
endif

##
## Maintainer related targets
##

site: all
	(cd site && $(MAKE) $(AM_MAKEFLAGS) site)

update-download:
	rm -r -f fetch-dir && \
	mkdir fetch-dir && \
	$(FETCH) $(HOST)doc/INSTALL fetch-dir/INSTALL && \
	chmod a+r fetch-dir/INSTALL && \
	chmod u+w fetch-dir/INSTALL && \
	$(FETCH) $(HOST)doc/COPYINGv2 fetch-dir/COPYING && \
	chmod a+r fetch-dir/COPYING && \
	chmod u+w fetch-dir/COPYING && \
	mv -f fetch-dir/* ./ || { \
		rm -r -f fetch-dir ; \
		exit 1 ; \
	} && \
	rm -r -f fetch-dir

update-local: Makefile update-download

maintainer-check-local: Makefile
