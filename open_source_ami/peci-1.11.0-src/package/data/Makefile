#----------------------------------------------------------------------
#                         Makefile to build peci
#                 Copyright (c) 2008 American Megatrends Inc 
#-----------------------------------------------------------------------

DEBUG     := n
TARGET    := peci
OBJS      := pecimain.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/dbgout
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/peci

-include ${TOOLDIR}/rules/Rules.make.drivers
