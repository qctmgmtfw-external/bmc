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
 * session_data.h
 * Session Data
**/

#ifndef __SESSION_DATA_H__
#define __SESSION_DATA_H__

#include <semaphore.h>
#include <pthread.h>
#include "coreTypes.h"
#include "videopkt.h"
#include "usermdefs.h"
#include "sessioncfg.h"
#include "blowfish.h"

#define HASH_SIZE       16

#define START_SESSION_HOOK	1
#define STOP_SESSION_HOOK	0

/**
 * @struct session_info
 * @brief Per Session information.
**/
typedef struct
{

	BOOL                    is_webPreviewer;        /* TRUE if this session is webpreviewer session. Otherwise false*/
	BOOL			is_used;		/* TRUE if this session entry is used / FALSE otherwise */
	BOOL			is_valid;		/* TRUE is this is a valid session */
	BOOL			clean;			/* TRUE if the session needs to be cleaned up */
	BOOL			pause;
	int			h_session;		/* Handle to the session */
	int			login_timer;		/* Tick count before which a login needs to be attempted */
	int			activity_timer;		/* Tick count before which an activity needs to be done */
	int			worker_thread_id;	/* Worker thread that will be used to transfer the fragment */
	sem_t*			pev_wakeup;		/* Event to be used to wakeup the worker thread */
	pthread_mutex_t*	p_access_guard;		/* Atomic access to be used for accessing this structure */
	pthread_mutex_t*	p_global_guard;		/* Atomic access to be used for accessing this structure */
	BOOL			stop;			/* Flag to stop this thread */
	BOOL			thread_stopped;		/* Flag to indicate the thread is stopped */
	char			ip_addr [16];		/* IP Address of the connection 	*/
	int			portno;			/* Port number of the client	*/
	int			socket;			/* Socket to be used for communication with the client */
	u32			bandwidth;		/* Bandwidth of the connection */
	u32			max_frag_size;		/* Maximum fragment size to be used for communication  */
	u8			fps;			/* Acceptable frames / second	*/
	BOOL 			is_ctrl_pkt;		/* TRUE if this is a control packet */
	int			ctrl_pkt_type;		/* Type to differentiate between regular ctrl pkt and auto bandwidth detect packet */
	u32*			p_pending_clients;	/* Total number of clients that are still transfering - 0 if transfer complete */
	BOOL*			p_abort;		/* Flag used to tell the clients to abort TRUE to abort FALSE otherwise */
	u32*			p_abort_count;		/* Number of clients that have aborted */
	sem_t*			pev_transfer_complete;	/* Event to indicate that the transfer is complete */
	u8*			p_frame_hdr;		/* Pointer to the header of the frame to be transmitted */
	u32			frame_hdr_size;		/* Size of the frame header */
	u8*			p_frame_data;		/* Pointer to the frame data */
	u32			frame_size;		/* Size of the frame */
	BOOL			send_fragmented;	/* TRUE if need to be sent fragmented */
	ivtp_hdr_t*		p_ctrl_pkt;		/* Pointer to control packet if it's request for control packet */
	u8			transfer_status;	/* Return status code for the transfer */
	BOOL			is_busy;		/* TRUE if busy */
	u16			tsend;			/* time taken to send frame/frag data */
	u8 			quality_level;		/* Quality level set for this session */
	BOOL			is_stop_signal;
	u8 			encrypt_key [MAX_PASSWORD_LEN_HASHED + 1];
	BFHANDLE 		h_encrypt;
	BOOL			is_encrypted;
	BOOL			is_send_encrypt_status;
	BOOL			is_auto_bw_detect;
	u32			auto_bw_pkt_size;
	BOOL			is_send_mouse_mode;	/* indicates send mouse mode pkt pending*/
	u32			mouse_mode_pkt_size;	/* indicates the size of mouse mode pkt*/
	u32			media_active;		/* indicates if media redir is ON or OFF */
	char			session_token [HASH_SIZE]; 	/* hashed token will be used */
	char			websession_token [2*HASH_SIZE + 3];	/* Web Session Token */
	BOOL                    is_send_video_engine_configs;
        BOOL                    is_send_active_clients;
	pthread_mutex_t*	p_access_detect_auto_bw; 	/* syncronization for auto bw detect */
	BOOL				is_socket_close_ctrl_pkt;
	unsigned short		socket_close_status;
	BOOL				is_kvm_priv_ctrl_pkt;
	unsigned short		kvm_pkt_status;
	unsigned char			kvm_priv;
	char                             ip_addr_client[TOKENCFG_MAX_CLIENTIP_LEN+1];/**IP address of the First session**/
	char					ip_addr_othersession [TOKENCFG_MAX_CLIENTIP_LEN+1];/**IP address of the Second session**/
	char					 username[TOKENCFG_MAX_USERNAME_LEN];/**User name of the First session**/
	char 				username_other_session [TOKENCFG_MAX_USERNAME_LEN];/**User name of the Second session**/
	BOOL				is_power_status_send;
	BOOL 				is_power_cmd_status_send;
	u8					host_power_cmd_status;
	void*			soc_session_hdr;
} session_info_t;

/**
 *  * @struct session_list_t
 *   * Structure describing a session list
 *   **/
typedef struct
{
        int                                     num_session;
        BOOL                                    is_new_session;
        BOOL                                    qlzw_init_done;
        session_info_t*                         p_session [MAX_POSSIBLE_KVM_SESSIONS];

} session_list_t;


#endif

