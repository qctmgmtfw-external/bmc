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

/***************************************************************
* @file   	Cmworker.c
* @author 	Govind Kothandapani		<govindk@ami.com>
*		Baskar Parthiban		<bparthiban@ami.com>
*		Varadachari Sudan Ayanam 	<varadacharia@ami.com>
* @brief  	Client worker thread implementation
****************************************************************/

#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "cmworker.h"
#include "sessionmgr.h"
#include "adviserdbg.h"
#include "featuredef.h"
#include "coreTypes.h"
#include "sync.h"
#include<sys/prctl.h>

#define SELECT_RESPONSE_TIMEOUT_SECS		30 //5 //Quanta

/*--------------------- Function Prototypes -------------------------------*/
static void 	cmw_send_ctrl_pkt 	(session_info_t* p_si);
static void 	cmw_send_frame_data 	(session_info_t* p_si);
static int	cmw_send_auto_bw_pkt 	(session_info_t* p_si);
static ssize_t 	cmw_send 		(int s, const void *msg, size_t len, int flags, session_info_t* p_si);
static int     	cmw_wait 		(session_info_t* p_si, int num_ms);
extern CoreFeatures_T g_corefeatures;

/**
 * cm_worker_thread
**/
int
cm_worker_thread (session_info_t* p_session_info)
{
	u8		status = ERROR;
	int retval;
	prctl(PR_SET_NAME,__FUNCTION__,0,0,0);

	while (!p_session_info->stop)
	{
		/* wait for the wakeup event */
		TINFO_4 ("Waiting for wake up event\n");
		status = sem_wait (p_session_info->pev_wakeup);
		if (status == ERR_TIMEOUT) { continue; }

		TINFO_4 ("WORKER THREAD WAKE UP\n");
		/* If there is some other error - internal error abort this thread */
		if (status != SUCCESS)
		{
			acquire_mutex (p_session_info->p_access_guard);
			p_session_info->transfer_status = ERR_CRITICAL;
			p_session_info->ctrl_transfer_status = ERR_CRITICAL;
			release_mutex (p_session_info->p_access_guard);
			TINFO_4 ("Internal error - closing the worker thread\n");
			break;
		}

		/* If this is a request to send control packets - send it immediately */
		if (p_session_info->is_ctrl_pkt)
		{
			if (p_session_info->ctrl_pkt_type != AUTO_BANDWIDTH_DETECT_CTRL_PKT)
			{
				cmw_send_ctrl_pkt   (p_session_info);
			}
			else
			{
				TINFO_4 ("CM Auto Bandwidth packet to send\n");
				retval =cmw_send_auto_bw_pkt (p_session_info);
				if(retval!=0)
				{
						TCRIT("Error in cmw_send_auto_bw_pkt()\n ");
						return -1;
				}
			}
		}
		if (p_session_info->is_frame_pkt)
		{
			cmw_send_frame_data (p_session_info);
		}
	}

	/* Set the flag indicating this worker thread is stopped */
	p_session_info->thread_stopped = TRUE;

	TINFO_1 ("Worker Thread Exit\n");
	return -1;
}


/**
 * send_ctrl_pkt
**/
static void
cmw_send_ctrl_pkt (session_info_t* p_si)
{
	//ivtp_hdr_t	*hdr;

	/* Acquire exclusive access to session info structure */
	acquire_mutex (p_si->p_access_guard);

	//hdr = (ivtp_hdr_t *)p_si->p_ctrl_pkt;

	/* Send the header in the control packet */
	if (-1 == cmw_send (p_si->socket, p_si->p_ctrl_pkt, sizeof (ivtp_hdr_t), 0, p_si))
	{
		TINFO_4 ("Unable to Send Ctrl packet header\n");
		sm_set_ctrl_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
		p_si->ctrl_transfer_status = ERR_CONNECTION_LOST;
	}
	else
	{
		TINFO_4 ("Ctrl packet data send :%ld\n", p_si->frame_size);
		if ( (p_si->ctrl_frame_size) && (p_si->p_ctrl_frame_data != NULL) )
		{
			/* Send the data packet */
			if (-1 == cmw_send (p_si->socket, p_si->p_ctrl_frame_data, p_si->ctrl_frame_size, 0, p_si))
			{
				TINFO_4 ("Unable to Send Ctrl packet data\n");
				sm_set_ctrl_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
				p_si->ctrl_transfer_status = ERR_CONNECTION_LOST;
			}
			else
			{
				p_si->ctrl_transfer_status = SUCCESS;
			}
		}
		else
		{
			p_si->ctrl_transfer_status = SUCCESS;
		}
	}
	p_si->is_ctrl_pkt = FALSE;

	/* Release exclusive access to session info structure */
	release_mutex (p_si->p_access_guard);

	sm_clean_sessions();

	/* Acquire exclusive access to global session variables */
	acquire_mutex (p_si->p_global_guard);

	/* Decrement the pending clients count */
	(*(p_si->p_pending_clients))--;

	/* If no more pending clients the transfer has been completed - flag it */
	if ((int)*(p_si->p_pending_clients) <= 0)
	{
		sem_post (p_si->pev_transfer_complete);
	}

	/* Release exclusive access to global session variables */
	release_mutex (p_si->p_global_guard);

	return;
}


