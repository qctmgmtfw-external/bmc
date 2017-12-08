/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/

# ifndef __AST_VIDEOCAP_IOCTL_H__
# define __AST_VIDEOCAP_IOCTL_H__

/* Character device IOCTL's */
#define ASTCAP_MAGIC  		'a'
#define ASTCAP_IOCCMD		_IOWR(ASTCAP_MAGIC, 0, ASTCap_Ioctl)

typedef enum FPGA_opcode {
    ASTCAP_IOCTL_RESET_VIDEOENGINE = 0,
    ASTCAP_IOCTL_START_CAPTURE,
    ASTCAP_IOCTL_STOP_CAPTURE,
    ASTCAP_IOCTL_GET_VIDEO,
	ASTCAP_IOCTL_GET_CURSOR,
    ASTCAP_IOCTL_CLEAR_BUFFERS,
    ASTCAP_IOCTL_SET_VIDEOENGINE_CONFIGS,
    ASTCAP_IOCTL_GET_VIDEOENGINE_CONFIGS,
    ASTCAP_IOCTL_SET_SCALAR_CONFIGS,
    ASTCAP_IOCTL_ENABLE_VIDEO_DAC,
} ASTCap_OpCode;

typedef enum {
	ASTCAP_IOCTL_SUCCESS = 0,
	ASTCAP_IOCTL_ERROR,
	ASTCAP_IOCTL_NO_VIDEO_CHANGE,
	ASTCAP_IOCTL_BLANK_SCREEN,
} ASTCap_ErrCode;

typedef struct {
	ASTCap_OpCode OpCode;
	ASTCap_ErrCode ErrCode;
	unsigned long Size;
	void *vPtr;
	unsigned char Reserved [2];
} ASTCap_Ioctl;

#endif /* !__AST_VIDEOCAP_IOCTL_H__ */
