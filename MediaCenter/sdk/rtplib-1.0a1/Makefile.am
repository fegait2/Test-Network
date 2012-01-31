## Process this file with automake to produce Makefile.in

## Copyright 1997, 1998 Lucent Technologies; all rights reserved


AUTOMAKE_OPTIONS = foreign

lib_LIBRARIES = librtp.a librtpunix.a

noinst_PROGRAMS = example1 example2

librtp_a_SOURCES = rtp_api.c rtp_api_internal.c rtp_collision.c \
	rtp_encrypt.c rtp_mlist.c rtp_mlist_internal.c \
	tclHash.c \
	rtp_api.h rtp_api_internal.h rtp_collision.h \
	rtp_encrypt.h rtp_mlist.h rtp_mlist_internal.h \
	rtp_lowlevel.h \
        sysdep.h \
	tclHash.h

librtpunix_a_SOURCES = rtp_unix.c md5c.c host2ip.c rtp_highlevel.c\
	rtp_unix.h rtp_highlevel.h rtp_highlevel_internal.h\
	global.h md5.h 

example1_SOURCES = example1.c

example2_SOURCES = example2.c

LDADD = librtpunix.a librtp.a -lm

EXTRA_DIST = NOTES TODO rtp_api.html rtp_test.html

DISTCLEANFILES = .deps/*
