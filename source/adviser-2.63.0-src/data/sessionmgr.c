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
* @file   	Session_mgr.c
* @author 	Govind Kothandapani		 <govindk@ami.com>
*			Baskar Parthiban		 <bparthiban@ami.com>
*			Varadachari Sudan Ayanam <varadacharia@ami.com>
* @brief  	Session management functions implementation
****************************************************************/
#define _GNU_SOURCE

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <semaphore.h> 
#include "fcntl.h"
#include "signal.h"
#include <linux/ioctl.h>
#include<sys/prctl.h>
#include <sys/sysinfo.h>
#include "cmdhndlr.h"
#include "sessionmgr.h"
#include "sync.h"
#include "advisercfg.h"
#include "adviserdbg.h"
#include "token.h"
#include "videopkt.h"
#include "video_misc.h"
#include "video_interface.h"
#include "ncml.h"
#include "encrypt.h" 
#include "libipmi_session.h"
#include "libipmi_struct.h"
#include "libipmi_errorcodes.h"
#include "libipmi_AMIOEM.h"
#include "featuredef.h"
#include "usb_ioctl.h"
#include "vmedia_instance.h"
#include "flashlib.h"
#include "adviser_cfg.h"
#include "errno.h"
#include <pthread.h> 
#include "racsessioninfo.h"

//our mutex for access control between timer thread and others
//note that this mutex is to protect between two threads
//first off the tiemr thread is the only other threas that access the table structures
//so this mutexing IS EXPENSIVE in time. we use pthread_mutexes so....
pthread_mutex_t table_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;


#define MUTEX_LOCK() pthread_mutex_lock(&table_mutex)
#define MUTEX_UNLOCK() pthread_mutex_unlock(&table_mutex)

extern int do_usb_ioctl(int cmd, u8 *data);
extern void display_control (u8 lockstatus);
void start_own_timer_thread();
void* timer_thread(void* Param);
extern int send_stop_session(session_info_t* session,int status);

/**-------------------------------- Local Definitions -----------------------------------*/
#define FRAGMENT_MIN_SIZE		(10 * 1024)
#define DEFAULT_ENCRYPTION_KEY		"zyxwvutsr02938"

/**-------------------------------- Module Variables ------------------------------------*/
session_info_t				*m_session = NULL;
static BOOL				m_is_new_session;
static u32				m_num_pending_clients;
static BOOL				m_abort;
static u32				m_abort_count;
static BOOL				m_stop;
static BOOL				m_refresh;
static BOOL				m_is_web_previewer;
static time_t 				m_web_capture_time;

sem_t					m_start_avse;
sem_t					m_start_cmdhndlr;
//sem_t					m_start_record_thread;
static pthread_mutex_t			m_global_guard;
pthread_mutex_t				*m_session_guard = NULL;
sem_t					*m_ev_wakeup = NULL;
static sem_t				m_ev_transfer_complete;
static sem_t				m_ev_stop_session_complete;
pthread_mutex_t				*m_session_auto_bw = NULL;
//added for kvm sharing
PENDING_SESSION		*m_pending_session = NULL;
pthread_mutex_t			*m_pending_session_guard = NULL;
//send kvm-sharing response packet
BOOL				g_kvm_priv_resp_send = FALSE;
session_info_t		*m_kvm_priv_master_sess = NULL; //kvm privilege master ression info.
session_info_t		*m_kvm_priv_slave_sess = NULL; //kvm privilege requesting ression info.

extern int      			PipeFD; 
unsigned short                  	g_stop_signal_status=0;
unsigned short                  g_host_power_status=0;
BOOL                  	g_mouse_media_status=FALSE;
BOOL			g_active_client_send=FALSE; //flag to send the active clients to the session
BOOL			g_pending_client_send = FALSE; //flag to send the pending clients to the session
BOOL			g_power_status_send = FALSE; //flag to send the power status to active clients

extern SERVICE_CONF_STRUCT		AdviserConf;
extern int				max_session;
u8					*service_info_data = NULL;
int					services_changed = 0;
extern int				kvm_max_session;
extern CoreFeatures_T g_corefeatures;
int				master_sess_index = -1;
BOOL 				g_media_license_send = FALSE;
void OnInitdefaultalue();
extern u8 g_host_lock_auto_feature;
extern u8 g_host_is_locked;
unsigned short                  g_host_lock_status_send=0;
int g_TimerTickSeconds; //the number of seconds interval for timers
unsigned short                  g_vmedia_update_status=0;

//added for kvm reconnect
extern int g_kvm_retry_count;
extern int g_kvm_retry_interval;

// contains difference of the number of frames received and painted in H5 client.
// the least value of difference will be chosen as g_fps_diff.
// based on which the frame capture will be delayed. 
int g_fps_diff = -1;
/**
 * init_session_mgr
**/
int
init_session_mgr (void)
{
	int i;

	if (0 != pthread_mutex_init (&m_global_guard, 0)) { return -1; }
	
	/* These 2 semaphores are used in avse_main() and cmdhndlr_main() for holding the thread */
	if (sem_init (&m_start_avse, 0, 0) < 0) { return -1; }
	if (sem_init (&m_start_cmdhndlr, 0, 0) < 0) { return -1; }
//	if (sem_init (&m_start_record_thread, 0, 0) < 0) { return -1; }

	acquire_mutex (&m_global_guard);

	/* Iniitialise session variables */
	if (sem_init (&m_ev_transfer_complete, 0, 0) < 0) { return -1; }
	if (sem_init (&m_ev_stop_session_complete, 0, 0) < 0)      { return -1; }
	m_is_new_session			= FALSE;
	m_num_pending_clients		= 0;
	m_abort						= FALSE;
	m_abort_count				= 0;
	m_stop						= FALSE;
	m_is_web_previewer				= FALSE;
	m_web_capture_time				= 0;

	for (i = 0; i < max_session; i++)
	{
		if (sem_init (&(m_ev_wakeup [i]), 0, 0) < 0) 			 { return -1; }
		if (0 != pthread_mutex_init (&(m_session_guard [i]), 0)) { return -1; }
	
		if (0 != pthread_mutex_init (&(m_session_auto_bw [i]), 0)) { return -1; }
		if ((&m_session [i] == NULL))
		{
			TCRIT ("Memory doesn't allocated for session\n");
			return -1;
		}
		m_session [i].is_webPreviewer                           = FALSE;
		m_session [i].is_used					= FALSE;
		m_session [i].is_valid					= FALSE;
		m_session [i].clean						= FALSE;
		m_session [i].pause					= FALSE;
		m_session [i].h_session					= i;
		m_session [i].login_timer				= 0;
		m_session [i].activity_timer			= 0;
		m_session [i].worker_thread_id			= 0;
		m_session [i].media_active 			= 0;
		m_session [i].pev_wakeup				= &(m_ev_wakeup [i]);
		m_session [i].p_access_guard			= &(m_session_guard [i]);
		m_session [i].p_global_guard			= &m_global_guard;
		m_session [i].stop						= FALSE;
		m_session [i].thread_stopped			= FALSE;
		m_session [i].p_pending_clients			= &m_num_pending_clients;
		m_session [i].p_abort					= &m_abort;
		m_session [i].p_abort_count				= &m_abort_count;
		m_session [i].pev_transfer_complete 	= &m_ev_transfer_complete;
		m_session [i].is_busy					= FALSE;
		m_session [i].worker_thread_id 			= i;
		m_session [i].is_stop_signal			= FALSE;
		m_session [i].Logout_Type				= STOP_SESSION;
		
		memset(m_session [i].encrypt_key,0,MAX_PASSWORD_LEN_HASHED + 1);
		strncpy ((char *)m_session [i].encrypt_key, DEFAULT_ENCRYPTION_KEY,
				MAX_PASSWORD_LEN_HASHED);
		m_session [i].h_encrypt					= NULL;
		m_session [i].is_encrypted				= FALSE;
		m_session [i].is_send_encrypt_status	= FALSE;

		m_session [i].is_auto_bw_detect 		= FALSE;
		m_session [i].auto_bw_pkt_size 			= 0;
		m_session [i].portno					= 0;
		memset (m_session [i].ip_addr, 0, sizeof (m_session [i].ip_addr));

		m_session [i].socket					= 0;
		//added for kvm sharing
		m_session [i].is_kvm_priv_ctrl_pkt			= FALSE;
		m_session [i].kvm_pkt_status			= 0;
		m_session [i].p_access_detect_auto_bw	= &(m_session_auto_bw [i]);
		//added for Power  feature
		m_session [i].host_power_cmd_status		= 0;
		m_session [i].is_power_cmd_status_send		= FALSE;
		m_session [i].is_power_status_send			= FALSE;
		m_session [i].is_send_user_macro		= FALSE;
		m_session [i].soc_session_hdr = NULL;
		m_session [i].ipmiprev = 0;
		m_session[i].is_master_change		=FALSE;
		m_session[i].KVM_pkt = 0;
		m_session[i].kvm_priv =KVM_REQ_DENIED; 
		m_session[i].sess_ipmi_res = malloc(sizeof(ipmi_res_data)*MAX_IPMI_COMMAND_LIMIT);
		if(m_session[i].sess_ipmi_res == NULL)
		{
			TCRIT("\nmalloc failed for sess_ipmi_res\n");
		}
		m_session[i].is_host_lock_status = 0;
		if (0 != pthread_mutex_init (&(m_pending_session_guard [i]), 0)) { return -1; }
		m_pending_session[i].pending_state = PEND_STATE_NORMAL;
		m_pending_session[i].start_time = 0;
		m_pending_session [i].slave_sess_index = -1;
		memset(&m_pending_session [i].client_ip,0,TOKENCFG_MAX_CLIENTIP_LEN);
		memset(&m_pending_session [i].user_name,0,TOKENCFG_MAX_USERNAME_LEN);
		m_session[i].racsession_id=-1;
		m_session[i].failed_time = 0;
		m_session[i].disconnected_clean = false;
		m_session[i].is_alive = false;
	}
	OnInitdefaultalue();
	release_mutex (&m_global_guard);

	return 0;
}
void OnInitdefaultalue()
{
	AdviserCfg_T  pAdviserCfg;
	GetAdviserCfg(& pAdviserCfg);
	u8 lockstatus = 0; //unlock host monitor if its locked
	g_host_lock_auto_feature = pAdviserCfg.hostlock_feature;
	Onhostlock_status ( &lockstatus);
	if (lockstatus == HOSTLOCK_SUCCESS) {
			g_host_is_locked = 0;
	}
	else if(lockstatus == HOSTUNLOCK_SUCCESS){
		g_host_is_locked = 1;
	}
	
}
session_info_t * sm_get_master_session()
{
	if (master_sess_index >=0)
		return &(m_session[master_sess_index]);
	return NULL;
}

void sm_set_master_sess_index(int master_index)
{
	master_sess_index = master_index; 
}
int sm_get_master_sess_index()
{
	return master_sess_index;
}

