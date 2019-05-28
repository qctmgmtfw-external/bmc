/***************************************************************
****************************************************************
**                                                            **
**    (C)Copyright 2009-2015, American Megatrends Inc.        **
**                                                            **
**            All Rights Reserved.                            **
**                                                            **
**        6145-F, Northbelt Parkway, Norcross,                **
**                                                            **
**        Georgia - 30071, USA. Phone-(770)-246-8600.         **
**                                                            **
****************************************************************
 ****************************************************************/

# ifndef __AST_VIDEOCAP_IOCTL_H__
# define __AST_VIDEOCAP_IOCTL_H__

/* Character device IOCTL's */
#define ASTCAP_MAGIC  		'a'
#define ASTCAP_IOCCMD		_IOWR(ASTCAP_MAGIC, 0, ASTCap_Ioctl)

#define  ASTCAP_IOCTL_RESET_VIDEOENGINE         _IOW('a', 0, int)
#define  ASTCAP_IOCTL_START_CAPTURE             _IOW('a', 1, int)
#define  ASTCAP_IOCTL_STOP_CAPTURE              _IOW('a', 2, int) 
#define  ASTCAP_IOCTL_GET_VIDEO                 _IOR('a', 3, int)
#define  ASTCAP_IOCTL_GET_CURSOR                _IOR('a', 4, int)
#define  ASTCAP_IOCTL_CLEAR_BUFFERS             _IOW('a', 5, int)
#define  ASTCAP_IOCTL_SET_VIDEOENGINE_CONFIGS   _IOW('a', 6, int)
#define  ASTCAP_IOCTL_GET_VIDEOENGINE_CONFIGS   _IOR('a', 7, int)
#define  ASTCAP_IOCTL_SET_SCALAR_CONFIGS        _IOW('a', 8, int)
#define  ASTCAP_IOCTL_ENABLE_VIDEO_DAC          _IOW('a', 9, int)

#define ASTCAP_IOCTL_SUCCESS            _IOR('a', 10, int)
#define ASTCAP_IOCTL_ERROR              _IOR('a', 11, int)
#define ASTCAP_IOCTL_NO_VIDEO_CHANGE    _IOR('a', 12, int)
#define ASTCAP_IOCTL_BLANK_SCREEN       _IOR('a', 13, int)


typedef struct {
	int OpCode;
	int ErrCode;
	unsigned long Size;
	void *vPtr;
	unsigned char Reserved [2];
} ASTCap_Ioctl;

#endif /* !__AST_VIDEOCAP_IOCTL_H__ */
