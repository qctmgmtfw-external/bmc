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

#ifndef __AST_VIDEOCAP_REG_H__
#define __AST_VIDEOCAP_REG_H__

#define AST_VIDEOCAP_REG_BASE		0x1E700000
#define AST_VIDEOCAP_REG_SIZE		SZ_128K
#define AST_VIDEOCAP_IRQ			7

#define AST_VIDEOCAP_KEY									0x000
#define AST_VIDEOCAP_SEQ_CTRL								0x004
#define AST_VIDEOCAP_CTRL									0x008
#define AST_VIDEOCAP_TIMING_GEN_H							0x00C /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 0 */
#define AST_VIDEOCAP_TIMING_GEN_V							0x010 /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 0 */
#define AST_VIDEOCAP_DIRECT_FRAME_BUF_ADDR					0x00C /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 1 */
#define AST_VIDEOCAP_DIRECT_FRAME_BUF_CTRL					0x010 /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 1 */
#define AST_VIDEOCAP_SCALING_FACTOR							0x014
#define AST_VIDEOCAP_SCALING_FILTER_PARAM0					0x018
#define AST_VIDEOCAP_SCALING_FILTER_PARAM1					0x01C
#define AST_VIDEOCAP_SCALING_FILTER_PARAM2					0x020
#define AST_VIDEOCAP_SCALING_FILTER_PARAM3					0x024
#define AST_VIDEOCAP_BCD_CTRL								0x02C
#define AST_VIDEOCAP_CAPTURE_WINDOW							0x030
#define AST_VIDEOCAP_COMPRESS_WINDOW						0x034
#define AST_VIDEOCAP_COMPRESS_STREAM_BUF_PROCESS_OFFSET		0x038
#define AST_VIDEOCAP_COMPRESS_STREAM_BUF_READ_OFFSET		0x03C
#define AST_VIDEOCAP_CRC_BUF_ADDR							0x040
#define AST_VIDEOCAP_SRC_BUF1_ADDR							0x044
#define AST_VIDEOCAP_SCAN_LINE_OFFSET						0x048
#define AST_VIDEOCAP_SRC_BUF2_ADDR							0x04C
#define AST_VIDEOCAP_BCD_FLAG_BUF_ADDR						0x050
#define AST_VIDEOCAP_COMPRESSED_BUF_ADDR					0x054
#define AST_VIDEOCAP_STREAM_BUF_SIZE						0x058
#define AST_VIDEOCAP_STREAM_BUF_OFFSET						0x05C
#define AST_VIDEOCAP_COMPRESS_CTRL							0x060
#define AST_VIDEOCAP_COMPRESSED_DATA_COUNT					0x070
#define AST_VIDEOCAP_COMPRESSED_BLOCK_COUNT					0x074
#define AST_VIDEOCAP_FRAME_END_OFFSET						0x078
#define AST_VIDEOCAP_COMPRESS_FRAME_COUNT					0x07C
#define AST_VIDEOCAP_EDGE_DETECT_H							0x090
#define AST_VIDEOCAP_EDGE_DETECT_V							0x094
#define AST_VIDEOCAP_MODE_DETECT_STATUS						0x098
#define AST_VIDEOCAP_CTRL2									0x300
#define AST_VIDEOCAP_ICR									0x304
#define AST_VIDEOCAP_ISR									0x308
#define AST_VIDEOCAP_MODE_DETECT_PARAM						0x30C
#define AST_VIDEOCAP_MEM_RESTRICT_START						0x310
#define AST_VIDEOCAP_MEM_RESTRICT_END						0x314
#define AST_VIDEOCAP_PRIMARY_CRC_PARAME						0x320
#define AST_VIDEOCAP_SECONDARY_CRC_PARAM					0x324
#define AST_VIDEOCAP_DATA_TRUNCATION						0x328