/**
 * sm_reserve_session
**/
hsession_t
sm_reserve_session (int sock, int con_port)
{
	int			i = 0;
	int 			si = -1;
	unsigned int 		nlen = 0;
	struct  sockaddr_in    	sin;
	void 			*soc_handle = NULL;
	void 			(*get_soc_hdr) (void **);

	if(0)
	{
		con_port=con_port; /* -Wextra, fix for unused parameters */
	}
	/* Check for an empty slot */
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (!m_session [i].is_used)
		{
			si 		= i;
			release_mutex (m_session [i].p_access_guard);
			break;
		}
		release_mutex (m_session [i].p_access_guard);
	}


	if (-1 == si)
	{
		return INVALID_SESSION_ID;
	}


	/* Initialise the session parameters */
	acquire_mutex (m_session [si].p_access_guard);
	m_session [si].is_used 			= TRUE;
	m_session [si].is_valid			= FALSE;
	m_session [si].clean			= FALSE;
	m_session [si].pause			= FALSE;
	m_session [si].login_timer		= LOGIN_TIMEOUT_TICK;
	m_session [si].socket			= sock;
	m_session [si].bandwidth		= DEFAULT_SESSION_BANDWIDTH;
	m_session [si].media_active		= 0;
	m_session [si].max_frag_size	= ((m_session [si].bandwidth / (1024*1024)) * FRAGMENT_SEND_TIME_MS) < FRAGMENT_MIN_SIZE ?
									  FRAGMENT_MIN_SIZE : ((m_session [si].bandwidth / (1024*1024)) * FRAGMENT_SEND_TIME_MS);
	TINFO_4 ("max frag size:%ld\n", m_session [si].max_frag_size);
	m_session [si].is_ctrl_pkt 		= FALSE;
	m_session [si].is_frame_pkt 		= FALSE;
	m_session [si].transfer_status	= 0;
	m_session [si].ctrl_transfer_status	= 0;
	m_session [si].quality_level	= DEFAULT_QUALITY_LEVEL;
	m_session [si].tsend			= 0;
	m_session [si].fps				= DEFAULT_FPS;
	m_session [si].Logout_Type		= 0;


	nlen = sizeof(struct sockaddr);
	getpeername (sock, (struct sockaddr*) &sin, &nlen);
	sprintf (m_session [si].ip_addr,
			 "%d.%d.%d.%d",
			 (unsigned char)((sin.sin_addr.s_addr >> 24) & 0xFF),
			 (unsigned char)((sin.sin_addr.s_addr >> 16) & 0xFF),
			 (unsigned char)((sin.sin_addr.s_addr >> 8) & 0xFF),
			 (unsigned char)((sin.sin_addr.s_addr >> 0) & 0xFF));
	m_session [si].portno		= sin.sin_port;
	//TINFO ("Client ip is:%d.%d.%d.%d port:%d\n", 
	TINFO_4 ("Client ip is:%d.%d.%d.%d port:%d\n", 
			 (unsigned char)((sin.sin_addr.s_addr >> 0) & 0xFF),
			 (unsigned char)((sin.sin_addr.s_addr >> 8) & 0xFF),
			 (unsigned char)((sin.sin_addr.s_addr >> 16) & 0xFF),
			 (unsigned char)((sin.sin_addr.s_addr >> 24) & 0xFF),
					ntohs(m_session [si].portno));

	//TDBG ("Resetting Encrypt variables\n");
	m_session [si].h_encrypt				= NULL;
	m_session [si].is_encrypted				= FALSE;
	m_session [si].is_send_encrypt_status	= FALSE;

	// This opens up the LibVideo for Utilizing its definitions
	soc_handle = dlopen(LIBVIDEO_PATH, RTLD_LAZY);
	if(!soc_handle)
	{
		TCRIT("%s\n",dlerror());
	}
	
	else
	{
	// This will call definition to update the soc session hdr
		get_soc_hdr = dlsym(soc_handle, GET_SOC_HDR_FN);	
		if (get_soc_hdr)
		{
			get_soc_hdr (&(m_session[si].soc_session_hdr));
		}

		dlclose(soc_handle);
	}

	release_mutex (m_session [si].p_access_guard);

	return i;
}


/**
 * sm_grant_session
**/
int
sm_grant_session (hsession_t hsession)
{
	acquire_mutex (m_session [hsession].p_access_guard);
	/* Make sure that the session is in used */
	if (!(m_session [hsession].is_used)) 
	{ 
		release_mutex (m_session [hsession].p_access_guard);
		return -1; 
	}

	/* Make the session valid */
	m_session [hsession].is_valid 			= TRUE;
	m_session [hsession].pause 			= FALSE;
	m_session [hsession].activity_timer		= DEFAULT_ACTIVITY_TIMEOUT_TICK;
	m_session [hsession].media_active 		= 0;
	m_session[hsession].activity_cnt_down	= AdviserConf.SessionInactivityTimeout;
	m_session[hsession].lastrecvtime = 0;

	release_mutex (m_session [hsession].p_access_guard);

	/* Mark that there is new session is in place */
	acquire_mutex (&m_global_guard);
	m_is_new_session = TRUE;
	release_mutex (&m_global_guard);

	return 0;
}


/**
 * sm_delete_session
**/
int
sm_delete_session (hsession_t hsession)
{
	/* mark the session as not used */
	acquire_mutex (&m_session_guard [hsession]);

	m_session [hsession].socket	  = 0;
	m_session [hsession].is_used  = FALSE;
	m_session [hsession].is_valid = FALSE;
	m_session [hsession].pause    = FALSE;

	TINFO_1 ("Deleting session with %s\n", m_session [hsession].ip_addr);

	TDBG ("Resetting Encrypt variables\n");
	m_session [hsession].h_encrypt					= NULL;
	m_session [hsession].is_encrypted				= FALSE;
	m_session [hsession].is_send_encrypt_status	= FALSE;
	release_mutex (&m_session_guard [hsession]);

	return 0;
}


/**
 * sm_set_session_bw
**/
int
set_session_bw (hsession_t hsession, u32 bandwidth)
{
	acquire_mutex (&m_session_guard [hsession]);
	m_session [hsession].bandwidth		= bandwidth;
//	m_session [hsession].bandwidth		= DEFAULT_SESSION_BANDWIDTH;
	m_session [hsession].max_frag_size	= ((m_session [hsession].bandwidth / 1000) * FRAGMENT_SEND_TIME_MS) < FRAGMENT_MIN_SIZE ?
											FRAGMENT_MIN_SIZE : (m_session [hsession].bandwidth / FRAGMENT_SEND_TIME_MS);
	release_mutex (&m_session_guard [hsession]);

	return 0;
}


/**
 * set_session_fps
**/
int
set_session_fps (hsession_t hsession, u8 fps)
{
	acquire_mutex (&m_session_guard [hsession]);
	m_session [hsession].fps	= fps;
	release_mutex (&m_session_guard [hsession]);

	return 0;
}


/**
 * sm_get_session_list
**/
int
sm_get_session_list (session_list_t* p_session_list, BOOL clear_new_flag)
{
	int		i;

	p_session_list->num_session = 0;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (&m_session_guard [i]);

		if (m_session [i].is_used)
		{
			p_session_list->p_session [p_session_list->num_session] = &(m_session [i]);
			p_session_list->num_session++;
		}

		release_mutex (&m_session_guard [i]);
	}


	acquire_mutex (&m_global_guard);
	p_session_list->is_new_session 		= m_is_new_session;
	if (clear_new_flag) 	{ m_is_new_session = FALSE; }
	release_mutex (&m_global_guard);

	return 0;
}

/**
 * sm_get_services_info
**/
int
sm_get_services_info (session_list_t* p_session_list, session_list_t *p_send_list, u8 *servicesinfo)
{
	int 			index = 0;
	int 			retval = 0;
	SERVICE_CONF_STRUCT	*services_config = (SERVICE_CONF_STRUCT *)(servicesinfo);
	int 			datalen = 0;

	// List of services information to be sent to the clients
	char *ServiceNameList[KVM_SERVICE_LIST] = 
	{
		WEB_SERVICE_NAME,
                KVM_SERVICE_NAME,
                CDMEDIA_SERVICE_NAME,
                FDMEDIA_SERVICE_NAME,
                HDMEDIA_SERVICE_NAME
	};

	if(0)
	{
		p_send_list=p_send_list; /* -Wextra, fix for unused parameters */
		p_session_list=p_session_list;
	}
	// Services already modified and previous state is still not sent
	// Repeating the ncml configuration capture
	if (services_changed)
	{
		TDBG ("Got an updated configuration before we could send the current. Repeating task...");
		services_changed = 0;
	}

	// Find the length of data to be sent
	datalen = sizeof(SERVICE_CONF_STRUCT) * KVM_SERVICE_LIST;

	// Clear the slate before capturing the config
	memset(servicesinfo, 0, datalen);

	// Loop through all the services and get their info into the buffer
	for (index = 0; index < KVM_SERVICE_LIST; index++)
	{
		retval = get_service_configurations(ServiceNameList[index], services_config);
		if (retval != 0)
		{
			printf("Error in fetching the Configuration for the particular service\n");
			goto error_out;
		}

		services_config++;
	}

	// Set the flag to send the captured data to clients
	services_changed = 1;

error_out:
	return retval;
}

/**
 * get_free_session
**/
session_info_t*
get_free_session (hsession_t hsession)
{
	if (!(m_session [hsession].is_used)) { return &(m_session [hsession]); }
	else { return NULL; }
}



