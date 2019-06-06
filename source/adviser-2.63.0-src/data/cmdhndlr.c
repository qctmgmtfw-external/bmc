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
 * cmdhndlr.c
**/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <string.h>
#include "coreTypes.h"
#include "videopkt.h"
#include "cmdhndlr.h"
#include "adviserdbg.h"
#include "reader.h"
#include "utils.h"
#include "conmgr.h"
#include "sessionmgr.h"
#include "iusb.h"
#include "avse.h"

#include "hid_cmdhandlr.h"

#include "sessionlogin.h"
#include "compress.h"
#include "rle.h"
#include "qlzw.h"
#include "advisercfg.h"

#include <linux/ioctl.h>
#include "usb_ioctl.h"
#include "video_interface.h"
#include "video_misc.h"
#include "token.h"
#include "sync.h"
#include "encrypt.h"
#include "ncml.h"
#include "utils.h"

#include "libipmi_session.h"
#include "libipmi_struct.h"
#include "libipmi_errorcodes.h"
#include "libipmi_AMIOEM.h"
#include "libipmi_ChassisDevice.h"
#include "adviser_cfg.h"
#include<sys/prctl.h>
#include "featuredef.h"
#include  "userprivilege.h"
#include "Extendedpriv.h"
#include "user_audits.h"
#include "racsessioninfo.h"

extern int OnGethostlockfeature_dynm(int* pAdviser_hostlckfeature);
#define TIMEOUT 10
#define POWERCTL_DEFAULT_TIMEOUT 2
#define MAX_HID_PKT_SIZE		(2 * 1024)
#define ENCRYPTION_ENABLED		(255)
#define max(a, b) 			(((a) > (b)) ? (a) : (b))

#define WEB_PREVIEWER_CAP_FILE           "/var/capture/webPreview.cap"
#define WEB_PREVIEWER_CAP_DIRECTORY           "/var/capture"
#define SHIFT_8		8
#define SLEEP_TIME 50*1000*1000 
#define SLEEP_TIME_1 10*1000
extern u8	g_mouse_mode;
extern u8	g_err_mouse_mode_init;
extern sem_t	m_start_avse;
extern sem_t	m_start_cmdhndlr;
extern SERVICE_CONF_STRUCT	AdviserConf;
extern int	kvm_max_session;
extern u8	*service_info_data;
extern CoreFeatures_T g_corefeatures;
extern int do_usb_ioctl(int cmd, u8 *data);
extern BOOL		g_media_license_status;

u8      m_compression_type = SOFT_COMPRESSION_RLE_QLZW;

