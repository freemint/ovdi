#
# Makefile for et6knova.vdi - Graphics driver for oVDI using ET6000 emulator.prg.
#
TARGET = ft2.vdi

SHELL = /bin/sh

srcdir = .
top_srcdir = ../..
subdir = ft2

default: all

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES
include $(top_srcdir)/PHONY

all-here: $(TARGET)

# default overwrites
INCLUDES = -I$(top_srcdir)
DEFINITIONS =

LD = $(CC) $(GENERAL) -nostdlib -Wl,--entry -Wl,_init
LIBS = $(LIBKERN) -lc -lfreetype -lz
CPU = 020-60

# default definitions
SGENFILES = $(TARGET)
OBJS = $(COBJS:.c=.o) $(SOBJS:.s=.o)

$(TARGET): $(OBJS) $(LIBKERNTARGET) #$(LIBGENERICTARGET)
	$(LD) $(CFLAGS) $(GENERAL) -o $@ $(OBJS) $(LIBS)


include $(top_srcdir)/DEPENDENCIES
