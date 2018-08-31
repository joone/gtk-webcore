# Process this file with automake to produce Makefile.in
YACC=bison

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


libnrcore_khtml_css_h_private_sources=\
	css_base.h\
	css_ruleimpl.h \
	css_stylesheetimpl.h \
	css_valueimpl.h \
	csshelper.h \
	cssparser.h \
	cssstyleselector.h \
	parser.cpp \
	cssproperties.h \
	cssvalues.h \
	css_computedstyle.h \
	parser.h \
	$(NULL)

libnrcore_khtml_css_cpp_sources=\
	css_base.cpp\
	css_ruleimpl.cpp \
	css_stylesheetimpl.cpp \
	css_valueimpl.cpp \
	csshelper.cpp \
	cssparser.cpp \
	css_computedstyle.cpp \
	cssstyleselector.cpp \
	$(NULL)

PARSERFILES = \
	parser.cpp \
	parser.h \
	tokenizer.cpp \
	$(NULL)

#commented because of Windows compability
# parser.cpp parser.h: parser.y
# 	$(YACC) -d -p cssyy parser.y && mv parser.tab.c parser.cpp; \
# 	if test -f parser.tab.h; then \
# 	if cmp -s parser.tab.h parser.h; then rm -f parser.tab.h; \
# 	else mv parser.tab.h parser.h; fi \
# 	else :; fi

# tokenizer.cpp: maketokenizer tokenizer.flex
# 	cd $(srcdir); \
# 	flex tokenizer.flex && perl maketokenizer lex.yy.c > $@; \
# 	rm -f lex.yy.c


# VALUEFILES = \
# 	cssvalues.c \
# 	cssvalues.h \
# 	cssvalues.gperf \
# 	$(NULL)

# PROPFILES = \
# 	cssproperties.c \
# 	cssproperties.h \
# 	cssproperties.gperf \
# 	$(NULL)

# $(VALUEFILES):makevalues cssvalues.in
# 	sh $< 

# $(PROPFILES):makeprop cssproperties.in
# 	sh $< 

noinst_LTLIBRARIES = libnrcore_khtml_css.la

noinst_HEADERS = \
	cssproperties.c \
	cssvalues.c \
	parser.cpp \
	tokenizer.cpp \
	$(NULL)

libnrcore_khtml_css_la_SOURCES=\
	$(libnrcore_khtml_css_cpp_sources)\
	$(libnrcore_khtml_css_h_private_sources)\
	$(NULL)

# remove $(PARSERFILES)  entry if generating files

libnrcore_khtml_css_la_LDFLAGS=
libnrcore_khtml_css_la_LIBADD=


# BUILT_SOURCES = $(PARSERFILES) $(VALUEFILES) $(PROPFILES)
# MAINTAINERCLEANFILES = $(BUILT_SOURCES)

# EXTRA_DIST = parser.y