#define AST_VIDEOCAP_HW_CURSOR_STATUS						0x340
#define AST_VIDEOCAP_HW_CURSOR_POSITION						0x344
#define AST_VIDEOCAP_HW_CURSOR_PATTERN_ADDR					0x348
#define AST_VIDEOCAP_VGA_SCRATCH_8C8F						0x34C
#define AST_VIDEOCAP_VGA_SCRATCH_9093						0x350
#define AST_VIDEOCAP_VGA_SCRATCH_9497						0x354
#define AST_VIDEOCAP_VGA_SCRATCH_989B						0x358
#define AST_VIDEOCAP_VGA_STATUS								0x35C

/* protection key register */
#define AST_VIDEOCAP_KEY_MAGIC								0x1A038AA8

/* sequence control register */
#define AST_VIDEOCAP_SEQ_CTRL_COMPRESS_STATUS				0x00040000 /* bit 18 */
#define AST_VIDEOCAP_SEQ_CTRL_CAP_STATUS					0x00010000 /* bit 16 */
#define AST_VIDEOCAP_SEQ_CTRL_FORMAT_SHIFT					10
#define AST_VIDEOCAP_SEQ_CTRL_FORMAT_MASK					0x00000C00 /* bits[11:10] */
#define AST_VIDEOCAP_SEQ_CTRL_WDT							0x00000080 /* bit 7 */
#define AST_VIDEOCAP_SEQ_CTRL_FULL							0x00000040 /* bit 6 */
#define AST_VIDEOCAP_SEQ_CTRL_AUTO_COMPRESS					0x00000020 /* bit 5 */
#define AST_VIDEOCAP_SEQ_CTRL_COMPRESS						0x00000010 /* bit 4 */
#define AST_VIDEOCAP_SEQ_CTRL_CAP_MULTI						0x00000008 /* bit 3 */
#define AST_VIDEOCAP_SEQ_CTRL_COMPRESS_IDLE					0x00000004 /* bit 2 */
#define AST_VIDEOCAP_SEQ_CTRL_CAP							0x00000002 /* bit 1 */
#define AST_VIDEOCAP_SEQ_CTRL_MODE_DETECT					0x00000001 /* bit 0 */

#define AST_VIDEOCAP_SEQ_CTRL_FORMAT_YUV444					0x00
#define AST_VIDEOCAP_SEQ_CTRL_FORMAT_YUV420					0x01

/* video control register */
#define AST_VIDEOCAP_CTRL_MAX_FRAME_RATE_SHIFT				16
#define AST_VIDEOCAP_CTRL_MAX_FRAME_RATE_MASK				0x00FF0000 /* bits[23:16] */
#define AST_VIDEOCAP_CTRL_CLK_MODE							0x00002000 /* bit 13 */
#define AST_VIDEOCAP_CTRL_BIT_NUM							0x00001000 /* bit 12 */
#define AST_VIDEOCAP_CTRL_CLK_DELAY_SHIFT					10
#define AST_VIDEOCAP_CTRL_CLK_DELAY_MASK					0x00000C00 /* bits[11:10] */
#define AST_VIDEOCAP_CTRL_HW_CSR_OVERLAY					0x00000100 /* bit 8 */ /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 0 */
#define AST_VIDEOCAP_CTRL_AUTO_MODE							0x00000100 /* bit 8 */ /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 1 */
#define AST_VIDEOCAP_CTRL_FORMAT_SHIFT						6
#define AST_VIDEOCAP_CTRL_FORMAT_MASK						0x000000C0 /* bits[7:6] */
#define AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF			0x00000020 /* bit 5 */
#define AST_VIDEOCAP_CTRL_DE_SIGNAL							0x00000010 /* bit 4 */ /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 0 */
#define AST_VIDEOCAP_CTRL_BPP								0x00000010 /* bit 4 */ /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 1 */
#define AST_VIDEOCAP_CTRL_EXT_SRC							0x00000008 /* bit 3 */ /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 0 */
#define AST_VIDEOCAP_CTRL_COLOR_MODE						0x00000008 /* bit 3 */ /* AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF == 1 */
#define AST_VIDEOCAP_CTRL_SRC								0x00000004 /* bit 2 */
#define AST_VIDEOCAP_CTRL_VSYNC_POLARITY					0x00000002 /* bit 1 */
#define AST_VIDEOCAP_CTRL_HSYNC_POLARITY					0x00000001 /* bit 0 */

