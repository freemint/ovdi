#
# Makefile for highwire
#
TARGET = ../libft2.a

# compiler settings
CC = gcc -g
AS = $(CC) -c
LD = $(CC) 
CP = cp
RM = rm -f

#CPU = 68000
#CPU = 68030
#CPU = 68040
CPU = 68020-60

DEFS =

OPTS = $(CPU:%=-m%) -fomit-frame-pointer \
       -O2

WARN = \
	-Wall \
	-Wmissing-prototypes \
	-Wshadow \
	-Wpointer-arith \
	-Wcast-qual -Werror

#INCLUDE = -I/usr/GEM/include

CFLAGS = $(INCLUDE) $(WARN) $(OPTS) $(DEFS)
ASFLAGS = $(OPTS)
LDFLAGS = 
#LIBS = -L/usr/GEM/lib -lgem -liio -lungif -ljpeg -lpng -lz -lm #-lsocket
LIBS = -lfreetype -lz

OBJDIR = obj$(CPU:68%=.%)


#
# C source files
#
$(OBJDIR)/%.o: %.c
	@echo "$(CC) $(CFLAGS) -c $< -o $@"; \
	$(CC) -Wp,-MMD,.deps/$(<:.c=.P_) $(CFLAGS) -c $< -o $@
	@cat .deps/$(<:.c=.P_) \
	    | sed "s,^\(.*\)\.o:,$(OBJDIR)/\1.o:," > .deps/$(<:.c=.P)
	@rm -f .deps/$(<:.c=.P_)

#
# files
#
CFILES = \
	ft2_calls.c

HDR =	\
	ft2_calls.h

SFILES = 

OBJS = $(SFILES:%.s=$(OBJDIR)/%.o) $(CFILES:%.c=$(OBJDIR)/%.o)
OBJS_MAGIC := $(shell mkdir ./$(OBJDIR) > /dev/null 2>&1 || :)

DEPENDENCIES = $(addprefix ./.deps/, $(patsubst %.c,%.P,$(CFILES)))


$(TARGET): $(OBJS)
	$(LD) -o $@ $(CFLAGS) $(OBJS) $(LIBS)

000: ; $(MAKE) CPU=68000
030: ; $(MAKE) CPU=68030
040: ; $(MAKE) CPU=68040

clean:
	rm -Rf *.bak */*.bak */*/*.bak *[%~] */*[%~] */*/*[%~]
	rm -Rf obj.* */obj.* */*/obj.* .deps */.deps */*/.deps *.o */*/*.o
	rm -Rf *.app *.[gt]tp *.prg *.ovl */*.ovl

distclean: clean


#
# dependencies
#
DEPS_MAGIC := $(shell mkdir ./.deps > /dev/null 2>&1 || :)

-include $(DEPENDENCIES)
