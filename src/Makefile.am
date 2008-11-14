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

include $(srcdir)/Makefile.def
include $(srcdir)/config/Makefile.inc
include $(srcdir)/libs/Makefile.inc
include $(srcdir)/core/Makefile.inc
include $(srcdir)/mem/Makefile.inc
include $(srcdir)/archs/Makefile.inc
include $(srcdir)/dbg/Makefile.inc

EXTRA_DIST += rules.cfg

##
## Configuration related targets
##
CFG_ALL  = $(builddir)/rules.pkg
CFG_USER = $(USERCONFIG)

CFG_CLEANFILES           = $(CFG_ALL)
CFG_MAINTAINERCLEANFILES = $(CFG_ALL) $(CFG_USER) $(CFG_USER).old $(CFG_USER).new

DEFAULT_CFG = $(top_srcdir)/src/archs/$(architecture)/bsp/configs/default.usr
CURRENT_CFG = $(CFG_USER)

default-config-load:
	cp $(DEFAULT_CFG) $(CURRENT_CFG)

default-config-save:
	cp $(CURRENT_CFG) $(DEFAULT_CFG)

##
## NOTE:
##     We need some tools built before starting interactive configuration.
##     We could selectively build them here but we prefer to rebuild all the
##     tools in order to avoid hardwiring dependencies ...
##
pre-config:
	$(MAKE) all -C $(top_builddir)/tools

$(CFG_ALL): $(CFG_FILES)
	$(PACKRULES) $(PACKRULES_FLAGS) -p $(top_srcdir)/src -i archs/$(architecture)/rules.cfg -o $(CFG_ALL) || {	\
		rm -f $(CFG_ALL) ;											\
		exit 1;													\
	}

menu-config: pre-config $(CFG_ALL)
	{																\
		if test ! -f $(CFG_USER) ; then												\
			touch $(CFG_USER) ;												\
		fi ;															\
		cp -f $(CFG_USER) $(CFG_USER).old ;											\
		cp -f $(CFG_USER) $(CFG_USER).new ;											\
		$(CHOOSE) $(CHOOSE_FLAGS) -m curses -p $(CHOOSEDIR)/lkc -r $(CFG_ALL) -i $(CFG_USER).old -o $(CFG_USER).new || exit 1 ;	\
		cp -f $(CFG_USER).new $(CFG_USER) ;											\
	} || {																\
		rm -f $(CFG_USER).old ;													\
		rm -f $(CFG_USER).new ;													\
		exit 1 ;														\
	}
	rm -f $(CFG_USER).old
	rm -f $(CFG_USER).new

text-config: pre-config $(CFG_ALL)
	{																\
		if test ! -f $(CFG_USER) ; then												\
			touch $(CFG_USER) ;												\
		fi ;															\
		cp -f $(CFG_USER) $(CFG_USER).old ;											\
		cp -f $(CFG_USER) $(CFG_USER).new ;											\
		$(CHOOSE) $(CHOOSE_FLAGS) -m text -p $(CHOOSEDIR)/lkc -r $(CFG_ALL) -i $(CFG_USER).old -o $(CFG_USER).new || exit 1 ;	\
		cp -f $(CFG_USER).new $(CFG_USER) ;											\
	} || {																\
		rm -f $(CFG_USER).old ;													\
		rm -f $(CFG_USER).new ;													\
		exit 1 ;														\
	}
	rm -f $(CFG_USER).old
	rm -f $(CFG_USER).new

CLEANFILES           += $(CFG_CLEANFILES)
MAINTAINERCLEANFILES += $(CFG_MAINTAINERCLEANFILES)

##
## kernel related
##
noinst_PROGRAMS  = kernel$(EXEEXT)
noinst_LIBRARIES = $(MODULES_BUILTIN)


BUILD_FILE   = core/build.h
BUILD_NUMBER = `if test -e "$(BUILD_FILE)" ; then $(GETDEFINE) $(GETDEFINE_FLAGS) -s BUILD_NUMBER -i $(BUILD_FILE) ; else echo 0 ; fi`
## Rebuilt every time BUILD_FILE but don't ship it because it will be
## re-built at the first make ...
.PHONY:	$(BUILD_FILE)
$(BUILD_FILE):
	$(MKBUILD) $(MKBUILD_FLAGS) -g BUILD_H -F $(BUILD_FILE)
BUILT_SOURCES        += $(BUILD_FILE)
MAINTAINERCLEANFILES += $(BUILD_FILE)