/**
 * clean_sessions
**/
void
sm_clean_sessions (void)
{
	int 		i,retval;
	BOOL		post_stop_session_flag = FALSE;
	BOOL		SessionExit = FALSE;
	void 		*video_handle = NULL;
	void 		*session_handle = NULL;
	int 		(*sess_ctrl) (session_info_t*, u8, u8);
	void		(*set_bandwidth_mode) (session_info_t*, u8);
	session_list_t	session_list;
	session_info_t *master_session = NULL;
	session_info_t *slave_session = NULL;
	time_t			curr_time;
	int num_valid_session = 0;								//holds number of valid session count
	//int add_sel;
	sm_get_valid_session_list(&session_list, FALSE);

	// Opens the provided library for access
	video_handle = dlopen(LIBVIDEO_PATH, RTLD_LAZY);
	if(!video_handle)
	{
		TCRIT("%s\n",dlerror());	
		return ;
	}
	session_handle = dlopen(VIDEO_HOOKS_PATH, RTLD_LAZY);
	if(!session_handle)
	{
		TCRIT("%s\n",dlerror());
		dlclose(video_handle);
		return;
	}
	if((sm_get_num_valid_sessions()  == 0) && (sm_get_master_sess_index() >= 0))
	{
		sm_set_master_sess_index(-1);
	}
	master_session = sm_get_master_session();

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		//Dont reset clean flag when session is in permission request.
		if((g_corefeatures.kvm_reconnect_support == ENABLED) && (m_session [i].kvm_priv != KVM_REQ_PROGRESS))
		{
			time(&curr_time);
			if(!m_session[i].disconnected_clean)
			{
				m_session [i].clean = false;
			}
			// reduced the buffer time to 5.
			/* when there is no change in host video, no packet will be sent to jviewer, so the failed time won't be updated
			   check lastrectime to make sure that host with no change in video is properly terminated when reconnect fails
			   lastrectime contains the time of last packet received.
			*/
			// Since detecting the network connect/disconnect in the client system takes upto approximately 8 seconds, 
			// a buffer time of 15 seconds have been added while calculating diff with lastrecvtime
			if((IsTimedOut(m_session[i].failed_time ,((time_t)((g_kvm_retry_count * g_kvm_retry_interval) + 5))) && (m_session[i].failed_time > 0)) ||
			   (IsTimedOut(m_session[i].lastrecvtime,((time_t)((g_kvm_retry_count * g_kvm_retry_interval) + 15))) && (m_session[i].lastrecvtime > 0)))
			{
				m_session [i].clean = true;
			}
		}
		if (m_session [i].is_used && m_session [i].clean)
		{
			m_session [i].disconnected_clean = false;  //Quanta		
			DeleteToken(m_session[i].session_token);
			slave_session = &m_session[m_pending_session[i].slave_sess_index] ;
			//is_valid flag will be set in approve_session()
			//Following operation should be done only for valid JViewer sessions.
			if(m_session [i].is_valid)
			{

				release_mutex (m_session [i].p_access_guard);
				if( ! (sm_get_num_valid_sessions() - 1)/*reduce the current valid session we are about to clean*/
						|| (sm_get_master_sess_index() == m_session [i].h_session) )/*Master session gets closed with out assigning next master.*/
				{
					sm_set_master_sess_index(-1);//No more master session. So clear master index.
					TDBG("Unlocking the host lock monitor");
					/*If there is no more master session, unlock host display.*/
					display_control (0);
					sm_set_host_lock_status();
				}

				acquire_mutex (m_session [i].p_access_guard);
				m_session [i].is_valid 	= FALSE;
				release_mutex (m_session [i].p_access_guard);

				acquire_mutex (&m_pending_session_guard[i]);
				if( m_pending_session[i].pending_state == PEND_STATE_WAIT_IN_PROGRESS )
				{
					if((NULL !=master_session  ) && (NULL != slave_session))
					{
						if( &m_session[i] == master_session)
						{
							
							slave_session->KVM_pkt = IVTP_KVM_SHARING;
							slave_session->kvm_pkt_status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + (KVM_REQ_PARTIAL<<8) );
							sm_set_kvm_priv_slave_sess(slave_session);
							sm_set_kvm_priv_resp_send();
							retval=approve_session(slave_session, &m_pending_session[i].auth_pkt,m_pending_session[i].user_name,m_pending_session[i].client_ip, KVM_REQ_PARTIAL);
							if(retval!=0)
							{
								TCRIT("Error in approve_session function\n");
								dlclose(video_handle);
								dlclose(session_handle);
								return ;
							}
							memset(&m_pending_session[i], 0 , sizeof(PENDING_SESSION));
						}
						else if( &m_session[i] == slave_session )
						{
							sm_set_kvm_priv_flag(master_session, STATUS_KVM_PRIV_CANCEL );
							memset(&m_pending_session[i], 0 , sizeof(PENDING_SESSION));
						}
					}
				}
				release_mutex (&m_pending_session_guard[i]);

				acquire_mutex (m_session [i].p_access_guard);

				// This will call the Hook whenever a session gets closed and decrements total number of sessions.
				if (session_handle)
				{
					if(m_session[i].Logout_Type==STOP_SESSION_TIMED_OUT){
						kvm_loginLogoutAudit(m_session[i].username, m_session[i].ip_addr_client,SEL_AUTO_LOGOUT_SUCCESS );
					}
					else{
						kvm_loginLogoutAudit(m_session[i].username, m_session[i].ip_addr_client,SEL_LOGOUT_SUCCESS );
					}
					
					sess_ctrl = dlsym(session_handle, START_STOP_SSN);	
					if (sess_ctrl)
					{
						sess_ctrl (&m_session[i], (u8)(--(session_list.num_session)), STOP_SESSION_HOOK);
					}
				}
				if(g_corefeatures.session_management == ENABLED)
				{
					racsessinfo_unregister_session(m_session [i].racsession_id,SEL_LOGOUT_SUCCESS);
				}
				if (video_handle)
				{
					// If no sessions are available, then reset the bandwidth mode to normal mode.
					if (session_list.num_session == 0)
					{
						set_bandwidth_mode = dlsym(video_handle, LOW_BANDWIDTH_MODE);
						if (set_bandwidth_mode)
						{
							set_bandwidth_mode (&m_session[i], 0);
						}
					}
				}
			}
			//Clearing session structure for used sessions
			m_session [i].is_used	= FALSE;
//			m_session [i].is_valid 	= FALSE;
			m_session [i].clean	= FALSE;	
			m_session [i].pause	= FALSE;
			m_session [i].is_webPreviewer = FALSE;
			m_session [i].ipmiprev = 0;
			m_session [i].activity_cnt_down = 0;
			m_session [i].lastrecvtime = 0;

			m_session [i].failed_time = 0;
			m_session [i].disconnected_clean = false;
			m_session [i].is_alive = false;

			if (m_session [i].media_active)
			{
				m_session [i].media_active = 0;
				sm_close_associates();
			}

			if (m_session [i].is_stop_signal) /* Clean request from avse */
			{
				m_session [i].is_stop_signal 	= FALSE;
				post_stop_session_flag			= TRUE;
			}
			release_mutex (m_session [i].p_access_guard);
			
			acquire_mutex (&m_pending_session_guard[i]);
			if( m_pending_session[i].pending_state == PEND_STATE_WAIT_IN_PROGRESS )
			{
				if( &m_session[i] == master_session )
				{
					slave_session->KVM_pkt = IVTP_KVM_SHARING;
					slave_session->kvm_pkt_status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + (KVM_REQ_PARTIAL<<8) );
					sm_set_kvm_priv_slave_sess(slave_session);
					sm_set_kvm_priv_resp_send();
					retval=approve_session(slave_session, &m_pending_session[i].auth_pkt,m_pending_session[i].user_name,m_pending_session[i].client_ip, KVM_REQ_PARTIAL);
					if(retval!=0)
					{
						TCRIT("Error in approve_session function\n");
						dlclose(video_handle);
						dlclose(session_handle);
						return ;
					}
					memset(&m_pending_session[i], 0 , sizeof(PENDING_SESSION));
				}
				else if( &m_session[i] == slave_session )
				{
					sm_set_kvm_priv_flag(master_session, STATUS_KVM_PRIV_CANCEL );
					memset(&m_pending_session[i], 0 , sizeof(PENDING_SESSION));
				}
			}
			release_mutex (&m_pending_session_guard[i]);
			
			acquire_mutex (m_session [i].p_access_guard);
			if( m_session[i].is_kvm_priv_ctrl_pkt )
			{
				m_session[i].is_kvm_priv_ctrl_pkt = FALSE;
				m_session[i].kvm_pkt_status= 0;
			}

			//send response to  pending sessions on master exit
			if( &m_session[i] == master_session )
			{
				sm_set_pending_client_response();
			}

			if(m_session[i].is_master_change == TRUE)
			{
				m_session[i].is_master_change = FALSE;
			}

			memset(&m_session[i].username, 0 , sizeof(m_session[i].username));
			memset(&m_session[i].username_other_session, 0 , sizeof(m_session[i].username_other_session));
			//m_session[i].userprivilege=0;
			//m_session[i].token_invalidate_flag=FALSE;
			//added for ip  address
			memset(&m_session[i].ip_addr_client, 0 , sizeof(m_session[i].ip_addr_client));
			memset(&m_session[i].ip_addr_othersession, 0 , sizeof(m_session[i].ip_addr_othersession));
			memset(&m_session[i].client_uname, 0 , sizeof(m_session[i].client_uname));
			memset(&m_session[i].mac_addr, 0 , sizeof(m_session[i].mac_addr));
			m_session[i].slave_index = -1;
			m_session[i].KVM_pkt = 0;
			m_session[i].kvm_priv = KVM_REQ_DENIED;

			WriteSessionToken(&m_session [i], REMOVE);
			//clear websession token
			memset(&m_session[i].websession_token, 0 , sizeof(m_session[i].websession_token));

			// Shutdown blocks the RDWR communication with the clients, thereby clients get updated immd'tly
			// Close closes the socket and frees up the resources in fd table	
			shutdown (m_session [i].socket, SHUT_RDWR);
			close(m_session [i].socket);
			if (m_session [i].h_encrypt)
			{
				CloseEncryptionHandle (m_session [i].h_encrypt);
			}
			m_session [i].h_encrypt					= NULL;
			m_session [i].is_encrypted				= FALSE;
			m_session [i].is_send_encrypt_status	= FALSE;
		
			if (m_session [i].soc_session_hdr)
			{
				free(m_session [i].soc_session_hdr);
				m_session [i].soc_session_hdr = NULL;
			}
			SessionExit = TRUE;
		}
		release_mutex (m_session [i].p_access_guard);
	}

	if(SessionExit ==  TRUE){
		num_valid_session = sm_get_num_valid_sessions();
		//update active session  count
		if(g_corefeatures.session_management != ENABLED)
		{
			if(set_active_session(KVM_SERVICE_ID_BIT,num_valid_session) != 0)
			{
				TCRIT("Adviser:Unable to get the active session count");
			}
		}
		
		if (num_valid_session > 0)
		{
			sm_set_active_client_send();
		}
		else 
		{
			//Always turn display on if no valid session 
			display_control (0);
			if(g_corefeatures.power_consumption_virtual_device_usb == ENABLED)
			{
				
			if(!IsCardInActiveFlashMode() && !IsCardInFlashMode())
			{
				if(!IsLmediaRmediaRunning() && !IsVirtualMediaRunning() && IsPowerconsumptionmode())
				{
					if (0 != do_usb_ioctl(USB_DISABLE_ALL_DEVICE, NULL) )
					{
						TINFO_4("Adviserd : %s : Unable to Disable the HUb", __FUNCTION__ );
						}
					}
				}
			}
		}
		SessionExit = FALSE;
		}
	if(post_stop_session_flag)
	{
		TINFO_4 ("Posting wake up for Avse stop session wait\n");
		sem_post (&m_ev_stop_session_complete);
	}

	if (video_handle)	
		dlclose(video_handle);
	if (session_handle)
		dlclose(session_handle);
}


/**
 * get_session_info
**/
session_info_t*
get_session_info (hsession_t hsession)
{
	if (m_session [hsession].is_used) { return &(m_session [hsession]); }
	else { return (session_info_t*) INVALID_SESSION_ID; }
}


/**
 * sm_tick_handler
**/
void
sm_tick_handler (int s)
{
	int	i;
	if(0)
	{
		s=s; /* -Wextra, fix for unused parameters */
	}
	for (i = 0; i < max_session; i++)
	{
		if (m_session [i].is_used)
		{
			if (m_session [i].is_valid)
			{
				m_session [i].activity_timer--;
				if (m_session [i].activity_timer == 0)
				{
					m_session [i].clean = TRUE;
				}
			}
			else
			{
				m_session [i].login_timer--;
				if (m_session [i].login_timer == 0)
				{
					m_session [i].clean = TRUE;
				}
			}
		}
	}
}


/**
 * sm_prepare_session_for_transfer
**/
void
sm_prepare_session_for_transfer (void)
{
	acquire_mutex (&m_global_guard);

	m_num_pending_clients		= 0;
	m_abort						= FALSE;
	m_abort_count				= 0;
	m_stop						= FALSE;

	release_mutex (&m_global_guard);
}

/**
 * sm_wait_for_transfer_complete
**/
void
sm_wait_for_transfer_complete (void)
{
	sem_wait (&m_ev_transfer_complete);
}

/**
 * sm_get_num_webpreview_sessions
**/
int
sm_get_num_webpreview_sessions (void)
{
	int i;
	int nsession = 0;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (&m_session_guard [i]);
		if (m_session [i].is_used && m_session [i].is_webPreviewer)
		{
			nsession++;
		}
		release_mutex (&m_session_guard [i]);
		
	}

	return nsession;
}

/**
 * sm_get_num_session
**/
int
sm_get_num_valid_sessions (void)
{
	int i;
	int nsession = 0;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (&m_session_guard [i]);
		if (m_session [i].is_used && m_session [i].is_valid)
		{
			nsession++;
		}
		release_mutex (&m_session_guard [i]);
		
	}

	return nsession;
}
/**
 * sm_get_num_session
**/
int
sm_get_kvm_num_session (void)
{
        int i;
        int nsession = 0;

        for (i = 0; i < max_session; i++)
        {
                acquire_mutex (&m_session_guard [i]);

                if (m_session [i].is_used && (!m_session [i].is_webPreviewer))
                {
                        nsession++;
                }

                release_mutex (&m_session_guard [i]);
        }

        return nsession;
}
/**
 * sm_get_num_session
**/
int
sm_get_num_session (void)
{
	int i;
	int nsession = 0;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (&m_session_guard [i]);

		if (m_session [i].is_used)
		{
			nsession++;
		}

		release_mutex (&m_session_guard [i]);
	}

	return nsession;
}


void
sm_set_abort_flag (hsession_t hsession)
{
	if(0)
	{
		hsession=hsession; /* -Wextra, fix for unused parameters */
	}
	acquire_mutex (&m_global_guard);
	m_abort = TRUE;
	release_mutex (&m_global_guard);
}

/**
 * set_refresh_on
**/
int
sm_set_refresh_on (hsession_t hsession)
{
	acquire_mutex (&m_global_guard);
	m_refresh	= TRUE;
	release_mutex (&m_global_guard);
	if(0)
	{
		hsession=hsession; /* -Wextra, fix for unused parameters */
	}
	return 0;
}


/**
 * sm_pause_redirection
**/
int
sm_pause_redirection (hsession_t hsession ,BOOL status)
{
	acquire_mutex (&m_session_guard [hsession]);
	m_session [hsession].pause_status =status;// True if the h5viewer window is minimized
	m_session [hsession].pause = TRUE;
	release_mutex (&m_session_guard [hsession]);

	return 0;
}

/**
 * sm_resume_redirection
**/
int
sm_resume_redirection (hsession_t hsession)
{
	acquire_mutex (&m_global_guard);
	acquire_mutex (&m_session_guard [hsession]);
	m_session [hsession].pause_status = FALSE;
	m_session [hsession].pause = FALSE;
	release_mutex (&m_session_guard [hsession]);
	release_mutex (&m_global_guard);

	return 0;
}

/**
 * skip_video
**/ 
int skip_video(session_list_t* session_list)
{
	int index=0; 
	if(session_list ==  NULL)
		return false;
	
	for (index = 0; index < session_list->num_session;index++)
	{
		acquire_mutex (session_list->p_session [index]->p_access_guard);
		if(session_list->p_session[index] != NULL)
		{//if any one of the session is jviewer, video is not skipped.
		if(session_list->p_session[index]->is_h5viewer != TRUE)
		{
			release_mutex (session_list->p_session [index]->p_access_guard);
			return false;
		}
		else
		{//if any one of the h5viewer session window is maximized, the video is not skipped
			if(session_list->p_session[index]->pause_status == FALSE)
			{
				release_mutex (session_list->p_session [index]->p_access_guard);
				return false;
			}
		}
		}
		release_mutex (session_list->p_session [index]->p_access_guard);
	}
	
	//if all the sessions are h5viewer and the window is minimized then video is skipped.
	return true;
}

