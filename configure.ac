#
# Autoconf (and automake and libtool) configuration for tidypath
#

AC_INIT([tidypath], [0.1.5])
AM_INIT_AUTOMAKE([foreign subdir-objects])

# Needed for libtool
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/tidypath.c])

#
# libtool support
#

AC_USE_SYSTEM_EXTENSIONS
LT_INIT

#
# Extra args for configure
#

AC_ARG_ENABLE(
    [free],
    [AS_HELP_STRING([--enable-free], [free internally allocated memory @<:@default is no@:>@])],
    [AS_IF([test "${enableval}" = "yes"], [AC_DEFINE(FREE_MEMORY)])]
)

#
# tidypath uses c99
#

AC_PROG_CC
AC_PROG_CC_C99

# 
# Use the native strchrnul if we have it
#

AC_CHECK_FUNC([strchrnul], [], [NEED_STRCHRNUL=1])
AS_IF([test "$NEED_STRCHRNUL" = "1"], [AC_DEFINE([NEED_STRCHRNUL])])

# AM_CONDITIONAL needs to be at the top level
AM_CONDITIONAL([NEED_STRCHRNUL], [test "$NEED_STRCHRNUL" = "1"])

#
# Generate output files
#

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