#define AST_VIDEOCAP_CTRL_NO_DELAY							0x00000000 /* 00 */
#define AST_VIDEOCAP_CTRL_DELAY_1NS							0x00000400 /* 01 */
#define AST_VIDEOCAP_CTRL_INV_CLK_NO_DELAY					0x00000800 /* 10 */
#define AST_VIDEOCAP_CTRL_INV_CLK_DELAY_1NS					0x00000C00 /* 11 */

/* video timing generation setting register */
#define AST_VIDEOCAP_TIMING_GEN_FIRST_PIXEL_SHIFT			16
#define AST_VIDEOCAP_TIMING_GEN_FIRST_PIXEL_MASK			0xFFFF0000 /* bits[27:16] */
#define AST_VIDEOCAP_TIMING_GEN_LAST_PIXEL_SHIFT			0
#define AST_VIDEOCAP_TIMING_GEN_LAST_PIXEL_MASK				0x0000FFFF /* bits[11:0] */

/* scaling factor register */
#define AST_VIDEOCAP_SCALING_FACTOR_V_SHIFT					16
#define AST_VIDEOCAP_SCALING_FACTOR_V_MASK					0xFFFF0000 /* bits[31:16] */
#define AST_VIDEOCAP_SCALING_FACTOR_H_SHIFT					0
#define AST_VIDEOCAP_SCALING_FACTOR_H_MASK					0x0000FFFF /* bits[15:0] */
#define AST_VIDEOCAP_SCALING_FACTOR_MAX						0x1000 /* 4096 */

/* scaling filter parameter registers */
#define AST_VIDEOCAP_SCALING_FILTER_PARAM_ONE				0x00200000
#define AST_VIDEOCAP_SCALING_FILTER_PARAM_HALF_UP			0x00101000
#define AST_VIDEOCAP_SCALING_FILTER_PARAM_HALF_DOWN			0x08080808

/* capture/compression window registers */
#define AST_VIDEOCAP_WINDOWS_H_SHIFT						16
#define AST_VIDEOCAP_WINDOWS_H_MASK							0xFFFF0000 /* bits[27:16] */
#define AST_VIDEOCAP_WINDOWS_V_SHIFT						0
#define AST_VIDEOCAP_WINDOWS_V_MASK							0x0000FFFF /* bits[10:0] */

/* block change detection control registger */
#define AST_VIDEOCAP_BCD_CTRL_TOLERANCE_SHIFT				16
#define AST_VIDEOCAP_BCD_CTRL_TOLERANCE_MASK				0x00FF0000 /* bits[23:16] */
#define AST_VIDEOCAP_BCD_CTRL_ENABLE						0x00000001 /* bit 0 */
#define AST_VIDEOCAP_BCD_CTRL_DISABLE						0x00000000 /* bit 0 */

/* size of compressed data register */
#define AST_VIDEOCAP_COMPRESSED_DATA_COUNT_UNIT				4

/* number of compressed blocks register */
#define AST_VIDEOCAP_COMPRESSED_BLOCK_COUNT_SHIFT			16
#define AST_VIDEOCAP_COMPRESSED_BLOCK_COUNT_MASK			0x3FFF0000 /* bits[29:16] */

/* edge detection registers */
#define AST_VIDEOCAP_EDGE_DETECT_END_SHIFT					16
#define AST_VIDEOCAP_EDGE_DETECT_END_MASK					0x0FFF0000 /* bits[27:16] */
#define AST_VIDEOCAP_EDGE_DETECT_START_SHIFT				0
#define AST_VIDEOCAP_EDGE_DETECT_START_MASK					0x00000FFF /* bits[11:0] */


/* bits of hardware cursor status register */
#define AST_VIDEOCAP_HW_CURSOR_STATUS_TYPE                  0x00000200 /* bit 9 */
#define AST_VIDEOCAP_HW_CURSOR_STATUS_ENABLE                0x00000100 /* bit 8 */
#define AST_VIDEOCAP_HW_CURSOR_STATUS_DRIVER_MASK           0x000000FF /* bits[7:0] */
#define AST_VIDEOCAP_HW_CURSOR_STATUS_DRIVER_MGAIC          0xA8

