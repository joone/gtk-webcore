# Process this file with automake to produce Makefile.in

SUBDIRS = css dom html xml misc rendering ecma editing

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



libnrcore_khtml_h_private_sources=\
	khtml_events.h\
	khtml_part.h\
	khtmlview.h\
	khtmlpart_p.h\
	khtml_part.moc\
	khtmlview.moc

libnrcore_khtml_cpp_sources=\
	khtml_events.cpp\
	khtml_part.cpp\
	khtmlview.cpp

noinst_LTLIBRARIES = libnrcore_khtml.la

libnrcore_khtml_la_SOURCES=\
	$(libnrcore_khtml_cpp_sources)\
	$(libnrcore_khtml_h_private_sources)

libnrcore_khtml_la_LDFLAGS= -version-info 0:0:0
libnrcore_khtml_la_LIBADD= \
	css/libnrcore_khtml_css.la \
	dom/libnrcore_khtml_dom.la \
	html/libnrcore_khtml_html.la \
	xml/libnrcore_khtml_xml.la \
	misc/libnrcore_khtml_misc.la \
	rendering/libnrcore_khtml_rendering.la \
	ecma/libnrcore_khtml_ecma.la \
	editing/libnrcore_khtml_editing.la
