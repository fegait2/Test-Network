# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = .
prefix = /usr/local
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/rtplib
pkglibdir = $(libdir)/rtplib
pkgincludedir = $(includedir)/rtplib

top_builddir = .

ACLOCAL = /home/lee/workspace/Test-Network/MediaCenter/src/rtplib-1.0b2/missing aclocal
AUTOCONF = /home/lee/workspace/Test-Network/MediaCenter/src/rtplib-1.0b2/missing autoconf
AUTOMAKE = /home/lee/workspace/Test-Network/MediaCenter/src/rtplib-1.0b2/missing automake
AUTOHEADER = /home/lee/workspace/Test-Network/MediaCenter/src/rtplib-1.0b2/missing autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
CC = gcc
LN_S = ln -s
LOCALDIR = 
MAKEINFO = /home/lee/workspace/Test-Network/MediaCenter/src/rtplib-1.0b2/missing makeinfo
PACKAGE = rtplib
RANLIB = ranlib
VERSION = 1.0b2

AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = librtp.a librtpunix.a

noinst_PROGRAMS = example1 example2

librtp_a_SOURCES = rtp_api.c rtp_api_internal.c rtp_collision.c 	rtp_encrypt.c rtp_mlist.c rtp_mlist_internal.c 	tclHash.c 	rtp_api.h rtp_api_internal.h rtp_collision.h 	rtp_encrypt.h rtp_mlist.h rtp_mlist_internal.h 	rtp_lowlevel.h         sysdep.h 	tclHash.h


librtpunix_a_SOURCES = rtp_unix.c md5c.c host2ip.c rtp_highlevel.c 	rtp_unix.h rtp_highlevel.h rtp_highlevel_internal.h 	global.h md5.h 


example1_SOURCES = example1.c

example2_SOURCES = example2.c

LDADD = librtpunix.a librtp.a -lm

EXTRA_DIST = NOTES TODO rtp_api.html rtp_test.html rtp_win32.c rtp_win32.h

