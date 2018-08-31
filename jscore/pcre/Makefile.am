# Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES = libjscore_pcre.la

libjscore_pcre_cpp_sources=\
	get.c \
	maketables.c \
	study.c \
	pcre.c \
	pcreposix.c


libjscore_pcre_h_private_sources=\
	internal.h\
	pcre-config.h\
	pcre.h\
	pcreposix.h

chartables.c: 
	$(CC) -o dftables dftables.c
	./dftables > $(srcdir)/chartables.c


libjscore_pcre_la_SOURCES = \
	$(libjscore_pcre_cpp_sources)\
	$(libjscore_pcre_h_private_sources)


libjscore_pcre_la_LDFLAGS = 
libjscore_pcre_la_LIBADD  = 

check_PROGRAMS = pcredemo pcregrep pcretest
pcrelib=libjscore_pcre.la
pcredemo_SOURCES=pcredemo.c 
pcredemo_LDADD=$(pcrelib) 
pcregrep_SOURCES=pcregrep.c 
pcregrep_LDADD=$(pcrelib)
pcretest_SOURCES=pcretest.c 
pcretest_LDADD=$(pcrelib) 


BUILT_SOURCES = chartables.c
EXTRA_DIST = chartables.c
MAINTAINERCLEANFILES = dftables