/**
 * build_send_list
**/
void sm_build_send_list (session_list_t* send_list, session_list_t* session_list)
{
	int i = 0;
	u32 tsend = 0;
	u32 tswap = 0;
	u32 twait = 0;
	u32  send_size = 0;

	send_list->is_new_session = session_list->is_new_session;

	/* Get the session list*/
	sm_get_valid_session_list (session_list, FALSE);

	/* build the send list */
	send_list->num_session = 0;
	for (i = 0; i < session_list->num_session; i++)
	{
		//Skip sessions which transfer status was set to connection lost.
		if((session_list->p_session [i]->transfer_status == ERR_CONNECTION_LOST) ||
		(session_list->p_session [i]->ctrl_transfer_status == ERR_CONNECTION_LOST) )
		{
			continue;
		}
		//skip the session info if the session is h5viewer and in pause state
		if((session_list->p_session[i]->is_h5viewer == TRUE) && (session_list->p_session[i]->pause_status == TRUE))
		{
			continue;
		}	
		/* Get exclusive access to session info */
		acquire_mutex (session_list->p_session [i]->p_access_guard);

		/* Send it to only the sessions that have finished or there is a swap */
		if ((session_list->p_session [i]->tsend) == 0) 
		{
			tsend = (send_size * 1000) / session_list->p_session [i]->bandwidth;

			/**
			 * if time taken to send this packet is less than the time
			 * left for next swap or if this is a swap -
			 * add this to send list
			 **/
			if (tsend <= tswap)
			{
				/**
				 * Should not send packets to client in pause state
				 * Checking for the pause flag here which is set when client is paused
				 * Not adding that session to the send list if it is paused
				 **/
				if (session_list->p_session[i]->pause != TRUE)
				{
					send_list->p_session [send_list->num_session]         = session_list->p_session [i];
					send_list->p_session [send_list->num_session]->tsend += tsend;

					/* calculate the time left for the next capture */
					twait = (twait > tsend) ? tsend : twait;

					send_list->num_session++;
				}
			}
		}

		/**
		 * Time left for the next swap is the maximum of all the
		 * sessions send time
		 **/
		tswap = (tswap < session_list->p_session [i]->tsend) ? session_list->p_session [i]->tsend : tswap;

		/* Release exclusive access to session info */
		release_mutex (session_list->p_session [i]->p_access_guard);
	}
}

/**
 * sm_user_macro
**/
int sm_user_macro (hsession_t hsession)
{
	acquire_mutex (&m_session_guard [hsession]);
	m_session [hsession].is_send_user_macro= TRUE;
	release_mutex (&m_session_guard [hsession]);
	return 0;
}

void sm_build_user_macro_send_list (session_list_t* send_list, session_list_t* session_list)
{
	int	i;
	
	/* build the send list */
	send_list->num_session = 0;
	
	if(0)
	{
		session_list=session_list; /* -Wextra, fix for unused parameters */
	}
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_send_user_macro)
		{
			send_list->p_session [send_list->num_session]	= &(m_session [i]);
			send_list->p_session [send_list->num_session]->tsend	= 0;
			TINFO_4 ("Socket[%d] :%d\n", i, send_list->p_session [send_list->num_session]->socket);
			send_list->num_session++;
		}
		release_mutex (m_session [i].p_access_guard);
	}
}


void sm_build_keep_alive_send_list (session_list_t* send_list, session_list_t* session_list)
{
	int	i;

	/* build the send list */
	send_list->num_session = 0;
	if(0)
	{
		session_list=session_list; /* -Wextra, fix for unused parameters */
	}
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_alive)
		{
			send_list->p_session [send_list->num_session]	= &(m_session [i]);
			send_list->p_session [send_list->num_session]->tsend	= 0;
			send_list->num_session++;
		}
		release_mutex (m_session [i].p_access_guard);
	}
}

void 
sm_reset_user_macro_send_list  (void)
{
	int i;
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if ((m_session [i].is_used) && (m_session [i].is_valid)&& (m_session [i].is_send_user_macro))
		{
			m_session [i].is_send_user_macro = FALSE;
			TINFO_4 ("Session id to Reset:%d\n", i);
		}
		release_mutex (m_session [i].p_access_guard);
	}
}

void 
sm_reset_keep_alive_send_list  (session_list_t* p_session_list)
{
	int i;

	if (p_session_list->num_session == 0)
	{
		return ;
	}

	for (i = 0; i < p_session_list->num_session; i++)
	{
		session_info_t* p_session_info = p_session_list->p_session [i];

		/* ... Acquire exclusive access to the Client info */
		acquire_mutex (p_session_info->p_access_guard);
	
		p_session_info->is_alive = FALSE;

		/* ... Release exclusive access to the client info */
		release_mutex (p_session_info->p_access_guard);

	}
}

BOOL
sm_is_set_send_usermacro(void)
{
	int i;
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_send_user_macro)
		{
			TINFO_4 ("send MAcro flag\n");
			release_mutex (m_session [i].p_access_guard);
			return TRUE;
		}
		release_mutex (m_session [i].p_access_guard);
	}

	return FALSE;
}

BOOL
sm_is_set_keep_alive_flag(void)
{
	int i;
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_alive)
		{
			release_mutex (m_session [i].p_access_guard);
			return TRUE;
		}
		release_mutex (m_session [i].p_access_guard);
	}

	return FALSE;
}


/**
 * sm_set_stop_signal
**/
void
sm_set_stop_signal (unsigned short status)
{
	int i;

	TINFO_4 ("set stop signal\n");

	for (i = 0; i < max_session; i++)
	{
		// Acquiring mutex for m_session [i].p_access_guard is removed as 
		// this function will be invoked only in Signal handler.
		// Acquiring here may result in Deadlock.
		if ((m_session [i].is_used) && (m_session [i].is_valid))
		{
			m_session [i].is_stop_signal = TRUE;
			 g_stop_signal_status = status;
		}
	}

}


/**
 * sm_is_set_stop_signal
**/
BOOL
sm_is_set_stop_signal (void)
{
	int i;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_stop_signal)
		{
			TINFO_4 ("is stop signal\n");
			release_mutex (m_session [i].p_access_guard);
			return TRUE;
		}
		release_mutex (m_session [i].p_access_guard);
	}

	return FALSE;
}


/**
*set the socket status packet to send to the client
**/

//added for socket close
void 
sm_set_socket_close_ctrl_packet(session_info_t *session,unsigned short status)
{
	acquire_mutex (session->p_access_guard);
	session->is_socket_close_ctrl_pkt = TRUE;
	session->socket_close_status = status;
	release_mutex (session->p_access_guard);
}

/**
*Check the socket status packet is set to send the packet 
**/

session_info_t*  sm_is_socket_close_ctrl_packet()
{		int i;
	
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_socket_close_ctrl_pkt )
		{
			release_mutex (m_session [i].p_access_guard);
			return &m_session[i];
					
		}
		release_mutex (m_session [i].p_access_guard);
	}
	
	return NULL;
}
	
/**
*After send the socket status packet and reset the control packet flag
**/
void
sm_reset_socket_cntrl_pkt (session_info_t *p_session)
{
int i;
	if(0)
	{
		p_session=p_session; /* -Wextra, fix for unused parameters */
	}
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		m_session [i].is_socket_close_ctrl_pkt  = FALSE;
		release_mutex (m_session [i].p_access_guard);
	}
	


	return;
}
/**
 * sm_wait_for_stop_session
**/
void
sm_wait_for_stop_session_complete (void)
{
	sem_wait (&m_ev_stop_session_complete);
}


/**
 * is_any_session_encrypted ()
**/
BOOL
is_any_session_encrypted ()
{
	int 		i;
	int 		retval = FALSE;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if ((m_session [i].is_used) && (m_session [i].is_valid))
		{
			if (m_session [i].is_encrypted)
			{
				//TINFO_4 ("more active sessions with encryption:%d\n", i);
				printf("more active sessions with encryption:%d\n", i);
				retval = TRUE;
			}
		}
		release_mutex (m_session [i].p_access_guard);
	}

	return retval;

}


/**
 * set_encrypt_flag
**/
void
sm_set_encrypt_flag (hsession_t hsession)
{
	int i;
	acquire_mutex (m_session [hsession].p_access_guard);

	if (m_session [hsession].is_used)
	{
		m_session [hsession].is_encrypted = TRUE;
	}

	release_mutex (m_session [hsession].p_access_guard);
	/* encryption status has to be sent to clients */
	for ( i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard); 
		if ((m_session [i].is_used) && (m_session [i].is_valid))
		{
			if (!(m_session [i].is_encrypted)) 
			{
				 m_session [i].is_encrypted = true; 
			}
		}
		release_mutex (m_session [i].p_access_guard); 
	} 
		
	sm_set_send_encrypt_status (hsession);
}


/**
 * sm_reset_encrypt_flag
**/
void
sm_reset_encrypt_flag (hsession_t hsession)
{
	acquire_mutex (m_session [hsession].p_access_guard);

	if ((m_session [hsession].is_used) && (m_session [hsession].is_valid))
	{
		m_session [hsession].is_encrypted = FALSE;
	}

	release_mutex (m_session [hsession].p_access_guard);

	/*if still any other session still active with encryption revert all the changes */
	if (is_any_session_encrypted ())
	{
		if ((m_session [hsession].is_used) && (m_session [hsession].is_valid)) 
		{
			m_session [hsession].is_encrypted = TRUE; 
		}
		sm_set_send_encrypt_status (hsession); 
	}

}


/**
 * sm_set_send_encrypt_status_flag
**/
void
sm_set_send_encrypt_status (hsession_t hsession)
{

	acquire_mutex (m_session [hsession].p_access_guard);

	if ((m_session [hsession].is_used) && (m_session [hsession].is_valid))
	{
		m_session [hsession].is_send_encrypt_status = TRUE;
	}

	release_mutex (m_session [hsession].p_access_guard);

}


/**
 * sm_is_set_send_encrypt_status
**/
BOOL
sm_is_set_send_encrypt_status (void)
{
	int 		i;
	int 		retval = FALSE;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if ((m_session [i].is_used) && (m_session [i].is_valid))
		{
			if ( m_session [i].is_send_encrypt_status )
			{
				TINFO_4 ("Send Encrypt Status Set:%d\n", i);
				retval = TRUE;
			}
		}
		release_mutex (m_session [i].p_access_guard);
	}

	return retval;
}


/**
 * sm_reset_send_encrypt_status
**/
void
sm_reset_send_encrypt_status (void)
{

	int i;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);

		if ((m_session [i].is_used) && (m_session [i].is_valid)
			&& (m_session [i].is_send_encrypt_status))
		{
			m_session [i].is_send_encrypt_status = FALSE;
		}

		release_mutex (m_session [i].p_access_guard);
	}

}
/**
 * sm_set_send_auto_bw_detect
**/
void
sm_set_send_auto_bw_detect (hsession_t hsession, u32 auto_bw_size)
{
	TINFO_4 ("set send Auto Detect BW flag:%d size:%ld\n", hsession, auto_bw_size);
	
	acquire_mutex (m_session [hsession].p_access_detect_auto_bw);
	acquire_mutex (m_session [hsession].p_access_guard);

	if ((m_session [hsession].is_used) && (m_session [hsession].is_valid))
	{
		m_session [hsession].is_auto_bw_detect 	= TRUE;
		m_session [hsession].auto_bw_pkt_size	= auto_bw_size;
	}

	release_mutex (m_session [hsession].p_access_guard);
	release_mutex (m_session [hsession].p_access_detect_auto_bw);
}


/**
 * sm_is_set_auto_bw_detect
**/
BOOL
sm_is_set_auto_bw_detect (void)
{
	int i;
	BOOL retval = FALSE;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_auto_bw_detect)
		{
			retval = TRUE;
		}
		release_mutex (m_session [i].p_access_guard);
	}

	return retval;
}


/**
 * sm_build_auto_bw_detect_send_list
**/
void
sm_build_auto_bw_detect_send_list (session_list_t* send_list, session_list_t* session_list)
{
	int 			i;

	/* build the send list */
	send_list->num_session = 0;

	if(0)
	{
		session_list=session_list; /* -Wextra, fix for unused parameters */
	}
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_auto_bw_detect)
		{
			send_list->p_session [send_list->num_session] 			= &(m_session [i]);
			send_list->p_session [send_list->num_session]->tsend  	= 0;
			TINFO_4 ("Socket[%d] :%d\n", i, send_list->p_session [send_list->num_session]->socket);
			send_list->num_session++;
		}
		release_mutex (m_session [i].p_access_guard);
	}
}


