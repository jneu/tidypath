# Needed for libtool
ACLOCAL_AMFLAGS = -I m4

#
# tidypath
#

bin_PROGRAMS = tidypath

tidypath_SOURCES = src/tidypath.c src/main.c
if NEED_STRCHRNUL
tidypath_SOURCES += src/compat/strchrnul.c src/compat/strchrnul.h
endif

# This nop hack is needed since the same sources are used for both tidypath and libtidypath
tidypath_CFLAGS = $(AM_CFLAGS)

#
# libtidypath
#

lib_LTLIBRARIES = libtidypath.la

libtidypath_la_SOURCES = src/tidypath.c
if NEED_STRCHRNUL
libtidypath_la_SOURCES += src/compat/strchrnul.c src/compat/strchrnul.h
endif

#
# Headers to be installed
#

include_HEADERS = src/tidypath.h

#
# Tests to be run
#

TESTS = test/basic.test test/nop.test test/missing_env.test
XFAIL_TESTS = test/missing_env.test

#
# Extras to include in `make dist`
#

EXTRA_DIST = .gitignore LICENSE README.md bootstrap $(TESTS)
