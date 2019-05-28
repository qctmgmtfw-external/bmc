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
****************************************************************/

#ifndef __AST_VIDEOCAP_DATA_H__
#define __AST_VIDEOCAP_DATA_H__

#include <linux/wait.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include "ast_videocap_hw.h"

// Quanta, Integrate ASPEED provided JPEG capture engine for VNC usage, need additional 16MB for JPEG capture use
// #define MEM_POOL_SIZE (0x1800000)
#define MEM_POOL_SIZE (0x2800000)

extern void *ast_videocap_reg_virt_base;
extern void *ast_videocap_video_buf_virt_addr;
extern void *ast_videocap_video_buf_phys_addr;

extern struct ast_videocap_engine_info_t ast_videocap_engine_info;

extern int WaitingForModeDetection;
extern int WaitingForCapture;
extern int WaitingForCompression;

#endif  /* !__AST_VIDEOCAP_DATA_H__ */
