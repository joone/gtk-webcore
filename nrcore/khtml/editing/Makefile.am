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
	-DAPPLE_CHANGES\
	$(NULL)


libnrcore_khtml_editing_h_private_sources=\
	htmlediting_impl.h \
	htmlediting.h\
	jsediting.h\
	$(NULL)


libnrcore_khtml_editing_cpp_sources=\
	htmlediting.cpp \
	htmlediting_impl.cpp \
	jsediting.cpp \
	$(NULL)

LUT_FILES = \
	$(NULL)

noinst_LTLIBRARIES = libnrcore_khtml_editing.la
libnrcore_khtml_editing_la_LDFLAGS=
libnrcore_khtml_editing_la_LIBADD= 

libnrcore_khtml_editing_la_SOURCES=\
	$(libnrcore_khtml_editing_cpp_sources)\
	$(libnrcore_khtml_editing_h_private_sources)\
	$(LUT_FILES)\
	$(NULL)




CREATE_HASH_TABLE = $(top_srcdir)/../JavaScriptCore/kjs/create_hash_table

%.lut.h:  
	perl $(CREATE_HASH_TABLE) %.cpp > $@

BUILT_SOURCES = $(LUT_FILES)
MAINTAINERCLEANFILES = $(BUILT_SOURCES)

