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

#ifndef _VIDEO_PKT_SOC_H_
#define _VIDEO_PKT_SOC_H_

#include <stdint.h>

#define IVTP_HW_CURSOR			(0x1002)
#define IVTP_GET_VIDEO_CONFIG	(0x1003)
#define IVTP_SET_VIDEO_CONFIG	(0x1004)

typedef struct {
	uint16_t  iEngVersion;
	uint16_t  wHeaderLen;

	// SourceModeInfo
	uint16_t  SourceMode_X;
	uint16_t  SourceMode_Y;
	uint16_t  SourceMode_ColorDepth;
	uint16_t  SourceMode_RefreshRate;
	uint8_t    SourceMode_ModeIndex;

	// DestinationModeInfo
	uint16_t  DestinationMode_X;
	uint16_t  DestinationMode_Y;
	uint16_t  DestinationMode_ColorDepth;
	uint16_t  DestinationMode_RefreshRate;
	uint8_t    DestinationMode_ModeIndex;

	// FRAME_HEADER
	uint32_t   FrameHdr_StartCode;
	uint32_t   FrameHdr_FrameNumber;
	uint16_t  FrameHdr_HSize;
	uint16_t  FrameHdr_VSize;
	uint32_t   FrameHdr_Reserved[2];
	uint8_t    FrameHdr_CompressionMode;
	uint8_t    FrameHdr_JPEGScaleFactor;
	uint8_t    FrameHdr_JPEGTableSelector;
	uint8_t    FrameHdr_JPEGYUVTableMapping;
	uint8_t    FrameHdr_SharpModeSelection;
	uint8_t    FrameHdr_AdvanceTableSelector;
	uint8_t    FrameHdr_AdvanceScaleFactor;
	uint32_t   FrameHdr_NumberOfMB;
	uint8_t    FrameHdr_RC4Enable;
	uint8_t    FrameHdr_RC4Reset;
	uint8_t    FrameHdr_Mode420;

	// INF_DATA
	uint8_t    InfData_DownScalingMethod;
	uint8_t    InfData_DifferentialSetting;
	uint16_t  InfData_AnalogDifferentialThreshold;
	uint16_t  InfData_DigitalDifferentialThreshold;
	uint8_t    InfData_ExternalSignalEnable;
	uint8_t    InfData_AutoMode;
	uint8_t    InfData_VQMode;

	// COMPRESS_DATA
	uint32_t   CompressData_SourceFrameSize;
	uint32_t   CompressData_CompressSize;
	uint32_t   CompressData_HDebug;
	uint32_t   CompressData_VDebug;

	uint8_t    InputSignal;
	uint16_t	Cursor_XPos;
	uint16_t	Cursor_YPos;
} __attribute__ ((packed)) frame_hdr_t;

#endif /* _VIDEO_PKT_SOC_H_ */
