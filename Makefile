#
# Makefile for ovdi
#

SHELL = /bin/sh
SUBDIRS = src

srcdir = .
top_srcdir = ..
subdir = ovdi

default: all

include $(top_srcdir)/CONFIGVARS
include $(top_srcdir)/RULES
include $(top_srcdir)/PHONY

all-here:

# default overwrites

# default definitions
