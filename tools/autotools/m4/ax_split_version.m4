#####
#
# SYNOPSIS
#
#   AX_SPLIT_VERSION(VERSION)
#
# DESCRIPTION
#
#   Splits a version numberin the format MAJOR[.MINOR[.MICRO[-EXTRA]]] into
#   its separeate components and sets the variables AX_MAJOR_VERSION,
#   AX_MINOR_VERSION, AX_MICRO_VERSION and AX_EXTRA_VERSION.
#
#   This macro is based upon AX_SPLIT_VERSION macro by Tom Howard

#
# LAST MODIFICATION
#
#   2008-02-29
#
# COPYLEFT
#
#   Copyright (c) 2008 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_SPLIT_VERSION],[dnl
    AX_VERSION=$1

    AX_MAJOR_VERSION=`echo "$AX_VERSION" | \
	sed 's/^\([[0-9]]*\).*$/\1/'`
    AX_MINOR_VERSION=`echo "$AX_VERSION" | \
        sed 's/^[[0-9]]*\.\([[0-9]]*\).*$/\1/'`
    AX_MICRO_VERSION=`echo "$AX_VERSION" | \
        sed 's/^[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\).*$/\1/'`
    AX_EXTRA_VERSION=`echo "$AX_VERSION" | \
        sed 's/^[[0-9]]*\.[[0-9]]*\.[[0-9]]*-\?\(.*\)$/\1/'`
])
