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

/**
 * Conmgr.h
 * Connection manager data structures and associated definitions
**/

#ifndef CONMGR_H
#define CONMGR_H

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "dbgout.h"
#include "coreTypes.h"
#include "advisererrno.h"
#include "sessionmgr.h"


int
connection_acceptor (int port);

int
connect_to_hidserver (char *ip_addr, int port);

int
cm_send_video_frame ( 	void* 		p_frame_hdr,
			u32		frame_hdr_size,
			void* 		p_frame_data,
			u32		frame_size,
			session_list_t*	p_client_list,
			BOOL		send_immediate);

int
send_control_pkt (ivtp_hdr_t* p_pkt, session_list_t* p_session_list, u8* data, int size, int pkt_type);

void
form_control_pkt_hdr (ivtp_hdr_t* p_pkt, int type, int size);

BOOL
cm_is_busy (void);

int
cm_wait (void);

int
cm_stop (session_info_t* p_client_info);

int
wait_for_event (sem_t event, u16 time);

int
cm_abort_current_frame (hsession_t hsession);

#endif /* CONMGR_H */