/**
 * sm_reset_auto_bw_detect
**/
void
sm_reset_auto_bw_detect (session_list_t* send_list)
{
	int i;

	for (i = 0; i < send_list->num_session; i++)
	{
		acquire_mutex (send_list->p_session [i]->p_access_guard);

//		if ((m_session [i].is_used) && (m_session [i].is_valid)
//			&& (m_session [i].is_auto_bw_detect))
//		{
			send_list->p_session [i]->is_auto_bw_detect = FALSE;
			send_list->p_session [i]->auto_bw_pkt_size 	= 0;
//		}

		release_mutex (send_list->p_session [i]->p_access_guard);
	}

}


/**
 * sm_get_valid_session_list
**/
int
sm_get_valid_session_list (session_list_t* p_session_list, BOOL clear_new_flag)
{
	int		i;
	int nH5session = 0;
	int curr_fps_diff = 0;

	p_session_list->num_session = 0;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (&m_session_guard [i]);

		if (m_session [i].is_used && m_session [i].is_valid)
		{
			p_session_list->p_session [p_session_list->num_session] = &(m_session [i]);
			p_session_list->num_session++;
			//if h5viewer is session is active 
			if ((m_session [i].is_h5viewer == TRUE) && 
				(m_session [i].pause_status == FALSE)) 
			{ 
				nH5session++; 
				// update the least fps count to curr_fps_diff 
				// so that when there are more than one h5 session 
				// the number of frames sent will be reduced such that 
				// the least performing h5 client could paint the received frames. 
				if((curr_fps_diff <= 0) || (m_session[i].fps_diff < curr_fps_diff)) 
				{ 
					curr_fps_diff = m_session[i].fps_diff; 
				} 
			}			
		}

		release_mutex (&m_session_guard [i]);
	}

	acquire_mutex (&m_global_guard);
	g_is_h5viewer_active = (nH5session > 0) ? 1 : 0;
	p_session_list->is_new_session 		= m_is_new_session;
	g_fps_diff = curr_fps_diff;
	if (clear_new_flag) 	{ m_is_new_session = FALSE; }
	release_mutex (&m_global_guard);

	return 0;
}

/**
 * sm_close_associates
**/
void sm_close_associates()
{
	int cdfd = 0;
	int fdfd = 0;
	pid_t cdpid = 0;
	pid_t fdpid = 0;


	/**
	 * Opening and reading the CD and FD PID file for acquiring the PID of those processes
	**/
	cdfd = open(CD_PID_FILE, O_RDONLY);
	if (!cdfd)
	{
		printf("Unable to open the PID file for CDServer\n");
		return;
	}

	fdfd = open(FD_PID_FILE, O_RDONLY);
	if (!fdfd)
	{
		printf("Unable to open the PID file for FDServer\n");
		close(cdfd);
		return;
	}

	if (read(cdfd, &cdpid, sizeof(cdpid)) != sizeof(cdpid))
	{
		printf("Unable to read the PID for CDServer\n");
		close(cdfd);
		close(fdfd);
		return;
	}

	if (read(fdfd, &fdpid, sizeof(fdpid)) != sizeof(fdpid))
	{
		printf("Unable to read the PID for FDServer\n");
		close(cdfd);
		close(fdfd);
		return;
	}		
	
	/**
     * These PIDs are associated with the in-active video session. Closing it too.
	**/
	printf("Closing the Active Media Connections...\n");	
	kill(cdpid, SIGUSR1);
	kill(fdpid, SIGUSR1);	
	
	close(cdfd);
	close(fdfd);
}	

/**
 * sm_set_sessiontoken
**/
void
sm_set_sessiontoken (hsession_t hsession, char* stoken)
{
	int ret;
	acquire_mutex (&m_session_guard [hsession]);

	ret=snprintf (m_session [hsession].session_token, HASH_SIZE+1,"%s", stoken);
	if(ret>=HASH_SIZE+1)
	{
		TCRIT("Buffer overflow\n");
	}


	/* In case of G4/X2 the session token will be used as a key for encryption
	   handle generation. Assign session token to encryption key field of session
	   info structure
	*/
	TINFO_4 ("setting encrypt key\n");
	ret=snprintf ((char *)m_session [hsession].encrypt_key, HASH_SIZE+1, "%s",stoken);
	if(ret>=HASH_SIZE+1)
		{
		TCRIT("Buffer overflow\n");
		}
	release_mutex (&m_session_guard [hsession]);

}


// ALERT ALERT
// This function holds good only if the max client connections is only 2.
// If max client connection is changed, this function also must be changed accordingly
session_info_t *
sm_get_first_session()
{

	session_info_t	*first_session = NULL;
		int i;
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (&m_session_guard [i]);
		
		if (m_session [i].is_used && m_session [i].is_valid && (! m_session [i].is_webPreviewer) )
		{
			first_session = &m_session [i];
		}
		release_mutex (&m_session_guard [i]);

	}

	return first_session;
}


/**
*Add the session information to pending session info structure until the permission given by the first client
**/
void
sm_add_pending_session( session_info_t *slave,unsigned char * user_name,unsigned char * client_ip,auth_t *auth_pkt)
{
	struct sysinfo sys_info;
	int index = slave->h_session,ret ; 

	if (index < 0 && index > max_session)
		return ;
	
	acquire_mutex (&m_pending_session_guard[index]);	

	m_pending_session[index].pending_state = PEND_STATE_WAIT_IN_PROGRESS;
	if(!sysinfo(&sys_info))
	{
	 	m_pending_session[index].start_time   = sys_info.uptime;
	}
	m_pending_session[index].slave_sess_index = slave->h_session;
	ret=snprintf(m_pending_session[index].user_name,TOKENCFG_MAX_USERNAME_LEN,"%s",(char *)user_name);
	if(ret>=TOKENCFG_MAX_USERNAME_LEN)
	{
		TCRIT("Buffer overflow\n");
	}
	ret=snprintf(m_pending_session[index].client_ip,TOKENCFG_MAX_CLIENTIP_LEN,"%s",(char *)client_ip);
	if(ret>=TOKENCFG_MAX_CLIENTIP_LEN)
		{
		TCRIT("Buffer overflow\n");
		}

	
	if(auth_pkt != NULL)
		memcpy(&m_pending_session[index].auth_pkt, auth_pkt, sizeof(m_pending_session[index].auth_pkt) );
	release_mutex (&m_pending_session_guard[index]);
}

/**
 * Wakes up the auto-record thread waiting on this semaphore
 * Will not wake up if already a recording is in progress
**/
void
sm_wake_up_record_thread (void)
{
	void                    *soc_handle = NULL;
	int                     (*start_video_recording) (void);

        soc_handle = dlopen(VIDEORECORD_LIBRARY, RTLD_LAZY);
        if(!soc_handle)
        {
        	TCRIT("%s\n",dlerror());
        }

        // This will call definition to update the soc session hdr
        else
        {
                start_video_recording = dlsym(soc_handle, START_VIDEO_RECORDING);
                if (start_video_recording)
                {
                        if (start_video_recording () < 0)
			{
				TINFO("Unable to start the video recording\n");
			}
                }

                dlclose(soc_handle);
        }
}

/**
*set the KVM privelage flag for sending the packet to the client
**/
void
sm_set_kvm_priv_flag (session_info_t *first_session, unsigned short status)
{
	acquire_mutex (first_session->p_access_guard);
	first_session->is_kvm_priv_ctrl_pkt = TRUE;
	first_session->kvm_pkt_status = status;
	release_mutex (first_session->p_access_guard);
}

/**
 * sm_set_keep_alive_flag
**/
int sm_set_keep_alive_flag (hsession_t hsession)
{
	acquire_mutex (&m_session_guard [hsession]);
	m_session [hsession].is_alive = TRUE;
	release_mutex (&m_session_guard [hsession]);
	return 0;
}

/**
*Check the previlage flag for sending the packet to the client
**/

session_info_t *
sm_get_kvm_priv_flag_session()
{
	int i;
	
	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_kvm_priv_ctrl_pkt == TRUE)
		{
			release_mutex (m_session [i].p_access_guard);
			
			return &m_session[i];
		}
		release_mutex (m_session [i].p_access_guard);
	}

	return NULL;

}

/**
*Reset the KVM previlage packet for sent packet
**/
void
sm_reset_kvm_priv_flag (session_info_t *p_session)
{
	acquire_mutex (p_session->p_access_guard);
	p_session->is_kvm_priv_ctrl_pkt = FALSE;
	p_session->kvm_pkt_status = 0;
	release_mutex (p_session->p_access_guard);

	return;
}

session_info_t * 
sm_check_valid_session(ip_usr_datapkt *nextmaster)
{
	int sess_index = nextmaster->sess_index;
 
	 if (m_session [sess_index].is_used && m_session [sess_index].is_valid && (! m_session [sess_index].is_webPreviewer) )
	{
		return  &m_session [sess_index];
	}

	return  NULL;
	 
}
/**
*set the KVM master change flag for sending the packet to the client
**/
void
sm_set_master_change_flag (session_info_t *first_session)
{
	acquire_mutex (first_session->p_access_guard);
	first_session->is_master_change = TRUE;
	release_mutex (first_session->p_access_guard);
}
 
 /**
 *Check the master chahnge flag for sending the packet to the client
 **/ 
 session_info_t *
 sm_get_kvm_master_change_session()
 {
	 int i;
	 
	 for (i = 0; i < max_session; i++)
	 {
		 acquire_mutex (m_session [i].p_access_guard);
		 if (m_session [i].is_master_change == TRUE)
		 {
			 release_mutex (m_session [i].p_access_guard);
			 
			 return &m_session[i];
		 }
		 release_mutex (m_session [i].p_access_guard);
	 } 
	 return NULL; 
 }

/**
*Reset the KVM master change packet for sent packet
**/
void
sm_reset_master_change_flag (session_info_t *p_session)
{
	acquire_mutex (p_session->p_access_guard);
	p_session->is_master_change = FALSE;
	release_mutex (p_session->p_access_guard);

	return;
}

/**
 * sm_set_pending_client_response
**/
void
sm_set_pending_client_response ()
{	
	acquire_mutex (&m_global_guard);
	g_pending_client_send = TRUE;
	release_mutex (&m_global_guard);
	
	return ;
}

/**
 * sm_reset_pending_client_response
**/
void
sm_reset_pending_client_response ()
{	
	acquire_mutex (&m_global_guard);
	g_pending_client_send = FALSE;
	release_mutex (&m_global_guard);
	
	return ;
}

/**
 * sm_is_set_pending_client_response
**/
BOOL
sm_is_set_pending_client_response()
{
	BOOL ret=FALSE;
	acquire_mutex (&m_global_guard);
	ret=	g_pending_client_send;
	release_mutex (&m_global_guard);
	return ret;
}


