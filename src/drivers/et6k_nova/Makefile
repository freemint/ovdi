#
# Makefile for et6knova.vdi - Graphics driver for oVDI using ET6000 emulator.prg.
#
TARGET = et6knova.vdi

SHELL = /bin/sh
SUBDIRS =

srcdir = .
top_srcdir = ../..
subdir = et6k_nova

default: all

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES
include $(top_srcdir)/PHONY

all-here: $(TARGET)

# default overwrites
INCLUDES = -I$(top_srcdir)
DEFINITIONS =

LD = $(CC) $(GENERAL) -nostdlib -Wl,--entry -Wl,_init
LIBS = $(LIBKERN) $(LIBGENERIC) -lgcc
CPU = 020-60

# default definitions
SGENFILES = $(TARGET)
OBJS = $(COBJS:.c=.o) $(SOBJS:.s=.o)

$(TARGET): $(OBJS) $(LIBKERNTARGET) $(LIBGENERICTARGET)
	$(LD) $(CFLAGS) $(GENERAL) -o $@ $(OBJS) $(LIBS)


include $(top_srcdir)/DEPENDENCIES
