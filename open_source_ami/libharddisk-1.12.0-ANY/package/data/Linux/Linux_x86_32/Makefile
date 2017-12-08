#########################################################################
#			FLOPPY Library for Linux				#
#########################################################################
CC        = gcc
AR	  = ar

FLOPPYLIB  = ../../../lib/libharddisk/Linux_x86_32/libharddiskLinux.a

OBJS	  = LinuxHarddiskReader.o HarddiskMediaReader.o
OBJS	 += HarddiskImgFileReader.o
OBJS	 += harddiskimagereader.o
OBJS	  += dbgout.o

SRC       = ../LinuxHarddiskReader.cpp ../../Common/HarddiskMediaReader.cpp
SRC       += ../../Common/HarddiskImgFileReader.cpp
SRC       += ../../Common/harddiskimagereader.c
SRC	  += ../../Common/dbgout.c

INCLUDES  = -I../../include
INCLUDES  += -I../../../../Build/include/dbgout
INCLUDES  += -I../../../include/libkvm
INCLUDES  += -I../../../../Build/include
INCLUDES  += -I../../../../Build/include/global
INCLUDES  += -I../../../../Build/include/usbe
CFLAGS   += -DPROBE_USB_FLOPPIES -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 
CFLAGS   += -D_GNU_SOURCE -fPIC
RM	  = rm -rf

all: $(OBJS)
ifeq (${shell arch},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_64 for x86_64 platforms."
else
	mkdir -p ../../../lib/libharddisk/Linux_x86_32/
	$(AR) $(ARFLAGS) $(FLOPPYLIB) $(OBJS)
	 @echo -e "\n*********************\noutput file libfloppyLinux.a stored in software/lib/libharddisk/Linux_x86_32\n*********************"
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
	$(RM) $(FLOPPYLIB)
endif