DISTCLEANFILES = .deps/*
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = config.h
CONFIG_CLEAN_FILES = 
LIBRARIES =  $(lib_LIBRARIES)


DEFS = -DHAVE_CONFIG_H -I. -I$(srcdir) -I.
CPPFLAGS = 
LDFLAGS = 
LIBS = -lnsl 
librtp_a_LIBADD = 
librtp_a_OBJECTS =  rtp_api.o rtp_api_internal.o rtp_collision.o \
rtp_encrypt.o rtp_mlist.o rtp_mlist_internal.o tclHash.o
librtpunix_a_LIBADD = 
librtpunix_a_OBJECTS =  rtp_unix.o md5c.o host2ip.o rtp_highlevel.o
AR = ar
PROGRAMS =  $(noinst_PROGRAMS)

example1_OBJECTS =  example1.o
example1_LDADD = $(LDADD)
example1_DEPENDENCIES =  librtpunix.a librtp.a
example1_LDFLAGS = 
example2_OBJECTS =  example2.o
example2_LDADD = $(LDADD)
example2_DEPENDENCIES =  librtpunix.a librtp.a
example2_LDFLAGS = 
CFLAGS = -g -O2 -W -Wall -Wmissing-prototypes -Wstrict-prototypes -DPROTOTYPES
COMPILE = $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
DIST_COMMON =  README ./stamp-h.in Makefile.am Makefile.in TODO \
aclocal.m4 config.h.in configure configure.in install-sh missing \
mkinstalldirs


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = tar
GZIP_ENV = --best
SOURCES = $(librtp_a_SOURCES) $(librtpunix_a_SOURCES) $(example1_SOURCES) $(example2_SOURCES)
OBJECTS = $(librtp_a_OBJECTS) $(librtpunix_a_OBJECTS) $(example1_OBJECTS) $(example2_OBJECTS)

all: all-redirect
.SUFFIXES:
.SUFFIXES: .S .c .o .s
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --foreign --include-deps Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status

$(ACLOCAL_M4):  configure.in 
	cd $(srcdir) && $(ACLOCAL)

config.status: $(srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	$(SHELL) ./config.status --recheck
$(srcdir)/configure: $(srcdir)/configure.in $(ACLOCAL_M4) $(CONFIGURE_DEPENDENCIES)
	cd $(srcdir) && $(AUTOCONF)

config.h: stamp-h
	@if test ! -f $@; then \
		rm -f stamp-h; \
		$(MAKE) stamp-h; \
	else :; fi
stamp-h: $(srcdir)/config.h.in $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES= CONFIG_HEADERS=config.h \
	     $(SHELL) ./config.status
	@echo timestamp > stamp-h 2> /dev/null
$(srcdir)/config.h.in: $(srcdir)/stamp-h.in
	@if test ! -f $@; then \
		rm -f $(srcdir)/stamp-h.in; \
		$(MAKE) $(srcdir)/stamp-h.in; \
	else :; fi
$(srcdir)/stamp-h.in: $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOHEADER)
	@echo timestamp > $(srcdir)/stamp-h.in 2> /dev/null

mostlyclean-hdr:

clean-hdr:

distclean-hdr:
	-rm -f config.h

maintainer-clean-hdr:

mostlyclean-libLIBRARIES:

clean-libLIBRARIES:
	-test -z "$(lib_LIBRARIES)" || rm -f $(lib_LIBRARIES)

distclean-libLIBRARIES:

maintainer-clean-libLIBRARIES:

install-libLIBRARIES: $(lib_LIBRARIES)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(libdir)
	@list='$(lib_LIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done
	@$(POST_INSTALL)
	@list='$(lib_LIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(RANLIB) $(DESTDIR)$(libdir)/$$p"; \
	    $(RANLIB) $(DESTDIR)$(libdir)/$$p; \
	  else :; fi; \
	done

uninstall-libLIBRARIES:
	@$(NORMAL_UNINSTALL)
	list='$(lib_LIBRARIES)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(libdir)/$$p; \
	done

.c.o:
	$(COMPILE) -c $<

.s.o:
	$(COMPILE) -c $<

.S.o:
	$(COMPILE) -c $<

mostlyclean-compile:
	-rm -f *.o core *.core

clean-compile:

distclean-compile:
	-rm -f *.tab.c

maintainer-clean-compile:

librtp.a: $(librtp_a_OBJECTS) $(librtp_a_DEPENDENCIES)
	-rm -f librtp.a
	$(AR) cru librtp.a $(librtp_a_OBJECTS) $(librtp_a_LIBADD)
	$(RANLIB) librtp.a

librtpunix.a: $(librtpunix_a_OBJECTS) $(librtpunix_a_DEPENDENCIES)
	-rm -f librtpunix.a
	$(AR) cru librtpunix.a $(librtpunix_a_OBJECTS) $(librtpunix_a_LIBADD)
	$(RANLIB) librtpunix.a

mostlyclean-noinstPROGRAMS:

clean-noinstPROGRAMS:
	-test -z "$(noinst_PROGRAMS)" || rm -f $(noinst_PROGRAMS)

distclean-noinstPROGRAMS:

maintainer-clean-noinstPROGRAMS:

example1: $(example1_OBJECTS) $(example1_DEPENDENCIES)
	@rm -f example1
	$(LINK) $(example1_LDFLAGS) $(example1_OBJECTS) $(example1_LDADD) $(LIBS)

example2: $(example2_OBJECTS) $(example2_DEPENDENCIES)
	@rm -f example2
	$(LINK) $(example2_LDFLAGS) $(example2_OBJECTS) $(example2_LDADD) $(LIBS)

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP)
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	here=`pwd` && cd $(srcdir) \
	  && mkid -f$$here/ID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES) config.h.in $(TAGS_DEPENDENCIES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS)'; \
	unique=`for i in $$list; do echo $$i; done | \
	  awk '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)config.h.in$$unique$(LISP)$$tags" \
	  || (cd $(srcdir) && etags $(ETAGS_ARGS) $$tags config.h.in $$unique $(LISP) -o $$here/TAGS)

mostlyclean-tags:

clean-tags:

distclean-tags:
	-rm -f TAGS ID

maintainer-clean-tags:

distdir = $(PACKAGE)-$(VERSION)
top_distdir = $(distdir)

# This target untars the dist file and tries a VPATH configuration.  Then
# it guarantees that the distribution is self-contained by making another
# tarfile.
distcheck: dist
	-rm -rf $(distdir)
	GZIP=$(GZIP_ENV) $(TAR) zxf $(distdir).tar.gz
	mkdir $(distdir)/=build
	mkdir $(distdir)/=inst
	dc_install_base=`cd $(distdir)/=inst && pwd`; \
	cd $(distdir)/=build \
	  && ../configure --srcdir=.. --prefix=$$dc_install_base \
	  && $(MAKE) $(AM_MAKEFLAGS) \
	  && $(MAKE) $(AM_MAKEFLAGS) dvi \
	  && $(MAKE) $(AM_MAKEFLAGS) check \
	  && $(MAKE) $(AM_MAKEFLAGS) install \
	  && $(MAKE) $(AM_MAKEFLAGS) installcheck \
	  && $(MAKE) $(AM_MAKEFLAGS) dist
	-rm -rf $(distdir)
	@banner="$(distdir).tar.gz is ready for distribution"; \
	dashes=`echo "$$banner" | sed s/./=/g`; \
	echo "$$dashes"; \
	echo "$$banner"; \
	echo "$$dashes"
dist: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP_ENV) $(TAR) chozf $(distdir).tar.gz $(distdir)
	-rm -rf $(distdir)
dist-all: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP_ENV) $(TAR) chozf $(distdir).tar.gz $(distdir)
	-rm -rf $(distdir)
distdir: $(DISTFILES)
	-rm -rf $(distdir)
	mkdir $(distdir)
	-chmod 777 $(distdir)
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
example1.o: example1.c rtp_api.h rtp_highlevel.h
example2.o: example2.c rtp_api.h rtp_highlevel.h
host2ip.o: host2ip.c
md5c.o: md5c.c global.h md5.h
rtp_api.o: rtp_api.c sysdep.h config.h rtp_api_internal.h \
	rtp_mlist_internal.h tclHash.h rtp_api.h rtp_lowlevel.h \
	rtp_mlist.h rtp_encrypt.h rtp_collision.h
rtp_api_internal.o: rtp_api_internal.c sysdep.h config.h \
	rtp_api_internal.h rtp_mlist_internal.h tclHash.h rtp_api.h \
	rtp_lowlevel.h rtp_mlist.h rtp_encrypt.h rtp_collision.h
rtp_collision.o: rtp_collision.c sysdep.h config.h rtp_api_internal.h \
	rtp_mlist_internal.h tclHash.h rtp_api.h rtp_lowlevel.h \
	rtp_collision.h rtp_mlist.h rtp_encrypt.h
rtp_encrypt.o: rtp_encrypt.c sysdep.h config.h rtp_api_internal.h \
	rtp_mlist_internal.h tclHash.h rtp_api.h rtp_lowlevel.h \
	rtp_encrypt.h
rtp_highlevel.o: rtp_highlevel.c rtp_api.h rtp_lowlevel.h \
	rtp_highlevel.h rtp_highlevel_internal.h rtp_unix.h sysdep.h \
	config.h global.h md5.h
rtp_mlist.o: rtp_mlist.c sysdep.h config.h rtp_mlist_internal.h \
	tclHash.h rtp_api.h rtp_api_internal.h rtp_lowlevel.h \
	rtp_mlist.h rtp_collision.h
rtp_mlist_internal.o: rtp_mlist_internal.c sysdep.h config.h \
	rtp_mlist_internal.h tclHash.h rtp_api.h rtp_api_internal.h \
	rtp_lowlevel.h
rtp_unix.o: rtp_unix.c rtp_unix.h rtp_api.h sysdep.h config.h
tclHash.o: tclHash.c sysdep.h config.h tclHash.h

info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
all-recursive-am: config.h
	$(MAKE) $(AM_MAKEFLAGS) all-recursive

install-exec-am: install-libLIBRARIES
install-exec: install-exec-am

install-data-am:
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-libLIBRARIES
uninstall: uninstall-am
all-am: Makefile $(LIBRARIES) $(PROGRAMS) config.h
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(libdir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*
	-test -z "$(DISTCLEANFILES)" || rm -f $(DISTCLEANFILES)

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-hdr mostlyclean-libLIBRARIES \
		mostlyclean-compile mostlyclean-noinstPROGRAMS \
		mostlyclean-tags mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-hdr clean-libLIBRARIES clean-compile \
		clean-noinstPROGRAMS clean-tags clean-generic \
		mostlyclean-am

clean: clean-am

distclean-am:  distclean-hdr distclean-libLIBRARIES distclean-compile \
		distclean-noinstPROGRAMS distclean-tags \
		distclean-generic clean-am

distclean: distclean-am
	-rm -f config.status

maintainer-clean-am:  maintainer-clean-hdr maintainer-clean-libLIBRARIES \
		maintainer-clean-compile \
		maintainer-clean-noinstPROGRAMS maintainer-clean-tags \
		maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am
	-rm -f config.status

.PHONY: mostlyclean-hdr distclean-hdr clean-hdr maintainer-clean-hdr \
mostlyclean-libLIBRARIES distclean-libLIBRARIES clean-libLIBRARIES \
maintainer-clean-libLIBRARIES uninstall-libLIBRARIES \
install-libLIBRARIES mostlyclean-compile distclean-compile \
clean-compile maintainer-clean-compile mostlyclean-noinstPROGRAMS \
distclean-noinstPROGRAMS clean-noinstPROGRAMS \
maintainer-clean-noinstPROGRAMS tags mostlyclean-tags distclean-tags \
clean-tags maintainer-clean-tags distdir info-am info dvi-am dvi check \
check-am installcheck-am installcheck all-recursive-am install-exec-am \
install-exec install-data-am install-data install-am install \
uninstall-am uninstall all-redirect all-am all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
