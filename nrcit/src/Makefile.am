# Process this file with automake to produce Makefile.in
SUBDIRS = gtk

NULL =

AM_CPPFLAGS = @DEFS@\
	-DPKGDATADIR=\"$(pkgdatadir)/\"\
	$(NULL)	

if USE_LIBCURL
cond_libcurl_sources = HttpCurl.cpp HttpCurl.h
else !USE_LIBCURL
cond_libcurl_sources =
endif

EXTRA_DIST =  HttpCurl.cpp HttpCurl.h 

libnrcit_cpp_sources=\
BridgeImpl.cpp \
Http.cpp\
osb.cpp\
osbimpl.cpp\
rootimpl.cpp\
frameimpl.cpp\
PageCache.cpp\
ImageRendererFactory.cpp\
ImageRenderer.cpp\
XftTextRendererFactory.cpp\
XftTextRenderer.cpp\
GdkXftContext.cpp\
UnicodeImpl.cpp\
ResourceLoadListener.cpp\
PageLoadListener.cpp\
SimpleHistory.cpp\
VisitedURLHistory.cpp\
$(NULL)


libnrcit_h_private_sources=\
BridgeImpl.h \
Http.h\
PageCache.h\
osbimpl.h\
rootimpl.h\
prefimpl.h\
frameimpl.h\
ImageRenderer.h\
ImageRendererFactory.h\
XftTextRendererFactory.h\
XftTextRenderer.h\
GdkXftContext.h\
GdkHelpers.h\
GLibHelpers.h\
CGContextProvider.h\
GObjectMisc.h\
UnicodeImpl.h\
ResourceLoadListener.h\
PageLoadListener.h\
SimpleHistory.h\
VisitedURLHistory.h\
$(NULL)

libnrcit_h_public_sources=\
osb.h\
$(NULL)

lib_LTLIBRARIES = libnrcit.la

EXTRA_HEADERS = 

libnrcit_la_SOURCES =\
	$(libnrcit_cpp_sources)\
	$(libnrcit_h_private_sources)\
	$(cond_libcurl_sources)\
	$(NULL)

libnrcitincludedir = $(includedir)/osb
libnrcitinclude_HEADERS = $(libnrcit_h_public_sources)
libnrcit_la_LDFLAGS = -version-info 0:0:0 @LIBCURL_LIBS@

libnrcit_la_LIBADD  = gtk/libnrcit_gtkapi.la

BUILT_SOURCES =	
MAINTAINERCLEANFILES = $(BUILT_SOURCES) 

