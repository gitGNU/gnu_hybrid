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
include $(srcdir)/src/Makefile.def

EXTRA_DIST =
BUILT_SOURCES =
CLEANFILES =
MAINTAINERCLEANFILES =

include $(top_srcdir)/Makefile.top

ACLOCAL_AMFLAGS = -I tools/autotools/m4 --install

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
## NOTE:
##     Makefile.am included files are automatically distributed by automake
##     rules
##
EXTRA_DIST +=					\
	README COPYING INSTALL THANKS		\
	bootstrap .bootstrap bootstrap.aux	\
	perl.af

##
## Configuration related targets
##
default-config-load:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) default-config-load)

default-config-save:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) default-config-save)

menu-config:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) menu-config)

text-config:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) text-config)

##
## Miscellaneous targets
##
stats:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) stats)

map:
	(cd src && $(MAKE) $(AM_MAKEFLAGS) map)

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
	@echo "    stats               -- Dump some statistics"
	@echo "    map                 -- Generate debugging files"
	@echo "    doxygen             -- Build doxygen documentation (in docs directory)"
	@echo ""
	@echo "    *automake*          -- Standard automake targets"
	@echo ""

doxygen: all
	(cd docs && $(MAKE) $(AM_MAKEFLAGS) doxygen)

##
## Maintainer related targets
##

site: all
	(cd site && $(MAKE) $(AM_MAKEFLAGS) site)

URL_BU = "http://github.com/salvestrini/build-utils/tree/master/maint"

EXTERNAL =                                      \
        $(URL_BU)/bootstrap

update-local: Makefile
	rm -r -f fetch-dir && \
	mkdir fetch-dir && \
	for i in $(EXTERNAL) ; do \
	        URL="$$i" ; \
	        NAME=`echo "$$i" | $(SED) -e 's,^.*/,,'` ; \
	        $(FETCH) $$URL?raw=true fetch-dir/$$NAME && \
	        mv fetch-dir/$$NAME $$NAME || { \
	                rm -f fetch-dir/$$NAME ; \
	                exit 1 ; \
	        } \
	done && \
	rm -r -f fetch-dir
	chmod a+x bootstrap || { exit 1 ; }

maintainer-check-local: Makefile
