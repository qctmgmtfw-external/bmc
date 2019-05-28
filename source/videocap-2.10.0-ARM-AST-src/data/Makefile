DEBUG := n
TARGET := videocap
# Quanta, Integrate ASPEED provided JPEG capture engine for VNC usage
# OBJS := ast_videocap_module.o ast_videocap_proc.o ast_videocap_mmap.o ast_videocap_ioctl.o ast_videocap_engine.o
OBJS := ast_videocap_module.o ast_videocap_proc.o ast_videocap_mmap.o ast_videocap_ioctl.o ast_videocap_engine.o ast_video.o

EXTRA_CFLAGS += -I${SPXINC}/helper

-include ${TOOLDIR}/rules/Rules.make.drivers
