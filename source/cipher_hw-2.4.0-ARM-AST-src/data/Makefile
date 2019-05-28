DEBUG     := n
TARGET	  := cipher_hw
OBJS      := ast_cipher.o aes_core.o rc4.o rsa.o

EXTRA_CFLAGS += -I${SPXINC}/global
EXTRA_CFLAGS += -I${SPXINC}/helper
EXTRA_CFLAGS += -I${SPXINC}/cipher

KBUILD_EXTRA_SYMBOLS += ${SPXINC}/cipher/Module.symvers

-include ${TOOLDIR}/rules/Rules.make.drivers

