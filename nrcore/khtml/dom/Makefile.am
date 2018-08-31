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



libnrcore_khtml_dom_h_private_sources=\
	css_rule.h\
	css_stylesheet.h\
	css_value.h\
	dom2_events.h\
	dom2_range.h\
	dom2_traversal.h\
	dom2_views.h\
	dom_core.h\
	dom_doc.h\
	dom_element.h\
	dom_exception.h\
	dom_html.h\
	dom_misc.h\
	dom_node.h\
	dom_string.h\
	dom_text.h\
	dom_xml.h\
	html_base.h\
	html_block.h\
	html_document.h\
	html_element.h\
	html_form.h\
	html_head.h\
	html_image.h\
	html_inline.h\
	html_list.h\
	html_misc.h\
	html_object.h\
	html_table.h

libnrcore_khtml_dom_cpp_sources=\
	css_rule.cpp \
	css_stylesheet.cpp \
	css_value.cpp \
	dom2_events.cpp \
	dom2_range.cpp \
	dom2_traversal.cpp \
	dom2_views.cpp \
	dom_doc.cpp \
	dom_element.cpp \
	dom_misc.cpp \
	dom_node.cpp \
	dom_string.cpp \
	dom_text.cpp \
	dom_xml.cpp \
	html_base.cpp\
	html_block.cpp\
	html_document.cpp\
	html_element.cpp\
	html_form.cpp\
	html_head.cpp\
	html_image.cpp\
	html_inline.cpp\
	html_list.cpp\
	html_misc.cpp\
	html_object.cpp\
	html_table.cpp

noinst_LTLIBRARIES = libnrcore_khtml_dom.la

libnrcore_khtml_dom_la_SOURCES=\
	$(libnrcore_khtml_dom_cpp_sources)\
	$(libnrcore_khtml_dom_h_private_sources)

libnrcore_khtml_dom_la_LDFLAGS=
libnrcore_khtml_dom_la_LIBADD=
