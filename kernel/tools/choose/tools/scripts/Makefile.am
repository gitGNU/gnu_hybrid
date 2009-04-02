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

EXTRA_DIST =                                    \
	fetch.in                                \
	gitlog2ChangeLog.in                     \
	NEWS-query.in				\
	whitespace-detect.in

noinst_SCRIPTS =                                \
	fetch                                   \
	gitlog2ChangeLog                        \
	NEWS-query				\
	whitespace-detect

CLEANFILES = $(noinst_SCRIPTS)

NEWS-query: NEWS-query.in Makefile
	( rm -f $@ $@.tmp && \
	  $(do_subst) < $(srcdir)/NEWS-query.in > $@.tmp && \
	  chmod +x $@.tmp && \
	  chmod a-w $@.tmp && \
	  mv -f $@.tmp $@ ) || { \
	        rm -f $@ $@.tmp ; \
	        exit 1 ; \
	}

gitlog2ChangeLog: gitlog2ChangeLog.in Makefile
	( rm -f $@ $@.tmp && \
	  $(do_subst) < $(srcdir)/gitlog2ChangeLog.in > $@.tmp && \
	  chmod +x $@.tmp && \
	  chmod a-w $@.tmp && \
	  mv -f $@.tmp $@ ) || { \
	        rm -f $@ $@.tmp ; \
	        exit 1 ; \
	}

fetch: fetch.in Makefile
	( rm -f $@ $@.tmp && \
	  $(do_subst) < $(srcdir)/fetch.in > $@.tmp && \
	  chmod +x $@.tmp && \
	  chmod a-w $@.tmp && \
	  mv -f $@.tmp $@ ) || { \
	        rm -f $@ $@.tmp ; \
	        exit 1 ; \
	}

whitespace-detect: whitespace-detect.in Makefile
	( rm -f $@ $@.tmp && \
	  $(do_subst) < $(srcdir)/whitespace-detect.in > $@.tmp && \
	  chmod +x $@.tmp && \
	  chmod a-w $@.tmp && \
	  mv -f $@.tmp $@ ) || { \
	        rm -f $@ $@.tmp ; \
	        exit 1 ; \
	}

##
## Maintainer related targets
##
update-local: Makefile

maintainer-check-local: Makefile
