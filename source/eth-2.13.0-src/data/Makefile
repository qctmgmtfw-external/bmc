DEBUG     		:= n
TARGET	 	  	:= eth
OBJS			:= eth_mod.o ether.o rndis.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/dbgout
EXTRA_CFLAGS += -I${SPXINC}/usbe
EXTRA_CFLAGS += -I${SPXINC}/iUSB

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/usbe/Module.symvers
KBUILD_EXTRA_SYMBOLS += ${SPXINC}/iUSB/Module.symvers

-include $(TOOLDIR)/rules/Rules.make.drivers


