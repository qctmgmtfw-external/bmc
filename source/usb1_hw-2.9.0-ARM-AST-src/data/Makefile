DEBUG := n
TARGET := usb1_hw
OBJS := ast_usbhub.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/dbgout
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/usbe

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/usbe/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers
