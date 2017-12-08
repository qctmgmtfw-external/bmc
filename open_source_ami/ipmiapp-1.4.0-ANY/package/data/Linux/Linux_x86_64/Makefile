CC = gcc
TARGET_DIR = ../../obj/Linux_x86_64
CFLAGS = -DCONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE=0x10000
CFLAGS += -I../../../../Build/include/dbgout
CFLAGS += -I../../../../Build/incluce/
CFLAGS += -I../../../../Build/include/ipmi
CFLAGS += -I../../../../Build/include/libipmi
CFLAGS += -I../../../../Build/include/global
CFLAGS += -I../../../../Build/include/fmh
CFLAGS += -I../../../../Build/include/flash


LIBS = -L../../../lib/libipmi/Linux_x86_64 -lipmi
SRC =  ../../Common/Nwusb.c ../../Common/utils.c 
OBJ = *.o
OUTPUT = $(TARGET_DIR)/IPMIApp

all:
ifeq (${shell arch},x86_64)
	mkdir -p $(TARGET_DIR)
	$(CC) -c $(SRC) $(CFLAGS)
	$(CC) -o $(OUTPUT) $(OBJ) $(LIBS)
	@echo -e "\n***********************\noutput file  stored in /obj/Linux_x86_64\n***********************"
else
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_32 for x86_32 platforms."
endif
clean:
ifeq (${shell arch},x86_64)
	rm -rf $(OUTPUT)
	rm -rf $(OBJ)
else
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_32 for x86_32 platforms."
endif
