/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

#ifndef __VIDEO_INTERFACE_H__
#define __VIDEO_INTERFACE_H__

#include "coreTypes.h"
#include "videopkt.h"
#include "session_data.h"

typedef void (*p_thread_main_fn) (void); 

typedef struct 
{
	u8	num_threads;
	p_thread_main_fn*  thread_fns;

} soc_threads_info_t ;

#define CAPTURE_VIDEO_ERROR	-1
#define CAPTURE_VIDEO_SUCCESS	 0
#define CAPTURE_VIDEO_NO_CHANGE  1
#define CAPTURE_VIDEO_NO_SIGNAL  2

#define LIBVIDEO_PATH		"/usr/local/lib/libvideo.so"
#define VIDEO_HOOKS_PATH	"/usr/local/lib/libpdkapp.so"
#define HOST_LOCK_FN		"PDK_AfterHostLock"
#define START_STOP_SSN		"PDK_StartStopSession"
#define CONTROL_DISPLAY_SSN	"display_control_ssn"
#define CONTROL_DISPLAY		"display_control"
#define GET_SOC_HDR_FN		"get_soc_session_hdr"

#define VIDEORECORD_LIBRARY		"/usr/local/lib/libautovideorecord.so"
#define START_VIDEO_RECORDING		"start_video_recording"
#define WAIT_ON_VIDEO_RECORD_THREAD	"wait_on_video_record_thread" 
#define START_VIDEO_RECORD_THREAD	"start_video_record_thread"
#define SEND_VIDEO_DATA			"send_video_data"

/*******************************************************************/
// Interface functions between Adviser and Libvideo

int init_video_resources ( void );
void release_video_resources ( void );

void resume_video ( void );
int handle_soc_specific_video_cmd (session_info_t* p_si, ivtp_hdr_t* hdr);
int capture_video (u32*  frame_hdr_size, u32*  data_buf_size, void**  pp_frame_hdr, void** pp_comp_buf, session_list_t* session_list);
int get_soc_threads_info ( soc_threads_info_t* soc_th_info );
int send_soc_specific_data (session_list_t *send_list, void* p_frame_hdr);
void display_control_ssn (session_info_t* p_si, u8 lockstatus, u8 SkipSend);
void display_control (u8 lockstatus);
void control_xcursor_thread (session_info_t* p_si, u8 xcursorthread);
void get_soc_session_hdr (void** soc_session_hdr);
#endif

