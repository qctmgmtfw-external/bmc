#########################################################################
#			CDROM Library for Mach 				#
#########################################################################
CC        = gcc
AR	  = ar

CDROMLIB  = ../../../lib/libcdrom/Mac_x86_64/libcdromMac.a

OBJS	  = MacMediaReader.o CDROMMediaReader.o CDROMImgFileReader.o imagereader.o 
OBJS	  += dbgout.o

SRC       = ../MacMediaReader.cpp ../../Common/CDROMMediaReader.cpp
SRC       += ../../Common/CDROMImgFileReader.cpp 
SRC	  += ../../Common/imagereader.c
SRC	  += ../../Common/dbgout.c

INCLUDES  = -I../../include
INCLUDES  += -I../../../../Build/include/dbgout
INCLUDES  += -I../../../include/libkvm
INCLUDES  += -I../../../../Build/include
INCLUDES  += -I../../../../Build/include/global
INCLUDES  += -I../../../../Build/include/usbe

CFLAGS += -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE 

RM	  = rm -rf

all: $(OBJS)
ifeq (${shell echo $$HOSTTYPE},x86_64)
	mkdir -p ../../../lib/libcdrom/Mac_x86_64/	
	$(AR) $(ARFLAGS) $(CDROMLIB) $(OBJS)
	 @echo -e "\n*********************\noutput file libcdromMac.a stored in software/lib/libcdrom/Mac_x86_64\n*********************"
else
	@echo "Architecture Mismatch. Please use Makefile in Mac_x86_32 for x86 platforms."
endif
$(OBJS): $(SRC)
ifeq (${shell echo $$HOSTTYPE},x86_64)
	$(CC) $(CFLAGS) -c $(SRC) $(INCLUDES)
else	
	@echo "Architecture Mismatch. Please use Makefile in Mac_x86_32 for x86 platforms."
	@exit 1
endif

docs:
	doxygen ../../Doxyfile

clean:
ifeq (${shell echo $$HOSTTYPE},x86_64)
	$(RM) $(OBJS)
	$(RM) $(CDROMLIB)
else
	@echo "Architecture Mismatch. Please use Makefile in Mac_x86_32 for x86 platforms."
endif

