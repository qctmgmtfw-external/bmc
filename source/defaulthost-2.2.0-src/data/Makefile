#----------------------------------------------------------------------
#                     Makefile to build app defaulthost
#                 Copyright (c) 2008 American Megatrends Inc 
#-----------------------------------------------------------------------
#------- Comment/uncomment the following line to enable/disable debugging---------
DEBUG = n
#---------------------- Change according to your files ----------------------------
TARGET = defaulthost 			# Output file
SRC 		= defaulthost.c	# Source File


CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/hostname
CFLAGS += -I${SPXINC}/featuredefine

LIBS += -L${SPXLIB}/dbgout -ldbgout
LIBS += -L${SPXLIB}/hostname -lhostname
LIBS += -L${SPXLIB}/iniparser -liniparser 
LIBS += -L${SPXLIB}/featuredefine -lfeaturedefine 

include ${TOOLDIR}/rules/Rules.make.apps

