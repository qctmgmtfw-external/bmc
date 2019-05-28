DEBUG     		:= n
TARGET	 	  	:= usbe
OBJS			:= mod_reg.o coreusb.o descriptors.o setup.o entry.o bot.o proc.o driver.o linux.o module.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/dbgout

-include ${TOOLDIR}/rules/Rules.make.drivers