void sm_build_pending_session_send_list (session_list_t* send_list, session_list_t* session_list)
{
	int	index,retval;
	session_info_t *slave_session = NULL;
	
	/* build the send list */
	send_list->num_session = 0;
	
	if(0)
	{
		session_list=session_list; /* -Wextra, fix for unused parameters */
	}
	for (index = 0; index < max_session;index++)
	{
		if( m_pending_session[index].pending_state == PEND_STATE_WAIT_IN_PROGRESS )
		{
			acquire_mutex (&m_pending_session_guard[index]);
			slave_session = &m_session[m_pending_session[index].slave_sess_index];
			if(slave_session != NULL)
			{
				if((slave_session->is_valid == FALSE) && (slave_session->kvm_priv == KVM_REQ_PROGRESS))
				{
					//Approve the new session first before sendign master switch pkt
					retval=approve_session(slave_session, &m_pending_session[index].auth_pkt,m_pending_session[index].user_name,m_pending_session[index].client_ip, KVM_REQ_PARTIAL);
					if(retval!=0)
					{
						TCRIT("Error in approve_session function\n");
						return ;
					}
				}
				//Send the KVM Master session change status to slave sessions alone
				if(slave_session->kvm_priv != KVM_REQ_ALLOWED)
				{
					//Add to pending session list to send packet to notify master switch
					send_list->p_session [send_list->num_session]	= &(m_session[slave_session->h_session]);
					send_list->p_session [send_list->num_session]->tsend	= 0;
					send_list->num_session++;
				}

				memset(&m_pending_session[index], 0, sizeof(PENDING_SESSION) );
			}
		}
		release_mutex (&m_pending_session_guard[index]);
	}
}
/**
*Method check the permission given by First user and send the status to the second user
**/
void
sm_kvm_priv_take_action(session_info_t *p_si, unsigned char status,int index)
{
	unsigned char kvm_priv;
	int new_session = 0,retval=0;

	session_info_t *master_session = NULL;
	session_info_t *slave_session = &m_session[m_pending_session[index].slave_sess_index];
	
	if(0)
	{
		p_si=p_si; /* -Wextra, fix for unused parameters */
	}
	if( m_pending_session[index].pending_state != PEND_STATE_WAIT_IN_PROGRESS )
	{
		TINFO_4("ALERT ALERT. Got kvm_priv action call even though pending state is not WAIT_IN_PROGRESS");
		return;
	}
	
	acquire_mutex (&m_pending_session_guard[index]);	

	if( slave_session ==NULL)
	{
		release_mutex (&m_pending_session_guard[index]);
		return;
	}
	if( slave_session->is_valid ==TRUE )
		new_session = 1;

	if(new_session == 0) //0 menant for YES
		slave_session->KVM_pkt = IVTP_KVM_SHARING;
	else
		slave_session->KVM_pkt = IVTP_SET_NEXT_MASTER;
	
	slave_session->kvm_pkt_status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + ( ((u16)status) <<8 ) );
	sm_set_kvm_priv_slave_sess(slave_session);
	sm_set_kvm_priv_resp_send();

	kvm_priv = (unsigned char)((status & 0xFF) );

	
	switch(kvm_priv)
		{

		case KVM_REQ_ALLOWED :
			if(new_session == 0) //0 menant for YES
			{
				retval=approve_session(slave_session, &m_pending_session[index].auth_pkt,m_pending_session[index].user_name,m_pending_session[index].client_ip, kvm_priv);
				if(retval!=0)
				{
					TCRIT("Error in approve_session function\n");
					return ;
				}
			}
			else
			{
				slave_session->kvm_priv = KVM_REQ_ALLOWED;
			}

			master_session = sm_get_master_session();
			master_session->KVM_pkt = IVTP_KVM_SHARING;
			master_session->kvm_pkt_status = (unsigned short) (STATUS_KVM_PRIV_SWITCH_USER + (KVM_REQ_PARTIAL<<8) );
			sm_set_kvm_priv_master_sess(master_session);
			sm_set_kvm_priv_resp_send();

			master_session->kvm_priv = KVM_REQ_PARTIAL;
			sm_set_master_sess_index(slave_session->h_session);

			memset(&m_pending_session[index], 0, sizeof(PENDING_SESSION) );

			break;

		case KVM_REQ_DENIED :	
				memset(&m_pending_session[index], 0, sizeof(PENDING_SESSION) );
				break;


		case KVM_REQ_PARTIAL :	
		case KVM_REQ_BLOCKED :	
			if(new_session == 0) //0 menant for YES
				retval=approve_session(slave_session, &m_pending_session[index].auth_pkt,m_pending_session[index].user_name,m_pending_session[index].client_ip, kvm_priv);
				if(retval!=0)
				{
					TCRIT("Error in approve_session function\n");
					return ;
				}
			else
				slave_session->kvm_priv = KVM_REQ_PARTIAL;

			memset(&m_pending_session[index], 0, sizeof(PENDING_SESSION) );
				break;

		default :

				TCRIT("No Proper Privilege	from Client 1.");	
								
					break;

					
		}

		release_mutex (&m_pending_session_guard[index]);
		
}

/**
 * This function will set the flag that will be used to check whether to send the
 * KVM privilege response packet or not.
 **/
void
sm_set_kvm_priv_resp_send()
{
	acquire_mutex (&m_global_guard);
	g_kvm_priv_resp_send = TRUE;
	release_mutex (&m_global_guard);
	return ;
}

/**
 * This function will reset the flag that will be used to check whether to send the
 * KVM privilege response packet or not.
 **/
void
sm_reset_kvm_priv_resp_send()
{
	acquire_mutex (&m_global_guard);
	g_kvm_priv_resp_send = FALSE;
	release_mutex (&m_global_guard);
	return ;
}

/**
 * This function returns the status of the g_kvm_priv_resp_send flag;
 **/
BOOL
sm_is_kvm_priv_resp_send()
{
	BOOL ret;
	acquire_mutex (&m_global_guard);
	ret = g_kvm_priv_resp_send;
	release_mutex (&m_global_guard);
	return ret;
}

/**
 * This function sets the session information regarding the slave session to which
 * KVM sharing packet is being sent. 
 **/
void
sm_set_kvm_priv_slave_sess(session_info_t *session)
{
	acquire_mutex (&m_global_guard);
	m_kvm_priv_slave_sess = session;
	release_mutex (&m_global_guard);
	return ;
}

/**
 * This function gets the session information regarding the slave session to which
 * KVM sharing packet is being sent. 
 **/
session_info_t *
sm_get_kvm_priv_slave_sess()
{
	return m_kvm_priv_slave_sess;
}

/**
 * This function sets the session information regarding the master session to which
 * KVM sharing packet is being sent. 
 **/
void
sm_set_kvm_priv_master_sess(session_info_t *session)
{
	acquire_mutex (&m_global_guard);
	m_kvm_priv_master_sess = session;
	release_mutex (&m_global_guard);
	return ;
}

/**
 * This function gets the session information regarding the master session to which
 * KVM sharing packet is being sent. 
 **/
session_info_t *
sm_get_kvm_priv_master_sess()
{
	return m_kvm_priv_master_sess;
}

/**
 * This function resets the session information regarding the master session to which
 * KVM sharing packet has been sent.
 * The KVM packet type and the kvm packet status will be reset in this case.
 **/
void
sm_reset_kvm_priv_send_sess_info(session_info_t *session)
{
	acquire_mutex (&m_global_guard);
	session->KVM_pkt = 0;
	session->kvm_pkt_status = -1;
	release_mutex (&m_global_guard);
	return ;
}
/**
 * This function is to send approval command to the client
 * Whenever a client connects to the server, it waits for an approval from the server to proceed
 * If the server approves the connection, it sends a command and it creates a session for the same
 * and similarly for disconnection also.
**/
int send_session_approval (int socket, BOOL approval,int status)
{
	ivtp_hdr_t v_hdr;
	int retval = 0;
	char *macaddresses = NULL;
	if (approval == TRUE)
	{
		int len = 0;
		int numsession = -1;

		if (g_corefeatures.allow_multiple_kvm_session_from_same_client != ENABLED)
		{
			numsession = sm_get_num_valid_sessions();
			if(numsession>0)
			{
				len  = (numsession*TOKENCFG_MAX_MAC_LEN)+1;
				macaddresses = (char*)malloc(len);
				if(macaddresses==NULL)
				{
					TCRIT("Memory not allocated to macaddresses\n");
					return -1;
				}
				memset(macaddresses,0,len);
			}
		}

		v_hdr.type = IVTP_SESSION_ACCEPTED;
		v_hdr.pkt_size 	= len;
		v_hdr.status 	= 0;

		if ( -1 == send (socket, &v_hdr, sizeof (ivtp_hdr_t), MSG_NOSIGNAL) )
		{
			TCRIT("Could not send accepted approval packet to the client\n");
			retval = -1;
		}

		if( (g_corefeatures.allow_multiple_kvm_session_from_same_client != ENABLED) && (numsession >0))
		{
			if(retval == 0)
			{
				sm_get_active_client_mac(macaddresses,len);
				if( send(socket, macaddresses, len, MSG_NOSIGNAL) == -1 )
				{
					TCRIT("Error while sending client mac addresses.");
					retval = -1;
				}
			}

			if((numsession >0)  &&(len>0))
			{
				if(macaddresses != NULL)
				{
				free (macaddresses);
				macaddresses = NULL;
				}
			}
		}
	}
	else
	{
		v_hdr.type = IVTP_MAX_SESSION_CLOSING;
		v_hdr.pkt_size 	= 0;
		v_hdr.status 	= status;
		if ( -1 == send (socket, &v_hdr, sizeof (ivtp_hdr_t), MSG_NOSIGNAL) )
		{
			TCRIT("Could not send failure packet to the client\n");
			retval =-1;
		}
	}
	if(macaddresses != NULL)
	{
		free (macaddresses);
		macaddresses = NULL;
	}
	return retval ;
}


/**
 * sm_is_power_status_send
**/
session_info_t *
sm_is_power_status_send ( )
{
	int i;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_power_status_send)
		{
			release_mutex (m_session [i].p_access_guard);
			
			return &m_session[i];
		}
		release_mutex (m_session [i].p_access_guard);
	};

	return NULL;
}

/**
 * sm_is_power_status_send
**/
session_info_t *
sm_is_power_cmd_status_send( )
{
	int i;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].is_power_cmd_status_send)
		{
			release_mutex (m_session [i].p_access_guard);
			
			return &m_session[i];
		}
		release_mutex (m_session [i].p_access_guard);
	};

	return NULL;
}


/**
 * sm_set_power_status_send
**/
void
sm_set_power_status_send (hsession_t hsession)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
	m_session [hsession].is_power_status_send = TRUE;
		
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}

/**
 * sm_set_power_status_send
**/
void
sm_set_power_status (int host_status)
{
	
	acquire_mutex (&m_global_guard);
	g_host_power_status = host_status;
	release_mutex (&m_global_guard);
	
	return ;
}
/**
 * sm_reset_power_status_send
**/
void
sm_reset_power_status_send (hsession_t hsession)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
	m_session [hsession].is_power_status_send = FALSE;
		
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}

/**
 * sm_set_power_status_send
**/
void
sm_set_power_crl_resp (hsession_t hsession,int response)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
	m_session [hsession].host_power_cmd_status = response;
		
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}


/**
 * sm_set_power_status_send
**/
void
sm_set_power_crl_resp_send (hsession_t hsession)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
	m_session [hsession].is_power_cmd_status_send = TRUE;
		
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}

 

 
/**
 * sm_reset_power_status_send
**/
void
sm_reset_power_crl_resp_send (hsession_t hsession)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
	if(m_session [hsession].is_power_cmd_status_send > 0)
		m_session [hsession].is_power_cmd_status_send--;
		
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}

/**
 * sm_set_raw_ipmi_resp_send
**/
void
sm_set_raw_ipmi_resp_send (hsession_t hsession)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
		m_session [hsession].send_raw_ipmi_data_count++;
		
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}

/**
 * sm_reset_raw_ipmi_resp_send
**/
void
sm_reset_raw_ipmi_resp_send (hsession_t hsession, int index)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
	if(m_session [hsession].send_raw_ipmi_data_count > 0)
		m_session [hsession].send_raw_ipmi_data_count--;
	// changed the resetting of the dwresplength from 0 to -1
	// because if it is 0 then it might mean that the command response has no data
	m_session [hsession].sess_ipmi_res[index].dwresplength = -1;
	if(m_session [hsession].sess_ipmi_res[index].ipmi_resp_data != NULL)
	{
		free(m_session [hsession].sess_ipmi_res[index].ipmi_resp_data);
		m_session [hsession].sess_ipmi_res[index].ipmi_resp_data=NULL;
	}
	m_session [hsession].sess_ipmi_res[index].raw_cmd_sucess = 0;
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}

/**
 * sm_is_send_raw_ipmi_resp_send
**/
session_info_t *
sm_is_send_raw_ipmi_resp_send( )
{
	int i;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (m_session [i].p_access_guard);
		if (m_session [i].send_raw_ipmi_data_count > 0)
		{
			release_mutex (m_session [i].p_access_guard);
			
			return &m_session[i];
		}
		release_mutex (m_session [i].p_access_guard);
	};

	return NULL;
}

/**
 * sm_set_host_lock_status
**/
void
sm_set_host_lock_status ()
{
	acquire_mutex (&m_global_guard);
	g_host_lock_status_send = 1;
	release_mutex (&m_global_guard);
	return ;
}
/**
 * sm_reset_host_lock_status
**/
void
sm_reset_host_lock_status ()
{
	acquire_mutex (&m_global_guard);
	g_host_lock_status_send = 0;
	release_mutex (&m_global_guard);
	return ;
}
/**
 * sm_is_host_lock_status
**/
BOOL 
sm_is_host_lock_status( )
{
	BOOL ret;
	acquire_mutex (&m_global_guard);
	ret =g_host_lock_status_send ;
	release_mutex (&m_global_guard);

	return ret ;
}