#define AST_VIDEOCAP_HW_CURSOR_TYPE_MONOCHROME              0
#define AST_VIDEOCAP_HW_CURSOR_TYPE_COLOR                   1

typedef struct {
	unsigned short HorizontalTotal;
	unsigned short VerticalTotal;
	unsigned short HorizontalActive;
	unsigned short VerticalActive;
	unsigned char RefreshRate;
	//double    HorizontalFrequency;
	unsigned long HorizontalFrequency;
	unsigned short HSyncTime;
	unsigned short HBackPorch;
	unsigned short VSyncTime;
	unsigned short VBackPorch;
	unsigned short HLeftBorder;
	unsigned short HRightBorder;
	unsigned short VBottomBorder;
	unsigned short VTopBorder;
} VESA_MODE;

typedef struct {
	unsigned short HorizontalActive;
	unsigned short VerticalActive;
	unsigned short RefreshRateIndex;
	unsigned long PixelClock;
} INTERNAL_MODE;

typedef struct {
	unsigned short HorizontalActive;
	unsigned short VerticalActive;
	unsigned short RefreshRate;
	unsigned char ADCIndex1;
	unsigned char ADCIndex2;
	unsigned char ADCIndex3;
	unsigned char ADCIndex5;
	unsigned char ADCIndex6;
	unsigned char ADCIndex7;
	unsigned char ADCIndex8;
	unsigned char ADCIndex9;
	unsigned char ADCIndexA;
	unsigned char ADCIndexF;
	unsigned char ADCIndex15;
	int HorizontalShift;
	int VerticalShift;
} EXTERNAL_MODE;

struct ast_videocap_mode_info_t {
	unsigned short x;
	unsigned short y;
	unsigned short ColorDepth;
	unsigned short RefreshRate;
	unsigned char ModeIndex;
};

struct ast_videocap_compress_data_t {
	unsigned long SourceFrameSize;
	unsigned long CompressSize;
	unsigned long HDebug;
	unsigned long VDebug;
};

struct ast_videocap_inf_data_t {
	unsigned char DownScalingMethod;
	unsigned long DifferentialSetting;
	unsigned long AnalogDifferentialThreshold;
	unsigned long DigitalDifferentialThreshold;
	unsigned char ExternalSignalEnable;
	unsigned char AutoMode;
	unsigned char DirectMode;
	unsigned char VQMode;
};

struct ast_videocap_frame_hdr_t {
	unsigned long StartCode;
	unsigned long FrameNumber;
	unsigned short HSize;
	unsigned short VSize;
	unsigned long Reserved[2];
	unsigned long CompressionMode;
	unsigned long JPEGScaleFactor;
	unsigned char JPEGTableSelector;
	unsigned long JPEGYUVTableMapping;
	unsigned long SharpModeSelection;
	unsigned long AdvanceTableSelector;
	unsigned long AdvanceScaleFactor;
	unsigned long NumberOfMB;
	//unsigned char VQ_YLevel;
	//unsigned char VQ_UVLevel;
	//VQ_INFO VQVectors;
	unsigned char RC4Enable;
	unsigned char RC4Reset;
	unsigned char Mode420;
};

struct ast_videocap_engine_info_t {
	unsigned short iEngVersion;
	struct ast_videocap_mode_info_t src_mode;
	struct ast_videocap_mode_info_t dest_mode;
	struct ast_videocap_frame_hdr_t FrameHeader;
	struct ast_videocap_inf_data_t INFData;
	struct ast_videocap_compress_data_t CompressData;
	unsigned char NoSignal;
	unsigned short MemoryBandwidth;
	unsigned long TotalMemory;
	unsigned long VGAMemory;
};

