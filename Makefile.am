ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = tidypath

tidypath_SOURCES = src/tidypath.c src/main.c
if NEED_STRCHRNUL
tidypath_SOURCES += src/compat/strchrnul.c src/compat/strchrnul.h
endif
tidypath_CFLAGS = $(AM_CFLAGS)

lib_LTLIBRARIES = libtidypath.la

libtidypath_la_SOURCES = src/tidypath.c
if NEED_STRCHRNUL
libtidypath_la_SOURCES += src/compat/strchrnul.c src/compat/strchrnul.h
endif

include_HEADERS = src/tidypath.h
