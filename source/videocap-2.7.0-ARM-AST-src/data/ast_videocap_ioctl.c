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

#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/aio.h>
#include <linux/bigphysarea.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "ast_videocap_hw.h"
#include "ast_videocap_data.h"
#include "ast_videocap_functions.h"
#include "ast_videocap_ioctl.h"

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
  #define USE_UNLOCKED_IOCTL  
  #endif
#endif


void ioctl_reset_video_engine(ASTCap_Ioctl *ioc)
{
	ast_videocap_reset_hw();
	ioc->Size = 0;
	ioc->ErrCode = ASTCAP_IOCTL_SUCCESS;
}

void ioctl_start_video_capture(ASTCap_Ioctl *ioc)
{
	StartVideoCapture(&ast_videocap_engine_info);
	ioc->Size = 0;
	ioc->ErrCode = ASTCAP_IOCTL_SUCCESS;
}

void ioctl_stop_video_capture(ASTCap_Ioctl *ioc)
{
	StopVideoCapture();
	ioc->Size = 0;
	ioc->ErrCode = ASTCAP_IOCTL_SUCCESS;
}

void ioctl_get_videoengine_configs(ASTCap_Ioctl *ioc)
{
	ast_videocap_get_engine_config(&ast_videocap_engine_info, ioc->vPtr, &(ioc->Size));
	ioc->ErrCode = ASTCAP_IOCTL_SUCCESS;
}

void ioctl_set_videoengine_configs(ASTCap_Ioctl *ioc)
{
	ast_videocap_set_engine_config(&ast_videocap_engine_info, ioc->vPtr);
	ioc->Size = 0;
	ioc->ErrCode = ASTCAP_IOCTL_SUCCESS;
}

void ioctl_get_video(ASTCap_Ioctl *ioc)
{
	ioc->ErrCode = ast_videocap_create_video_packet(&ast_videocap_engine_info, &(ioc->Size));
}

void ioctl_enable_video_dac(ASTCap_Ioctl *ioc)
{
    ast_videocap_enable_video_dac(&ast_videocap_engine_info, ioc->vPtr);
	ioc->Size = 0;
	ioc->ErrCode = ASTCAP_IOCTL_SUCCESS;
}

void ioctl_get_cursor(ASTCap_Ioctl *ioc)
{
	ioc->ErrCode = ast_videocap_create_cursor_packet(&ast_videocap_engine_info, &(ioc->Size));
}

void ioctl_clear_buffers(ASTCap_Ioctl *ioc)
{
	memset(ast_videocap_video_buf_virt_addr, 0, 0x1800000);
	ioc->ErrCode = ASTCAP_IOCTL_SUCCESS;
}

#ifdef USE_UNLOCKED_IOCTL
long ast_videocap_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
#else
int ast_videocap_ioctl(struct inode *inode,struct file *filep, unsigned int cmd, unsigned long arg)
#endif
{
	ASTCap_Ioctl ioc;
	int ret;
#ifndef USE_UNLOCKED_IOCTL
	/* Validation : If valid dev */
	if (!inode)
		return -EINVAL;
#endif

	/* Validation : Check it is ours */
	if (_IOC_TYPE(cmd) != ASTCAP_MAGIC)
		return -EINVAL;

	/* Copy from user to kernel space */
	/* On Failure Returns number of bytes that could not be copied. On success, this will be zero */
	if( copy_from_user(&ioc, (char *) arg, sizeof(ASTCap_Ioctl)))
		return -1;

	switch (ioc.OpCode) {
	case ASTCAP_IOCTL_RESET_VIDEOENGINE:
		ioctl_reset_video_engine(&ioc);
		break;
	case ASTCAP_IOCTL_START_CAPTURE:
		ioctl_start_video_capture(&ioc);
		break;
	case ASTCAP_IOCTL_GET_VIDEO:
		ioctl_get_video(&ioc);
		break;
	case ASTCAP_IOCTL_GET_CURSOR:
		ioctl_get_cursor(&ioc);
		break;
	case ASTCAP_IOCTL_CLEAR_BUFFERS:
		ioctl_clear_buffers(&ioc);
		break;
	case ASTCAP_IOCTL_STOP_CAPTURE:
		ioctl_stop_video_capture(&ioc);
		break;
	case ASTCAP_IOCTL_GET_VIDEOENGINE_CONFIGS:
		ioctl_get_videoengine_configs(&ioc);
		break;
	case ASTCAP_IOCTL_SET_VIDEOENGINE_CONFIGS:
		ioctl_set_videoengine_configs(&ioc);
		break;
    case ASTCAP_IOCTL_ENABLE_VIDEO_DAC:
        ioctl_enable_video_dac(&ioc);
        break;
	default:
		//printk ("Unknown Ioctl\n");
		return -EINVAL;
	}

	/* Check if argument is writable */
	ret = access_ok(VERIFY_WRITE, (char *) arg, sizeof(ASTCap_Ioctl));
	if (ret == 0)
		return -EINVAL;

	/* Copy back the results to user space */
	/* On Failure Returns number of bytes that could not be copied. On success, this will be zero */
	if( copy_to_user((char *) arg, &ioc, sizeof(ASTCap_Ioctl)))
		return -1;

	return 0;
}
