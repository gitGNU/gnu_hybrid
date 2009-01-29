##
## Input file for automake
##
## Copyright (C) 2007, 2008 Francesco Salvestrini
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

SITE_SRCS =					\
	contents.as				\
	files.as				\
	footer.sxml				\
	header.sxml				\
	layout.sxml				\
	map.as					\
	pages.as				\
	site.as					\
	site.css				\
						\
	index.scm				\
	installation.scm			\
	downloads.scm				\
	development.scm				\
	todos.sxml

EXTRA_DIST  = $(SITE_SRCS)
CLEAN_FILES =

todos.dump: $(top_srcdir)/.dnt.db
	$(DNT) --database=$(top_srcdir)/.dnt.db \
	       --mono \
	       dump \
	       --format="(li \"%t\")" \
	       --width=0 \
	       --output=todos.dump || { \
		rm -f todos.dump ; \
		exit 1 ; \
	}
CLEAN_FILES += todos.dump

todos.sxml: todos.dump
	rm -f todos.sxml && \
	touch todos.sxml && \
	echo "(h2 \"TODOs\")" >> todos.sxml && \
	echo "(ul"            >> todos.sxml && \
	cat todos.dump        >> todos.sxml && \
	echo ")"              >> todos.sxml || { \
		rm -f todos.sxml ; \
		exit 1 ; \
	}
CLEAN_FILES += todos.sxml

site: $(SITE_SRCS)
	$(SITETOOL) -v --mode initialize && \
	$(SITETOOL) -v --mode build && \
	$(SITETOOL) -v --mode validate && \
	touch site.stamp || { \
		$(SITETOOL) -v --mode clean ; \
		rm -f site.stamp ; \
		exit 1 ; \
	}

maintainer-clean-local:
	if test -e site.stamp ; then \
		$(SITETOOL) --mode clean || exit 1 ; \
		rm -f site.stamp ; \
	fi

##
## Maintainer related targets
##
update-local: Makefile

maintainer-check-local: Makefile
