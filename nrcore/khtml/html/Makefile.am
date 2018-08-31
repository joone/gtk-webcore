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



libnrcore_khtml_html_h_private_sources=\
	dtd.h \
	html_baseimpl.h \
	html_blockimpl.h \
	html_documentimpl.h \
	html_elementimpl.h \
	html_formimpl.h\
	html_headimpl.h \
	html_imageimpl.h \
	html_inlineimpl.h \
	html_listimpl.h \
	html_miscimpl.h \
	html_tableimpl.h \
	html_objectimpl.h\
	htmlparser.h \
	htmltokenizer.h\
	html_documentimpl.moc\
	htmltokenizer.moc


libnrcore_khtml_html_cpp_sources=\
	dtd.cpp \
	html_baseimpl.cpp \
	html_blockimpl.cpp \
	html_documentimpl.cpp \
	html_elementimpl.cpp \
	html_formimpl.cpp\
	html_headimpl.cpp \
	html_imageimpl.cpp \
	html_inlineimpl.cpp \
	html_listimpl.cpp \
	html_miscimpl.cpp \
	html_objectimpl.cpp\
	htmlparser.cpp\
	html_tableimpl.cpp \
	htmltokenizer.cpp

noinst_LTLIBRARIES = libnrcore_khtml_html.la

EXTRA_DIST = doctypes.cpp kentities.c

libnrcore_khtml_html_la_SOURCES=\
	$(libnrcore_khtml_html_cpp_sources)\
	$(libnrcore_khtml_html_h_private_sources)

libnrcore_khtml_html_la_LDFLAGS=
libnrcore_khtml_html_la_LIBADD=

