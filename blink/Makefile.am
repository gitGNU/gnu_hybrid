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
	src					\
	docs

##
## NOTE:
##     Options listed in AUTOMAKE_OPTIONS variable are applied on a
##     per-`Makefile' basis.
##
AUTOMAKE_OPTIONS = check-news

##
## NOTE:
##     Makefile.sub is automatically distributed by automake rules
##
EXTRA_DIST =						\
	TODO NEWS 					\
	BUGS README COPYING AUTHORS INSTALL THANKS	\
	bootstrap					\
	configure.ac.in					\
	.todo

##
## Maintainer related targets
##

DNT_DB     = $(top_builddir)/.dnt.db
DEVTODO_DB = $(top_srcdir)/.todo
DNT_FLAGS  = --database=$(DNT_DB) --mono

$(DNT_DB): $(DEVTODO_DB)
	$(DEVTODO_TO_DNT)			\
		--input=$(DEVTODO_DB)		\
		--output=$(DNT_DB)		\
		--force || {			\
		rm -f $(DNT_DB) ;		\
		exit 1 ;			\
	}

TODO: $(DNT_DB)
	$(DNT) $(DNT_FLAGS)			\
	       show				\
	       --filter=not-done		\
	       --line-format="* %t"		\
	       --width=0			\
	       --indent-fill=""			\
	       --unindent-fill=""		\
	       --level-fill=""			\
	       --output=TODO || {		\
		rm -f TODO ;			\
		exit 1 ;			\
	}

NEWS: $(DNT_DB)
	touch NEWS
	echo "${PACKAGE_VERSION}:" >> NEWS
	script="$(top_builddir)/tools/maint/gittag-to-date";	\
	r_head="";						\
	d_head="";						\
	r_tail="";						\
	d_tail="";						\
	for i in `$$script | $$SED 's/\ /:/g' | sort -r -k 1`;	\
	do							\
		if test "x$$r_head" = "x"; then			\
			r_head="`echo $$i | sed 's/^.*://'`"; 	\
			d_head="`echo $$i | sed 's/:.*$$//'`";	\
			continue;				\
		else						\
			if test "x$$d_head" = "x"; then		\
				rm -f NEWS;			\
				exit 1;				\
			fi;					\
		fi;						\
		r_tail="`echo $$i | sed 's/^.*://'`";		\
		d_tail="`echo $$i | sed 's/:.*$$//'`";		\
		echo "$$r_head:" >> NEWS;			\
		echo "* $$d_tail -> $$d_head" >> NEWS;		\
		r_head="$$r_tail";				\
		d_head="$$d_tail";				\
	done || {						\
		rm -f NEWS ;					\
		exit 1;						\
	};							\
	d_tail="1970-01-01";					\
	echo "$$r_head:" >> NEWS;				\
	echo "* $$d_tail -> $$d_head" >> NEWS

announcement:
	$(ANNOUNCE_GEN)				\
	  --release-type=alpha			\
	  --package=$(PACKAGE_TARNAME)		\
	  --prev=$(PACKAGE_VERSION).test	\
	  --curr=$(PACKAGE_VERSION)		\
	  --gpg-key-id=TEST			\
	  --url-dir=TEST

dist-hook: NEWS
	$(GITLOG_TO_CHANGELOG) > $(distdir)/ChangeLog || {	\
		rm -f ChangeLog ;				\
		exit 1 ;					\
	}

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
