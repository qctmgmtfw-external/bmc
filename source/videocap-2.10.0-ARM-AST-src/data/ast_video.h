 /********************************************************************************
* File Name     : drivers/video/ast_video.h
* Author         : Ryan Chen
* Description   : ASPEED Video Engine
*
* Copyright (C) ASPEED Tech. Inc.
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by the Free Software Foundation;
* either version 2 of the License, or (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,  but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

*   History      :
*    1. 2012/12/27 Ryan Chen create this file
*        
*
********************************************************************************/
//VR08[2]
typedef enum ast_video_source {
	VIDEO_SOURCE_INT_VGA = 0,
	VIDEO_SOURCE_INT_CRT,
	VIDEO_SOURCE_EXT_ADC,
	VIDEO_SOURCE_EXT_DIGITAL,
} video_source;

//VR08[5]
typedef enum ast_vga_mode {
	VIDEO_VGA_DIRECT_MODE = 0,
	VIDEO_VGA_CAPTURE_MODE,
} vga_mode;

//VR08[4]
typedef enum ast_video_dis_en {
	VIDEO_EXT_DE_SIGNAL = 0,
	VIDEO_INT_DE_SIGNAL,
} display_enable;

typedef enum video_color_format {
	VIDEO_COLOR_RGB565 = 0,
	VIDEO_COLOR_RGB888,
	VIDEO_COLOR_YUV444,
	VIDEO_COLOR_YUV420,
} color_formate;

typedef enum vga_color_mode {
	VGA_NO_SIGNAL = 0,
	EGA_MODE,
	VGA_MODE,
	VGA_15BPP_MODE,
	VGA_16BPP_MODE,
	VGA_32BPP_MODE,
	VGA_CGA_MODE,
	VGA_TEXT_MODE,
} color_mode;

typedef enum video_stage {
	NONE,
	POLARITY,
	RESOLUTION,
	INIT,
	RUN,
} stage;

struct ast_video_plat_data {
	u32 (*get_clk)(void);
	void (*ctrl_reset)(void);
	void (*vga_display)(u8 enable);
	u8 (*get_vga_display)(void);
	u32 (*get_vga_base)(void);
//	compress_formate compress;
};

//+?????
struct ast_video_config
{
	u8	engine;					//0: engine 0, engine 1
	u8	compression_mode; 		//0:DCT, 1:DCT_VQ mix VQ-2 color, 2:DCT_VQ mix VQ-4 color		9:
	u8	compression_format;		//0:ASPEED 1:JPEG	
	u8	capture_format;			//0:CCIR601-2 YUV, 1:JPEG YUV, 2:RGB for ASPEED mode only, 3:Gray 
	u8	rc4_enable;				//0:disable 1:enable
	u8 	EncodeKeys[256];		

	u8	YUV420_mode;			//0:YUV444, 1:YUV420
	u8	Visual_Lossless;
	u8	Y_JPEGTableSelector;
	u8	AdvanceTableSelector;
	u8	AutoMode;
};

struct ast_auto_mode
{
	u8	engine_idx;					//set 0: engine 0, engine 1
	u8	differential;					//set 0: full, 1:diff frame
	u8	mode_change;				//get 0: no, 1:change
	u32	total_size;					//get 
	u32	block_count;					//get 
};

struct ast_scaling
{
	u8	engine;					//0: engine 0, engine 1
	u8	enable;
	u16	x;
	u16	y;
};

struct ast_mode_detection
{
	unsigned char		result;		//0: pass, 1: fail
	unsigned short	src_x;
	unsigned short	src_y;
};

//IOCTL ..

#define VIDEOIOC_BASE       'a'

#define AST_VIDEO_IOC_GET_VGA_SIGNAL			_IOR(VIDEOIOC_BASE, 15, unsigned char)
#define AST_VIDEO_GET_MEM_SIZE_IOCRX			_IOR(VIDEOIOC_BASE, 16, unsigned long)
#define AST_VIDEO_GET_JPEG_OFFSET_IOCRX		_IOR(VIDEOIOC_BASE, 17, unsigned long)
#define AST_VIDEO_VGA_MODE_DETECTION			_IOWR(VIDEOIOC_BASE, 18, struct ast_mode_detection*)

#define AST_VIDEO_ENG_CONFIG					_IOW(VIDEOIOC_BASE, 19, struct ast_video_config*)
#define AST_VIDEO_SET_SCALING					_IOW(VIDEOIOC_BASE, 20, struct ast_scaling*)

#define AST_VIDEO_AUTOMODE_TRIGGER			_IOWR(VIDEOIOC_BASE, 21, struct ast_auto_mode*)
#define AST_VIDEO_CAPTURE_TRIGGER				_IOWR(VIDEOIOC_BASE, 22, unsigned long)
#define AST_VIDEO_COMPRESSION_TRIGGER		_IOWR(VIDEOIOC_BASE, 23, unsigned long)

#define AST_VIDEO_SET_VGA_DISPLAY				_IOW(VIDEOIOC_BASE, 24, int)

