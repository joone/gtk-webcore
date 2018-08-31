# Process this file with automake to produce Makefile.in
NULL =

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
	-I$(top_srcdir)/ForwardingHeaders \
	$(NULL)


libnrcore_khtml_ecma_h_private_sources=\
	kjs_binding.h \
	kjs_css.h \
	kjs_dom.h \
	kjs_events.h \
	kjs_html.h \
	kjs_navigator.h \
	kjs_proxy.h \
	kjs_range.h \
	kjs_traversal.h \
	kjs_views.h \
	kjs_window.h \
	kjs_window.moc\
	xmlhttprequest.h \
	xmlserializer.h \
	$(NULL)


libnrcore_khtml_ecma_cpp_sources=\
	kjs_binding.cpp \
	kjs_css.cpp \
	kjs_dom.cpp \
	kjs_events.cpp \
	kjs_html.cpp \
	kjs_navigator.cpp \
	kjs_proxy.cpp \
	kjs_range.cpp \
	kjs_traversal.cpp \
	kjs_views.cpp \
	kjs_window.cpp\
	xmlhttprequest.cpp \
	xmlserializer.cpp \
	$(NULL)

LUT_FILES = \
	kjs_css.lut.h \
	kjs_dom.lut.h \
	kjs_html.lut.h \
	kjs_events.lut.h \
	kjs_navigator.lut.h \
	kjs_range.lut.h \
	kjs_traversal.lut.h \
	kjs_views.lut.h \
	kjs_window.lut.h \
	xmlhttprequest.lut.h \
	xmlserializer.lut.h \
	$(NULL)

noinst_LTLIBRARIES = libnrcore_khtml_ecma.la
libnrcore_khtml_ecma_la_LDFLAGS=
libnrcore_khtml_ecma_la_LIBADD= 

libnrcore_khtml_ecma_la_SOURCES=\
	$(libnrcore_khtml_ecma_cpp_sources)\
	$(libnrcore_khtml_ecma_h_private_sources)\
	$(LUT_FILES)\
	$(NULL)




CREATE_HASH_TABLE = $(top_srcdir)/../JavaScriptCore/kjs/create_hash_table

%.lut.h:  
	perl $(CREATE_HASH_TABLE) %.cpp > $@

BUILT_SOURCES = $(LUT_FILES)
MAINTAINERCLEANFILES = $(BUILT_SOURCES)

