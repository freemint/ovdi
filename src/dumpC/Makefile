#
# Makefile for dumpC
#
TARGETS = dumpc.prg

SHELL = /bin/sh
SUBDIRS =

srcdir = .
top_srcdir = ../../..
subdir = dumpC

default: all

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES
include $(top_srcdir)/PHONY

all-here: $(TARGETS)

# default overwrites
LIBS =

# default definitions
OBJS = $(COBJS:.c=.o)
GENFILES = $(TARGETS)

TESTITOBJS	= dumpC.o
OBJS = $(COBJS:.c=.o) $(SOBJS:.s=.o) $(SSOBJS:.S=.o)

dumpc.prg: $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^ $(LIBS)

include $(top_srcdir)/DEPENDENCIES
