# Process this file with automake to produce Makefile.in

AM_CPPFLAGS = @DEFS@\
	-I$(top_srcdir)/khtml \
	-I$(top_srcdir)/khtml/dom \
	-I$(top_srcdir)/khtml/css \
	-I$(top_srcdir)/khtml/html \
	-I$(top_srcdir)/khtml/misc/ \
	-I$(top_srcdir)/khtml/rendering/ \
	-I$(top_srcdir)/khtml/xml \
	-I$(top_srcdir)/kwiq/ \
	-I$(top_srcdir)/khtml/ecma/ \
	-I$(top_srcdir)/khtml/editing/ \
	-I$(top_srcdir)/ForwardingHeaders



libnrcore_khtml_misc_h_private_sources=\
	arena.h \
	decoder.h\
	helper.h \
	htmlhashes.h \
	htmlattrs.h \
	htmltags.h  \
	khtmldata.h  \
	khtml_text_operations.h \
	khtmllayout.h  \
	loader_client.h  \
	loader.h   \
	shared.h   \
	stringit.h \
	loader.moc


libnrcore_khtml_misc_cpp_sources=\
	arena.cpp \
	decoder.cpp\
	helper.cpp \
	htmlhashes.cpp \
	khtml_text_operations.cpp \
	loader.cpp \
	stringit.cpp

noinst_LTLIBRARIES = libnrcore_khtml_misc.la

libnrcore_khtml_misc_la_SOURCES=\
	$(libnrcore_khtml_misc_cpp_sources)\
	$(libnrcore_khtml_misc_h_private_sources)

htmlattrs.h htmlattrs.c:
	perl makeattrs htmlattrs.in
htmltags.h htmltags.c:
	perl maketags htmltags.in

EXTRA_DIST = htmltags.c htmlattrs.c
libnrcore_khtml_misc_la_LDFLAGS=
libnrcore_khtml_misc_la_LIBADD=

MAINTAINER_CLEANFILES= htmlattrs.h htmlattrs.c htmltags.h htmltags.c
