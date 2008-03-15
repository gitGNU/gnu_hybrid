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

HOST = "http://autoconf-archive.cryp.to"

DOWNLOAD =					\
	ax_prog_bison.m4			\
	ax_prog_flex.m4				\
	ax_path_missing.m4			\
	ax_missing_prog.m4			\
	mp_with_curses.m4

MACROS =					\
	$(DOWNLOAD)

FETCH = $(top_builddir)/tools/scripts/fetch

update:
	rm -r -f fetch-dir && \
	mkdir fetch-dir && \
	for i in $(DOWNLOAD) ; do \
		$(FETCH) $(HOST)/$$i fetch-dir/$$i || { \
			rm -f fetch-dir/$$i ; \
			exit 1 ; \
		} \
	done && \
	for i in $(DOWNLOAD) ; do \
		mv fetch-dir/$$i $$i || { \
			exit 1 ; \
		} \
	done && \
	rm -r -f fetch-dir