/**
 * sm_is_mouse_media_status
**/
BOOL 
sm_is_mouse_media_status( )
{
	BOOL ret;
	acquire_mutex (&m_global_guard);
	ret =g_mouse_media_status ;
	release_mutex (&m_global_guard);

	return ret ;
}


/**
 * sm_set_mouse_media_status
**/
void
sm_set_mouse_media_status ()
{
	
	// Acquiring mutex for m_global_guard is removed as this function will be invoked 
	// only in Signal handler. Acquiring here may result in Deadlock.
	g_mouse_media_status = TRUE;
	return ;
}

/**
 * sm_set_mouse_media_status
**/
void
sm_reset_mouse_media_status ()
{

	acquire_mutex (&m_global_guard);
	g_mouse_media_status = FALSE;
	release_mutex (&m_global_guard);
	
	return ;
}


/**
 * sm_set_vmedia_update_status
**/
void
sm_set_vmedia_update_status ()
{
	acquire_mutex (&m_global_guard);
	g_vmedia_update_status = 1;
	release_mutex (&m_global_guard);
	return ;
}
/**
 * sm_reset_vmedia_update_status
**/
void
sm_reset_vmedia_update_status ()
{
	acquire_mutex (&m_global_guard);
	g_vmedia_update_status = 0;
	release_mutex (&m_global_guard);
	return ;
}
/**
 * sm_is_vmedia_update_status
**/
BOOL 
sm_is_vmedia_update_status ( )
{
	BOOL ret;
	acquire_mutex (&m_global_guard);
	ret =g_vmedia_update_status ;
	release_mutex (&m_global_guard);

	return ret ;
}
/**
 * sm_setactive_client_send
**/
void
sm_set_active_client_send ()
{
	
	acquire_mutex (&m_global_guard);
	g_active_client_send = TRUE;
	release_mutex (&m_global_guard);
	
	return ;
}


/**
 * sm_reset_active_client_send
**/
void
sm_reset_active_client_send ()
{
	
	acquire_mutex (&m_global_guard);
	g_active_client_send = FALSE;
	release_mutex (&m_global_guard);
	
	return ;
}

/**
 * sm_is_set_active_client_send
**/
BOOL
sm_is_set_active_client_send ()
{
	BOOL ret=FALSE;
	
	acquire_mutex (&m_global_guard);
	ret=	g_active_client_send;
	release_mutex (&m_global_guard);
	
	return ret;
}

/**
 * sm_set_power_status_send_all
**/
void
sm_set_power_status_send_all ()
{
	acquire_mutex (&m_global_guard);
	g_power_status_send= TRUE;
	release_mutex (&m_global_guard);
	
	return ;
}

/**
 * sm_reset_power_status_send_all
**/
void
sm_reset_power_status_send_all ()
{
	acquire_mutex (&m_global_guard);
	g_power_status_send = FALSE;
	release_mutex (&m_global_guard);
	
	return ;
}

/**
 * sm_is_set_power_status_send_all
**/
BOOL
sm_is_set_power_status_send_all ()
{
	BOOL ret=FALSE;
	
	acquire_mutex (&m_global_guard);
	ret=	g_power_status_send;
	release_mutex (&m_global_guard);
	
	return ret;
}

/**
 * sm_build_active_clietns
**/
void
sm_build_active_client_info ( char *ipuserdata)
{
	int 		index = 0;
	int		session_count = 0;
#ifdef JVIEWER_DEBUG
	char 		samplename[TOKENCFG_MAX_USERNAME_LEN]="NAME1\0";
	char 		sampleip[TOKENCFG_MAX_CLIENTIP_LEN+1]="10.244.333.44";
#endif
	ip_usr_datapkt	*userpkt = (ip_usr_datapkt *)ipuserdata;

	// Loop through maximum possible sessions
	// Make sure that the session in concer is not cleaned/webpreview and is valid
	// If so, we copy that info to our buffer else skip session
	for(index = 0; index < max_session; index++)
	{
		acquire_mutex (m_session [index].p_access_guard);

		if( (m_session [index].is_valid) && (m_session [index].is_used) && (!m_session [index].clean) && (!m_session [index].is_webPreviewer) )
		{
			if (session_count >= kvm_max_session)
			{
				TINFO ("Maximum KVM Sessions reached. Spurious !!!\n");
				release_mutex (m_session [index].p_access_guard);
				break;
			}
			memset(userpkt->username_othersession,0,TOKENCFG_MAX_USERNAME_LEN);
			memset(userpkt->ip_add_othersession,0,TOKENCFG_MAX_CLIENTIP_LEN+1);
			#ifdef JVIEWER_DEBUG
				sprintf(samplename, "NAME%d", index);
				sprintf(sampleip, "10.0.6.%d", index);
				strncpy(userpkt->username_othersession, samplename, TOKENCFG_MAX_USERNAME_LEN-1);
				strncpy(userpkt->ip_add_othersession, sampleip,TOKENCFG_MAX_CLIENTIP_LEN);
				userpkt->sess_index = (char)index;
				
			#else
				strncpy(userpkt->username_othersession, m_session[index].username, TOKENCFG_MAX_USERNAME_LEN-1);
				strncpy(userpkt->ip_add_othersession, m_session [index].ip_addr_client, TOKENCFG_MAX_CLIENTIP_LEN);
				userpkt->sess_index = index;
				
			#endif
			userpkt->ipmi_priv = m_session [index].ipmiprev;
			session_count++;
			userpkt++;

		}


		release_mutex (m_session [index].p_access_guard);
	}

	return ;
}

/**
 * sm_get_num_pending_sessions
**/
int 
sm_get_num_pending_sessions()
{
	int i;
	int nsession = 0;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (&m_pending_session_guard [i]);

		if ( m_pending_session[i].pending_state == PEND_STATE_WAIT_IN_PROGRESS)
		{
			nsession++;
		}
		release_mutex (&m_pending_session_guard [i]);
	}

	return nsession;
}

/**
*Peridically this methos check the pending session is available if available send the timeout packet to the client
**/
void 
sm_check_pending_sessions()
{

	int i = 0,retval;
	session_info_t 	*master_session = sm_get_master_session();
	session_info_t 	*slave_session = NULL;

	for (i = 0; i < max_session; i++)
	{
		acquire_mutex (&m_pending_session_guard[i]);

		if( m_pending_session[i].pending_state == PEND_STATE_WAIT_IN_PROGRESS )
		{
			time_t	currtime;
			time(&currtime);

			if(sm_is_session_reconn(master_session->h_session) == TRUE)
			{
				//
				//////////////////////////////
				// send packet to second session saying that first user is in reconnect, so default privleges are assigned.
				// normal send....
				///////////////////////////////////

				slave_session = &m_session[m_pending_session[i].slave_sess_index];

				if( NULL != slave_session)
				{
					slave_session->KVM_pkt = IVTP_KVM_SHARING;
					slave_session->kvm_pkt_status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + (KVM_REQ_MASTER_RECONN<<8) );
					sm_set_kvm_priv_slave_sess(slave_session);
					sm_set_kvm_priv_resp_send();

					if(slave_session->is_valid == FALSE)
					{
						retval=approve_session(slave_session, &m_pending_session[i].auth_pkt,m_pending_session[i].user_name,m_pending_session[i].client_ip, KVM_REQ_ALLOWED);
						if(retval!=0)
						{
							TCRIT("Error in approve_session function\n");
							return ;
						}
					}
					slave_session->kvm_priv = KVM_REQ_ALLOWED;
					sm_set_pending_client_response();
				}
			}
			else if( IsTimedOut(m_pending_session[i].start_time,PENDING_SESSION_TIMEOUT) ||  IsTimedOut(m_pending_session[i].start_time, 0))
			{
				if(master_session !=NULL)
				{
					master_session->KVM_pkt = IVTP_KVM_SHARING;
					master_session->kvm_pkt_status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER1 + (KVM_REQ_TIMEOUT<<8) );
					sm_set_kvm_priv_master_sess(master_session);
					sm_set_kvm_priv_resp_send();
					master_session->kvm_priv = KVM_REQ_PARTIAL;
				}
				//
				//////////////////////////////
				// send packet to second session saying that first user failed to respond within given
				// time, so default privleges are assigned.
				// normal send....
				///////////////////////////////////

				slave_session = &m_session[m_pending_session[i].slave_sess_index];

				if( NULL != slave_session)
				{
					slave_session->KVM_pkt = IVTP_KVM_SHARING;
					slave_session->kvm_pkt_status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + (KVM_REQ_TIMEOUT<<8) );
					sm_set_kvm_priv_slave_sess(slave_session);
					sm_set_kvm_priv_resp_send();

					if(slave_session->is_valid == FALSE)
					{
						retval=approve_session(slave_session, &m_pending_session[i].auth_pkt,m_pending_session[i].user_name,m_pending_session[i].client_ip, KVM_REQ_ALLOWED);
						if(retval!=0)
						{
							TCRIT("Error in approve_session function\n");
							return ;
						}
					}
					slave_session->kvm_priv = KVM_REQ_ALLOWED;
					sm_set_master_sess_index(slave_session->h_session);
					sm_set_pending_client_response();

				}
				memset(&m_pending_session[i], 0, sizeof(PENDING_SESSION) );
			}
		}

		release_mutex (&m_pending_session_guard[i]);
	}
}

int WriteSessionToken(session_info_t* p_si, int Status)
{
		int ret;
        websession_info_t WebSession_Info;
        memset(&WebSession_Info,0,sizeof(WebSession_Info));

        WebSession_Info.Status = Status;
        ret=snprintf(WebSession_Info.websession_token, sizeof(WebSession_Info.websession_token),"%s", p_si->websession_token);
        if(ret>=(signed int)sizeof(WebSession_Info.websession_token))
        {
        	TCRIT("Buffer overflow\n");
        }
        if (PipeFD > 0) {
                if ( -1 == write(PipeFD, &WebSession_Info, sizeof(websession_info_t)) )
                {
                        TCRIT("Could not write the token to Pipe.\n");
                        return -1;
                }
        }

        return 0;
}

void sm_set_webpreviewer_status(BOOL status) {
        acquire_mutex (&m_global_guard);
	m_is_web_previewer = status;
	release_mutex (&m_global_guard);
}

session_info_t * 
sm_is_set_webpreviewer_packet_send() {
	int i;

		for (i = 0; i < max_session; i++)
		{
			acquire_mutex (m_session [i].p_access_guard);
			if (m_session [i].is_webPreviewer)
			{
				//release_mutex (m_session [i].p_access_guard);
				if(m_session [i].is_webPreviewer_data_send)
				{
					release_mutex (m_session [i].p_access_guard);
					return &m_session[i];
				}
			}
			release_mutex (m_session [i].p_access_guard);
		}

		return NULL;
}

void
sm_set_webPreviewer_data_send (hsession_t hsession)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
	m_session [hsession].is_webPreviewer_data_send = TRUE;
		
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}

void
sm_reset_webPreviewer_data_send (hsession_t hsession)
{
	
	acquire_mutex (m_session [hsession].p_access_guard);
	m_session [hsession].is_webPreviewer_data_send = FALSE;
		
	release_mutex (m_session [hsession].p_access_guard);
	
	return ;
}


BOOL sm_get_webpreviewer_status()
{
	return m_is_web_previewer;
}
void set_webpreivewer_capture_time()
{
     	acquire_mutex (&m_global_guard);
     	time(&m_web_capture_time);
     	release_mutex (&m_global_guard);
}
BOOL is_valid_web_capture()
{
	time_t current_time;
        time(&current_time);
	double diff = difftime(current_time, m_web_capture_time);
	if(diff < WEB_PREIVEW_CAPTURE_INTERVAL)
		return FALSE;
 	return TRUE;
}


/**
 * sm_set_media_license_send
**/
void
sm_set_media_license_send ()
{
	
	acquire_mutex (&m_global_guard);
	g_media_license_send= TRUE;
	release_mutex (&m_global_guard);
	
	return ;
}

/**
 * sm_reset_media_license_send
**/
void
sm_reset_media_license_send ()
{
	
	acquire_mutex (&m_global_guard);
	g_media_license_send = FALSE;
	release_mutex (&m_global_guard);
	
	return ;
}

/**
 * sm_is_set_media_license_send
**/
BOOL
sm_is_set_media_license_send ()
{
	BOOL ret=FALSE;
	
	acquire_mutex (&m_global_guard);
	ret=	g_media_license_send;
	release_mutex (&m_global_guard);
	
	return ret;
}

