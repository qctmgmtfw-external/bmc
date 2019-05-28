DEBUG := n
TARGET := gpio_hw
OBJS := gpiomain_hw.o 

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/gpio

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/gpio/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers

