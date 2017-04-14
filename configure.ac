AC_INIT([tidypath], [0.0.4])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([src/tidypath.c])

AC_USE_SYSTEM_EXTENSIONS
LT_INIT

AC_ARG_ENABLE(
    [free],
    [AS_HELP_STRING([--enable-free], [free internally allocated memory @<:@default is no@:>@])],
    [AS_IF([test "${enableval}" = "yes"], [AC_DEFINE(FREE_MEMORY)])]
)

AC_PROG_CC
AC_PROG_CC_C99

AC_CHECK_FUNC([strchrnul], [], [NEED_STRCHRNUL=1])
AM_CONDITIONAL([NEED_STRCHRNUL], [test "$NEED_STRCHRNUL" = "1"])
AS_IF([test "$NEED_STRCHRNUL" = "1"], [AC_DEFINE([NEED_STRCHRNUL])])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