/**
 * sm_is_registered_client
**/
int 
sm_is_registered_client(auth_t auth_pkt)
{
    int i;

    for (i = 0; i < max_session; i++)
    {
        acquire_mutex (&m_session_guard [i]);

        if (m_session [i].is_used && (!m_session [i].is_webPreviewer))
        {
            if((strncmp(auth_pkt.ipaddr_client,m_session [i].ip_addr_client,TOKENCFG_MAX_CLIENTIP_LEN))==0)
            {
                if((strncmp(auth_pkt.client_uname,m_session [i].client_uname,TOKENCFG_MAX_CLIENT_USERNAME_LEN))==0)
                {
                    release_mutex (&m_session_guard [i]);
                    return TRUE;
                }
            }
        }

        release_mutex (&m_session_guard [i]);
    }
    return FALSE;
}

/**
 * sm_reset_kvm_timer
**/
void 
sm_reset_kvm_timer(int session_index)
{
    acquire_mutex (&m_session_guard [session_index]);
    if (m_session [session_index].is_used && m_session [session_index].is_valid)
    {
        m_session [session_index].activity_cnt_down = AdviserConf.SessionInactivityTimeout;
    }
    release_mutex (&m_session_guard [session_index]);
}
/**
 * session_timercall
**/
static void 
session_timercall()
{
    int i;
    int timedout = 0;
    for (i = 0; i < max_session; i++)
    {
        timedout = 0;
        acquire_mutex (&m_session_guard [i]);
        if (m_session [i].is_used && m_session [i].is_valid)
        {
            if(m_session [i].media_active != 1)
            {
                m_session [i].activity_cnt_down -= g_TimerTickSeconds;
                if(m_session [i].activity_cnt_down  <=0)
                {
                    timedout = 1;
                }
            }
        }
        release_mutex (&m_session_guard [i]);
        if(timedout==1 ){
        	
            send_stop_session(&m_session [i],STOP_SESSION_TIMED_OUT);
        }
    }
    return;
}

#ifdef ICC_OS_LINUX
void start_own_timer_thread()
{
    pthread_t   thread_id;
    if (0 != pthread_create (&thread_id, NULL, timer_thread, &g_TimerTickSeconds))
    {
        TCRIT ("Error creating timer thread - %d (%s)\n", errno, strerror(errno));
    }
}

void* timer_thread(void* Param)
{
    int SleepIntervalSecs = *(int*)Param;
    prctl(PR_SET_NAME,__FUNCTION__,0,0,0);

    while(1)
    {
        int timeleftSecs;
        timeleftSecs = SleepIntervalSecs;

        while(timeleftSecs)
        {
            timeleftSecs = sleep(timeleftSecs);
        }

        if(MUTEX_LOCK()!=0)
        {
           TCRIT("Error in pthread_mutex_unlock\n");
        }	
                //call the Timer
        session_timercall();
        if(MUTEX_UNLOCK()!=0)
        {
           TCRIT("Error in pthread_mutex_unlock\n");
        }
    }
}
#endif
int Ondisconnect_session(int racsession_id,int session_type)
{
	int index=0;
	int		session_count = 0;
	int cleanup = 0;
	if( session_type == 5 )
	{
		for(index = 0; index < max_session; index++)
		{
			acquire_mutex (m_session [index].p_access_guard);

			if( (m_session [index].is_valid) && (m_session [index].is_used) && (!m_session [index].clean) && (!m_session [index].is_webPreviewer) )
			{
				if (session_count >= kvm_max_session)
				{
					TINFO ("Maximum KVM Sessions reached. Spurious !!!\n");
					release_mutex (m_session [index].p_access_guard);
					break;
				}
				if((signed int)m_session [index].racsession_id == racsession_id)
				{
					release_mutex (m_session [index].p_access_guard);
					send_stop_session(&m_session [index],STOP_SESSION_DISCONNECT);
					
					acquire_mutex (m_session [index].p_access_guard);
					if(g_corefeatures.kvm_reconnect_support == ENABLED)
					{
						m_session [index].disconnected_clean = TRUE;
					}
					m_session [index].clean = TRUE;
					cleanup = 1;
					release_mutex (m_session [index].p_access_guard);
					break;
				}
				
				session_count++;
			}
			release_mutex (m_session [index].p_access_guard);
		}
		if (cleanup == 1)
			sm_clean_sessions();
	}
	return 0;
}

void clear_invalid_register_session()
{
	
	session_list_t		session_list;
	unsigned int i=0;
	int j=0;
	int ret=0;
	session_info_record_t *session_info_records;
	session_info_header_t hdr;
	uint32 total_records=0;
	
	sm_get_valid_session_list(&session_list, TRUE);
	
	ret=racsessinfo_getallrecords(&session_info_records,&hdr,&total_records);
	
	int recordfound =0;
	if(ret == 0)
	{
		if(hdr.sess_type_logins[SESSION_TYPE_VKVM] != 0)
		{
			if(session_list.num_session == 0)
			{
				for(i=0;i<total_records;i++)
				{
					if(session_info_records[i].session_type == SESSION_TYPE_VKVM)
					{
						racsessinfo_unregister_session(session_info_records[i].racsession_id,0);
					}
				}
			}
			else
			{
				for(i=0;i<total_records;i++)
				{
					if(session_info_records[i].session_type == SESSION_TYPE_VKVM )
					{
						for(j=0;j<session_list.num_session;j++)
						{
							if(session_info_records[i].racsession_id == session_list.p_session[j]->racsession_id)
							{
								recordfound=1;
							}
						}
						if(recordfound == 0)
						{
							racsessinfo_unregister_session(session_info_records[i].racsession_id,0);
							TCRIT("\nInvalid session clearing the Entry from racsesion::%d",session_info_records[i].racsession_id);
						}
					}
				}
			}
		}
		
		
	}
	if(total_records > 0)
	free(session_info_records);
}

/**
 * sm_get_active_client_mac
**/
void
sm_get_active_client_mac ( char *macaddress,int len)
{
	int 		index = 0;

	if(0)
	{
		len=len; /* -Wextra, fix for unused parameters */
	}
	// Loop through maximum possible sessions
	// Make sure that the session in concer is not cleaned/webpreview and is valid
	// If so, we copy that info to our buffer else skip session
	for(index = 0; index < max_session; index++)
	{
		acquire_mutex (m_session [index].p_access_guard);

		if( (m_session [index].is_valid) && (m_session [index].is_used) && (!m_session [index].clean) && (!m_session [index].is_webPreviewer) )
		{
			if(strlen(m_session[index].mac_addr) > 0)
			{
				//copy only Max mac address size. len variable will hold (numsession*TOKENCFG_MAX_MAC_LEN)+1
				strncpy(macaddress, m_session[index].mac_addr,TOKENCFG_MAX_MAC_LEN-1);
				macaddress+=TOKENCFG_MAX_MAC_LEN;
			}
		}
		release_mutex (m_session [index].p_access_guard);
	}
	return ;
}

BOOL
sm_get_kvm_priv(int index, ivtp_hdr_t * hdr)
{
	BOOL ret = false;
	acquire_mutex (m_session [index].p_access_guard);

	if(g_corefeatures.kvm_reconnect_support == ENABLED)
	{
		//clear the clean flag if disconnect flag was not set
		if(!m_session[index].disconnected_clean)
			m_session [index].clean = 0;
	}

	if( (m_session [index].is_valid) && (m_session [index].is_used) && (!m_session [index].clean) )
	{
		hdr->status = (unsigned short) m_session [index].kvm_priv;
		ret = true;
	}

	release_mutex (m_session [index].p_access_guard);

	return ret;
}

BOOL
sm_set_reconn_sock (int index,int sock)
{

	acquire_mutex (m_session [index].p_access_guard);
	if(m_session[index].socket >0)
	{
		close(m_session[index].socket);
	}
	m_session[index].socket = sock;
	m_session[index].failed_time = 0;
	m_session[index].clean = false;
	m_session[index].is_alive = false;
	m_session[index].transfer_status = 0;

	release_mutex (m_session [index].p_access_guard);
	return TRUE;
}

/*
*sm_is_sessinfo_changed
* param reconnect_req_t
* return TRUE/FALSE
* Desc : checks whether session information 
		like ip and MAc address are same or not
*/
BOOL sm_is_sessinfo_changed(reconnect_req_t  recon)
{
	char *position;
	if((0 == strlen(m_session[recon.index].ip_addr_client) )||
		(0 == strlen(m_session[recon.index].mac_addr)))
		return FALSE;

	/* truncating the scope id of the link local address according to RFC 4007*/

	if (( position = strchr(recon.ipaddr_client,'%')) != NULL)
	memset(position, 0 ,((TOKENCFG_MAX_CLIENTIP_LEN+1) - (position - recon.ipaddr_client)));

	if(strncmp(m_session[recon.index].ip_addr_client,recon.ipaddr_client,TOKENCFG_MAX_CLIENTIP_LEN)!=0)
	{
		return TRUE;
	}
	else if(strncmp(m_session[recon.index].mac_addr,recon.mac_addr,TOKENCFG_MAX_MAC_LEN)!=0)
	{
		return TRUE;
	}
	return FALSE;
}

/*
*sm_set_clean_flag
* param session index
* Desc : sets clean flag to clean the session
*/
void sm_set_clean_flag(int index)
{
	acquire_mutex (m_session [index].p_access_guard);
	m_session [index].is_used = true;//Quanta
	m_session[index].clean = true;
	if(g_corefeatures.kvm_reconnect_support == ENABLED)
		m_session[index].disconnected_clean= true;

	release_mutex (m_session [index].p_access_guard);
}

/*
*sm_set_transfer_status
* param  index and status 
* Desc : sets transfer status 
*/
void sm_set_transfer_status(int index,int status)
{
	if(m_session[index].is_used == true)
	{
		m_session[index].clean = true;
		m_session[index].transfer_status= status;
	}
}

/*
*sm_get_transfer_status
* param  index  
* Desc : gets transfer status 
*/
int sm_get_transfer_status(int index)
{	
	if(m_session[index].is_used == true)
	{
		return m_session[index].transfer_status;
	}
	return 0;
}

void sm_set_host_lock_info()
{
	AdviserCfg_T pAdviserCfg;
	GetAdviserCfg(&pAdviserCfg);
	g_host_lock_auto_feature = pAdviserCfg.hostlock_feature;
}

/**
 * sm_set_last_recv_time
**/
void 
sm_set_last_recv_time(int session_index)
{
    struct sysinfo sys_info;
    acquire_mutex (&m_session_guard [session_index]);
    if (m_session [session_index].is_used && m_session [session_index].is_valid)
    {
        if(!sysinfo(&sys_info))
        {
            m_session [session_index].lastrecvtime   = sys_info.uptime;
        }
    }
    release_mutex (&m_session_guard [session_index]);
}


/*
*sm_is_session_reconn
* param session_index
* return TRUE/FALSE
* Desc : checks whether session is reconnting 
*/
BOOL sm_is_session_reconn(int index)
{
	BOOL reconn = FALSE;

	acquire_mutex (&m_session_guard [index]);
	if (m_session [index].is_used && m_session [index].is_valid)
	{
		if( (IsTimedOut(m_session[index].lastrecvtime, 5)) || (m_session [index].transfer_status == ERR_CONNECTION_LOST) ||
			(m_session [index].ctrl_transfer_status == ERR_CONNECTION_LOST))
		{
			reconn = TRUE;
		}
	}
	release_mutex (&m_session_guard [index]);

	return reconn;
}

/*
*sm_set_ctrl_transfer_status
* param  index  and status
* Desc : sets ctrl transfer status 
*/
void sm_set_ctrl_transfer_status(int index,int status)
{
	if(m_session [index].is_used)
	{
		m_session[index].clean = true;
		m_session[index].ctrl_transfer_status= status;
	}
}

/*
*sm_get_ctrl_transfer_status
* param  index
* Desc : gets ctrl transfer status 
*/
int sm_get_crl_transfer_status(int index)
{	
	if(m_session[index].is_used == true)
	{
		return m_session[index].ctrl_transfer_status;
	}
	return 0;
}

/**
 * sm_getracsession_id
**/
int 
sm_getracsession_id(int session_index)
{
    int ret = -1;
    acquire_mutex (&m_session_guard [session_index]);
    if (m_session [session_index].is_used && m_session [session_index].is_valid)
    {
        ret =  m_session [session_index].racsession_id;
    }
    release_mutex (&m_session_guard [session_index]);
    return ret;
}

