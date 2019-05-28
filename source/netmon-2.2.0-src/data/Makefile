DEBUG     := n
TARGET    := netmon

OBJS   := netmonitor.o
ifeq ($(DEBUG),y)
EXTRA_CFLAGS += -DNETMON_DEBUG
endif

EXTRA_CFLAGS += -I$(PWD)/include		# Local header files 

#---------------- Do not modify any thing below this line ------------------------
-include ${TOOLDIR}/rules/Rules.make.drivers


