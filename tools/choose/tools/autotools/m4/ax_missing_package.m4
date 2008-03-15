## Autoconf macros for missing package error printing.
##
## Author: Francesco Salvestrini <salvestrini@users.sourceforge.net>

## Index
## -----
##
## AX_MISSING_PACKAGE -- dump an error message

## Code
## ----

# AX_MISSING_PACKAGE -- dump an error message
#
# Usage: AX_MISSING_PACKAGE(package-name,[package-URL])
#
AC_DEFUN([AX_MISSING_PACKAGE],[dnl
  if test -z "$2" ; then
	AC_MSG_ERROR([
***
*** $1 package seems missing. In order to continue it must be installed.
***
])
  else
	AC_MSG_ERROR([
***
*** $1 package seems missing. In order to continue it must be installed.
*** It should be found at the following address:
***
***     $2
***
])
  fi
])
## ax_missing_package.m4 ends here