##
## NOTE:
##     kernel.ld and module.ld are obtained from kernel.ld.in and module.ld.in
##     in the arch related directory. CONFIG_LMA and CONFIG_VMA get exported
##     in the procedure
##
CONFIG_LMA = `$(GETDEFINE) $(GETDEFINE_FLAGS) -i $(top_builddir)/src/config/userconfig.h -s CONFIG_LMA`
CONFIG_VMA = `$(GETDEFINE) $(GETDEFINE_FLAGS) -i $(top_builddir)/src/config/userconfig.h -s CONFIG_VMA`

KERNELLDSCRIPT = $(top_builddir)/src/archs/$(architecture)/kernel.ld
$(top_builddir)/src/archs/$(architecture)/kernel.ld: Makefile $(top_srcdir)/src/archs/$(architecture)/kernel.ld.in
	( rm -f $@ $@.tmp && \
	  $(SED) -e 's,[@]CONFIG_LMA[@],'$(CONFIG_LMA)',g' \
		 -e 's,[@]CONFIG_VMA[@],'$(CONFIG_VMA)',g' \
	  < $(top_srcdir)/src/archs/$(architecture)/kernel.ld.in > $@.tmp && \
	  chmod -x $@.tmp && \
	  chmod a-w $@.tmp && \
	  mv -f $@.tmp $@ ) || { \
	        rm -f $@ $@.tmp ; \
	        exit 1 ; \
	}
CLEANFILES += $(KERNELLDSCRIPT)

MODULESLDSCRIPT = $(top_builddir)/src/archs/$(architecture)/module.ld
$(top_builddir)/src/archs/$(architecture)/module.ld: Makefile $(top_srcdir)/src/archs/$(architecture)/module.ld.in
	( rm -f $@ $@.tmp && \
	  $(SED) -e 's,[@]CONFIG_LMA[@],'$(CONFIG_LMA)',g' \
		 -e 's,[@]CONFIG_VMA[@],'$(CONFIG_VMA)',g' \
	  < $(top_srcdir)/src/archs/$(architecture)/module.ld.in > $@.tmp && \
	  chmod -x $@.tmp && \
	  chmod a-w $@.tmp && \
	  mv -f $@.tmp $@ ) || { \
	        rm -f $@ $@.tmp ; \
	        exit 1 ; \
	}
CLEANFILES += $(MODULESLDSCRIPT)

##
## NOTE:
##    The arch multiboot sources must came first in order to have a multiboot
##    compliant image
##
kernel_SOURCES      =				\
	$(arch_multiboot_SRCS)			\
	$(arch_SRCS)				\
	$(boot_SRCS)				\
	$(mem_SRCS)				\
	$(libs_SRCS)				\
	$(core_SRCS)				\
	$(dbg_SRCS)
kernel_LDADD        = $(MODULES_BUILTIN) $(ELKLIB_LDADD)
kernel_DEPENDENCIES = $(KERNELLDSCRIPT) $(CFG_ALL)
kernel_LDFLAGS      = $(KERNELLDFLAGS)
kernel_CCASFLAGS    = $(KERNELCCASFLAGS)
kernel_CPPFLAGS     = $(KERNELCPPFLAGS)
kernel_CFLAGS       = $(KERNELCFLAGS)
kernel_CXXFLAGS     = $(KERNELCXXFLAGS)

##
## modules related
##
#MODULES_DB  = modules/modules.db
#CLEANFILES += $(MODULES_DB)
#
#$(MODULES_DB): Makefile $(MODULES_AUTOLOAD) $(MODULES_PRELOAD)
#	rm -f $(MODULES_DB) || exit 1 ; \
#	MODDBL="$(MODDB) $(MODDB_FLAGS) --database $(MODULES_DB)" ; \
#	$$MODDBL --create || \
#		{ rm -f $(MODULES_DB) ; exit 1 ; } ; \
#	for i in $$MODULES ; do \
#		$$MODDBL --add   $$i                || \
#			{ rm -f $(MODULES_DB) ; exit 1 ; } ; \
#		$$MODDBL --flags $$i --set autoload || \
#			{ rm -f $(MODULES_DB) ; exit 1 ; } ; \
#	done ; \
#	for i in $$MODULES ; do \
#		$$MODDBL --add   $$i               || \
#			{ rm -f $(MODULES_DB) ; exit 1 ; } ; \
#		$$MODDBL --flags $$i --set preload || \
#			{ rm -f $(MODULES_DB) ; exit 1 ; } ; \
#	done

##
## map related
##
MAPFILE     = system.map
CLEANFILES += $(MAPFILE)
$(MAPFILE): Makefile $(SYMSFILE) $(SYMSTAB2MAP)
	$(SYMSTAB2MAP) $(SYMSTAB2MAP_FLAGS) -i $(SYMSFILE) -o $(MAPFILE) || { \
		rm -f $(MAPFILE) ; \
		exit 1 ; \
	}

