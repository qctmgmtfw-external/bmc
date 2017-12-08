#########################################################################
#			FLOPPY Library for Linux				#
#########################################################################
CC        = gcc
AR	  = ar

FLOPPYLIB  = ../../../lib/libfloppy/Linux_x86_64/libfloppyLinux.a

OBJS	  = LinuxFloppyReader.o FloppyMediaReader.o
OBJS	 += FloppyImgFileReader.o
OBJS	 += floppyimagereader.o
OBJS	  += dbgout.o

SRC       = ../LinuxFloppyReader.cpp ../../Common/FloppyMediaReader.cpp
SRC       += ../../Common/FloppyImgFileReader.cpp
SRC       += ../../Common/floppyimagereader.c
SRC	  += ../../Common/dbgout.c

INCLUDES  = -I../../include
#INCLUDES  += -I../../../../DBGOUT
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
	mkdir -p ../../../lib/libfloppy/Linux_x86_64/
	$(AR) $(ARFLAGS) $(FLOPPYLIB) $(OBJS)
	 @echo -e "\n*********************\noutput file libfloppyLinux.a stored in software/lib/libfloppy/Linux_x86_64\n*********************"
else
	@echo -e "Architecture Mismatch. Please use Makefile.x86 for x86 architecture."
	@exit 1
endif

$(OBJS): $(SRC)
ifeq (${shell arch},x86_64)	
	$(CC) $(CFLAGS) -c $(SRC) $(INCLUDES)
else
	@echo -e "Architecture Mismatch. Please use Makefile.x86 for x86 architecture."
	@exit 1
endif
docs:
	doxygen ../../Doxyfile

clean:
ifeq (${shell arch},x86_64)
	$(RM) $(OBJS)
	$(RM) $(FLOPPYLIB)
else
	@echo -e "Architecture Mismatch. Please use Makefile.x86 for x86 architecture."
endif

