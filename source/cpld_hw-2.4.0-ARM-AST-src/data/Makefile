DEBUG := n
TARGET := cpld_hw

OBJS := cpld.o
ifdef CONFIG_SPX_FEATURE_ALTERA_SUPPORT
    OBJS += altera.o
    OBJS += jed.o
    OBJS += jbc/jbicomp.o
    OBJS += jbc/jbijtag.o
    OBJS += jbc/jbimain.o
    OBJS += jbc/jbistub.o
else
    OBJS += lattice.o
    OBJS += jed.o
    
endif

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/jtag
EXTRA_CFLAGS += -I${SPXINC}/jtag_hw

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/jtag/Module.symvers
KBUILD_EXTRA_SYMBOLS += ${SPXINC}/jtag_hw/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