/**
 * send_frame_data
**/
static void
cmw_send_frame_data (session_info_t* p_si)
{
	u16			frag_hdr_size;
	u32			frag_size;
	video_frag_hdr_t	vf_hdr;
	u32			bytes_left = p_si->frame_size;
	u32			frame_ix   = 0;
	u16			frag_num;
	int			twait;

	/* Acquire exclusive access to session info structure */
	acquire_mutex (p_si->p_access_guard);

	/* Calculate the first fragment size */
	frag_hdr_size	= (sizeof (video_frag_hdr_t) + p_si->frame_hdr_size);
	if (p_si->send_fragmented)
	{
		frag_size		= ((p_si->frame_size + p_si->frame_hdr_size) < p_si->max_frag_size) ?
						   p_si->frame_size : p_si->max_frag_size - p_si->frame_hdr_size;
	}
	else
	{
		frag_size		= p_si->frame_size;
	}


	/* Send the first fragment out */
	/* ... Send the video fragment header */
	vf_hdr.hdr.type		= IVTP_VIDEO_FRAGMENT;
	vf_hdr.hdr.pkt_size = frag_hdr_size - sizeof (ivtp_hdr_t) + frag_size;
	frag_num			= (frag_size == p_si->frame_size) ? 0x8000 : 0;
	vf_hdr.frag_num		= frag_num;
	if (-1 == cmw_send (p_si->socket, &vf_hdr, sizeof (video_frag_hdr_t), 0, p_si))
	{
		sm_set_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
		p_si->transfer_status = ERR_CONNECTION_LOST;
		goto	sfd_exit;
	}
	TINFO_4 ("sent first frag size:%ld, num:%x\n", frag_size, vf_hdr.frag_num);

	/* ... Send the frame header */
	if (-1 == cmw_send (p_si->socket, p_si->p_frame_hdr, p_si->frame_hdr_size, 0, p_si))
	{
		sm_set_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
		p_si->transfer_status = ERR_CONNECTION_LOST;
		goto	sfd_exit;
	}

	/* ... Send the first installment of frame data */
	if (-1 == cmw_send (p_si->socket, p_si->p_frame_data, frag_size, 0, p_si))
	{
		sm_set_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
		p_si->transfer_status = ERR_CONNECTION_LOST;
		goto	sfd_exit;
	}

	/* Update the indices */
	bytes_left		= p_si->frame_size - frag_size;
	frame_ix		= frag_size;

	/* Send the remainder of the frame */
	while ((bytes_left > 0) && !*(p_si->p_abort))
	{
		/* Wait for the current fragment to be sent */
		twait = ((frag_size * 1000) / p_si->bandwidth);

		if (0 != twait)
		{
		//	TINFO_4 ("w:%d,cx:%d\n", twait, p_si->p_frame_hdr->params.cell_x);
			release_mutex (p_si->p_access_guard);		/* Release the mutex for other threads */
			if (-1 == cmw_wait (p_si, twait))
			{
				acquire_mutex (p_si->p_access_guard);
				/* Must have been a request to abort - exit the loop */
				goto	sfd_exit;
			}
			acquire_mutex (p_si->p_access_guard);
		}

		frag_size	= (bytes_left < p_si->max_frag_size) ? bytes_left : p_si->max_frag_size;
		frag_num++;

		/* Send the video fragment header */
		vf_hdr.hdr.type		= IVTP_VIDEO_FRAGMENT;
		vf_hdr.hdr.pkt_size = sizeof (video_frag_hdr_t) - sizeof (ivtp_hdr_t) + frag_size;
		vf_hdr.frag_num		= (frag_size == bytes_left) ? 0x8000 + frag_num : frag_num;
		if (-1 == cmw_send (p_si->socket, &vf_hdr, sizeof (video_frag_hdr_t), 0, p_si))
		{
			sm_set_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
			p_si->transfer_status = ERR_CONNECTION_LOST;
			goto	sfd_exit;
		}

		/* Send the frame data */
		if (-1 == cmw_send (p_si->socket, p_si->p_frame_data + frame_ix, frag_size, 0, p_si))
		{
			sm_set_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
			p_si->transfer_status = ERR_CONNECTION_LOST;
			goto	sfd_exit;
		}
		TINFO_4 ("sent frag size:%ld, num:%x\n", frag_size, vf_hdr.frag_num);

		/* update the indices */
		bytes_left	-= frag_size;
		frame_ix	+= frag_size;
	}
	p_si->transfer_status = 0;		/* Diffrentiate between connection lost and abort request */

sfd_exit:
	p_si->is_frame_pkt = FALSE;
	/* Release exclusive access to session info structure */
	release_mutex (p_si->p_access_guard);

	sm_clean_sessions();

	/* Acquire exclusive access to global session variables */
	acquire_mutex (p_si->p_global_guard);

	/* If we have aborted the transaction - increment the abort count */
	if (bytes_left != 0) { (*(p_si->p_abort_count))++; }

	/* Decrement the pending clients count */
	(*(p_si->p_pending_clients))--;

	TINFO_4 ("Pending clients : %d\n", *(p_si->p_pending_clients));
	/* If no more pending clients the transfer has been completed - flag it */
	if ((int)*(p_si->p_pending_clients) <= 0)
	{
		TINFO_3 ("Transfer Complete\n");
		sem_post (p_si->pev_transfer_complete);
	}

	release_mutex (p_si->p_global_guard);
}


/**
 * cmw_send_auto_bw_pkt
**/
static int
cmw_send_auto_bw_pkt (session_info_t* p_si)
{
	ivtp_hdr_t		resp_hdr;
	u8*				data;
	int 			num_send = 1;
	int 			i = 0;

	if (p_si->auto_bw_pkt_size > BANDWIDTH_1MBPS)
	{
		num_send = p_si->auto_bw_pkt_size / BANDWIDTH_1MBPS;
		TINFO_4 ("Req Bw is greater than %d send in %d times\n", BANDWIDTH_1MBPS, num_send);
		data = (u8*) malloc (BANDWIDTH_1MBPS);
		if(data==NULL)
		{
			TCRIT("unable to allocate memory to data\n");
			return -1;
		}
		memset (data, 0, BANDWIDTH_1MBPS);
		resp_hdr.pkt_size = BANDWIDTH_1MBPS;
		resp_hdr.type	  = IVTP_BW_DETECT_RESP;
		resp_hdr.status	  = num_send;
	}
	else
	{
		data = (u8*) malloc (p_si->auto_bw_pkt_size);
		if(data==NULL)
		{
			TCRIT("unable to allocate memory to data\n");
			return -1;
		}
		memset (data, 0, p_si->auto_bw_pkt_size);
		resp_hdr.pkt_size = p_si->auto_bw_pkt_size;
		resp_hdr.type	  = IVTP_BW_DETECT_RESP;
		resp_hdr.status	  = num_send;
	}

	/* Acquire exclusive access to session info structure */
	acquire_mutex (p_si->p_access_guard);

	/* Send the header in the control packet */
	if (-1 == cmw_send (p_si->socket, &resp_hdr, sizeof (ivtp_hdr_t), 0, p_si))
	{
		TINFO ("Unable to Send Auto BW packet header\n");
		sm_set_ctrl_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
		p_si->ctrl_transfer_status = ERR_CONNECTION_LOST;
	}
	else
	{
		for (i = 0; i < num_send; i++)
		{
			TINFO_4 ("Ctrl packet data send :%ld\n", p_si->frame_size);
			/* Send the data packet */
			if (-1 == cmw_send (p_si->socket, (char*)data, resp_hdr.pkt_size, 0, p_si))
			{
				TINFO ("Unable to Send Auto BW data packet\n");
				sm_set_ctrl_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
				p_si->ctrl_transfer_status = ERR_CONNECTION_LOST;
				break;
			}
			else
			{
				p_si->ctrl_transfer_status = SUCCESS;
			}
		}
	}

	p_si->is_ctrl_pkt = FALSE;
	/* Release exclusive access to session info structure */
	release_mutex (p_si->p_access_guard);

	sm_clean_sessions();

	/* Acquire exclusive access to global session variables */
	acquire_mutex (p_si->p_global_guard);

	/* Decrement the pending clients count */
	(*(p_si->p_pending_clients))--;

	/* If no more pending clients the transfer has been completed - flag it */
	if ((int)*(p_si->p_pending_clients) <= 0)
	{
		sem_post (p_si->pev_transfer_complete);
	}

	free(data);

	/* Release exclusive access to global session variables */
	release_mutex (p_si->p_global_guard);

	return 0;
}


