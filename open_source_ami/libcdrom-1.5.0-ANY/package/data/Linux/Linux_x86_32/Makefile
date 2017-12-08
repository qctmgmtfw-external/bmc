#########################################################################
#			CDROM Library for Linux				#
#########################################################################
CC        = gcc
AR	  = ar

CDROMLIB = ../../../lib/libcdrom/Linux_x86_32/libcdromLinux.a

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

CFLAGS += -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -fPIC #-m64

RM	  = rm -rf

all: $(OBJS)
ifeq (${shell arch},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_64 for x86_64 platforms."
else	
	mkdir -p ../../../lib/libcdrom/Linux_x86_32/	
	$(AR) $(ARFLAGS) $(CDROMLIB) $(OBJS)
	 @echo -e "\n*********************\noutput file libcdromLinux.a stored in software/lib/libcdrom/Linux_x86_32\n*********************"
endif

$(OBJS): $(SRC)
ifeq (${shell arch},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_64 for x86_64 platforms."
	@exit 1
else
	$(CC) $(CFLAGS) -c $(SRC) $(INCLUDES)
endif

docs:
	doxygen ../../Doxyfile
clean:
ifeq (${shell arch},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_64 for x86_64 platforms."
else
	$(RM) $(OBJS)
	$(RM) $(CDROMLIB)
endif


