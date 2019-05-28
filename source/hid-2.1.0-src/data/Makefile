DEBUG     		:= n
TARGET	 	  	:= hid
OBJS			:= hid_mod.o keybd.o mouse.o 
ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT
OBJS+= pipe.o
endif

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/dbgout
EXTRA_CFLAGS += -I${SPXINC}/usbe
EXTRA_CFLAGS += -I${SPXINC}/iUSB

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/usbe/Module.symvers
KBUILD_EXTRA_SYMBOLS += ${SPXINC}/iUSB/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers


