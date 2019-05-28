DEBUG := n
TARGET := watchdog_hw
OBJS := ast_watchdog.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/watchdog

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/watchdog/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