struct ast_videocap_engine_config_t {
	unsigned char differential_setting;   /* Transmit Video Deltas Only ; radio btn, Yes/No = 1/0 */
	unsigned char dct_quant_quality;      /* JPEGScaleFactor; valid values are 0 to 31 */
	unsigned char dct_quant_tbl_select;   /* JPEGTableSelector; valid values are 0 to 7 */
	unsigned char sharp_mode_selection;   /* SharpModeSelection; radio button, Yes/No = 1/0  */
	unsigned char sharp_quant_quality;    /* AdvanceScaleFactor; valid values are 0 to 31 */
	unsigned char sharp_quant_tbl_select; /* AdvanceTableSelector; valid values are 0 to 7 */
	unsigned char compression_mode;       /* Compression Mode; valid values 0,1,2 */
	unsigned char vga_dac; /* enable/disable VGA DAC to turn on/off VGA output */
};

/* format of cursor pattern is ARGB444, each pixel is presented by 16 bits(2 bytes) */
#define AST_VIDEOCAP_CURSOR_BITMAP			(64 * 64)
#define AST_VIDEOCAP_CURSOR_BITMAP_DATA		(AST_VIDEOCAP_CURSOR_BITMAP * 2)

struct ast_videocap_cursor_info_t {
	uint8_t type; /* 0: monochrome, 1: color */
	uint32_t checksum;
	uint16_t pos_x;
	uint16_t pos_y;
	uint16_t offset_x;
	uint16_t offset_y;
	uint16_t pattern[AST_VIDEOCAP_CURSOR_BITMAP];
} __attribute__((packed));

#define AST_VIDEOCAP_FLAG_BUF_ALLOC_SZ		SZ_512K
#define AST_VIDEOCAP_HDR_BUF_SZ				SZ_16K
#define AST_VIDEOCAP_HDR_BUF_VIDEO_SZ		SZ_4K
#define AST_VIDEOCAP_COMPRESS_BUF_SZ		SZ_4M

#if (CONFIG_SPX_FEATURE_GLOBAL_MEMORY_SIZE == 0x3000000) || (CONFIG_SPX_FEATURE_GLOBAL_MEMORY_SIZE == 0x7000000) || \
	(CONFIG_SPX_FEATURE_GLOBAL_MEMORY_SIZE == 0xF000000) || (CONFIG_SPX_FEATURE_GLOBAL_MEMORY_SIZE == 0x1F000000)
	#define AST_VIDEOCAP_IN_BUF_SZ						0x900000 /* 9MB */
	#define AST_VIDEOCAP_FLAG_BUF_SZ					(((1920 * 1200) / (8 * 8)) * 4)
	#define AST_VIDEOCAP_REF_BUF_ADDR					(ast_videocap_video_buf_virt_addr)
	#define AST_VIDEOCAP_IN_BUF_ADDR					(AST_VIDEOCAP_REF_BUF_ADDR + AST_VIDEOCAP_IN_BUF_SZ)
	#define AST_VIDEOCAP_FLAG_BUF_ADDR					(AST_VIDEOCAP_IN_BUF_ADDR + AST_VIDEOCAP_IN_BUF_SZ)
	#define AST_VIDEOCAP_HDR_BUF_ADDR					(AST_VIDEOCAP_FLAG_BUF_ADDR + AST_VIDEOCAP_FLAG_BUF_ALLOC_SZ)
	#define AST_VIDEOCAP_COMPRESS_BUF_ADDR				(AST_VIDEOCAP_HDR_BUF_ADDR + AST_VIDEOCAP_HDR_BUF_SZ)
#else
	#define AST_VIDEOCAP_IN_BUF_SZ						0x800000 /* 8MB */
	#define AST_VIDEOCAP_FLAG_BUF_SZ					(((1600 * 1200) / (8 * 8)) * 4)
	#define AST_VIDEOCAP_REF_BUF_ADDR					(ast_videocap_video_buf_virt_addr)
	#define AST_VIDEOCAP_IN_BUF_ADDR					(AST_VIDEOCAP_REF_BUF_ADDR + AST_VIDEOCAP_IN_BUF_SZ)
	#define AST_VIDEOCAP_FLAG_BUF_ADDR					(AST_VIDEOCAP_IN_BUF_ADDR + AST_VIDEOCAP_IN_BUF_SZ)
	#define AST_VIDEOCAP_HDR_BUF_ADDR					(AST_VIDEOCAP_FLAG_BUF_ADDR + AST_VIDEOCAP_FLAG_BUF_ALLOC_SZ)
	#define AST_VIDEOCAP_COMPRESS_BUF_ADDR				(AST_VIDEOCAP_HDR_BUF_ADDR + AST_VIDEOCAP_HDR_BUF_SZ)
