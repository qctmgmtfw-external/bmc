DEBUG     := n
TARGET	  := kcs
OBJS      := kcs_timeout.o kcsmain.o proc.o

EXTRA_CFLAGS += -I${SPXINC}/global 
EXTRA_CFLAGS += -I${SPXINC}/dbgout
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/reset

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/reset/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
