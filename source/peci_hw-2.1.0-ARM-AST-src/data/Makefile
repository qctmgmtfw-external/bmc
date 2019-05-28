DEBUG := n
TARGET := peci_hw
OBJS := ast_peci.o

EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/peci

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/peci/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
