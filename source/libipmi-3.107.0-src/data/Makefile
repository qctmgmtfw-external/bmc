DEBUG = n
ifeq ($(CONFIG_SPX_FEATURE_MEMORY_TRACE_QOM), YES)
DEBUG = y
endif

ifeq ($(CONFIG_SPX_FEATURE_MEMORY_TRACE_SMASH), YES)
DEBUG = y
endif

LIBRARY_NAME=libipmi

SRC		+= libipmi_utils.c
SRC		+= libipmi_network.c libipmi_rmcp.c
SRC		+= libipmi_session.c
SRC		+= libipmi_network_session.c
SRC		+= libipmi_helper.c
SRC		+= libipmi_serial_session.c
SRC		+= libipmi_usb_session.c
SRC      += libipmi_uds_session.c
SRC		+= scsi.c

SRC		+= libipmi_IPM.c
SRC		+= libipmi_AppDevice.c
#SRC	+= libipmi_kcs_session.c
ifneq ($(CONFIG_SPX_FEATURE_DISABLE_LIBIPMI_IPMB_INTERFACE), YES)
SRC		+= libipmi_ipmb_session.c
SRC		+= ipmb_interface.c
SRC		+= ipmb_utils.c
endif
SRC		+= libipmi_PEF.c
SRC		+= libipmi_sensor.c
SRC		+= libipmi_sdr.c
SRC		+= libipmi_AMIOEM.c
SRC		+= libipmi_StorDevice.c 
SRC		+= sdr_cache.c
SRC		+= libipmi_XportDevice.c
SRC		+= sensor_helpers.c
SRC		+= libipmi_Chassis.c
SRC		+= libipmi_fru.c
SRC		+= libipmi_ncml.c
SRC		+= libipmi_AMIFlash.c
SRC		+= libipmi_Misc.c
SRC		+= libipmi_sysinventory.c

CFLAGS += -I./include
CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/fmh
CFLAGS += -I${SPXINC}/pdk
CFLAGS += -I${SPXINC}/ipmi
CFLAGS += -I${SPXINC}/libipmi
CFLAGS += -I${SPXINC}/network
CFLAGS += -I${SPXINC}/smtp
CFLAGS += -I${SPXINC}/flash
CFLAGS += -I$(SPXINC)/pamhelper
ifneq ($(CONFIG_SPX_FEATURE_DISABLE_LIBIPMI_IPMB_INTERFACE), YES)
CFLAGS += -I${SPXINC}/i2c
endif
CFLAGS += -I${SPXINC}/unix
CFLAGS += -I${SPXINC}/ncml
CFLAGS += -I${SPXINC}/hostname
CFLAGS += -I${SPXINC}/encryption
CFLAGS += -I${TARGETDIR}/usr/include

LIBS += -L$(SPXLIB)/libm -lm
ifneq ($(CONFIG_SPX_FEATURE_DISABLE_LIBIPMI_IPMB_INTERFACE), YES)
LIBS += -L$(SPXLIB)/i2c -li2c
endif
LIBS += -L${SPXLIB}/encryption -lencryption
LIBS += -L${SPXLIB}/unix -lunix
LIBS += ${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB}/libssl.so

include ${TOOLDIR}/rules/Rules.make.libs
