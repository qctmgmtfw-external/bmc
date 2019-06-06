#-------------------------------------------------------------------------------------------------------------------------
#                      Makefile to build libami encoding
#                 Copyright (c) 2008 American Megatrends Inc 
#-------------------------------------------------------------------------------------------------------------------------

#------- Comment/uncomment the following line to enable/disable debugging-------------------
DEBUG = n

#---------------------- Change according to your files ---------------------------------------------------------
LIBRARY_NAME 	= libencoding

SRC   +=  CyoDecode.c
SRC   +=  CyoEncode.c
SRC   +=  Encode.c

CFLAGS += -I${TARGETDIR}/usr/include
CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/ipmi
CFLAGS += -I${SPXINC}/pdk
CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/encryption
CFLAGS += -I${SPXINC}/unix

include ${TOOLDIR}/rules/Rules.make.libs
#----------------------------------------------------------------------------------------------------------------------------