#endif

#define COMPRESSION_MODE                    (3)
#define JPEG_TABLE_SELECTOR                 (4)
#define UV_JPEG_TABLE_SELECTOR              (23)
#define JPEG_YUVTABLE_MAPPING               (0)
#define JPEG_SCALE_FACTOR                   (16)
#define SHARP_MODE_SELECTION                (0)
#define ADVANCE_TABLE_SELECTOR              (7)
#define ADVANCE_SCALE_FACTOR                (23)
#define DOWN_SCALING_METHOD                 (0)
#define DIFF_SETTING                        (0)
#define RC4_ENABLE                          (0)
#define RC4_RESET                           (0)
#define ENCODE_KEYS                         "fedcba9876543210"
#define ANALOG_DIFF_THRESHOLD               (8)
#define DIGITAL_DIFF_THRESHOLD              (0)
#define EXTERNAL_SIGNAL_ENABLE              (0)
#define AUTO_MODE                           (1)
#define DISABLE_HW_CURSOR                   (1)

#define ENCODE_KEYS_LEN                     (16)

struct ast_videocap_video_hdr_t {
	unsigned short  iEngVersion;
	unsigned short  wHeaderLen;

	// SourceModeInfo
	unsigned short  SourceMode_X;
	unsigned short  SourceMode_Y;
	unsigned short  SourceMode_ColorDepth;
	unsigned short  SourceMode_RefreshRate;
	unsigned char    SourceMode_ModeIndex;

	// DestinationModeInfo
	unsigned short  DestinationMode_X;
	unsigned short  DestinationMode_Y;
	unsigned short  DestinationMode_ColorDepth;
	unsigned short  DestinationMode_RefreshRate;
	unsigned char    DestinationMode_ModeIndex;

	// FRAME_HEADER
	unsigned long   FrameHdr_StartCode;
	unsigned long   FrameHdr_FrameNumber;
	unsigned short  FrameHdr_HSize;
	unsigned short  FrameHdr_VSize;
	unsigned long   FrameHdr_Reserved[2];
	unsigned char    FrameHdr_CompressionMode;
	unsigned char    FrameHdr_JPEGScaleFactor;
	unsigned char    FrameHdr_JPEGTableSelector;
	unsigned char    FrameHdr_JPEGYUVTableMapping;
	unsigned char    FrameHdr_SharpModeSelection;
	unsigned char    FrameHdr_AdvanceTableSelector;
	unsigned char    FrameHdr_AdvanceScaleFactor;
	unsigned long   FrameHdr_NumberOfMB;
	unsigned char    FrameHdr_RC4Enable;
	unsigned char    FrameHdr_RC4Reset;
	unsigned char    FrameHdr_Mode420;

	// INF_DATA
	unsigned char    InfData_DownScalingMethod;
	unsigned char    InfData_DifferentialSetting;
	unsigned short  InfData_AnalogDifferentialThreshold;
	unsigned short  InfData_DigitalDifferentialThreshold;
	unsigned char    InfData_ExternalSignalEnable;
	unsigned char    InfData_AutoMode;
	unsigned char    InfData_VQMode;

	// COMPRESS_DATA
	unsigned long   CompressData_SourceFrameSize;
	unsigned long   CompressData_CompressSize;
	unsigned long   CompressData_HDebug;
	unsigned long   CompressData_VDebug;

