#########################################################################
#			FLOPPY Library for Linux				#
#########################################################################
CC        = gcc
AR	  = ar

FLOPPYLIB  = ../../../lib/libfloppy/Mac_x86_32/libfloppyMac.a

OBJS	  = MacFloppyReader.o FloppyMediaReader.o
OBJS	 += FloppyImgFileReader.o
OBJS	 += floppyimagereader.o
OBJS	  += dbgout.o

SRC       = ../MacFloppyReader.cpp ../../Common/FloppyMediaReader.cpp
SRC       += ../../Common/FloppyImgFileReader.cpp
SRC       += ../../Common/floppyimagereader.c
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
	mkdir -p ../../../lib/libfloppy/Mac_x86_32/
	$(AR) $(ARFLAGS) $(FLOPPYLIB) $(OBJS)
	@echo -e "\n*********************\noutput file libfloppyMac.a stored in software/lib/libfloppy/Mac_x86_32\n*********************"
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
