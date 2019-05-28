#----------------------------------------------------------------------
#                         Makefile to build pcie
#                 Copyright (c) 2014 American Megatrends Inc 
#-----------------------------------------------------------------------

DEBUG     := n
TARGET    := mctppcie
OBJS      := mctppciemain.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/dbgout
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/mctppcie

-include ${TOOLDIR}/rules/Rules.make.drivers
