#
# Makefile for 
#
TARGET = libgeneric.a

SHELL = /bin/sh
SUBDIRS =

srcdir = .
top_srcdir = ../..
subdir = generic

default: all

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES
include $(top_srcdir)/PHONY

all-here: $(TARGET)

# default overwrites

DEFINITIONS =
#LD = $(CC) -nostdlib -Wl,--entry -Wl,__start
LIBS =  $(LIBKERN)
CPU = 020-60
STRIPFLAGS = 

# default definitions
SGENFILES = $(TARGET)
OBJS = $(COBJS:.c=.o) $(SOBJS:.s=.o) $(SSOBJS:.S=.o)


$(TARGET): $(OBJS)
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@
	
#	$(LD) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# include $(top_srcdir)/DEPENDENCIES
