#########################################################################
#			CDROM Library for Linux				#
#########################################################################
CC        = gcc
AR	  = ar


CDROMLIB = ../../../lib/libcdrom/Linux_x86_64/libcdromLinux.a

OBJS	  = LinuxMediaReader.o CDROMMediaReader.o CDROMImgFileReader.o imagereader.o
OBJS	  += dbgout.o

SRC       = ../LinuxMediaReader.cpp ../../Common/CDROMMediaReader.cpp
SRC       += ../../Common/CDROMImgFileReader.cpp ../../Common/imagereader.c
SRC	  += ../../Common/dbgout.c

INCLUDES  = -I../../include
INCLUDES  += -I../../../include/libkvm
INCLUDES  += -I../../../../Build/include/dbgout
INCLUDES  += -I../../../../Build/include
INCLUDES  += -I../../../../Build/include/global
INCLUDES  += -I../../../../Build/include/usbe

CFLAGS += -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -fPIC

RM	  = rm -rf

all: $(OBJS)
ifeq (${shell arch},x86_64)
	mkdir -p ../../../lib/libcdrom/Linux_x86_64/	
	$(AR) $(ARFLAGS) $(CDROMLIB) $(OBJS)
	@echo -e "\n*********************\noutput file libcdromLinux.a stored in software/lib/libcdrom/Linux_x86_64\n*********************"
else
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_32 for x86 platforms."
endif

$(OBJS): $(SRC)
ifeq (${shell arch},x86_64)
	$(CC) $(CFLAGS) -c $(SRC) $(INCLUDES)
else	
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_32 for x86 platforms."
	@exit 1
endif
docs:
	doxygen ../../Doxyfile

clean:
ifeq (${shell arch},x86_64)
	$(RM) $(OBJS)
	$(RM) $(CDROMLIB)
else
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_32 for x86 platforms."
endif