	unsigned char    InputSignal;
	unsigned short	Cursor_XPos;
	unsigned short	Cursor_YPos;
} __attribute__((packed));
#define AST_VIDEOCAP_VIDEO_HEADER_SIZE (sizeof(struct ast_videocap_video_hdr_t))

/* Commands used between the video server and client */
#define AST_VIDEO_DATA              (0x01)
#define AST_HW_CURSOR_DATA          (0x02)

//  Parameters
//#define    SAMPLE_RATE                                 12000000.0
//Sudan changed
#define SAMPLE_RATE                                      24000000
#define MODEDETECTION_VERTICAL_STABLE_MININUM            0x4
#define MODEDETECTION_HORIZONTAL_STABLE_MININUM          0x4
#define MODEDETECTION_VERTICAL_STABLE_THRESHOLD          0x4
#define MODEDETECTION_HORIZONTAL_STABLE_THRESHOLD        0x4
#define HORIZONTAL_SCALING_FILTER_PARAMETERS_LOW         0xFFFFFFFF
#define HORIZONTAL_SCALING_FILTER_PARAMETERS_HIGH        0xFFFFFFFF
#define VIDEO_WRITE_BACK_BUFFER_THRESHOLD_LOW            0x08
#define VIDEO_WRITE_BACK_BUFFER_THRESHOLD_HIGH           0x04
#define VQ_Y_LEVELS                                      0x10
#define VQ_UV_LEVELS                                     0x05
#define EXTERNAL_VIDEO_HSYNC_POLARITY                    0x01
#define EXTERNAL_VIDEO_VSYNC_POLARITY                    0x01
#define VIDEO_SOURCE_FROM                                0x01
#define EXTERNAL_ANALOG_SOURCE                           0x01
#define USE_INTERNAL_TIMING_GENERATOR                    0x01
#define WRITE_DATA_FORMAT                                0x00
#define SET_BCD_TO_WHOLE_FRAME                           0x01
#define ENABLE_VERTICAL_DOWN_SCALING                     0x01
#define BCD_TOLERENCE                                    0xFF
#define BCD_START_BLOCK_XY                               0x0
#define BCD_END_BLOCK_XY                                 0x3FFF
#define COLOR_DEPTH                                      16
#define BLOCK_SHARPNESS_DETECTION_HIGH_THRESHOLD         0xFF
#define BLOCK_SHARPNESS_DETECTION_LOE_THRESHOLD          0xFF
#define BLOCK_SHARPNESS_DETECTION_HIGH_COUNTS_THRESHOLD  0x3F
#define BLOCK_SHARPNESS_DETECTION_LOW_COUNTS_THRESHOLD   0x1F
#define VQTABLE_AUTO_GENERATE_BY_HARDWARE                0x0
#define VQTABLE_SELECTION                                0x0
#define JPEG_COMPRESS_ONLY                               0x0
#define DUAL_MODE_COMPRESS                               0x1
#define BSD_H_AND_V                                      0x0
#define ENABLE_RC4_ENCRYPTION                            0x1
#define BSD_ENABLE_HIGH_THRESHOLD_CHECK                  0x0
#define VIDEO_PROCESS_AUTO_TRIGGER                       0x0
#define VIDEO_COMPRESS_AUTO_TRIGGER                      0x0
#define VIDEO_COMPRESS_SOURCE_BUFFER_ADDRESS             0x200000

/* Defines for CompressionMode */
#define COMP_MODE_YUV420            (0)
#define COMP_MODE_YUV444_JPG_ONLY   (1)
#define COMP_MODE_YUV444_2_CLR_VQ   (2)
#define COMP_MODE_YUV444_4_CLR_VQ   (3)

/* Timeout Types */
#define NO_TIMEOUT           0x00
#define MODE_DET_TIMEOUT     0x01
#define CAPTURE_TIMEOUT      0x02
#define COMPRESSION_TIMEOUT  0x04
#define NO_INPUT_TIMEOUT     0x05
#define SPURIOUS_TIMEOUT     0x06
#define AUTOMODE_TIMEOUT     0x08
#define OTHER_TIMEOUT        0x10

#endif  /* __AST_VIDEOCAP_REG_H__ */
