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


libnrcore_khtml_rendering_h_private_sources=\
	bidi.h \
	break_lines.h \
	font.h \
	render_arena.h \
	render_block.h\
	render_body.h \
	render_box.h \
	render_br.h \
	render_canvas.h \
	render_container.h \
	render_flexbox.h\
	render_flow.h \
	render_form.h\
	render_frames.h\
	render_image.h \
	render_inline.h\
	render_layer.h \
	render_line.h\
	render_list.h \
	render_object.h \
	render_replaced.h \
	render_style.h \
	render_table.h \
	render_text.h \
	table_layout.h\
	render_applet.h\
	render_form.moc\
	render_frames.moc\
	render_replaced.moc\
	$(NULL)

#render_root.h 
#	render_html.h 
libnrcore_khtml_rendering_cpp_sources=\
	bidi.cpp \
	break_lines.cpp \
	font.cpp \
	render_arena.cpp \
	render_block.cpp\
	render_body.cpp \
	render_box.cpp \
	render_br.cpp \
	render_canvas.cpp \
	render_container.cpp \
	render_flow.cpp \
	render_flexbox.cpp\
	render_form.cpp\
	render_frames.cpp\
	render_image.cpp \
	render_inline.cpp\
	render_layer.cpp \
	render_line.cpp\
	render_list.cpp \
	render_object.cpp \
	render_replaced.cpp \
	render_style.cpp \
	render_table.cpp \
	render_text.cpp \
	table_layout.cpp\
	render_applet.cpp\
	$(NULL)

noinst_LTLIBRARIES = libnrcore_khtml_rendering.la

libnrcore_khtml_rendering_la_SOURCES=\
	$(libnrcore_khtml_rendering_cpp_sources)\
	$(libnrcore_khtml_rendering_h_private_sources)

libnrcore_khtml_rendering_la_LDFLAGS=
libnrcore_khtml_rendering_la_LIBADD=
