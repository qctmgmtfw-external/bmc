DEBUG := n
TARGET := snoop_hw
OBJS := ast_snoop.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/snoop

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/snoop/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
