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
include $(srcdir)/src/Makefile.def

ACLOCAL_AMFLAGS = -I tools/autotools/m4

SUBDIRS =					\
	tools					\
	.					\
	src					\
	docs					\
	site

##
## NOTE:
##     Options listed in AUTOMAKE_OPTIONS variable are applied on a
##     per-`Makefile' basis.
##
AUTOMAKE_OPTIONS = check-news

##
## Configuration related targets
##
default-config-load:
	$(MAKE) default-config-load -C src

default-config-save:
	$(MAKE) default-config-save -C src

menu-config:
	$(MAKE) menu-config -C src

text-config:
	$(MAKE) text-config -C src

##
## Miscellaneous targets
##
stats:
	$(MAKE) stats -C src

debug:
	$(MAKE) system.map -C src

##
## XXX FIXME: The "iso" rules are temporary and for debugging purposes only.
##            Please move them somewhere else ASAP
##
if GRUB1_SUPPORT
image-grub1.iso: all Makefile
	rm -r -f image-fs && \
	mkdir image-fs && \
	cp src/kernel image-fs && \
	( cd image-fs &&	\
	  $(abs_top_builddir)/tools/scripts/mkiso-grub1 && \
	  cp image.iso $(abs_top_builddir)/image-grub1.iso ) || { \
		rm -r -f image-fs ; \
		exit 1 ; \
	} ; \
	rm -r -f image-fs
else
image-grub1.iso: all Makefile
endif
CLEANFILES += image-grub1.iso

if GRUB2_SUPPORT
image-grub2.iso: all Makefile
	rm -r -f image-fs && \
	mkdir image-fs && \
	cp src/kernel image-fs && \
	( cd image-fs &&	\
	  $(abs_top_builddir)/tools/scripts/mkiso-grub2 && \
	  cp image.iso $(abs_top_builddir)/image-grub2.iso ) || { \
		rm -r -f image-fs ; \
		exit 1 ; \
	} ; \
	rm -r -f image-fs
else
image-grub2.iso: all Makefile
endif
CLEANFILES += image-grub2.iso

##
## help related target
##
help:
	@echo ""
	@echo "  Targets:"
	@echo ""
	@echo "    default-config-load -- Retrieve default configuration"
	@echo "    default-config-save -- Saves current config as default"
	@echo "    text-config         -- Interactive configuration (text)"
	@echo "    menu-config         -- Interactive configuration (dialog)"
	@echo "    old-config          -- Reconfigure using old configuration"
	@echo "    stats               -- Dump some stats"
	@echo "    debug               -- Generate (useful) debugging files"
	@echo "    doxygen             -- Build doxygen documentation (in docs directory)"
	@echo ""
	@echo "    *automake*          -- Standard automake targets"
	@echo ""

doxygen: all
	(cd docs && $(MAKE) $(AM_MAKEFLAGS) doxygen)

##
## NOTE:
##     Makefile.sub is automatically distributed by automake rules
##
EXTRA_DIST +=							\
	BUGS TODO README NEWS COPYING AUTHORS INSTALL THANKS	\
	bootstrap						\
	configure.ac.in

##
## Maintainer related targets
##

announcement:
	$(ANNOUNCE_GEN)				\
	  --release-type=alpha			\
	  --package=$(PACKAGE_TARNAME)		\
	  --prev=$(PACKAGE_VERSION).test	\
	  --curr=$(PACKAGE_VERSION)		\
	  --gpg-key-id=TEST			\
	  --url-dir=TEST

dist-hook:
	$(GITLOG_TO_CHANGELOG) > $(distdir)/ChangeLog || {	\
		rm -f ChangeLog ;				\
		exit 1 ;					\
	}

site: all
	(cd site && $(MAKE) $(AM_MAKEFLAGS) site)

HOST = "http://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob_plain;hb=HEAD;f="

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
	}
	rm -r -f fetch-dir

update-local: Makefile update-download

maintainer-check-local: Makefile
