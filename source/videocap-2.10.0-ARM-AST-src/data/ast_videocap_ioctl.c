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

#include <linux/platform_device.h>	// Quanta
#include <linux/miscdevice.h>		// Quanta

#include "ast_videocap_hw.h"
#include "ast_videocap_data.h"
#include "ast_videocap_functions.h"
#include "ast_videocap_ioctl.h"

#include "ast_video.h"	// Quanta
#include "regs-video.h"	// Quanta

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
	// Quanta-----, Integrate ASPEED provided JPEG capture engine for VNC usage
	struct miscdevice *c = filep->private_data;
	struct ast_video_data *ast_video = dev_get_drvdata(c->this_device);


	struct ast_scaling set_scaling;
	struct ast_video_config video_config;
	
	int vga_enable = 0;
	struct ast_mode_detection mode_detection;	
	struct ast_auto_mode auto_mode;	
	void __user *argp = (void __user *)arg;
	// -----Quanta
#ifndef USE_UNLOCKED_IOCTL
	/* Validation : If valid dev */
	if (!inode)
		return -EINVAL;
#endif

	/* Validation : Check it is ours */
	if (_IOC_TYPE(cmd) != ASTCAP_MAGIC)
		return -EINVAL;

	if (video_capture_codec == ASPEED_PROPRIETARY_MODE) {	// Quanta

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
	// Quanta-----, Integrate ASPEED provided JPEG capture engine for VNC usage
	}
	else { // JPEG_JFIF_MODE 

			ret = 1;
		switch (cmd) {
			case AST_VIDEO_IOC_GET_VGA_SIGNAL:
				ret = put_user(ast_get_vga_signal(ast_video), (unsigned long __user *)arg);
				break;
			case AST_VIDEO_GET_MEM_SIZE_IOCRX:
				ret = __put_user(ast_video->video_mem_size, (unsigned long __user *)arg);
				break;
			case AST_VIDEO_GET_JPEG_OFFSET_IOCRX:
				ret = __put_user(ast_video->video_jpeg_offset, (unsigned long __user *)arg);
				break;
			case AST_VIDEO_VGA_MODE_DETECTION:
				ret = copy_from_user(&mode_detection, argp, sizeof(struct ast_mode_detection));
				ast_video_vga_mode_detect(ast_video, &mode_detection);
				ret = copy_to_user(argp, &mode_detection, sizeof(struct ast_mode_detection));
				break;	
			case AST_VIDEO_ENG_CONFIG:
				ret = copy_from_user(&video_config, argp, sizeof(struct ast_video_config));
	
				ast_video_set_eng_config(ast_video, &video_config);
				break;
			case AST_VIDEO_SET_SCALING:
				ret = copy_from_user(&set_scaling, argp, sizeof(struct ast_scaling));
				switch(set_scaling.engine) {
					case 0:
						ast_video_set_0_scaling(ast_video, &set_scaling);
						break;
					case 1:
						ast_video_set_1_scaling(ast_video, &set_scaling);
						break;
				}
				break;
			case AST_VIDEO_AUTOMODE_TRIGGER:
				ret = copy_from_user(&auto_mode, argp, sizeof(struct ast_auto_mode));
				ast_video_auto_mode_trigger(ast_video, &auto_mode);
				ret = copy_to_user(argp, &auto_mode, sizeof(struct ast_auto_mode));
				break;
			case AST_VIDEO_CAPTURE_TRIGGER:
				break;
			case AST_VIDEO_COMPRESSION_TRIGGER:
				break;
			case AST_VIDEO_SET_VGA_DISPLAY:
				ret = __get_user(vga_enable, (int __user *)arg);
				ast_scu_set_vga_display(vga_enable);
				break;
			case AST_VIDEO_SCU_RESET_VIDEO:
				ast_scu_reset_video();
				//rc4 init reset ..
				ast_videocap_write_reg(ast_videocap_read_reg(AST_VIDEO_CTRL) | VIDEO_CTRL_RC4_RST , AST_VIDEO_CTRL);
				ast_videocap_write_reg(ast_videocap_read_reg(AST_VIDEO_CTRL) & ~VIDEO_CTRL_RC4_RST , AST_VIDEO_CTRL);
				ast_video_ctrl_init(ast_video);
				break;
			default:
				ret = 3;
				break;
		}
	
		return ret;
	}
	// -----Quanta
}
