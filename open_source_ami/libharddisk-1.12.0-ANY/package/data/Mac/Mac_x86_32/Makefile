#########################################################################
#			FLOPPY Library for Linux				#
#########################################################################
CC        = gcc
AR	  = ar

FLOPPYLIB  = ../../../lib/libharddisk/Mac_x86_32/libharddiskMac.a

OBJS	  = MacHarddiskReader.o HarddiskMediaReader.o
OBJS	 += HarddiskImgFileReader.o
OBJS	 += harddiskimagereader.o
OBJS	  += dbgout.o

SRC       = ../MacHarddiskReader.cpp ../../Common/HarddiskMediaReader.cpp
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
ifeq (${shell echo $$HOSTTYPE},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Mac_x86_64 for x86_64 platforms."
else	
	mkdir -p ../../../lib/libharddisk/Mac_x86_32/
	$(AR) $(ARFLAGS) $(FLOPPYLIB) $(OBJS)
	@echo -e "\n*********************\noutput file libharddiskMac.a stored in software/lib/libharddisk/Mac_x86_32\n*********************"
endif
$(OBJS): $(SRC)
ifeq (${shell echo $$HOSTTYPE},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Mac_x86_64 for x86_64 platforms."
	@exit 1
else
	$(CC) $(CFLAGS) -c $(SRC) $(INCLUDES)
endif

docs:
	doxygen ../../Doxyfile

clean:
ifeq (${shell echo $$HOSTTYPE},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Mac_x86_64 for x86_64 platforms."
else
	$(RM) $(OBJS)
	$(RM) $(FLOPPYLIB)
endif
