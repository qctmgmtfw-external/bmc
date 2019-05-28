DEBUG := n
TARGET := adc_hw
OBJS := adcmain_hw.o 

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/adc

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/adc/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers

