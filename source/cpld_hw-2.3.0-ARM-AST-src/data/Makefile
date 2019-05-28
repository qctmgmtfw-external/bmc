DEBUG := n
TARGET := cpld_hw

OBJS := cpld.o
OBJS += lattice.o
OBJS += jed.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/jtag
EXTRA_CFLAGS += -I${SPXINC}/jtag_hw

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/jtag/Module.symvers
KBUILD_EXTRA_SYMBOLS += ${SPXINC}/jtag_hw/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