#define AST_VIDEO_SCU_RESET_VIDEO				_IOW(VIDEOIOC_BASE, 25, int)

/***********************************************************************/

struct fbinfo
{
	u16		x;
	u16		y;
	u8	color_mode;	//0:NON, 1:EGA, 2:VGA, 3:15bpp, 4:16bpp, 5:32bpp
	u32	PixelClock;
};

//For Socket Transfer head formate ..
struct compress_header {
	u32 data_len;
	u32 block_changed;
	u16	user_width;
	u16	user_height;
	u8	first_frame;
	u8	compress_type;
	u8	trigger_mode;
	u8	data_format;
	u8	mode;
	u8	VQMode;
	u8	Y_JPEGTableSelector;
	u8	UV_JPEGTableSelector;
	u8	AdvanceTableSelector;
	u8	Visual_Lossless;
};

struct ast_video_data {
	struct device		*misc_dev;
	int 	irq;				//Video IRQ number 
//	compress_header	
	struct compress_header			compress_mode;
        phys_addr_t             *stream_phy;            /* phy */
        u32                             *stream_virt;           /* virt */
        phys_addr_t             *buff0_phy;             /* phy */
        u32                             *buff0_virt;            /* virt */
        phys_addr_t             *buff1_phy;             /* phy */
        u32                             *buff1_virt;            /* virt */
        phys_addr_t             *bcd_phy;               /* phy */
        u32                             *bcd_virt;              /* virt */
        phys_addr_t             *jpeg_phy;              /* phy */
        u32                             *jpeg_virt;             /* virt */
        phys_addr_t             *jpeg_buf0_phy;              /* phy */
        u32                             *jpeg_buf0_virt;             /* virt */
        phys_addr_t             *jpeg_tbl_phy;          /* phy */
        u32                             *jpeg_tbl_virt;         /* virt */
	//+?????
        phys_addr_t             *hdr_phy;               /* phy */
        u32                             *hdr_virt;              /* virt */

	//config 
	video_source	input_source;	
	u8	rc4_enable;
	u8 EncodeKeys[256];
	u8	scaling;
		
//JPEG 
	u32		video_mem_size;			/* phy size*/		
	u32		video_jpeg_offset;			/* assigned jpeg memory size*/
	u8 mode_change;	
	struct completion	mode_detect_complete;
	struct completion	automode_complete;		
	struct completion	capture_complete;
	struct completion	compression_complete;		
	
	wait_queue_head_t 	queue;	

	u32 flag;
	wait_queue_head_t 	video_wq;	

	u32 thread_flag;
	struct task_struct 		*thread_task;



	struct fbinfo					src_fbinfo;
	struct fbinfo					dest_fbinfo;
	struct completion				complete;	
	u32		sts;
	u8		direct_mode;
	u8		stage;
	u32 	bandwidth;
	struct mutex lock;	

        bool is_open;

	
};

//  RC4 structure
struct rc4_state
{
    int x;
    int y;
    int m[256];
};
 
//+?????
#define ASPEED_PROPRIETARY_MODE		1
#define JPEG_JFIF_MODE			2
extern unsigned int video_capture_codec;

#define VIDEO_SESSION_NONE		0
#define VIDEO_SESSION_RKVM		1
#define VIDEO_SESSION_VNC		2
extern unsigned int video_session_info;

extern u8 ast_get_vga_signal(struct ast_video_data *ast_video);
extern void ast_video_set_eng_config(struct ast_video_data *ast_video, struct ast_video_config *video_config);
extern void ast_video_set_0_scaling(struct ast_video_data *ast_video, struct ast_scaling *scaling);
extern void ast_video_set_1_scaling(struct ast_video_data *ast_video, struct ast_scaling *scaling);
extern void ast_video_mode_detect_trigger(struct ast_video_data *ast_video);
extern void ast_video_vga_mode_detect(struct ast_video_data *ast_video, struct ast_mode_detection *mode_detect);
extern void ast_video_auto_mode_trigger(struct ast_video_data *ast_video, struct ast_auto_mode *auto_mode);
extern void ast_video_mode_detect_info(struct ast_video_data *ast_video);
extern void ast_video_ctrl_init(struct ast_video_data *ast_video);
extern void ast_scu_set_vga_display(u8 enable);
extern void ast_scu_reset_video(void);

// sysfs
extern ssize_t store_video_reset(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
extern const struct attribute_group video_attribute_group;
extern const struct attribute_group compress_attribute_groups[];
//#define JPEG_CAPTURE
//#define CONFIG_AST_VIDEO_DEBUG

#ifdef CONFIG_AST_VIDEO_DEBUG
	#define VIDEO_DBG(fmt, args...) printk( "%s() " fmt,__FUNCTION__, ## args)	
#else
	#define VIDEO_DBG(fmt, args...)
#endif

// #define ASPEED_SCU_DEBUG
#ifdef ASPEED_SCU_DEBUG
	#define SCUDBUG(fmt, args...) printk("%s() " fmt, __FUNCTION__, ## args)
#else
	#define SCUDBUG(fmt, args...)
#endif
//-?????
