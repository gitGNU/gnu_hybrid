##### http://autoconf-archive.cryp.to/ax_check_awk_or.html
#
# SYNOPSIS
#
#   AX_AWK_REQUIRED
#
# DESCRIPTION
#
#   Dump an error/warning message when a required functionality has not been
#   found
#
# LAST MODIFICATION
#
#  2007-08-17
#
# COPYLEFT
#
#  Copyright (c) 2007 Francesco Salvestrini <salvestrini@users.sourceforge.net>
#
#  Copying and distribution of this file, with or without
#  modification, are permitted in any medium without royalty provided
#  the copyright notice and this notice are preserved
#
##########################################################################
AC_DEFUN([AX_AWK_FUNCTIONALITY_BARRIER], [
  AC_ARG_ENABLE([awk_strict],
    AS_HELP_STRING([--disable-awk-checks], [disable awk related checks]), [
      AC_MSG_NOTICE([awk functionality checks disabled])
      disable_awk_barriers=yes
  ],[
      AC_MSG_NOTICE([awk functionality checks enabled])
      disable_awk_barriers=no
  ])
])

AC_DEFUN([AX_AWK_REQUIRED_FUNCTIONALITY_BARRIER], [
  AC_REQUIRE([AX_AWK_FUNCTIONALITY_BARRIER])
  AS_IF([test "x$disable_awk_barriers" = "xyes"],[
    AC_MSG_WARN([required awk functionality seems missing])
  ],[
    AC_MSG_ERROR([required awk functionality seems missing])
  ])
])

AC_DEFUN([AX_AWK_EXPECTED_FUNCTIONALITY_BARRIER], [
  AC_REQUIRE([AX_AWK_FUNCTIONALITY_BARRIER])
  AC_MSG_WARN([expected awk functionality seems missing])
])
