DEBUG := n
TARGET := bt_hw
OBJS := bthw_mod.o 

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/bt
EXTRA_CFLAGS += -I${SPXINC}/reset
EXTRA_CFLAGS += -I${SPXINC}/dbgout

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/reset/Module.symvers
KBUILD_EXTRA_SYMBOLS += ${SPXINC}/bt/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