SYMSFILE    = system.sym
CLEANFILES += $(SYMSFILE)
$(SYMSFILE): Makefile kernel$(EXEEXT) $(MODULES_OBJS) $(OBJ2SYMSTAB)
	$(OBJ2SYMSTAB) $(OBJ2SYMSTAB_FLAGS) -m -X -n -o $(SYMSFILE) -- $(KERNEL_OBJ) $(MODULES_OBJS) || { \
		rm -f $(SYMSFILE) ; \
		exit 1 ; \
	} 

##
## statistics related
##
stats: all
	@echo ""
	@echo "  Build number:" $(BUILD_NUMBER)
	@echo ""
	@echo "  Sections:"
	@echo ""
	@$(SECTIONS) $(SECTIONS_FLAGS) -P -S -L -V -- $(KERNEL_OBJ) $(MODULES_OBJS) || exit 1


##
## check target related
##
STACKSIZE = `$(GETDEFINE) $(GETDEFINE_FLAGS) -i $(top_builddir)/src/config/userconfig.h -s CONFIG_PAGE_SIZE`

KERNEL_OBJ   = kernel$(EXEEXT)
#MODULES_OBJS = $(MODULES_PRELOAD) $(MODULES_AUTOLOAD)

check-includes: Makefile
	$(INCLUDESCHECK) $(INCLUDESCHECK_FLAGS) -p $(top_srcdir)/src -p $(top_builddir)/src -- $(SOURCES) || exit 1

check-ifdefs: Makefile
	$(IFDEFSCHECK) $(IFDEFSCHECK_FLAGS) -f -D $(top_builddir)/src/config/rules.all -P CONFIG_ -p $(top_srcdir)/src -p $(top_builddir)/src -- $(SOURCES) || exit 1

check-stack: Makefile
	$(STACKCHECK) $(STACKCHECK_FLAGS) -a $(architecture) -s $(STACKSIZE) -- $(KERNEL_OBJ) $(MODULES_OBJS) || exit 1

check-boot: Makefile
	$(BOOTCHECK) $(BOOTCHECK_FLAGS) -a $(architecture) -i $(KERNEL_OBJ) || exit 1

HDRS = `$(FLHANDLER) $(FLHANDLER_FLAGS) -i h -i hxx -- $(SOURCES)`
check-guards: Makefile
	$(GUARDSCHECK) $(GUARDSCHECK_FLAGS) -p $(top_srcdir)/src -p $(top_builddir)/src -- $(HDRS) || exit 1

check-sections:
	$(SECTIONS) $(SECTIONS_FLAGS) -z -f -- $(KERNEL_OBJ) $(MODULES_OBJS) || exit 1

check-local: Makefile check-stack check-sections check-includes check-ifdefs check-guards check-boot

##
## install target related
##
#install-data-local: Makefile $(MODULES_AUTOLOAD) $(MODULES_PRELOAD)
#	$(MKDIR_P) $(DESTDIR)$(MODULESdir)
#	$(INSTALL_DATA) $(MODULES_DB) $(DESTDIR)$(MODULESdir)/modules.db
#	for i in $(MODULES_PRELOAD) ; do \
#		$(INSTALL_DATA) $$i $(DESTDIR)$(MODULESdir)/$$i ; \
#	done
#	for i in $(MODULES_AUTOLOAD) ; do \
#		$(INSTALL_DATA) $$i $(DESTDIR)$(MODULESdir)/$$i ; \
#	done

install-data-hook:

install-exec-local: Makefile kernel$(EXEEXT)
	$(MKDIR_P) $(DESTDIR)$(SYSTEMdir)
	$(INSTALL_PROGRAM) kernel$(EXEEXT) $(DESTDIR)$(SYSTEMdir)/kernel-$(VERSION)

install-exec-hook:

##
## uninstall target related
##
#uninstall-local:
#	rm -f $(DESTDIR)$(SYSTEMdir)/kernel-$(VERSION)
#	for i in $(MODULES_PRELOAD) ; do \
#		 rm -f $(DESTDIR)$(MODULESdir)/$$i ; \
#	done
#	for i in $(MODULES_AUTOLOAD) ; do \
#		rm -f $(DESTDIR)$(MODULESdir)/$$i ; \
#	done
#	rm -f $(DESTDIR)$(MODULESdir)/modules.db
#	rm -r -f $(DESTDIR)$(MODULESdir)

##
## maintainer-clean target related
##
maintainer-clean-local:
	rm -f arch

##
## dist target related
##
dist-hook:
	rm -f $(distdir)/arch
	rm -f $(BUILD_FILE)

##
## Maintainer related targets
##
update-local: Makefile

maintainer-check-whitespaces: Makefile
	$(WHITESPACE_DETECT) $(SOURCES)

maintainer-check-local: Makefile maintainer-check-whitespaces
