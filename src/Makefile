#
# Makefile for 
#
TARGET = ovdi.prg

SHELL = /bin/sh
SUBDIRS =

srcdir = .
top_srcdir = .
subdir = src

default: all

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES
include $(top_srcdir)/PHONY

all-here: $(TARGET)

# default overwrites
CFLAGS-gdf_text.c = -Wno-uninitialized
#CFLAGS-mousedrv.c = -fcall-saved-d0 -fcall-saved-d1 -fcall-saved-a0, -fcall-saved-a1

DEFINITIONS =
LD = $(CC) -nostdlib -Wl,--entry -Wl,__start
LIBS =  $(LIBKERN) $(LIBGENERIC)
CPU = 020-60
STRIPFLAGS = 

# default definitions
SGENFILES = $(TARGET)
OBJS = $(COBJS:.c=.o) $(SOBJS:.s=.o) $(SSOBJS:.S=.o)


$(TARGET): $(OBJS) $(LIBKERNTARGET) $(LIBGENERICTARGET)
	$(LD) $(CFLAGS) $(GENERAL) -o $@ $(OBJS) $(LIBS)

include $(top_srcdir)/DEPENDENCIES
