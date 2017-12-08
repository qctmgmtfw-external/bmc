DEBUG     		:= n
TARGET	 	  	:= iUSB
OBJS			:= iusb-scsi.o iusb-hdisk.o iusb-cdrom.o iusb-vendor.o iusb-hid.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/dbgout
EXTRA_CFLAGS += -I${SPXINC}/usbe

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/usbe/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers


