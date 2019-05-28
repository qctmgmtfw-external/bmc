DEBUG     := n
TARGET    := ncsi

OBJS   := core/ethtool.o
OBJS   += core/ncsi_if.o 
OBJS   += core/ncsicore.o 
OBJS   += core/ncsilib.o
OBJS   += core/interfaces.o
OBJS   += core/comm.o
OBJS   += core/proc.o
OBJS   += helper/datadisp.o

ifeq ($(DEBUG),y)
EXTRA_CFLAGS += -DNCSI_DEBUG
endif

EXTRA_CFLAGS += -I$(PWD)/include		# Local header files 

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper

#EXTRA_CFLAGS += -DNCSI_DEFER_DETECT

#---------------- Do not modify any thing below this line ------------------------
-include ${TOOLDIR}/rules/Rules.make.drivers


