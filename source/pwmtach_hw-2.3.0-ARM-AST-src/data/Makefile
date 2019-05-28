DEBUG := n
TARGET := pwmtach_hw
OBJS := ast_pwmtach.o

EXTRA_CFLAGS += -I${SPXINC}/global 
EXTRA_CFLAGS += -I${SPXINC}/pwmtach
EXTRA_CFLAGS += -I${SPXINC}/helper

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/pwmtach/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
