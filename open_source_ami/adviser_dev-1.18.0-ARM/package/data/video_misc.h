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

#ifndef _PORTING_VIDEO_MISC_H_
#define _PORTING_VIDEO_MISC_H_

#include "session_data.h"

u8     	g_send_full_screen;

//####################################################//
// Function Prototypes

//for solved the mouse of Remote Java View is not smooth When local sytem boot to Linux.
#if defined(SOC_AST2050) || defined(SOC_AST2150) || defined(SOC_AST2300)
#define USING_AST_HW		TRUE
#else
#define USING_AST_HW		FALSE
#endif

int soc_create_threads (void);
int soc_wait_for_threads_to_exit (void);
void check_if_logout( void );

void setflags_before_videocapture( session_list_t* sess_list);
void setflags_after_videocapture( int* prev_cap_status, int cap_status);
int send_non_video_frame_data( session_list_t* send_list, void* p_frame_hdr );

#endif

