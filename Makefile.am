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

##
## XXX FIXME: The "iso" rules are temporary and for debugging purposes only.
##            Please move them somewhere else ASAP
##
debug: image-grub1.iso image-grub2.iso Makefile
	$(MAKE) system.map -C src

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
	@echo ""
	@echo "    *automake*          -- Standard automake targets"
	@echo ""

##
## NOTE:
##     ChangeLogs are shipped by this directory Makefile.am even if they lie on
##     child directory ... CHANGELOGS variable is needed here for distribution
##     purposes ...
##
CHANGELOGS =					\
	ChangeLog

##
## NOTE:
##     Makefile.sub is automatically distributed by automake rules
##
EXTRA_DIST +=							\
	BUGS TODO README NEWS COPYING AUTHORS INSTALL THANKS	\
	$(CHANGELOGS)						\
	bootstrap						\
	configure.ac.in

##
## distribution related target
##
#NEWS: $(SCRIPTSDIR)/CL2NEWS Makefile $(CHANGELOGS)
#	 ( input="" ;					 \
#	   for j in $(CHANGELOGS) ; do			 \
#		 input="$$input $(srcdir)/$$j" ;	 \
#	   done ;					 \
#	   $(SCRIPTSDIR)/CL2NEWS -- $$input > NEWS ) ||	 \
#	 { rm -f NEWS ; exit 1 ; }

ChangeLog: $(srcdir)/NEWS
	HEAD_TAG=`$(SCRIPTSDIR)/NEWS-query $(srcdir)/NEWS 0` ;	\
	PREV_TAG=`$(SCRIPTSDIR)/NEWS-query $(srcdir)/NEWS 1` ;	\
	$(SCRIPTSDIR)/gitlog2ChangeLog $$PREV_TAG..$$HEAD_TAG > \
		ChangeLog || {					\
		rm -f ChangeLog ;				\
		exit 1;						\
	}

#dist-hook: ChangeLog NEWS
#	cp ChangeLog $(distdir)/ChangeLog || {	\
#		rm -f $(distdir)/ChangeLog ;	\
#		exit 1 ;			\
#	}
#	cp NEWS $(distdir)/NEWS || {		\
#		rm -f $(distdir)/NEWS ;		\
#		exit 1 ;			\
#	}

##
## NOTE:
##     DISTCLEANFILES already defined in Makefile.def
##
#DISTCLEANFILES =

##
## Maintainer related targets
##
site: all
	(cd site && $(MAKE) $(AM_MAKEFLAGS) site)

HOST = "http://git.savannah.gnu.org/gitweb/?p=gnulib.git;a=blob_plain;hb=HEAD;f=doc"

DOWNLOAD =					\
	INSTALL					\
	COPYINGv2

update-fixups:
	mv -f COPYINGv2 COPYING

update-download:
	rm -r -f fetch-dir && \
	mkdir fetch-dir && \
	for i in $(DOWNLOAD) ; do \
		$(FETCH) $(HOST)/$$i fetch-dir/$$i || { \
			rm -f fetch-dir/$$i ; \
			exit 1 ; \
		} \
	done && \
	for i in $(DOWNLOAD) ; do \
		mv -f fetch-dir/$$i $$i || { \
			exit 1 ; \
		} \
	done && \
	rm -r -f fetch-dir

update-local: Makefile update-download update-fixups

maintainer-check-local: Makefile
