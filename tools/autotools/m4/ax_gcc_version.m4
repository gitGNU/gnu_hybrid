#####
#
# SYNOPSIS
#
#   AX_GCC_VERSION
#
# DESCRIPTION
#
#   AX_GCC_VERSION retrieves the gcc version. Returns the version in the
#   GCC_VERSION variable if available, an empty string otherwise.
#
# LAST MODIFICATION
#
#   2008-03-06
#
# COPYLEFT
#
#   Copyright (C) 2008 Francesco Salvestrini
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

AC_DEFUN([AX_GCC_VERSION], [
  GCC_VERSION=""
  AX_GCC_OPTION([-dumpversion],[],[],[
    ax_gcc_version_option=yes
  ],[
    ax_gcc_version_option=no
  ])
  AS_IF([test "x$GCC" = "xyes"],[
    AS_IF([test "x$ax_gcc_version_option" != "no"],[
      AC_CACHE_CHECK([gcc version],[ax_cv_gcc_version],[
        ax_cv_gcc_version="`$CC -dumpversion`"
        AS_IF([test "x$ax_cv_gcc_version" = "x"],[
          ax_cv_gcc_version=""
        ])
      ])
      GCC_VERSION=$ax_cv_gcc_version
    ])
  ])
  AC_SUBST([GCC_VERSION])
])
