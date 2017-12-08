#------------------- Makefile for build LIBIPMI in x86 linux --------------


CC = gcc

LIBIPMI_MAJOR_VERSION	= 1
LIBIPMI_MINOR_VERSION	= 0
LIBIPMI_COMMON_NAME	= libipmi

TARGET_DIR		= ../../../lib/libipmi/Linux_x86_32
INSTALL_DIR		= /usr/lib
PACKAGE_DIR		= ../../../../links/libipmi/data
INCDIR			= ../../../../Build/include
LANPARAM_DIR		= ../../../../links/libipmiparams/data

LIBIPMI_SO_NAME		= $(LIBIPMI_COMMON_NAME).so.$(LIBIPMI_MAJOR_VERSION)
LIBIPMI_REAL_NAME	= $(LIBIPMI_COMMON_NAME).so.$(LIBIPMI_MAJOR_VERSION).$(LIBIPMI_MINOR_VERSION)
LIBIPMI_LINKER_NAME	= $(LIBIPMI_COMMON_NAME).so

CFLAGS +=-DCONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE=0x10000

#--------------------Include files-----------------------------------------
CFLAGS		+= -I$(INCDIR)
CFLAGS		+= -I$(INCDIR)/ipmi
CFLAGS		+= -I$(INCDIR)/ipmipdkcmds
CFLAGS		+= -I$(INCDIR)/libipmi
CFLAGS		+= -I$(INCDIR)/global
CFLAGS		+= -I$(INCDIR)/fmh
CFLAGS		+= -I$(INCDIR)/flash
CFLAGS		+= -I$(INCDIR)/network
CFLAGS		+= -I$(INCDIR)/hostname
CFLAGS		+= -I$(INCDIR)/smtp
CFLAGS		+= -I$(INCDIR)/dbgout
CFLAGS 		+= -I$(LANPARAM_DIR)/
CFLAGS		+= -I$(INCDIR)/pdk
CFLAGS		+= -I$(INCDIR)/ncml

#------------------------- LIBIPMI core source files -------------------------
SRC		=  $(PACKAGE_DIR)/aes.c $(PACKAGE_DIR)/hmac.c $(PACKAGE_DIR)/sha1.c
SRC		+= $(PACKAGE_DIR)/dbgout.c
SRC		+= $(PACKAGE_DIR)/libipmi_network.c $(PACKAGE_DIR)/libipmi_rmcp.c
SRC		+= $(PACKAGE_DIR)/libipmi_session.c
SRC		+= $(PACKAGE_DIR)/libipmi_network_session.c
SRC		+= $(PACKAGE_DIR)/libipmi_helper.c
SRC		+= $(PACKAGE_DIR)/libipmi_serial_session.c
SRC		+= $(PACKAGE_DIR)/libipmi_uds_session.c
SRC		+= $(PACKAGE_DIR)/libipmi_usb_session.c
SRC		+= $(PACKAGE_DIR)/scsi.c
SRC		+= $(PACKAGE_DIR)/libipmi_fru.c
#SRC		+= ../libipmi_ipmb_session.c
#SRC		+= ../ipmb_interface.c
SRC		+= $(LANPARAM_DIR)/params.c
#-------------------- Higher Level API ---------------------------------------
SRC		+= $(PACKAGE_DIR)/libipmi_IPM.c
SRC		+= $(PACKAGE_DIR)/libipmi_AppDevice.c
SRC		+= $(PACKAGE_DIR)/libipmi_PEF.c
SRC		+= $(PACKAGE_DIR)/libipmi_sensor.c
SRC		+= $(PACKAGE_DIR)/libipmi_sdr.c
SRC		+= $(PACKAGE_DIR)/libipmi_AMIOEM.c
SRC		+= $(PACKAGE_DIR)/libipmi_StorDevice.c
SRC		+= $(PACKAGE_DIR)/libipmi_XportDevice.c
SRC		+= $(PACKAGE_DIR)/sensor_helpers.c
SRC		+= $(PACKAGE_DIR)/libipmi_Chassis.c
SRC		+= $(PACKAGE_DIR)/libipmi_AMIFlash.c
SRC		+= $(PACKAGE_DIR)/sdr_cache.c  

OBJ		=  ./*.o

all: $(TARGET_DIR)/$(LIBIPMI_LINKER_NAME) 

$(TARGET_DIR)/$(LIBIPMI_LINKER_NAME):
ifeq (${shell arch},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_64 for x86_64 platforms."
else
	mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -fPIC -g -c -Wall $(SRC)
	$(CC) -shared -Wl,-soname,$(LIBIPMI_SO_NAME) \
		-o $(LIBIPMI_REAL_NAME) $(OBJ) -lc
	ln -sf $(LIBIPMI_REAL_NAME) $(LIBIPMI_LINKER_NAME)
	ln -sf $(LIBIPMI_REAL_NAME) $(LIBIPMI_SO_NAME)
	mv $(LIBIPMI_REAL_NAME) $(TARGET_DIR)
	mv $(LIBIPMI_LINKER_NAME) $(TARGET_DIR)
	mv $(LIBIPMI_SO_NAME) $(TARGET_DIR)
	@echo -e "\n***********************\noutput file stored in software/lib/libipmi/Linux_x86_32\n***********************"
endif

install:$(TARGET_DIR)/$(LIBIPMI_LINKER_NAME)
ifeq (${shell arch},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_64 for x86_64 platforms."
else
	install -c $(TARGET_DIR)/$(LIBIPMI_REAL_NAME) $(INSTALL_DIR)
	ln -sf $(INSTALL_DIR)/$(LIBIPMI_REAL_NAME) $(INSTALL_DIR)/$(LIBIPMI_LINKER_NAME)
	ln -sf $(INSTALL_DIR)/$(LIBIPMI_REAL_NAME) $(INSTALL_DIR)/$(LIBIPMI_SO_NAME)
	@echo -e "\n***********************\noutput file installed in $(INSTALL_DIR)\n***********************"
endif


clean:
ifeq (${shell arch},x86_64)
	@echo "Architecture Mismatch. Please use Makefile in Linux_x86_64 for x86_64 platforms."
else
	rm -f $(OBJ)
	rm -f $(TARGET_DIR)/*
	rm -f $(INSTALL_DIR)/$(LIBIPMI_SO_NAME)
	rm -f $(INSTALL_DIR)/$(LIBIPMI_LINKER_NAME)
	rm -f $(INSTALL_DIR)/$(LIBIPMI_REAL_NAME)
endif

