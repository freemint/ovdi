#
# Makefile for bioskbd.vdi - oVDI Bios keyboard driver
#
TARGET = bioskbd.vdi

SHELL = /bin/sh
SUBDIRS = 

srcdir = .
top_srcdir = ../..
subdir = bioskbd

default: all

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES
include $(top_srcdir)/PHONY

all-here: $(TARGET)

# default overwrites
INCLUDES = -I$(top_srcdir)
DEFINITIONS =

LD = $(CC) -nostdlib -Wl,--entry -Wl,_init
LIBS = $(LIBKERN) -lgcc
CPU = 020-60

# default definitions
SGENFILES = $(TARGET)
OBJS = $(COBJS:.c=.o) $(SOBJS:.s=.o)

$(TARGET): $(OBJS) $(LIBKERNTARGET)
	$(LD) $(CFLAGS) $(GENERAL) -o $@ $(OBJS) $(LIBS)


include $(top_srcdir)/DEPENDENCIES