/**
* cmw_send
**/
static ssize_t
cmw_send (int s, const void *msg, size_t len, int flags, session_info_t* p_si)
{
	size_t 		ix = 0;
	ssize_t 		sent_size = 0;
	struct timeval		tv = { 0,0 };
	if(0)
	{
		flags=flags; /* -Wextra, fix for unused parameters */
	}
	if(g_corefeatures.kvm_reconnect_support == ENABLED)
	{
		if((p_si->clean) ||(p_si->transfer_status == ERR_CONNECTION_LOST) ||
		(p_si->ctrl_transfer_status == ERR_CONNECTION_LOST))
		{
			TINFO_4("Could not send to the Client. %s\n", strerror(errno));
			return -1;
		}
	}

	TINFO_4 ("Sending packet len:%d\n", len);

	fd_set         	wfds;           /* Write file descriptors */
	int            	retval;         /* Select return value */

	do
	{
		FD_ZERO(&wfds);
		FD_SET (s, &wfds);
		tv.tv_sec = SELECT_RESPONSE_TIMEOUT_SECS;
		tv.tv_usec = 0;

		retval = select (s + 1, NULL, &wfds, NULL, &tv);
		if (retval > 0)
		{
			if (FD_ISSET (s, &wfds))
			{
				FD_ZERO(&wfds);
				sent_size = send (s, msg + ix, (len - ix), MSG_NOSIGNAL);
				if (sent_size == -1)
				{
					TINFO_4("Could not send to the Client. %s\n", strerror(errno));
					if (p_si->is_used)
					{
						p_si->clean = TRUE;
						sm_set_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
						if(g_corefeatures.kvm_reconnect_support == ENABLED)
						{
							if(p_si->failed_time == 0)
							{
								time(&p_si->failed_time);
							}
						}
					}
					return -1;
				}
				else
				{
					if(g_corefeatures.kvm_reconnect_support == ENABLED)
					{
						if(p_si->failed_time > 0)
						{
							p_si->failed_time = 0;
						}
					}
				}
			}
		}
		else if(retval == 0)
		{
			TINFO_4("Select Timedout. Closing the session.\n");
			if (p_si->is_used)
			{
				p_si->clean = TRUE;
				sm_set_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
				if(g_corefeatures.kvm_reconnect_support == ENABLED)
				{
					if(p_si->failed_time == 0)
					{
						time(&p_si->failed_time);
					}
				}
			}
			return -1;
		}
		else if (retval == EINTR)
		{
			TINFO_4("Select interrupted by Signal. Continuing again...\n");
			continue;
		} 
		else
		{
			TINFO_4("Some state in Select. Returning");
			if (p_si->is_used)
			{
				p_si->clean = TRUE;
				sm_set_transfer_status(p_si->h_session,ERR_CONNECTION_LOST);
				if(g_corefeatures.kvm_reconnect_support == ENABLED)
				{
					if(p_si->failed_time == 0)
					{
						time(&p_si->failed_time);
					}
				}
			}
			return -1;
		}

		ix += sent_size;
	} while (ix != len);

	return len;
}


/**
 * cmw_wait
**/
static int
cmw_wait (session_info_t* p_si, int num_ms)
{
	struct timespec sleep = {0, num_ms * 1000*1000};
	/* If there was a request for abort - exit */
	if (*(p_si->p_abort)) { return -1; }
	
	nanosleep(&sleep, NULL);

	return 0;
}