u8 g_host_is_locked = 0;
u8 g_host_lock_auto_feature = 0;
/*--------------------- Function Prototypes --------------------------------*/
static int on_hid_packet 		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int set_bandwidth 		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int set_fps 			(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int pause_redirection 		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int refresh_video_screen		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int resume_redirection 		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_set_compression_type  	(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_stop_command              (session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_enable_encryption  	(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_disable_encryption  	(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_bw_request		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_validate_video_session	(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_send_keyboard_LED		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_get_web_token		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_send_full_screen  	(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_media_state		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_kvm_priv_pkt(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_web_previewer_session     (session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_set_mouse_mode 		(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_power_ctrl (session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_power_status (session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_get_user_macro(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_set_user_macro(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_send_raw_ipmi(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_request_next_master(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int onHostDisplayControl(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_set_kbd_language(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int on_keep_alive_pkt(session_info_t* p_si, ivtp_hdr_t* p_hdr);
static int handle_video_cmd (session_info_t* p_si);
static int on_disconnect_session (session_info_t* p_si, ivtp_hdr_t* p_hdr);
extern int GetIUSBLEDPkt(u8 *LEDstatus, int no_wait);
extern int ResetKbdLEDStatus();
void display_control (u8 lockstatus);
static int on_fps_diff(session_info_t* p_si, ivtp_hdr_t* p_hdr);

extern uint16 MapUid_to_SELUid (INT8 *username, char *Shell);
/*---------------------- Module Variables ----------------------------------*/
hndlr_map_t m_hndlr_map [] =
{
	{ IVTP_HID_PKT,				on_hid_packet			},
	{ IVTP_SET_BANDWIDTH,			set_bandwidth			},
	{ IVTP_SET_FPS,				set_fps				},
	{ IVTP_PAUSE_REDIRECTION,		pause_redirection		},
	{ IVTP_REFRESH_VIDEO_SCREEN,		refresh_video_screen		},
	{ IVTP_RESUME_REDIRECTION,		resume_redirection		},
	{ IVTP_SET_COMPRESSION_TYPE,		on_set_compression_type 	},
        { IVTP_STOP_SESSION_IMMEDIATE,		on_stop_command			},
	{ IVTP_ENABLE_ENCRYPTION,		on_enable_encryption 		},
	{ IVTP_DISABLE_ENCRYPTION,		on_disable_encryption 		},
	{ IVTP_BW_DETECT_REQ,			on_bw_request     		},
	{ IVTP_VALIDATE_VIDEO_SESSION, 		on_validate_video_session 	},
	{ IVTP_GET_KEYBD_LED,			on_send_keyboard_LED     	},
	{ IVTP_GET_WEB_TOKEN,			on_get_web_token		},
	{ IVTP_GET_FULL_SCREEN,			on_send_full_screen		},
	{ IVTP_MEDIA_STATE,			on_media_state			},
	{ IVTP_KVM_SHARING,			on_kvm_priv_pkt			},
    	{ IVTP_WEB_PREVIEWER_SESSION,           on_web_previewer_session 	},
	{ IVTP_SET_MOUSE_MODE,			on_set_mouse_mode		},
	{IVTP_POWER_STATUS,				on_power_status	},
	{IVTP_POWER_CTRL_REQST,				on_power_ctrl	},
	{ IVTP_GET_USER_MACRO,			on_get_user_macro	},
	{ IVTP_SET_USER_MACRO,			on_set_user_macro	},
	{IVTP_IPMI_REQ_COMMAND,			on_send_raw_ipmi	},
	{IVTP_SET_NEXT_MASTER,			on_request_next_master},
	{ IVTP_DISPLAY_LOCK_SET,			onHostDisplayControl		},
	{ IVTP_SET_KBD_LANG	,			on_set_kbd_language	},
	{ IVTP_KEEP_ALIVE_PKT,          on_keep_alive_pkt },
	{ IVTP_KVM_DISCONNECT,          on_disconnect_session },
	{ IVTP_FPS_DIFF,          on_fps_diff },	
	{ 0xffff,				0				}
};

void kvm_loginLogoutAudit(char* uname, char* ipstr, uint8 state)
{
	LoginLogoutAudit (NULL, KVM_SERVICE_NAME, uname, ipstr, state);
}


static int
on_hid_packet (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	u8 	hid_pkt [MAX_HID_PKT_SIZE];

	int encrypted_data_size;

	p_hdr->pkt_size = htonlEx (p_hdr->pkt_size);
	if ((p_hdr->pkt_size) > MAX_HID_PKT_SIZE)
        {
                TCRIT("This doesn't seem to be our Token Packet. Ignoring this packet\n");
                return -1;
        }

	if (0 != recv_bytes (p_si->socket, hid_pkt, p_hdr->pkt_size, p_si))
	{
		return -1;
	}

	p_hdr->status = htonsEx (p_hdr->status);

	if (p_hdr->status == ENCRYPTION_ENABLED)
	{
		//unsigned char *encrypted_data;
		/* Decrypt the packet before passing on to HID server */
		/******     USB HID PACKET      ******/
		encrypted_data_size  = p_hdr->pkt_size - (sizeof(IUSB_HID_PACKET) - 1);
		//encrypted_data = (unsigned char*)(&hid_pkt[0]+sizeof(IUSB_HID_PACKET)-1);
		blowfishDecryptPacket ((char *) (&hid_pkt [0] + sizeof(IUSB_HID_PACKET) - 1), encrypted_data_size, p_si->h_encrypt);

	}

	//send_to_hid_server (hid_pkt, p_hdr->pkt_size);
	  if( p_si->kvm_priv == KVM_REQ_ALLOWED )
	    {
		hid_process_packet(p_si->socket, hid_pkt);
	    }

	return 0;
}


static int
set_bandwidth (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	u32		bandwidth;
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	/* Read the bandwidth */
	if (0 != recv_bytes (p_si->socket, (u8*) &bandwidth, sizeof (u32), p_si))
	{
		return -1;
	}

	bandwidth = htonlEx (bandwidth);
	TINFO_4 ("Received Bandwith Value to Set: %ld \n", bandwidth);

	set_session_bw (p_si->h_session, bandwidth);
	return 0;
}


static int
set_fps (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	u8		fps;
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	/* Read the quality */
	if (0 != recv_bytes (p_si->socket, (u8*) &fps, sizeof (u8), p_si))
	{
		return -1;
	}

	TINFO_4 ("FPS to set:%d\n", fps);
	set_session_fps (p_si->h_session, fps);
	return 0;
}


static int
pause_redirection (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	sm_pause_redirection (p_si->h_session,p_hdr->status);//The pause status is 1 when the H5viewer window is minimized
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	return 0;
}


static int
refresh_video_screen (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	TINFO_4 ("Refresh Video Packet reached\n");
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	sm_set_refresh_on (p_si->h_session);
	return 0;
}


static int
resume_redirection (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	sm_resume_redirection (p_si->h_session);
	resume_video();
	//send the configuration updates to the client when it resumes redirection.
	handleconfupdate();
	return 0;
}


static int
on_set_compression_type (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	u8 compression_type;

	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	/* Read the Compression Type */
	if (0 != recv_bytes (p_si->socket, &compression_type, sizeof (u8), p_si))
	{
		return -1;
	}

	TINFO_4 ("Set Compression type packet:%d\n", compression_type);

	if ( m_compression_type != compression_type ) 
	{
		qlzw_init();
		m_compression_type = compression_type;
	}
	return 0;
}


/**
 * JViewer sends a stop command when closed normally by either calling exit from menu or using the X in title
 * Under abnormal conditions, the read end identifies the socket closure
**/
static int 
on_stop_command (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	struct timespec sleep = {0, SLEEP_TIME_1};
        acquire_mutex (p_si->p_access_guard);
    	if(0)
    	{
    		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
    	}
		if (p_si->is_used)
		{
			p_si->clean = TRUE;
			if(g_corefeatures.kvm_reconnect_support == ENABLED)
			{
				p_si->disconnected_clean = true;
			}
		}
        release_mutex (p_si->p_access_guard);

        nanosleep(&sleep, NULL);
        sm_clean_sessions();

        return 0;
}


static int
on_enable_encryption (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	TINFO_4 ("Enable encryption\n");
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	sm_set_encrypt_flag (p_si->h_session);

	return 0;
}


static int
on_disable_encryption (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	TINFO_4 ("Disable encryption\n");
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	sm_reset_encrypt_flag (p_si->h_session);

	return 0;
}


static int
on_bw_request (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	u32		bandwidth_req;
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	/* Read the bandwidth */
	if (0 != recv_bytes (p_si->socket, (u8*) &bandwidth_req, sizeof (u32), p_si))
	{
		return -1;
	}

	bandwidth_req = htonlEx (bandwidth_req);
	sm_set_send_auto_bw_detect (p_si->h_session, bandwidth_req);
	return 0;
}



static int
on_validate_video_session (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	
	session_info_t		*first_session=NULL;


	char user_name[TOKENCFG_MAX_USERNAME_LEN];
	char client_ip[TOKENCFG_MAX_CLIENTIP_LEN];
	unsigned int user_priv = 0;
#ifndef JVIEWER_DEBUG
	int ret;
#endif

	auth_t  auth_pkt;
	u8	*pTemp;
	int num_session = 0;
	int num_pend_session = 0;
	usrpriv_t usrpriv;
	stoken_resp_t		session_resp;
	ivtp_hdr_t		hdr;
	int retval;
	BOOL master_reconn = FALSE;

	memset(&auth_pkt , 0 ,sizeof(auth_pkt));
	pTemp = (u8*)&auth_pkt;
	memcpy( pTemp, p_hdr, sizeof(ivtp_hdr_t) );
    auth_pkt.hdr.status=p_hdr->status; //1 if the session is h5viewer, 0 if the session is Jviewer.
	p_hdr->pkt_size = htonlEx (p_hdr->pkt_size);

	if ((p_hdr->pkt_size) >sizeof(auth_t))
	{
		TCRIT("This doesn't seem to be our Token Packet. Ignoring this packet\n");
		return -1;
	}

	/* receive the session token */
	if (0 != recv_bytes (p_si->socket,( u8* ) &pTemp[sizeof(ivtp_hdr_t)],sizeof(auth_t)- sizeof(ivtp_hdr_t),p_si))
	{
		return -1;
	}

	num_session = sm_get_kvm_num_session();
	/*Send MAX_SESSION_REACHED if num_session exceeds KVM max session*/
	if(num_session > kvm_max_session)
	{
		send_session_approval(p_si->socket,FALSE,0);
		acquire_mutex (p_si->p_access_guard);
		p_si->clean = TRUE;
		if(g_corefeatures.kvm_reconnect_support == ENABLED)
		{
			p_si->disconnected_clean = true;
		}
		release_mutex (p_si->p_access_guard);
		return 0;
	}

#ifndef JVIEWER_DEBUG

	/* validate the session token */
	ret = ValidateToken(auth_pkt.authtype.token_creds.session_token, &user_priv,(unsigned char *)user_name,(unsigned char *)client_ip,VIDEO_AUTHENTICATE);
	if (ret != 0 )
	{

		TWARN ("Session invalid: sending invalid Video session response\n");
		/* send validate session response */
		session_resp.hdr.type 		= IVTP_VALIDATE_VIDEO_SESSION_RESPONSE;
		session_resp.hdr.pkt_size 	= sizeof (u8);
		session_resp.hdr.status 	= 0;
		session_resp.sresp			= INVALID_SESSION;
		if ( -1 == send (p_si->socket, &session_resp, sizeof (stoken_resp_t), MSG_NOSIGNAL) )
		{
			TCRIT("Failed sending Invalid Video session response");
		}
		if(g_corefeatures.kvm_reconnect_support == ENABLED)
		{
			p_si->disconnected_clean = true;
		}

		return -1;
	}
#endif
	/* get the user privilege */
	if( GetUsrPriv(user_name, &usrpriv) == 0 )
	{
		/*if extended privilege is enabled check if kvm is enabled*/
		if(g_corefeatures.extended_privilege == ENABLED)
		{
			if(!IS_KVM_ENABLED(usrpriv.Extendedprivilege))
			{
				TWARN ("Session invalid: sending Not Sufficient priviliege response\n");
				/* send validate session response */
				session_resp.hdr.type 		= IVTP_VALIDATE_VIDEO_SESSION_RESPONSE;
				session_resp.hdr.pkt_size 	= sizeof (u8);
				session_resp.hdr.status 	= 0;
				session_resp.sresp			= NOT_SUFFICIENT_PRIV;
				kvm_loginLogoutAudit(user_name, client_ip, SEL_LOGIN_FAILED);
				if ( -1 == send (p_si->socket, &session_resp, sizeof (stoken_resp_t), MSG_NOSIGNAL) )
				{
					TCRIT("Failed sending Invalid Video session response");
				}
				return -1;
			}
		}
		/*assign the ipmi privilege so that it can be sent with user data*/
		/*ipmi privilege should be sent irrespective of extended privilege option, so the ipmipriv assignment statement has been moved here*/
		p_si->ipmiprev = usrpriv.ipmi.lanpriv;	
	}
// Send session valid response 
	/*session_resp.hdr.type 		= IVTP_VALIDATE_VIDEO_SESSION_RESPONSE;
	session_resp.hdr.pkt_size 	= sizeof (u8);
	session_resp.hdr.status 	= 0;
	session_resp.sresp			= VALID_SESSION;
	if ( -1 == send (p_si->socket, &session_resp, sizeof (stoken_resp_t), MSG_NOSIGNAL) )
	{
		TCRIT("Failed sending valid Video session response");
		return -1;
	}*/

	TINFO_4 ("Video Session valid\n");

	ret=snprintf(p_si->ip_addr_client,TOKENCFG_MAX_CLIENTIP_LEN,"%s",client_ip);
	if(ret>=TOKENCFG_MAX_CLIENTIP_LEN)
	{
		TCRIT("Buffer overflow\n");
		
	}
	ret=snprintf(p_si->username,TOKENCFG_MAX_USERNAME_LEN,"%s",user_name);
	if(ret>=TOKENCFG_MAX_USERNAME_LEN)
	{
		TCRIT("Buffer overflow\n");
		
	}
	#ifdef JVIEWER_DEBUG
	char ipdaar[TOKENCFG_MAX_CLIENTIP_LEN]="10.0.3.45";
	char username[TOKENCFG_MAX_USERNAME_LEN]="SAMPLE2";
			
	ret=snprintf(p_si->ip_addr_client,TOKENCFG_MAX_CLIENTIP_LEN,"%s",ipdaar);	
	if(ret>=TOKENCFG_MAX_CLIENTIP_LEN)
	{
		TCRIT("Buffer overflow\n");
	}
	ret=snprintf(p_si->username,TOKENCFG_MAX_USERNAME_LEN,"%s",username);
	if(ret>=TOKENCFG_MAX_USERNAME_LEN)
	{
		TCRIT("Buffer overflow\n");
		
	}	
	#endif
	#ifndef JVIEWER_DEBUG
	ret=snprintf(p_si->ip_addr_client,sizeof(p_si->ip_addr_client),"%s",auth_pkt.ipaddr_client);
	if(ret>=(signed int)sizeof(p_si->ip_addr_client))
	{
		TCRIT("Buffer overflow\n");
	}
	ret=snprintf(p_si->username,TOKENCFG_MAX_USERNAME_LEN,"%s",user_name);
	if(ret>=TOKENCFG_MAX_USERNAME_LEN)
	{
		TCRIT("Buffer overflow\n");
		
	}
	#endif
	ret=snprintf(p_si->client_uname,sizeof(p_si->ip_addr_client),"%s",auth_pkt.client_uname);
	if(ret>=(signed int)sizeof(p_si->ip_addr_client))
	{
		TCRIT("Buffer overflow\n");
		
	}
	ret=snprintf(p_si->mac_addr,TOKENCFG_MAX_MAC_LEN,"%s",auth_pkt.mac_addr);
	if(ret>=TOKENCFG_MAX_MAC_LEN)
	{
		TCRIT("Buffer overflow\n");
		
	}
	first_session = sm_get_master_session();
	num_pend_session = sm_get_num_pending_sessions();

	if ( first_session != NULL )
	{
		master_reconn = sm_is_session_reconn(first_session->h_session);

		//Handle only one full permission request  at a time. if second request comes send partial permission.
		// if master is reconnecting send partial permission to requesting session
		if((num_pend_session > 0) || (master_reconn == TRUE))
		{
			hdr.type = IVTP_KVM_SHARING;
			hdr.pkt_size=0;

			//if master session is in reconnect state
			hdr.status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + (((master_reconn == TRUE)? KVM_REQ_MASTER_RECONN : KVM_REQ_PROGRESS)<<8) );
			if( send(p_si->socket, &hdr, sizeof(ivtp_hdr_t), MSG_NOSIGNAL) == -1 )
			{
				TCRIT("Error while sending KVM Sharing pkt to remote client.");
				//return -1;
			}
			retval=approve_session(p_si,&auth_pkt,(char *)user_name,(char *)client_ip, KVM_REQ_PARTIAL);
			if(retval!=0)
			{
				TCRIT("Error in approve_session function\n");
				return -1;
			}
			return 0;
		}
	}
	
	 if( first_session != NULL )
	{
		// If second session, send req to first session and put second session till
		// there is a resp from first session
		// there is already a first session running.
		// check the return value...
		#ifdef JVIEWER_DEBUG
		char username_other_session[TOKENCFG_MAX_USERNAME_LEN-1]="SAMPLE";
		char username1[TOKENCFG_MAX_USERNAME_LEN-1]="SAMPLEUSERNAME";
		
		ret=snprintf(p_si->username_other_session,TOKENCFG_MAX_USERNAME_LEN-1,"%s",username_other_session);
		if(ret>=TOKENCFG_MAX_USERNAME_LEN-1)
		{
			TCRIT("Buffer overflow\n");
			
		}
		ret=snprintf(first_session->username_other_session,TOKENCFG_MAX_USERNAME_LEN-1,"%s",username1);
		if(ret>=TOKENCFG_MAX_USERNAME_LEN-1)
		{
			TCRIT("Buffer overflow\n");
			
		}
		#endif
		#ifndef JVIEWER_DEBUG	
		ret=snprintf(p_si->username_other_session,TOKENCFG_MAX_USERNAME_LEN,"%s",first_session->username);
		if(ret>=TOKENCFG_MAX_USERNAME_LEN)
		{
			TCRIT("Buffer overflow\n");
			
		}
		ret=snprintf(first_session->username_other_session,TOKENCFG_MAX_USERNAME_LEN,"%s",p_si->username);	
		if(ret>=TOKENCFG_MAX_USERNAME_LEN)
		{
			TCRIT("Buffer overflow\n");
			
		}
		#endif
			
		ret=snprintf(p_si->ip_addr_othersession,TOKENCFG_MAX_CLIENTIP_LEN+1,"%s",first_session->ip_addr_client);
		if(ret>=TOKENCFG_MAX_CLIENTIP_LEN+1)
		{
			TCRIT("Buffer overflow\n");
			
		}
		ret=snprintf(first_session->ip_addr_othersession,TOKENCFG_MAX_CLIENTIP_LEN,"%s",p_si->ip_addr_client);
		if(ret>=TOKENCFG_MAX_CLIENTIP_LEN)
		{
			TCRIT("Buffer overflow\n");
		}
		first_session->slave_index = p_si->h_session;
		p_si->kvm_priv = KVM_REQ_PROGRESS;

		//Requestion master session for KVM permission sharing
		if( send_req_kvm_wait(p_si,IVTP_KVM_SHARING) == 0 )
		{
			send_req_kvm_priv(first_session,IVTP_KVM_SHARING); //to first session
			
			sm_add_pending_session(p_si,(unsigned char * )user_name,(unsigned char * )client_ip,&auth_pkt);
			return 0;
		}
		else
		{
			//by returning -1, we are closing the socket.
			return -1;
		}
	}
	else
	{
		retval=approve_session(p_si,&auth_pkt,(char *)user_name,(char *)client_ip, KVM_REQ_ALLOWED);
		if(retval!=0)
		{
			TCRIT("Error in approve_session function\n");
			return -1;
		}
		sm_set_master_change_flag(p_si);
	}
    
	return 0;
}


static int 
on_kvm_priv_pkt(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{	
	ip_usr_datapkt userpkt;	
	//KVM will send session information . receive it.
	p_hdr->pkt_size = htonlEx (p_hdr->pkt_size);
	if (((p_hdr->pkt_size) >sizeof(ip_usr_datapkt))  && (p_hdr->pkt_size >0))
	{
		printf("\nThis doesn't seem to be our  Packet. Ignoring this packet\n");
		return -1;
	}

	/* receive the session info */
	if (0 != recv_bytes (p_si->socket,( u8* ) &userpkt,p_hdr->pkt_size,p_si))
	{
		return -1;
	}

	p_hdr->status= htonlEx (p_hdr->status);

	userpkt.sess_index = htonlEx(userpkt.sess_index);
	sm_kvm_priv_take_action(p_si, p_hdr->status>>8,userpkt.sess_index);
	//If Master permission switches. pending sessions hould be notified and give partial permission
	if((p_hdr->status>>8)== KVM_REQ_ALLOWED)
	{
		//set the flag to respond waiting pending sessions
		sm_set_pending_client_response();
	}

	return 0;
}
//////////////////////////
//
//approve_session
//
//////////////////////////
int  approve_session(session_info_t* p_si,  auth_t *auth_pkt, char *user_name,char *client_ip, unsigned char kvm_priv)
{
	int webpreview_num = 0;
	usb_mouse_mode_t	pkt_mouse_mode;
	void *session_handle = NULL;
	session_list_t	 session_list, send_list;
	int (*sess_ctrl) (session_info_t*, u8, u8);
	ivtp_hdr_t		v_hdr;
	usb_keybd_led_t 	pkt_keybd_led;
	//int 				add_sel;
	stoken_resp_t		session_resp;
	int autohostlock =0;
	u8 devicestatus=0;
	unsigned int	racsession_id;
	int UserId=0;
	char *p;
	
	
	session_resp.hdr.type 		= IVTP_VALIDATE_VIDEO_SESSION_RESPONSE;
	session_resp.hdr.pkt_size 	= sizeof (stoken_resp_t) - sizeof (ivtp_hdr_t);
	session_resp.hdr.status 	= 0;
	session_resp.sresp			= VALID_SESSION;
	session_resp.sindex 		= p_si->h_session;
	p_si->is_h5viewer            =auth_pkt->hdr.status;//1 if the session is h5viewer, 0 if the session is Jviewer
	if(0)
	{
		client_ip=client_ip; /* -Wextra, fix for unused parameters */
	}
	if(send (p_si->socket, &session_resp, sizeof (stoken_resp_t), MSG_NOSIGNAL)<0)
	{
		TCRIT("error in sending the data\n");
		return -1;
	}
			TINFO_4 ("Video Session valid\n");

        /* Gets the Keyboard LED Status from the Host
         * Sending the status everytime when a new session is validating*/
	if(GetIUSBLEDPkt(&(pkt_keybd_led.keybd_led), 1) == -1)/* get the keyboard LED status and return immediately */
		pkt_keybd_led.keybd_led = 0;

	/* Send Keyboard LED Status packet in the beginning */
        pkt_keybd_led.hdr.type = IVTP_GET_KEYBD_LED;
        pkt_keybd_led.hdr.pkt_size = 1;
        pkt_keybd_led.hdr.status = 0;
        if(send(p_si->socket, (const void *) &pkt_keybd_led, sizeof(pkt_keybd_led), MSG_NOSIGNAL)<0)
        {
        	TCRIT("error in sending the data\n");
        	return -1;
        }

	/**
	 * Initialize the Mouse mode 
	 * This will be done everytime when starting JViewer
	 * After validating the Video, Mouse mode will be obtained
	**/
	init_usb_mouse_mode();

	/* Send Mouse mode packet in the begining */
	pkt_mouse_mode.hdr.type 		= IVTP_GET_USB_MOUSE_MODE;
	pkt_mouse_mode.hdr.pkt_size 		= 1;
	pkt_mouse_mode.hdr.status 		= g_err_mouse_mode_init;
	pkt_mouse_mode.mouse_mode 		= g_mouse_mode;
	if(send (p_si->socket, (const void*) &pkt_mouse_mode, sizeof (pkt_mouse_mode), MSG_NOSIGNAL)<0)
	{
	      TCRIT("error in sending the data\n");
	      return -1;
	}

	/* Posts the semaphore waiting in avse_main() - Only if it is first session */
	sm_get_session_list (&session_list, FALSE);

	if(g_corefeatures.host_auto_lock == ENABLED)
	{
		// use runtime cfg value and always auto off is configured
		if (OnGethostlockfeature_dynm(&autohostlock) != 0)
			autohostlock = 0;     // Failed
		autohostlock &= (1 << 1); // AutoOff is Bit 1

		// when AutoOff is set, turn off on session start, 
		// not only for the first session
		if(autohostlock)
		{
			if(g_host_is_locked == 0)
				display_control (1);
		}
	}
	/* Send Encryption status */
	if (is_any_session_encrypted ())
	{
		sm_set_encrypt_flag(p_si->h_session);
		TINFO_4  ("Session with encryption in existence\n");

		v_hdr.type 		= IVTP_INITIAL_ENCRYPTION_STATUS;
		v_hdr.pkt_size 	= 0;
		v_hdr.status 	= 0;

		TINFO_4 ("About to send Initial encrypt status to clients\n");
		if(send (p_si->socket, (const void*) &v_hdr, sizeof (v_hdr), MSG_NOSIGNAL)<0)
		{
		     TCRIT("error in sending the data\n");
		     return -1;
		}
	}

        //send Media License status
        v_hdr.type              = IVTP_MEDIA_LICENSE_STATUS;
        v_hdr.pkt_size  = 0;
        v_hdr.status    = g_media_license_status;

        TINFO_4 ("About to send Media License status to clients\n");
        send (p_si->socket, (const void*) &v_hdr, sizeof (v_hdr), MSG_NOSIGNAL);

	/* assign token to session info structure */
	sm_grant_session (p_si->h_session);
	sm_set_sessiontoken (p_si->h_session,auth_pkt->authtype.token_creds.session_token);

	//If master session is not available. incoming new session will become master
	if( ( sm_get_master_session() ==  NULL)  && (p_si->kvm_priv != KVM_REQ_PROGRESS))
	{
		sm_set_master_sess_index(p_si->h_session);
	}
	p_si->kvm_priv = kvm_priv;
	/* Posts the semaphore waiting in avse_main() - Only if it is first session */
	sm_get_session_list (&session_list, FALSE);

	webpreview_num = sm_get_num_webpreview_sessions();

	// Opens the provided library for access
	session_handle = dlopen(VIDEO_HOOKS_PATH, RTLD_LAZY);
	
	// This is calling the hook every time a new session starts
	if (!session_handle)
	{
		TCRIT("%s\n",dlerror());
		return -1;
	}
	
		sess_ctrl = dlsym(session_handle, START_STOP_SSN);	
		if (sess_ctrl)
		{
			sess_ctrl (p_si, (u8)(session_list.num_session), START_SESSION_HOOK);
			kvm_loginLogoutAudit(p_si->username, p_si->ip_addr_client, SEL_LOGIN_SUCCESS);
		}

    /* truncating the scope id of the link local address */
    if ((p=strchr(p_si->ip_addr_client,'%'))!= NULL)
    memset(p, 0 ,((TOKENCFG_MAX_CLIENTIP_LEN+1)-(p-p_si->ip_addr_client)));
	
	if(g_corefeatures.session_management == ENABLED)
	{
		UserId = MapUid_to_SELUid (user_name, NULL);
		racsessinfo_register_session(SESSION_TYPE_VKVM,p_si->ip_addr_client,strlen(p_si->ip_addr_client),CONSOLE_TYPE_VKVM,p_si->username,strlen(p_si->username),
							p_si->ipmiprev,UserId,&racsession_id);
		p_si->racsession_id=racsession_id;
	}
	if (session_list.num_session == 1 || ((session_list.num_session - webpreview_num) == 1) )
	{
		if(g_corefeatures.power_consumption_virtual_device_usb == ENABLED)
		{
			if (0 != do_usb_ioctl(USB_GET_ALL_DEVICE_STATUS, &devicestatus) )
			{
				TWARN("Adviserd : %s : Unable to Enable the USB Virtual device ", __FUNCTION__ );
			}
			if(devicestatus == 0)
			{
				if (0 != do_usb_ioctl(USB_ENABLE_ALL_DEVICE, NULL) )
				{
					TWARN("Adviserd : %s : Unable to Enable the USB Virtual device ", __FUNCTION__ );
				}
			}
		}
		sem_post(&m_start_avse);
	}

	if(g_corefeatures.session_management != ENABLED)
	{
		if(set_active_session(KVM_SERVICE_ID_BIT,session_list.num_session) != 0)
		{
			TCRIT("Adviser:Unable to get the active session count");
		}
	}

	/* Init Blowfish */
	TINFO("Adviserd:Initializing Encryption Handle\n");
	p_si->h_encrypt = GetEncryptionHandleFromKey (p_si->encrypt_key, HASH_SIZE);

	// Send the Services information from ncml to all the sessions
	if (sm_get_services_info (&session_list, &send_list, service_info_data) != 0)
	{
		TINFO ("Unable to fetch/send the configuration data to the clients\n");
	}

	sm_set_active_client_send();
	
	if(g_corefeatures.session_management && g_corefeatures.vmedia_dynamic_free_instance_update == ENABLED)
	{
		sm_set_vmedia_update_status();
	}
	if(session_handle)
		dlclose(session_handle);
		return 0;
}


int 
send_req_kvm_wait(session_info_t* p_si,unsigned short pkttype)
{
	kvm_priv_ctrl_pkt_t hdr;

	hdr.hdr.type=pkttype ;

	hdr.hdr.pkt_size=sizeof(kvm_priv_ctrl_pkt_t)-sizeof(ivtp_hdr_t) ; 
	hdr.hdr.status = STATUS_KVM_PRIV_WAIT_USER2;

	memset(hdr.username_othersession,0,sizeof(hdr.username_othersession));
	memset(hdr.ip_add_othersession,0,sizeof(hdr.ip_add_othersession));

	strncpy(hdr.username_othersession,p_si->username_other_session,TOKENCFG_MAX_USERNAME_LEN-1);
	strncpy(hdr.ip_add_othersession,p_si->ip_addr_othersession,TOKENCFG_MAX_CLIENTIP_LEN);
	hdr.sess_index = (char)p_si->h_session;
	hdr.ipmi_priv = p_si->ipmiprev;

	if( send(p_si->socket, &hdr, sizeof(kvm_priv_ctrl_pkt_t), MSG_NOSIGNAL) == -1 )
	{
		TCRIT("Error while sending WAIT to remote client.");
		return -1;
	}

	return 0;
}


int 
send_req_kvm_priv(session_info_t * first_session,int pkttype)
{
	first_session->KVM_pkt = pkttype;
	sm_set_kvm_priv_flag(first_session, STATUS_KVM_PRIV_REQ_USER1);
	return 0;

}


static int 
on_send_keyboard_LED(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	usb_keybd_led_t pkt_keybd_led;
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
		p_si=p_si;
	}
	if(GetIUSBLEDPkt(&(pkt_keybd_led.keybd_led), 1) == -1)/* get the keyboard LED status and return immediately */
		pkt_keybd_led.keybd_led = 0;
	return 0;
}

static int 
on_get_web_token (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	unsigned int 	Length = ((2 * HASH_SIZE) + 3);
	char	stoken [Length];
	int num_session = 0;

	num_session = sm_get_kvm_num_session(); 
	memset(stoken,0,Length);

	if(num_session > kvm_max_session)
	{
		/*
 		 * Max Session reached
		 * Hence, sending a closure command to the client
 		 * to disconnect the connection
		 **/
		send_session_approval(p_si->socket, FALSE,0);
		acquire_mutex (p_si->p_access_guard);
		p_si->clean = TRUE;
		if(g_corefeatures.kvm_reconnect_support == ENABLED)
		{
			p_si->disconnected_clean = true;
		}
		release_mutex (p_si->p_access_guard);
		return -1;
	}
        p_hdr->pkt_size = htonlEx (p_hdr->pkt_size);
	if ((p_hdr->pkt_size) > Length)
	{
		TCRIT("This doesn't seem to be our Token Packet. Ignoring this packet\n");
		return -1;
	}

        /* receive the session token */
        if (0 != recv_bytes (p_si->socket, (unsigned char *)(&stoken[0]), p_hdr->pkt_size, p_si))
        {
                TWARN ("Unable to receive the CDROM Token to validate\n");
                return -1;
        }

	acquire_mutex (p_si->p_access_guard);
	memcpy(p_si->websession_token, &stoken[0], Length);
	release_mutex (p_si->p_access_guard);

	if ( -1 == WriteSessionToken(p_si, SET) )
	{
		TCRIT("Unable to write the received session token to the GoAhead Pipe\n");
		return -1;
	}

	/* Verify for web-session closure so as to close the KVM Client also */
	check_if_logout();

	return 0;
}

/**
 * @fn on_get_user_macro
 * on_get_user_macro - gets user info which session holds macro's
**/
static int 
on_get_user_macro(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	sm_user_macro(p_si->h_session);
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	return (0);
}

/**
 * @fn on_set_user_macro
 * on_set_user_macro - Writes user macro's into file
**/
static int 
on_set_user_macro(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	FILE *fp = NULL;
	user_macro UserMacro;
	unsigned char *pTemp;
	int write_ret=0;
	
	fp = fopen(USER_MACRO_FILE,"wb+");
	if(fp == NULL)
	{
		TCRIT("ERROR: Cannot open USER_MACRO_FILE for write mode!\n");
		return 0;
	}
	else
	{  
		memset(&(UserMacro.UserDefMacro),0,sizeof(UserMacro.UserDefMacro));
		pTemp = (unsigned char *)&UserMacro;
		memcpy( pTemp, p_hdr, sizeof(ivtp_hdr_t) );
		/* Read the User macro */
		if (0 != recv_bytes (p_si->socket,(u8*) &pTemp[sizeof(ivtp_hdr_t)], sizeof(user_macro) - sizeof(ivtp_hdr_t), p_si))
		{
			TCRIT("Error receiving USer Macro packet\n");
			fclose(fp);
			return -1;
		}
	
		pTemp = pTemp+7;//moving the pointer to data portion		
		write_ret = fwrite(&(UserMacro.UserDefMacro),1,sizeof(UserMacro.UserDefMacro),fp);
		if(write_ret == 0)
		{
			TCRIT("Error in  Writing the  Macro packet\n");
		}
		fclose(fp);
	}
	return (0);
}


static int 
on_send_full_screen 	(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	g_send_full_screen = 1;
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
		p_si=p_si;
	}
	return 0;
}


/**
 * on_media_state
 * Indicates to the session_info if that session holds any active media redirection
**/
static int 
on_media_state (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	if (p_hdr->status == 1)
	{
		p_si->media_active = 1;
		//reset kvm timer as media redirection is started.
		sm_reset_kvm_timer(p_si->h_session);
	}
	else
		p_si->media_active = 0;

	return 0;
}

/**
 * on_power_staus
 * Get the power status of the Host
**/
static int 
on_power_status (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	int wRet=0;
	uint8  privLevel = PRIV_LEVEL_ADMIN;
	chassis_status_T status;
	IPMI20_SESSION_T IPMISession;
	memset(&IPMISession, 0x0, sizeof(IPMISession));

	 wRet = LIBIPMI_Create_IPMI_Local_Session(&IPMISession,
                                                 "",
                                                 "",
                                                 &privLevel,NULL,
                                                 AUTH_BYPASS_FLAG,//AUTHFLAG
                                                 TIMEOUT);//SOL_SESSION_timeout

        if ( LIBIPMI_E_SUCCESS != wRet )
        {
		printf("Failed to create IPMI Session !!!%s\n",strerror(errno));
                return errno;
        }
	wRet = LIBIPMI_HL_GetChassisStatus( &IPMISession,  2, &status );
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	if( wRet != LIBIPMI_E_SUCCESS )
	    TCRIT("Unable to get the power status\n");
	LIBIPMI_CloseSession(&IPMISession);
	sm_set_power_status(status.power_state);
	//if power status received is power OFF, then, reset the LED status.
	if(status.power_state == 0)
		ResetKbdLEDStatus();
	sm_set_power_status_send(p_si->h_session);
	
		
	return 0;
}
/**
 * on_power_ctrl
 * Set the power status of the Host
**/
static int 
on_power_ctrl (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	
	int retval=0;
	int powerctrl =0;
	int wRet=0;
	uint8  privLevel = p_si->ipmiprev;
	IPMI20_SESSION_T IPMISession;
	// Post power control OEM handling
	void *video_handle = NULL;
	int (*afterPowerCtrl) (session_info_t*, u8);


	//Discard power control command from invalid sessions
	if(!p_si->is_valid || !p_si->is_used)
	{
		TDBG("\nPower control command from invalid session !!!\n");
		return 0;
	}
	memset(&IPMISession, 0x0, sizeof(IPMISession));

	 wRet = LIBIPMI_Create_IPMI_Local_Session(&IPMISession,
                                                 "",
                                                 "",
                                                 &privLevel,NULL,
                                                 AUTH_BYPASS_FLAG,//AUTHFLAG
                                                 TIMEOUT);//SOL_SESSION_timeout

        if (( LIBIPMI_E_SUCCESS != wRet ))
        {
        	printf("Failed to create IPMI Session !!!%s\n",strerror(errno));
        	retval=-1;
        	sm_set_power_crl_resp (p_si->h_session,retval);
        	sm_set_power_crl_resp_send(p_si->h_session);
        	return -1;
        }
	powerctrl = p_hdr->status;
	 switch( p_hdr->status)
    	{
        case 0:
            /* Power off the host */
            retval = LIBIPMI_HL_PowerOff( &IPMISession, POWERCTL_DEFAULT_TIMEOUT );
            break;

        case 1:
            /* Power on the host */
            retval = LIBIPMI_HL_PowerUp( &IPMISession, POWERCTL_DEFAULT_TIMEOUT );
            break;

        case 2:
            /* Power cycle the host */
        	retval = LIBIPMI_HL_PowerCycle( &IPMISession, POWERCTL_DEFAULT_TIMEOUT );
            break;

        case 3:
        	/* Hard Reste Host */
        	retval = LIBIPMI_HL_HardReset( &IPMISession, POWERCTL_DEFAULT_TIMEOUT );
        	break;

        case 4:
        	/* Diag Interrupt / NMI */
        	retval = LIBIPMI_HL_DiagInt( &IPMISession, POWERCTL_DEFAULT_TIMEOUT );
        	break;

        case 5:
        	/* Soft Power Off */
            retval = LIBIPMI_HL_SoftOff( &IPMISession, POWERCTL_DEFAULT_TIMEOUT );
            break;


        default:
            /* Unknown power command */
            retval = -1;
            TWARN( "Unknown power command %d\n", powerctrl );
            break;
   	}

	LIBIPMI_CloseSession(&IPMISession);
	sm_set_power_crl_resp (p_si->h_session,retval);
	sm_set_power_crl_resp_send(p_si->h_session);
	
	// Post power control OEM handling
	if(retval == 0){
	    video_handle = dlopen(VIDEO_HOOKS_PATH, RTLD_LAZY);
	    
	    if(video_handle)
	    {
	        afterPowerCtrl = dlsym(video_handle, POST_POWER_CONTROL);
	        if (afterPowerCtrl)
	        {
	            afterPowerCtrl (p_si, (u8)powerctrl);
	            dlclose(video_handle);
	        }
			else
			{
				dlclose(video_handle);
			}
	    }
	}
	return 0;
}
//Generic API to call raw IPMI Cmd from Adviser
int SendRawIPMICmd( INT8U NetFn, INT8U CmdNo, INT8U *pReqData, INT32U ReqDataLen, INT8U *pRes, uint32 *dwResLen, INT8U ipmipriv )
{
    IPMI20_SESSION_T hSession;
    uint16 wRet =  -1;

    memset((void*)&hSession, 0x0, sizeof(IPMI20_SESSION_T) );

    wRet=LIBIPMI_Create_IPMI_Local_Session(&hSession,"","",
                                          &ipmipriv,NULL,AUTH_BYPASS_FLAG,5);
    if( wRet != LIBIPMI_E_SUCCESS )
    {
        TWARN( "Error while establishing the session in adviserd::%x with privilege :: %x\n", wRet, ipmipriv);
        return wRet;
    }
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(&hSession,0 ,(NetFn << 2), CmdNo, pReqData, ReqDataLen, pRes, dwResLen,5);
    LIBIPMI_CloseSession(&hSession);
    return wRet;
}
//#endif
/**
 * @fn on_send_raw_ipmi
 ** on_send_raw_ipmi - send the raw ipmi commmand
**/
static int 
on_send_raw_ipmi(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
		
		//maximum request length FF FF FF FF for extended command handler
		//maximum request length FF for 
	
		char *rawbuffer=NULL;
		uint16 wRet =  -1;
		uint8 byNetFnLUN;
		uint8 byCommand;
		uint8 sequenceindex=0;
		char CmdBytesBuf[512]={0};
		char szResponseBuf[2048]={0};
		uint32 dwResLen;
		uint32 dwReqLen;
		uint8 ipmiprev = PRIV_LEVEL_ADMIN;
		unsigned int i=0;
		
		//Discard raw IPMI command from invalid sessions
		if(!p_si->is_valid || !p_si->is_used)
		{
			TDBG("\nRaw IPMI command from invalid session !!!\n");
			return 0;
		}

		p_hdr->pkt_size = htonlEx (p_hdr->pkt_size);
		rawbuffer = malloc(p_hdr->pkt_size);
		if(rawbuffer == NULL)
		{
			return 0;
		}

		if(p_hdr->pkt_size <= 0)
		{
			printf("Invalid Raw IPMI commands:\n");
		}
		/* receive the session token */
		if (0 != recv_bytes (p_si->socket,( u8* ) rawbuffer,p_hdr->pkt_size,p_si))
		{
			TCRIT("Not able to receive the IPMI request data in Adviser\n");
			if(rawbuffer != NULL)
			{
				free(rawbuffer);
				rawbuffer=NULL;
			}
			return 0;
		}
		// if max limit is reached then discard the ipmi commands.
		if(p_si->send_raw_ipmi_data_count >= ( MAX_IPMI_COMMAND_LIMIT ))
		{
		    TCRIT("\n IPMI_COMMAND_LIMIT = %d is reached. Try after sometime. \n", MAX_IPMI_COMMAND_LIMIT);
			if(rawbuffer != NULL)
			{
				free(rawbuffer);
				rawbuffer=NULL;
			}
			return 0;
		}
		
		// if limit is reached then for remaining commands (MAX_IPMI_COMMAND_LIMIT - IPMI_COMMAND_LIMIT) 
		// we should send try again.
		if(p_si->send_raw_ipmi_data_count >= IPMI_COMMAND_LIMIT)
		{
			wRet = TRY_AFTER_SOMETIME;
			p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].dwresplength = 0;
			goto EXIT;
		}
		ipmiprev = p_si->ipmiprev;
		sequenceindex = *rawbuffer;
		p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].sequenceid = sequenceindex;
		
		byNetFnLUN = *(rawbuffer + 1);
		byCommand =  *(rawbuffer + 2);
		dwReqLen = p_hdr->pkt_size - 3;
		if(sizeof(CmdBytesBuf) <= dwReqLen)
		{
			TCRIT("Buffer Overflow \n");
			if(rawbuffer != NULL)
			{
				free(rawbuffer);
				rawbuffer=NULL;
			}
			return 0;
		}
		for(i=0;i<dwReqLen;i++)
		{
			CmdBytesBuf[i] = *(rawbuffer+3+i);
		} 
        wRet = SendRawIPMICmd( byNetFnLUN, byCommand,(uint8*)&CmdBytesBuf,dwReqLen,(uint8*)&szResponseBuf, &dwResLen, ipmiprev );
		if(wRet != 0)
		{
			 goto EXIT;
		}
		
		//seting the data to send to the JViewer client
		p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].raw_cmd_sucess = wRet;
		p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].dwresplength = dwResLen;
		if(dwResLen > 0)
		{
			p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].ipmi_resp_data = malloc(dwResLen+1);
			if(p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].ipmi_resp_data == NULL)
			{
				goto EXIT;
			}
			*p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].ipmi_resp_data = sequenceindex;
			memcpy((p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].ipmi_resp_data+1),szResponseBuf,dwResLen);
		}

EXIT:
		p_si->sess_ipmi_res[p_si->send_raw_ipmi_data_count].raw_cmd_sucess = wRet;
		if(rawbuffer != NULL)
		{
			free(rawbuffer);
			rawbuffer=NULL;
		}
		sm_set_raw_ipmi_resp_send(p_si->h_session);
		return 0;
}

/**********************************************************************************/



/**
 * @fn cmdhndlr_main
 * Cmdhndlr_main - waits for socket activity, and maps appropriate call with its associated command.
**/
void
cmdhndlr_main (void)
{
	struct timeval  tv;
	session_list_t session_list;
	int retval = 0;
	fd_set socks;
	int i = 0;
	int highsock = 0;
	prctl(PR_SET_NAME,__FUNCTION__,0,0,0);
	struct timespec sleep = {0, SLEEP_TIME};
	while (1)
	{
		/* This semaphore is posted by connection mgr when new connection arrives. Until then, this thread waits */
		sem_wait (&m_start_cmdhndlr);

		while (1)
		{
			/**
			 * Get the list of sessions available from the Session Manager.
       	         	**/
			sm_get_session_list (&session_list, FALSE);

	                /* If there are no sessions - do not do anything */
	                if (session_list.num_session == 0)
       	         	{
	                	nanosleep(&sleep, NULL);
				break;
                	}

			FD_ZERO (&socks);
                	for (i = 0; i < session_list.num_session; i++)
                	{
				if (session_list.p_session [i]->is_used) {
                                        highsock = max (highsock, session_list.p_session [i]->socket);
                                        FD_SET (session_list.p_session [i]->socket, &socks);
                                }
                	}

			tv.tv_sec   = 5;
			tv.tv_usec  = 0;

			retval = select (highsock + 1, &socks, NULL, NULL, &tv);
			if (retval < 0)
			{
				if (errno == EINTR) { continue; }
				else
				{
					TCRIT ("select failed - Exiting program%d %d\n", EINTR, retval);
					perror ("select");
					return ;
				}
			}
			if (retval == 0) { continue; }

			for (i = 0; i < session_list.num_session; i++)
       		        {
       	        	        if (FD_ISSET (session_list.p_session [i]->socket, &socks))
                        	{
                                	/* This is an incoming command - handle it */
                                	if (0 != handle_video_cmd (session_list.p_session [i]))
                                	{
                                                acquire_mutex (session_list.p_session [i]->p_access_guard);
												if (session_list.p_session [i]->is_used)
												{
													session_list.p_session [i]->clean = TRUE;
													if(g_corefeatures.kvm_reconnect_support == ENABLED)
													{
														if(session_list.p_session [i]->failed_time == 0)
														{
															time(&session_list.p_session [i]->failed_time);
														}
													}
												}
                                                release_mutex (session_list.p_session [i]->p_access_guard);

                                        	sm_clean_sessions ();
                                	}
                        	}
                	}
		}
	}
}


/**
 * handle_video_cmd
**/
static int
handle_video_cmd (session_info_t* p_si)
{
	ivtp_hdr_t	hdr;
	hndlr_map_t*	p_hndlr_map;
	int ret = 0;

	/* Read the header */
	if (0 != recv_bytes (p_si->socket, (unsigned char*) &hdr, sizeof (ivtp_hdr_t), p_si))
	{
		return -1;
	}

	/* Process the command based on the packet type */
	p_hndlr_map = &m_hndlr_map [0];
	while (1)
	{
		/* Check for the end of command list */
		/* Also check, if that session is currently active and Valid. */
		/* This avoids any unvalidated session being accessing the SOC Data */
		if ((p_hndlr_map->cmd == 0xffff) && (p_si->is_used) && (p_si->is_valid)) 
		{
			//check if it is a soc-specific command
			ret = handle_soc_specific_video_cmd (p_si, &hdr);
			if (ret != 0)	
			{
				TINFO_1 ("Invalid command received - ignoring\n");
				return -1;
			} else {
				//last command received time
				sm_set_last_recv_time(p_si->h_session);
				if(p_si->activity_cnt_down < (signed int)AdviserConf.SessionInactivityTimeout)
				{
					//reset kvm timer as command is received.
					sm_reset_kvm_timer(p_si->h_session);
				}
				return 0;
			}
		}

		/* This is for Non SOC based commands being handled by Adviser */
		if (p_hndlr_map->cmd == hdr.type)
		{
			//last command received time
			sm_set_last_recv_time(p_si->h_session);
			/* keepalive packet will not be considered as normal packet for maintaining ideal timeout.
			 * Also need to add power status packet to exception list since When the host is powered off or in sleep mode, 
			 * jviewer will be querying for power status. In this case the timeout wont happen*/
			 /* fps diff packet will be sent from h5 client to adviser to notify how much packet it can process
			 * based on which the number of packets sent to h5 client will be reduced.
			 * This shouldn't affect h5 client timeout, so ignoring the time reset here.
			 */
			if((p_hndlr_map->cmd != IVTP_KEEP_ALIVE_PKT) && (p_hndlr_map->cmd != IVTP_POWER_STATUS) && (p_hndlr_map->cmd != IVTP_FPS_DIFF))
			{
				//reset kvm timer as command is received.
				sm_reset_kvm_timer(p_si->h_session);
			}
			return p_hndlr_map->hndlr (p_si, &hdr);
		}

		p_hndlr_map++;
	}
	return -1;
}

int On_write_File_web_preview(int cap_status, void *p_frame_hdr, void *p_data_buf, int data_buf_size, int frame_hdr_size)
{
	 char command[256] = {0};
	 int     fileDesc = -1;
	 char* filename = NULL; // /var folder doesn't have enough space to have this file
	       filename = WEB_PREVIEWER_CAP_FILE; 
			if(cap_status == CAPTURE_VIDEO_SUCCESS) {
				  set_webpreivewer_capture_time();
		
			  // Create the base directory for storing the Video data files
			  sprintf(command, "mkdir %s 2&>/dev/null", WEB_PREVIEWER_CAP_DIRECTORY);
			  if(system(command)<0)
			  {
				  TCRIT("ERROR:System call fails in %s file\n",command);
				  return -1;
			  }
			 			  
			  sprintf(command, "rm %s 2&>/dev/null", WEB_PREVIEWER_CAP_FILE);
			  if(system(command)<0)
			  {
				  TCRIT("ERROR:System call fails in %s file\n",command);
				  return -1;
			  }
			  
			  fileDesc = open(filename, O_WRONLY | O_CREAT,  666);
			  if(fileDesc == -1) {
					  TCRIT("File cannot be open %s\n", filename);
					  g_capture_web_preview_Status  = WEB_PREVIEWER_CAP_FAILURE; //FAILURE
			  }
				  else 
			  {
					 if(write(fileDesc, p_frame_hdr, frame_hdr_size)<0)
					 {
						 TCRIT("ERROR:writing from %s fails\n",p_frame_hdr);
						 close(fileDesc);
						 return -1;
					 }
					 if(write(fileDesc, p_data_buf, data_buf_size)<0)
					 {
					 	TCRIT("ERROR:writing from %s fails\n",p_data_buf);
					 	close(fileDesc);
					 	return -1;
					 }	 
					 close(fileDesc);
					 g_capture_web_preview_Status        = SUCCESS;
				  }
			  }
			  else if (cap_status == CAPTURE_VIDEO_NO_SIGNAL)
				  g_capture_web_preview_Status          = WEB_PREVIEWER_HOST_POWER_OFF; //FAILURE
			  else
				  g_capture_web_preview_Status          = WEB_PREVIEWER_CAP_FAILURE; //FAILURE
			return 0;
		
}

static int 
on_web_previewer_session (session_info_t* p_si, ivtp_hdr_t* p_hdr) 
{
      u32     frame_hdr_size = 0;
      u32     data_buf_size = 0;
      void*   p_frame_hdr = NULL;
      void*   p_data_buf = NULL;
      int     cap_status = 0;	
      //int     fileDesc = -1;
      //char command[256] = {0};
      session_list_t  session_list;
      //char* filename = NULL; // /var folder doesn't have enough space to have this file
      //filename = WEB_PREVIEWER_CAP_FILE; 
      web_cap_status_t captureStatus;
		acquire_mutex (p_si->p_access_guard);
        p_si->is_webPreviewer = TRUE;
        release_mutex (p_si->p_access_guard);

      if(0)
      {
    	  p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
      }
      if(sm_get_webpreviewer_status() == TRUE)
      {
	  // Already capture is on process
          captureStatus.capStatus = WEB_PREVIEWER_CAP_IN_PROGRESS;
          captureStatus.hdr.type           = IVTP_WEB_PREVIEWER_CAPTURE_STATUS;
          captureStatus.hdr.pkt_size       = sizeof (u8);
          captureStatus.hdr.status         = 0;
          if(send (p_si->socket, &captureStatus, sizeof (captureStatus), MSG_NOSIGNAL)<0)
          {
        	  TCRIT("error in sending the data\n"); 
        	  return -1;
          }
        	  return 0;
      }
      // Check for the last capture time. If capture happen within the WEB_PREVIEW_CAPTURE_INTERVAL, no need of capture again.
      if(is_valid_web_capture() == FALSE) 	
      {
          captureStatus.capStatus 	   = SUCCESS;
          captureStatus.hdr.type           = IVTP_WEB_PREVIEWER_CAPTURE_STATUS;
          captureStatus.hdr.pkt_size       = sizeof (u8);
          captureStatus.hdr.status         = 0;
          if(send (p_si->socket, &captureStatus, sizeof (captureStatus), MSG_NOSIGNAL)<0)
          {
        	  TCRIT("error in sending the data\n"); 
        	  return -1;
          }
        	  return 0;
      }
      if(sm_get_kvm_num_session() == 0)
      {
		  sm_set_webpreviewer_status(TRUE);
		  sm_grant_session (p_si->h_session);
		  sm_get_session_list (&session_list, FALSE);
		  session_list.is_new_session = TRUE;	
		  cap_status = capture_video ( &frame_hdr_size, &data_buf_size, &p_frame_hdr, &p_data_buf, &session_list );
		  // At first capture, mode was changed. So that capture will return the CAPTURE_VIDEO_NO_CHANGE. If it return CAPTURE_VIDEO_NO_CHANGE,
		  // We will try to capture again with valid mode.
		  if(cap_status == CAPTURE_VIDEO_NO_CHANGE || cap_status == CAPTURE_VIDEO_NO_SIGNAL) {
			cap_status = capture_video ( &frame_hdr_size, &data_buf_size, &p_frame_hdr, &p_data_buf, &session_list );
		  } 

		  On_write_File_web_preview(cap_status, p_frame_hdr, p_data_buf, (int)data_buf_size, (int)frame_hdr_size);
		  captureStatus.hdr.type           = IVTP_WEB_PREVIEWER_CAPTURE_STATUS;
		  captureStatus.hdr.pkt_size       = sizeof (u8);
		  captureStatus.hdr.status         = 0;
		  captureStatus.capStatus		   = g_capture_web_preview_Status;
		  if(send (p_si->socket, &captureStatus, sizeof (captureStatus), MSG_NOSIGNAL)<0)
		  {
			  TCRIT("error in sending the data\n");
			  return -1;
		  }
		  sm_set_webpreviewer_status(FALSE);
      }
      else
      {
    	  g_web_preview_write_file = TRUE;
    	  sm_set_webPreviewer_data_send(p_si->h_session);
    	  
      }
   
      return 0;
}

// Setting mouse mode. Request from the Jviewer to change the mouse mode. 
static int
on_set_mouse_mode (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	
        AdviserCfg_T adviser_cfg;
        GetAdviserCfg(&adviser_cfg);
        adviser_cfg.mouse_mode = p_hdr->status;
    	if(0)
    	{
    		p_si=p_si; /* -Wextra, fix for unused parameters */
    	}
        if( SetAdviserCfg(&adviser_cfg,1) != 0 )
        {
             return -1;
        }
		set_usb_mouse_mode(p_hdr->status);
	return 0;
}
//Requesting master parmission from master session
static int 
on_request_next_master(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	session_info_t		*session=NULL;
	ip_usr_datapkt userpkt;
	int pend_session =0;
	ivtp_hdr_t hdr;

	p_hdr->pkt_size = htonlEx (p_hdr->pkt_size);
	if ((p_hdr->pkt_size) >sizeof(ip_usr_datapkt))
	{
		TCRIT("\nThis doesn't seem to be our  Packet. Ignoring this packet\n");
		return -1;
	}

	// Master  session sends privilege information  to other active user.
	if(p_hdr->pkt_size >0)
	{
		/* receive the session info */
		if (0 != recv_bytes (p_si->socket,( u8* ) &userpkt,p_hdr->pkt_size,p_si))
		{
			return -1;
		}
		userpkt.sess_index =(char) htonlEx(userpkt.sess_index);
		session = sm_check_valid_session(&userpkt);
		//if selected session is valid 
		if(session != NULL)
		{
			//On Master  session exit if current master gives full permission to some other active user.
			if(p_hdr->status == 0)
			{
				sm_set_master_change_flag(session);
			}
		}
		//selected master session no more valid so set master session index as -1.
		else
		{
			sm_set_master_sess_index(-1);
		}
	}
	//Partial permisssion session requests full permission from Master session
	else if(p_hdr->pkt_size == 0)
	{
		pend_session = sm_get_num_pending_sessions();
		//Process only one request at a time. if second request comes send response as processing request.
		if(pend_session >0)
		{
			hdr.type = IVTP_KVM_SHARING;
			hdr.pkt_size=0;
			hdr.status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + (KVM_REQ_PROGRESS<<8) );
			if( send(p_si->socket, &hdr, sizeof(ivtp_hdr_t), MSG_NOSIGNAL) == -1 )
			{
				TCRIT("Error while sending FAILURE_RESP_MASTER_SETTING_DEFAULT to remote client.");
				//return -1;
			}
			return 0;
		}

		session = sm_get_master_session();
		 if( session != NULL ) 
		{
			//Master session reconnect in progress
			if(sm_is_session_reconn (session->h_session))
			{
				hdr.type = IVTP_KVM_SHARING;
				hdr.pkt_size=0;
				hdr.status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + (KVM_REQ_MASTER_RECONN<<8) );
				if( send(p_si->socket, &hdr, sizeof(ivtp_hdr_t), MSG_NOSIGNAL) == -1 )
				{
					TCRIT("Error while sending FAILURE_RESP_MASTER_SETTING_DEFAULT to remote client.");
					//return -1;
				}
				return 0;
			}
				
			#ifdef JVIEWER_DEBUG
				char username_other_session[TOKENCFG_MAX_USERNAME_LEN-1]="SAMPLE";
				char username1[TOKENCFG_MAX_USERNAME_LEN-1]="SAMPLEUSERNAME";
		
				strncpy(p_si->username_other_session,username_other_session,TOKENCFG_MAX_USERNAME_LEN-1);
				strncpy(session->username_other_session,username1,TOKENCFG_MAX_USERNAME_LEN-1);
			#endif
			#ifndef JVIEWER_DEBUG	
				strncpy(p_si->username_other_session,session->username,TOKENCFG_MAX_USERNAME_LEN-1);
				strncpy(session->username_other_session,p_si->username,TOKENCFG_MAX_USERNAME_LEN-1);	
			#endif

			strncpy(p_si->ip_addr_othersession,session->ip_addr_client,TOKENCFG_MAX_CLIENTIP_LEN);
			strncpy(session->ip_addr_othersession,p_si->ip_addr_client,TOKENCFG_MAX_CLIENTIP_LEN);
			
			session->slave_index =(char)p_si->h_session;
			if( send_req_kvm_wait(p_si,IVTP_SET_NEXT_MASTER) == 0 )
			{
				auth_t auth_pkt ;
				send_req_kvm_priv(session,IVTP_SET_NEXT_MASTER); //to first session
				sm_add_pending_session(p_si,(unsigned char * )p_si->username,(unsigned char * )p_si->ip_addr_client,&auth_pkt);
			}
			else
			{
				//by returning -1, we are closing the socket.
				return -1;
			}
		}
		//Master session not exist
		 else
		{
			sm_set_master_change_flag(p_si);
		}
	}

	return 0;
}

void display_control (u8 lockstatus)
{
    void *video_handle = NULL;
	void (*video_fn) (u8);
	int status;

	// if not enabled at all in project ...
	if(g_corefeatures.host_lock_feature != ENABLED)
		return;
	
	// If host is already locked and if we still get another lock, just neglect it	
	if ((g_host_is_locked == lockstatus) )
	{
		return;
	}
	if(g_corefeatures.host_lock_feature == ENABLED)
	{
		// use runtime value
		if (OnGethostlockfeature_dynm(&status) != 0)
			status = 0;     // Failed
		status &= (1 << 0); // Enabled is Bit 0
		//return from here only when fearue is disabled and tries to lock
		//should not return from here when feature is disabled but host is locked if adviser wants host unlock allow it
		//lockstatus - 1(lock) , lockstatus - 0(unlock)
		if((status == 0) && (lockstatus != 0))
		{
			return;
		}
	}
	Onhostlock_status ( &lockstatus);
	if (lockstatus == HOSTLOCK_SUCCESS) {
		g_host_is_locked = 0;
	}
	else if(lockstatus == HOSTUNLOCK_SUCCESS){
		g_host_is_locked = 1;
	}

	// This will call the video hook allocated for this event
	video_handle = dlopen(VIDEO_HOOKS_PATH, RTLD_LAZY);
	if(!video_handle)
		{
		TCRIT("%s\n",dlerror());
		}
	
	else
	{
		video_fn = dlsym(video_handle, HOST_LOCK_FN);	
		if (video_fn)
		{
			video_fn (g_host_is_locked);
		}

		dlclose(video_handle);
	}

	return;
}


static int
onHostDisplayControl(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	u8 lockstatus;

	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	/* Read whether to lock or unlock the monitor */
	if (sizeof(u8) != recv(p_si->socket, (u8 *)&lockstatus, sizeof (u8), 0))
	{
			return -1;
	}
	if(p_si->h_session == sm_get_master_sess_index())
	{
		display_control (lockstatus);
	}
	sm_set_host_lock_status();
	return 0;
}
/*
* on_set_kbd_language : sets keyboard layout language in config file
*/
static int 
on_set_kbd_language(session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
    char kbdlang[8] = {0};

    /* Read keyboard language */
    if ((signed long)p_hdr->pkt_size != recv(p_si->socket, &kbdlang, sizeof(kbdlang), 0))
    {
        TCRIT("Error in receiving Keyboard layout language \n");
        return -1;
    }

    /*validate keyboard language*/
    if(1 !=ValidKBDLang(kbdlang))
    {
        TCRIT(" Keyboard layout language %s not supported \n",kbdlang);
        return -2;
    }

    /*set keyboard language*/
    SetKeyLayoutCfg(kbdlang, sizeof(kbdlang));
    return 0;
}

static int
on_keep_alive_pkt (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{
	/* send response packet for keep alive request */
	sm_set_keep_alive_flag(p_si->h_session);
	if(0)
	{
		p_hdr=p_hdr; /* -Wextra, fix for unused parameters */
	}
	return 0;
}

static int  
on_fps_diff(session_info_t* p_si, ivtp_hdr_t* p_hdr) 
{ 
	p_si->fps_diff = p_hdr->status; 
	return 0; 
} 

static int
on_disconnect_session (session_info_t* p_si, ivtp_hdr_t* p_hdr)
{

	int ret = 0;
	session_disconect_record_t  discon;
	u8	*pTemp;
	pTemp = (u8*)&discon;
	memset(&discon,0,sizeof(session_disconect_record_t));

	ret = recv_bytes (p_si->socket,( u8* ) &pTemp[0],p_hdr->pkt_size,p_si);
	if(0 !=ret )
	{
		return -1;
	}

	/* send kill command to another session */
	Ondisconnect_session(sm_getracsession_id(discon.racsession_id),discon.session_type);
	return 0;
}

int on_reconnect_pkt(int socket,unsigned long pkt_size)
{
	int ret = 0;
	stoken_resp_t		session_resp;
	reconnect_req_t  recon_req;
	ivtp_hdr_t		hdr;

	session_info_t* p_si = NULL;
	char user_name[TOKENCFG_MAX_USERNAME_LEN];
	char client_ip[TOKENCFG_MAX_CLIENTIP_LEN];
	unsigned int user_priv = 0;
	int info_changed = 0;
	u8	*pTemp;
	pTemp = (u8*)&recon_req;

	memset(&recon_req,0,sizeof(reconnect_req_t));

	ret = recv_bytes (socket,( u8* ) &pTemp[0],pkt_size,p_si);

	if(0 !=ret )
	{
		return -1;
	}

	info_changed = sm_is_sessinfo_changed(recon_req);
	if(info_changed ==0)
		ret = ValidateToken(recon_req.session_token, &user_priv,(unsigned char *)user_name,(unsigned char *)client_ip,VIDEO_RECONNECT);
	if ((ret != 0 ) || (info_changed != 0))
	{
		TWARN ("Session invalid: sending invalid Video session response in Reconnect\n");
		if(info_changed >0)
			session_resp.sresp = INVALID_SESSION_INFO;
		else
			session_resp.sresp = INVALID_SESSION;
		session_resp.hdr.pkt_size = sizeof (u8);
	}
	else
	{
		session_resp.sresp = VALID_SESSION;
		session_resp.sindex = (char)htonsEx(recon_req.index);
		session_resp.hdr.pkt_size = sizeof (stoken_resp_t) - sizeof (ivtp_hdr_t);
	}
	/* send validate session response */

	session_resp.hdr.type 		= IVTP_VALIDATE_VIDEO_SESSION_RESPONSE;
	session_resp.hdr.status 	= 0;
	if ( -1 == send (socket, &session_resp, sizeof (stoken_resp_t), MSG_NOSIGNAL) )
	{
		TCRIT("Failed sending Invalid Video session response");
		return -1;
	}
	if((session_resp.sresp == INVALID_SESSION ) || (session_resp.sresp == INVALID_SESSION_INFO ))
	{
		//Invalid session to go for reconnect set clean flag to clean session.
		sm_set_clean_flag(recon_req.index);
		sm_clean_sessions();
		return 0;
	}

	hdr.type = IVTP_KVM_SHARING;
	hdr.pkt_size = 0;

	if(sm_get_kvm_priv(session_resp.sindex, &hdr) == true)
	{
		if((signed short)hdr.status >= KVM_REQ_ALLOWED && hdr.status <= KVM_REQ_TIMEOUT) 
		{
		hdr.status  = (unsigned short) (STATUS_KVM_PRIV_SWITCH_USER + ( ((u16)hdr.status) <<SHIFT_8 ) );

			if( send(socket, &hdr, sizeof(ivtp_hdr_t), MSG_NOSIGNAL) == -1 )
			{
				TCRIT("Error while sending KVM Sharing pkt to remote client.");
			}
		}
	}
	//send active client list
	sm_set_active_client_send();
	sm_set_reconn_sock(session_resp.sindex,socket);

	return 0;
	
}
