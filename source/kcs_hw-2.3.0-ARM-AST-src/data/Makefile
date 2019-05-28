DEBUG := n
TARGET := kcs_hw
OBJS := ast_kcs.o 

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/kcs

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/kcs/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
