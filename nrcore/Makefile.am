## Process this file with automake to produce Makefile.in
SUBDIRS = ForwardingHeaders kwiq khtml data Resources
EXTRA_DIST = \
	WebCorePrefix.h \
	LICENSE-APPLE    \
	LICENSE-LGPL-2   \
	LICENSE-LGPL-2.1 \
	TODO\
	README\
	COPYING\
	ChangeLog

lib_LTLIBRARIES=libnrcore.la
libnrcore_la_SOURCES=
libnrcore_la_LDFLAGS= -version-info 0:0:0
libnrcore_la_LIBADD=khtml/libnrcore_khtml.la kwiq/libnrcore_kwiq_gtk.la

# install-data-local:
# 	@$(NORMAL_INSTALL)
# 	if test -d $(srcdir)/pixmaps; then \
# 	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
# 	  for pixmap in $(srcdir)/pixmaps/*; do \
# 	    if test -f $$pixmap; then \
# 	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
# 	    fi \
# 	  done \
# 	fi

# dist-hook:
# 	if test -d pixmaps; then \
# 	  mkdir $(distdir)/pixmaps; \
# 	  for pixmap in pixmaps/*; do \
# 	    if test -f $$pixmap; then \
# 	      cp -p $$pixmap $(distdir)/pixmaps; \
# 	    fi \
# 	  done \
# 	fi

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA= osb-nrcore.pc 

