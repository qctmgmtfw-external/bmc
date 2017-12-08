#
# iniparser Makefile
#

CC      = gcc -m32
CFLAGS  = -O3 -Wall
AR	    = ar
ARFLAGS = rcv
RM      = rm -f

# Implicit rules

SUFFIXES = .o .c .h .a .so .sl

COMPILE.c=$(CC) $(CFLAGS) -c
.c.o:
	@(echo "compiling $< ...")
	@($(COMPILE.c) -o $@ $<)


SRCS = fmhcore.c \

OBJS = $(SRCS:.c=.o)

all:	libfmhcore.a

libfmhcore.a:	$(OBJS)
	@($(AR) $(ARFLAGS) libfmhcore.a $(OBJS))

clean:
	@($(RM) $(OBJS) libfmhcore.a)

