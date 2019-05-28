#------------------------------------------------------------------------------------------------------
#                         Makefile to build libraphters
#                 Copyright (c) 2012 American Megatrends Inc 
#------------------------------------------------------------------------------------------------------

#------- Set y/n in the following line to enable/disable debugging------
DEBUG = n

#---------------------- Change according to your files ---------------------------------------
LIBRARY_NAME	= libraphters

SRC   = 	dispatcher.c
SRC +=  raphters.c
SRC +=  request.c
SRC +=	response.c

CFLAGS += -I${SPXINC}/fastcgi
CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/unix
CFLAGS += -I${SPXINC}/qdecoder

LIBS += -L${SPXLIB}/fastcgi -lfcgi
LIBS += -L${SPXLIB}/qdecoder -lqdecoder
LIBS += -L${SPXLIB}/unix -lunix

include ${TOOLDIR}/rules/Rules.make.libs

#------------------------------------------------------------------------------------------------------
