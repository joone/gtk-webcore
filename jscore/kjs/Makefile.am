# Process this file with automake to produce Makefile.in
NULL=

AM_CPPFLAGS=-include $(top_srcdir)/JavaScriptCorePrefix.h


lib_LTLIBRARIES = libjscore.la

libjscore_cpp_sources=\
	array_object.cpp \
	bool_object.cpp \
	collector.cpp \
	date_object.cpp \
	debugger.cpp \
	dtoa.cpp \
	error_object.cpp \
	fpconst.cpp \
	function.cpp \
	function_object.cpp \
	identifier.cpp \
	internal.cpp \
	interpreter.cpp \
	interpreter_map.cpp \
	lexer.cpp \
	list.cpp \
	lookup.cpp \
	math_object.cpp \
	nodes2string.cpp \
	nodes.cpp \
	number_object.cpp \
	object.cpp \
	object_object.cpp \
	operations.cpp \
	property_map.cpp \
	reference.cpp \
	reference_list.cpp \
	regexp.cpp \
	regexp_object.cpp \
	scope_chain.cpp \
	string_object.cpp \
	testkjs.cpp \
	ustring.cpp \
	protected_values.cpp \
	value.cpp \
	$(NULL)

libjscore_h_private_sources=\
	array_instance.h \
	array_object.h \
	bool_object.h \
	collector.h \
	context.h \
	date_object.h \
	debugger.h \
	dtoa.h \
	error_object.h \
	function.h \
	function_object.h \
	internal.h \
	lexer.h \
	math_object.h \
	nodes.h \
	number_object.h \
	object_object.h \
	regexp.h \
	regexp_object.h \
	string_object.h \
	protected_values.h \
	new \
	$(NULL)

libjscore_h_public_sources=\
	simple_number.h \
	completion.h \
	list.h \
	scope_chain.h \
	reference.h \
	reference_list.h \
	property_map.h \
	identifier.h \
	lookup.h \
	operations.h \
	ustring.h \
	interpreter.h \
	interpreter_map.h \
	object.h \
	types.h \
	value.h \
	protect.h \
	../pcre/pcre.h\
	$(NULL)
#global.h 

LUT_FILES = math_object.lut.h lexer.lut.h array_object.lut.h date_object.lut.h string_object.lut.h number_object.lut.h
GRAMMAR_FILES = grammar.h grammar.cpp.h grammar.cpp

YFLAGS = -d --file-prefix=grammar --name-prefix=kjsyy --output-file=grammar.cpp

$(GRAMMAR_FILES): 
	$(YACC) $(YFLAGS) grammar.y

CREATE_HASH_TABLE = $(srcdir)/create_hash_table

lexer.lut.h:  
	perl $(CREATE_HASH_TABLE) keywords.table -i > $@

%.lut.h: 
	perl $(CREATE_HASH_TABLE) `echo $@ | sed -e s/.lut.h/.cpp/` -i > $@

BUILT_SOURCES = $(GRAMMAR_FILES) $(LUT_FILES) 

libjscore_la_SOURCES = \
	$(libjscore_cpp_sources)\
	$(libjscore_h_private_sources)\
	$(libjscore_h_public_sources)\
	$(LUT_FILES)\
	$(GRAMMAR_FILES)\
	$(NULL)

EXTRA_DIST = test.js

# $(libjscore_h_private_sources) get installed because khtml/ecma/kjs_bindings.cpp
# includes internal.h 

noinst_HEADERS = $(BUILT_SOURCES) $(LUT_FILES) 
libjscoreincludedir = $(includedir)/osb/JavaScriptCore/
libjscoreinclude_HEADERS = $(libjscore_h_public_sources) $(libjscore_h_private_sources)

libjscore_la_LDFLAGS = -version-info 0:0:0 @LINK_FLAGS@ 
libjscore_la_LIBADD  = ../pcre/libjscore_pcre.la

