# Process this file with automake to produce Makefile.in

NULL=

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



libnrcore_khtml_xml_h_private_sources=\
	dom2_eventsimpl.h \
	dom2_rangeimpl.h \
	dom2_traversalimpl.h \
	dom2_viewsimpl.h \
	dom_atomicstringlist.h\
	dom_docimpl.h \
	dom_elementimpl.h \
	dom_nodeimpl.h \
	dom_stringimpl.h \
	dom_textimpl.h \
	dom_xmlimpl.h \
	xml_tokenizer.h \
	dom_atomicstring.h \
	dom_selection.h \
	dom_position.h \
	dom_positioniterator.h \
	xml_namespace_table.h \
	xml_tokenizer.moc \
	dom_docimpl.moc \
	$(NULL)

libnrcore_khtml_xml_cpp_sources=\
	dom2_eventsimpl.cpp \
	dom2_rangeimpl.cpp \
	dom2_traversalimpl.cpp \
	dom2_viewsimpl.cpp \
	dom_docimpl.cpp \
	dom_elementimpl.cpp \
	dom_nodeimpl.cpp \
	dom_stringimpl.cpp \
	dom_textimpl.cpp \
	dom_xmlimpl.cpp \
	xml_tokenizer.cpp \
	dom_atomicstring.cpp \
	dom_selection.cpp \
	dom_position.cpp \
	dom_positioniterator.cpp \
	xml_namespace_table.cpp \
	$(NULL)

noinst_LTLIBRARIES = libnrcore_khtml_xml.la

libnrcore_khtml_xml_la_SOURCES=\
	$(libnrcore_khtml_xml_cpp_sources)\
	$(libnrcore_khtml_xml_h_private_sources)

libnrcore_khtml_xml_la_LDFLAGS=
libnrcore_khtml_xml_la_LIBADD=
