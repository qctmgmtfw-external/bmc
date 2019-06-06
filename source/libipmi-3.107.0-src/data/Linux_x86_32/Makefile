#------------------- Makefile for build LIBIPMI in x86 linux --------------


CC = gcc

LIBIPMI_MAJOR_VERSION	= 1
LIBIPMI_MINOR_VERSION	= 0
LIBIPMI_COMMON_NAME	= libipmi

TARGET_DIR		= ../../../../software/lib/libipmi/Linux_x86_32
INSTALL_DIR		= /usr/lib

LIBIPMI_SO_NAME		= $(LIBIPMI_COMMON_NAME).so.$(LIBIPMI_MAJOR_VERSION)
LIBIPMI_REAL_NAME	= $(LIBIPMI_COMMON_NAME).so.$(LIBIPMI_MAJOR_VERSION).$(LIBIPMI_MINOR_VERSION)
LIBIPMI_LINKER_NAME	= $(LIBIPMI_COMMON_NAME).so

CFLAGS +=-DCONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE=0x10000

#--------------------Include files-----------------------------------------
CFLAGS		+= -I../../../../Build/include
CFLAGS		+= -I../../../../Build/include/ipmi
CFLAGS		+= -I../../../../Build/include/libipmi
CFLAGS		+= -I../../../../Build/include/global
CFLAGS		+= -I../../../../Build/include/fmh
CFLAGS		+= -I../../../../Build/include/flash
CFLAGS 		+= -I../../../../Build/include/network
CFLAGS		+= -I../../../../Build/include/smtp
CFLAGS		+= -I../../../../Build/include/dbgout

ifdef CONFIG_SPX_FEATURE_SNMP_SUPPORT
CFLAGS      += -I../../../../Build/include/snmpusers
endif

#CFLAGS 		+= -I../../../../../firmware/IPMI/core/Include

#------------------------- LIBIPMI core source files -------------------------
SRC		=  ../aes.c ../hmac.c ../sha1.c
SRC		+= ../dbgout.c
SRC		+= ../libipmi_network.c ../libipmi_rmcp.c
SRC		+= ../libipmi_session.c
SRC		+= ../libipmi_network_session.c
SRC		+= ../libipmi_helper.c
SRC		+= ../libipmi_serial_session.c
#SRC		+= ../libipmi_kcs_session.c
SRC		+= ../libipmi_usb_session.c
SRC		+= ../scsi.c
#SRC		+= ../libipmi_ipmb_session.c
#SRC		+= ../ipmb_interface.c
#SRC		+= ../ipmb_utils.c
#-------------------- Higher Level API ---------------------------------------
SRC		+= ../libipmi_IPM.c
SRC		+= ../libipmi_AppDevice.c
SRC		+= ../libipmi_PEF.c
SRC		+= ../libipmi_sensor.c
SRC		+= ../libipmi_sdr.c
SRC		+= ../libipmi_AMIOEM.c
SRC		+= ../libipmi_StorDevice.c
SRC		+= ../libipmi_XportDevice.c
SRC		+= ../sensor_helpers.c
SRC		+= ../libipmi_Chassis.c
SRC		+= ../libipmi_AMIFlash.c
SRC             += ../sdr_cache.c  

OBJ		=  ./*.o

all: $(TARGET_DIR)/$(LIBIPMI_LINKER_NAME) 

$(TARGET_DIR)/$(LIBIPMI_LINKER_NAME):
	mkdir -p $(TARGET_DIR)
	$(CC) $(CFLAGS) -fPIC -g -c -Wall $(SRC)
	$(CC) -shared -Wl,-soname,$(LIBIPMI_SO_NAME) \
		-o $(LIBIPMI_REAL_NAME) $(OBJ) -lc
	ln -sf $(LIBIPMI_REAL_NAME) $(LIBIPMI_LINKER_NAME)
	ln -sf $(LIBIPMI_REAL_NAME) $(LIBIPMI_SO_NAME)
	mv $(LIBIPMI_REAL_NAME) $(TARGET_DIR)
	mv $(LIBIPMI_LINKER_NAME) $(TARGET_DIR)
	mv $(LIBIPMI_SO_NAME) $(TARGET_DIR)

install:$(TARGET_DIR)/$(LIBIPMI_LINKER_NAME)
	install -c $(TARGET_DIR)/$(LIBIPMI_REAL_NAME) $(INSTALL_DIR)
	ln -sf $(INSTALL_DIR)/$(LIBIPMI_REAL_NAME) $(INSTALL_DIR)/$(LIBIPMI_LINKER_NAME)
	ln -sf $(INSTALL_DIR)/$(LIBIPMI_REAL_NAME) $(INSTALL_DIR)/$(LIBIPMI_SO_NAME)


clean:
	rm -f $(OBJ)
	rm -f $(TARGET_DIR)/*
	rm -f $(INSTALL_DIR)/$(LIBIPMI_SO_NAME)
	rm -f $(INSTALL_DIR)/$(LIBIPMI_LINKER_NAME)
	rm -f $(INSTALL_DIR)/$(LIBIPMI_REAL_NAME)

