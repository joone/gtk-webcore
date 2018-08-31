## Process this file with automake to produce Makefile.in
NULL=

AM_CPPFLAGS = -DDATADIR=\"$(pkgdatadir)/\" \
	$(NULL)

osb_browser_c_sources=\
	callbacks.c\
	main.c \
	osb-browser.c\
	osb-browserglobal-marshal.c\
	osb-browserglobal-plugin.cc\
	osb-browserglobal-settings.c\
	osb-browserglobal.c\
	$(NULL)


osb_browser_h_sources=\
	osb-browser.h\
	osb-browserglobal-settings.h\
	osb-browserglobal.h\
	$(NULL)

osb_browser_private_h_sources=\
	callbacks.h\
	osb-browser-private.h\
	osb-browserglobal-marshal.h\
	osb-browserglobal-private.h\
	$(NULL)



osb-browserglobal-marshal.h: osb-browserglobal-marshal.list
	( glib-genmarshal --prefix=osb_browserglobal_marshal $< --header > $@.tmp \
	&& mv $@.tmp $@ ) \
	|| ( rm -f $@.tmp && exit 1 )
osb-browserglobal-marshal.c: osb-browserglobal-marshal.list
	( glib-genmarshal --prefix=osb_browserglobal_marshal $< --body > $@.tmp \
	&& mv $@.tmp $@ ) \
	|| ( rm -f $@.tmp && exit 1 )




bin_PROGRAMS = osb-browser

osb_browser_SOURCES=\
	$(osb_browser_c_sources)\
	$(osb_browser_h_sources)\
	$(osb_browser_private_h_sources)\
	$(NULL)

#include_HEADERS=


noinst_HEADERS=\
	$(osb_browser_h_sources)\
	$(osb_browser_private_h_sources)\
	$(NULL)

osb_browser_LDADD =  \
	$(NULL)

EXTRA_DIST=osb-browserglobal-marshal.list 
BUILT_SOURCES=osb-browserglobal-marshal.c osb-browserglobal-marshal.h
