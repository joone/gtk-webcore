# Process this file with automake to produce Makefile.in

NULL = 

AM_CPPFLAGS = @DEFS@\
	-I$(top_srcdir)/src/
	$(NULL)



libnrcit_gtkapi_h_private_sources=\
	gtk-khtml-private.h\
	gtk-khtml-marshal.h\
	$(NULL)

libnrcit_gtkapi_h_public_sources=\
	gtk-khtml.h\
	$(NULL)

libnrcit_gtkapi_cpp_sources=\
	gtk-khtml.cc\
	gtk-khtml-marshal.c\
	$(NULL)	

gtk-khtml-marshal.h: gtk-khtml-marshal.list
	( glib-genmarshal --prefix=gtk_khtml_marshal $< --header > $@.tmp \
	&& mv $@.tmp $@ ) \
	|| ( rm -f $@.tmp && exit 1 )
gtk-khtml-marshal.c: gtk-khtml-marshal.list
	( glib-genmarshal --prefix=gtk_khtml_marshal $< --body > $@.tmp \
	&& mv $@.tmp $@ ) \
        || ( rm -f $@.tmp && exit 1 )


noinst_LTLIBRARIES = libnrcit_gtkapi.la

libnrcit_gtkapiincludedir = $(includedir)/osb/
libnrcit_gtkapiinclude_HEADERS = $(libnrcit_gtkapi_h_public_sources)

libnrcit_gtkapi_la_SOURCES=\
	$(libnrcit_gtkapi_cpp_sources)\
	$(libnrcit_gtkapi_h_private_sources)

libnrcit_gtkapi_la_LDFLAGS= -version-info 0:0:0
libnrcit_gtkapi_la_LIBADD= 
#	$(top_builddir)/kwiq/libkwiq_gtk.la \
#	$(top_builddir)/khtml/libkwiq_khtml.la \
#	@LIBCURL_LIBS@

BUILT_SOURCES = gtk-khtml-marshal.c gtk-khtml-marshal.h
EXTRA_DIST = gtk-khtml-marshal.list
