SUBDIRS = pcre kjs

# FIXME: need to build the other kjs test program too

INCLUDES = -I $(top_srcdir)/JavaScriptCore/kjs
noinst_PROGRAMS = kjs/testkjs
kjs_testkjs_SOURCES = kjs/testkjs.cpp kjs/cxx.cpp kjs/cxx.h
kjs_testkjs_DEPENDENCIES = kjs/libjscore.la
kjs_testkjs_LDFLAGS =  kjs/libjscore.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA= osb-jscore.pc
EXTRA_DIST = JavaScriptCorePrefix.h README TODO COPYING ChangeLog

