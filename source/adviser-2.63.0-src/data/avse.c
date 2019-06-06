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
* @file   	Avse.c
* @author 	govind kothandapani		 <govindk@ami.com>
*		Baskar Parthiban		 <bparthiban@ami.com>
*		Varadachari Sudan Ayanam 	<varadacharia@ami.com>
* @brief  	Adaptive video server implementation
****************************************************************/

#include <linux/unistd.h>
#include <signal.h>
#include "coreTypes.h"
#include "avse.h"
#include "advisererrno.h"
#include "sessionmgr.h"
#include "conmgr.h"
#include "sync.h"
#include "adviserdbg.h"
#include "utils.h"
#include "hid_cmdhandlr.h"
#include "compress.h"
#include "qlzw.h"
#include "video_interface.h"
#include "video_misc.h"
#include "videopkt.h"
#include "dlfcn.h"
#include "ncml.h"
#include "adviser_cfg.h"
#include "procreg.h"
#include "unix.h"
#include<sys/prctl.h>
#include "featuredef.h"
#include "cmdhndlr.h"
#include "racsessioninfo.h"
#include  "userprivilege.h"

#define MAX_RETRY_COUNT		2
/*-------------------------- Local Definitions ----------------------------*/

extern sem_t  		m_start_avse;
extern unsigned short	g_stop_signal_status; //For sending the stop signal status
extern unsigned short	g_host_power_status; //For sending the host poweer status
extern unsigned short 	g_mouse_media_status;
extern u8		*service_info_data;
extern int		services_changed;
extern BOOL		g_active_client_send; //flag to send the active clients to the session
extern BOOL		g_pending_client_send; //flag to send the pending clients to the session
extern int		kvm_max_session;
u8 			gBlankScreen = 0;
extern u8	g_mouse_mode;
extern CoreFeatures_T g_corefeatures;
extern u8 g_host_is_locked;
extern u8 g_host_lock_auto_feature;
extern BOOL		g_media_license_status;
extern void adviser_unload_resource(void);
extern unsigned int g_kvm_retry_count;
extern unsigned int g_kvm_retry_interval;
//To synch Jviewer thread 
//with continuous recording
u8 g_sync_rec_jviewer = 0;
sem_t m_wait_rec;
/**
 * form_status_ctrl_pkt_hdr
**/
void form_status_ctrl_pkt_hdr (ivtp_hdr_t* p_pkt, int type, int size, u16 status)
{
	TINFO_4 ("CTRL PKT Size :%d\n", size);
	p_pkt->type 	= type;
	p_pkt->pkt_size = size;
	p_pkt->status 	= status;
}

/**
 * form_control_pkt_hdr
**/
void form_control_pkt_hdr (ivtp_hdr_t* p_pkt, int type, int size)
{
	TINFO_4 ("CTRL PKT Size :%d\n", size);
	p_pkt->type 	= type;
	p_pkt->pkt_size = size;
	p_pkt->status 	= 0;
}

/**
 * send_stop_signal
**/
void
send_stop_signal (session_list_t*	send_list, session_list_t* session_list)
{
	ivtp_hdr_t		v_hdr;

	/* send stop packet to all the clients */
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_STOP_SESSION_IMMEDIATE, 0,g_stop_signal_status);

	sm_build_send_list (send_list, session_list);

	TINFO_4 ("About to send Stop Control Packet\n");
	/* Send stop session pkt */
	send_control_pkt (&v_hdr, send_list, NULL, 0, 0);
}

/**
 * This function sends the KVM privilege packet to the client.
 * kvm_priv_pkt_info_t argument contains the information such as, 
 *   - The session information of the session to which the packet is sent.
 *   - The packet KVM response type.
 *   - The packet status value of the KVM response packet.
 **/
void
send_kvm_priv_resp(session_info_t *session)
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	if(session->is_valid == TRUE)
		session_list.is_new_session = TRUE;
	else
		session_list.is_new_session = FALSE;
	session_list.num_session = 1;
	session_list.p_session[0] = session;
	form_status_ctrl_pkt_hdr (&v_hdr, session->KVM_pkt, 0, session->kvm_pkt_status);
	send_control_pkt (&v_hdr, &session_list, NULL, 0, 0);
	sm_reset_kvm_priv_send_sess_info(session);
	return;
}
/**
 * send_stop_session
**/
int send_stop_session(session_info_t* session,int status)
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	
	if( session == NULL )
	{
		return 0;
	}
	session->Logout_Type=status;
	session_list.is_new_session = 0;
	session_list.num_session = 1;
	session_list.p_session[0] = session;
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_STOP_SESSION_IMMEDIATE, 0, status);//has to be checked
	send_control_pkt (&v_hdr, &session_list, NULL, 0, 0);
	return 0;
}

/**
 * send_power_status
**/
void
send_power_host_status (session_list_t*	send_list, session_list_t* session_list)
{
	ivtp_hdr_t		v_hdr;

	/* send stop packet to all the clients */
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_POWER_STATUS, 0,g_host_power_status);

	sm_build_send_list (send_list, session_list);

	TINFO_4 ("About to send Stop Control Packet\n");
	/* Send stop session pkt */
	send_control_pkt (&v_hdr, send_list, NULL, 0, 0);
}

/**
 * send_stop_signal
**/
void
send_active_client(session_list_t *send_list, session_list_t *session_list)
{
	ivtp_hdr_t		v_hdr;
	int 			datalen;
	char			*ipuserdata = NULL;
	int			index = 0;
	ip_usr_datapkt		*user_data = NULL;
	int active_max_session = 0;
	// Get the maximum data size to be filled
	active_max_session = sm_get_num_valid_sessions();
	datalen = sizeof(ip_usr_datapkt) * active_max_session;	

	// Allocate storage for the user info
	ipuserdata = (char *)malloc(datalen);
	if (ipuserdata == NULL)
	{
		TINFO ("Unable to allocate resource for holding the User information\n");
		return;
	}
	memset(ipuserdata, 0, datalen);

	user_data = (ip_usr_datapkt *)(ipuserdata);

	// Get the Active Sessions list and their user credentials
	sm_build_active_client_info(ipuserdata);

	for (index=0; index < active_max_session; index++)
	{
		user_data++;
	}

	// Frame the acive sessions information header
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_ACTIVE_CLIENTS, datalen, 0);
	
	// Build the list of sessions to send to
	sm_build_send_list (send_list, session_list);


	// Send the user credentials info to all the clients
	send_control_pkt (&v_hdr, send_list, (u8 *)ipuserdata, datalen, 0);
	
	// Release unused memory
	if (ipuserdata)
		free(ipuserdata);
}

/**
 * send_status_to_all_clients
**/
void
send_status_to_all_clients(session_list_t *send_list, session_list_t *session_list,int type,u16 status)
{
	ivtp_hdr_t		v_hdr;

	// Frame the IVTP header
	form_status_ctrl_pkt_hdr (&v_hdr, type, 0, status);
	
	// Build the list of sessions to send to
	sm_build_send_list (send_list, session_list);

	// Send the pacaket to all the clients
	send_control_pkt (&v_hdr, send_list, (u8*)&v_hdr, sizeof(ivtp_hdr_t), 0);
}

/**
 * send_pending_session_response
**/
void
send_pending_session_response(session_list_t *send_list, session_list_t *session_list)
{
	ivtp_hdr_t		v_hdr;

	v_hdr.status = (unsigned short) (STATUS_KVM_PRIV_ACK_USER2 + ( ((u16)KVM_NOT_MASTER) <<8 ) );

	// Frame the pendind sessions information header
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_KVM_SHARING, 0, v_hdr.status);
	
	// Build the list of sessions to send to
	sm_build_pending_session_send_list(send_list, session_list);

	// Send the privilege info to pending session
	send_control_pkt (&v_hdr, send_list, 0, 0, 0);
}

/**
 * handle_stop_signal
**/
void
handle_stop_signal (session_list_t*	send_list, session_list_t* session_list)
{
	int retry = MAX_RETRY_COUNT;
	int num_session = 0;
	
	TINFO_4 ("Waiting for Stop session complete\n");

	send_stop_signal (send_list, session_list);
	sm_wait_for_stop_session_complete ();

	//only Active sessions need to be closed. should not stop adviser
	if ((g_stop_signal_status == STOP_SESSION_WEBSERVER_RESTART) ||
		(g_stop_signal_status == STOP_SESSION) )
	{
		return;
	}
	
	// All the status for which adviser should be closed needs to be added here.
	if (g_stop_signal_status == STOP_SESSION_LICENSE_EXP)
	{
		// Adviser will be Stopped only here.
		while(retry > 0)
		{
			num_session = sm_get_kvm_num_session();
			if (num_session == 0)
			{
				adviser_unload_resource();
			}
			retry--;
			sleep(1);
		}
		//This should not happen .Anyway if code flow reaches here unload resources
		adviser_unload_resource();
	}
	else
	{
		send_signal_by_name("Adviserd", SIGINT);
	}
	return;
}


/**
 * handle_send_encrypt_status
**/
void
handle_send_encrypt_status (session_list_t* send_list, session_list_t* session_list)
{
	ivtp_hdr_t				v_hdr;

	sm_build_send_list (send_list, session_list);

	form_control_pkt_hdr (&v_hdr, IVTP_ENCRYPTION_STATUS, 0);

	TINFO_4 ("About to send encrypt status Packet to %d clients\n", send_list->num_session);
	/* Send encrypt_key pkt */
	send_control_pkt (&v_hdr, send_list, NULL, 0, 0);

	/* Reset the send encrypt key field in session list*/
	sm_reset_send_encrypt_status ();

	return;
}
/**
 * handle_stop_signal
**/
void
handle_web_preview_data_send (session_info_t *session )
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	web_cap_status_t captureStatus;
	int length;
	if( session == NULL )
	{
		return;
	}
	captureStatus.capStatus = g_capture_web_preview_Status;
	session_list.is_new_session = 0;
	session_list.num_session = 1;
	session_list.p_session[0] = session;
	length = sizeof(captureStatus.capStatus);
	
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_WEB_PREVIEWER_CAPTURE_STATUS, length,0);//has to be checked
	send_control_pkt (&v_hdr, &session_list, &g_capture_web_preview_Status, length, 0);
	sm_reset_power_crl_resp_send(session->h_session);
	
	return;
}


/**
 * send_service_info
**/
void
send_service_info (session_list_t *send_list, session_list_t *session_list, u8 *servicesinfo)
{
	ivtp_hdr_t		v_hdr;
	int 			datalen = 0;

	// Frame the data length to be sent to Clients
	datalen = sizeof(SERVICE_CONF_STRUCT) * KVM_SERVICE_LIST;	

	// Frame the service information packet header
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_SERVICE_INFO, datalen, 0);

	// Build the send list based on the current active session list
	sm_build_send_list (send_list, session_list);

	TINFO_4 ("About to send the Service information to Clients\n");

	// Sending the service information packet to all clients
	send_control_pkt (&v_hdr, send_list, servicesinfo, datalen, 0);
}

/**
 * send_media_license_info
**/
void
send_media_license_info (session_list_t *send_list, session_list_t *session_list)
{
	ivtp_hdr_t		v_hdr;

	// Frame the Media License information packet header
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_MEDIA_LICENSE_STATUS, 0,g_media_license_status);

	// Build the send list based on the current active session list
	sm_build_send_list (send_list, session_list);

	TINFO_4 ("About to send the Media License information to Clients\n");

	// Sending the Media License information packet to all clients
	send_control_pkt (&v_hdr, send_list, NULL, 0, 0);
}

void
send_mouse_media_info(session_list_t *send_list, session_list_t *session_list) 
{
	ivtp_hdr_t		v_hdr;
	int 			datalen = 0;
	VMediaCfg_T  pVMeiaCfg;
	AdviserCfg_T  pAdviserCfg;
	mouse_media_t mousemediainfo;
	kvm_reconnect_t kvmreconnectinfo;
	u8 lockstatus = 2;	// The value 2 is used to get the current status.
	
	memset(&mousemediainfo,0,sizeof(mouse_media_t));
	GetVMediaCfg( &pVMeiaCfg,g_corefeatures.lmedia_support,g_corefeatures.rmedia_support,g_corefeatures.power_consumption_virtual_device_usb);
	GetAdviserCfg(& pAdviserCfg);
	mousemediainfo.mouse_mode = pAdviserCfg.mouse_mode;
	g_mouse_mode = mousemediainfo.mouse_mode;
	g_host_lock_auto_feature = pAdviserCfg.hostlock_feature;
	mousemediainfo.hostlock = pAdviserCfg.hostlock_feature;
	mousemediainfo.media_info = pVMeiaCfg;

	if(g_corefeatures.kvm_reconnect_support == ENABLED)
	{
		g_kvm_retry_count = pAdviserCfg.retry_count;
		g_kvm_retry_interval = pAdviserCfg.retry_interval;
		kvmreconnectinfo.retrycount = pAdviserCfg.retry_count;
		kvmreconnectinfo.retryinterval = pAdviserCfg.retry_interval;
		kvmreconnectinfo.mouse_media_values = mousemediainfo;
		datalen = sizeof(kvm_reconnect_t);
	}
	else
	{
		datalen = sizeof(mouse_media_t);
	}

	// Frame the service information packet header
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_KVMMOUSE_MEDIA_INFO, datalen, 0);

	// Build the send list based on the current active session list
	sm_build_send_list (send_list, session_list);

	TINFO_4 ("About to send the Service information to Clients\n");

	// Sending the service information packet to all clients
	if(g_corefeatures.kvm_reconnect_support == ENABLED)
	{
		//sending kvm retry configuration values along with mouse media info
		send_control_pkt (&v_hdr, send_list, (u8 *)&kvmreconnectinfo, datalen, 0);
	}
	else
	{
		//sending mouse media info
		send_control_pkt (&v_hdr, send_list, (u8 *)&mousemediainfo, datalen, 0);
	}

	Onhostlock_status ( &lockstatus);
	// Update the Host lock status in the global variable.
	if (lockstatus == HOSTLOCK_SUCCESS) {
		g_host_is_locked = 0;
		// The value is modified, so update the host lock status to the client.
		sm_set_host_lock_status();
	}
	else if (lockstatus == HOSTUNLOCK_SUCCESS) {
		g_host_is_locked = 1;
		// The value is modified, so update the host lock status to the client.
		sm_set_host_lock_status();
	}
}

/**
 * handle_auto_bw_detect
**/
void
handle_auto_bw_detect (session_list_t*	send_list, session_list_t* session_list)
{

	ivtp_hdr_t v_hdr;
	int i;

	sm_build_auto_bw_detect_send_list (send_list, session_list);

	TINFO_4 ("About to Auto Detect Bw Packet\n");

	/* Bug#1 Fix: 
		In the following 2 lines
		send_control_pkt sends the auto_bw response packet to the client and sm_reset_auto_bw_detect
		resets the auto_bw flag in the session_info structure but sometimes send_control_pkt sends
		the response and by the time sm_reset_auto_bw_detect resets the auto_bw flag, the client might have
		sent a detect_auto_bw_ req packet again and auto_bw flag might have been set again in on_bw_request 
		function in cmdhndlr.c. So sm_reset_auto_bw_detect resets it the client waits forever to get
		response for second detect_auto_bw packet.
		used an additional mutex for synchronization.

	  Bug#2 Fix:
		We were resetting the auto_bw flag for all the open valid clients. 
		We must reset the flag to only the clients whichever we have sent the auto_bw response.
	*/

	/* lock all the mutex so that sm_set_send_auto_bw_detect will not change the 
	flags before we reset the flags */
	for (i = 0; i < send_list->num_session; i++)
		acquire_mutex (send_list->p_session [i]->p_access_detect_auto_bw);
	
	/* Send Auto Detect Bandwidth pkt */
	send_control_pkt (&v_hdr, send_list, NULL, 0, AUTO_BANDWIDTH_DETECT_CTRL_PKT);

	/* Reset the send color gain field in session list*/
	sm_reset_auto_bw_detect (send_list);

	for (i = 0; i < send_list->num_session; i++)
		release_mutex (send_list->p_session [i]->p_access_detect_auto_bw);

	return;
}
/**
 * handle_pesnding session
**/
void
handle_pending_sesssion_response (session_list_t*	send_list, session_list_t* session_list)
{
	TINFO_4 ("Going to handle the pending Session  response information\n");
	
	send_pending_session_response( send_list,  session_list);
	sm_reset_pending_client_response();
	return;
}

/**
*Handle KVM master packet
*
**/
void handle_kvm_master_packet( session_info_t *first_session )
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	
	if( first_session == NULL )
	{
		return;
	}
	
	session_list.is_new_session = 0;
	session_list.num_session = 1;
	session_list.p_session[0] = first_session;


	first_session->KVM_pkt= IVTP_SET_NEXT_MASTER;
	first_session->kvm_pkt_status  = (unsigned short) (STATUS_KVM_PRIV_SWITCH_USER + ( ((u16)KVM_REQ_ALLOWED) <<8 ) );
	form_status_ctrl_pkt_hdr (&v_hdr, first_session->KVM_pkt,0, first_session->kvm_pkt_status );
	send_control_pkt (&v_hdr, &session_list,0,0, 0);

	//set master index
	sm_set_master_sess_index(first_session->h_session);
	//update kvm_priv
	first_session->kvm_priv = KVM_REQ_ALLOWED;

	return;	
}

/**
*Handle KVM sharing previlage packet
*
**/
void handle_kvm_priv_packet( session_info_t *first_session )
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	ip_usr_datapkt  data;
	usrpriv_t usrpriv;
	
	if( first_session == NULL )
	{
		return;
	}
	
	session_list.is_new_session = 0;
	session_list.num_session = 1;
	session_list.p_session[0] = first_session;
		memset(data.username_othersession,0,sizeof(data.username_othersession));
		memset(data.ip_add_othersession,0,sizeof(data.ip_add_othersession));
		
		strncpy(data.username_othersession,first_session->username_other_session,TOKENCFG_MAX_USERNAME_LEN-1);
		strncpy(data.ip_add_othersession,first_session->ip_addr_othersession,TOKENCFG_MAX_CLIENTIP_LEN);
		data.sess_index =(char)first_session->slave_index;

		if( GetUsrPriv(data.username_othersession, &usrpriv) == 0 )
		{
			data.ipmi_priv = usrpriv.ipmi.lanpriv;
		}
		form_status_ctrl_pkt_hdr (&v_hdr, first_session->KVM_pkt, sizeof(ip_usr_datapkt), first_session->kvm_pkt_status );
		send_control_pkt (&v_hdr, &session_list,(u8 *) &data,sizeof(ip_usr_datapkt), 0);

	return;	
}

/**
*Handle the sending of KVM sharing privilege response packet.
*
**/
void handle_kvm_priv_resp()
{
	session_info_t *master_session = NULL, *slave_session = NULL;
	master_session = sm_get_kvm_priv_master_sess();
	slave_session = sm_get_kvm_priv_slave_sess();
	if(NULL != master_session && (0 < master_session->KVM_pkt)) /* &&
			-1 < (signed int)master_session->kvm_pkt_status))*/        /* Condition is always true */
	{
		send_kvm_priv_resp(master_session);
		sm_set_kvm_priv_master_sess(NULL);//set the master session info reference to NULL to avoid sending packet to wrong session.
	}
	if(NULL != slave_session && (0 < slave_session->KVM_pkt)) /*  &&
			-1 < slave_session->kvm_pkt_status))*/                     /* Condition is always true */
	{
		send_kvm_priv_resp(slave_session);
		sm_set_kvm_priv_slave_sess(NULL);//set the slave session info reference to NULL to avoid sending packet to wrong session.
	}
	return;
}

/**
*
*added for handle socket close
***/

void handle_socket_close(session_info_t *session )
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	
	if( session == NULL )
	{
		return;
	}

	session_list.is_new_session = 0;
	session_list.num_session = 1;
	session_list.p_session[0] = session;
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_KVM_SOCKET_STATUS, 0, session->socket_close_status);//has to be checked
	send_control_pkt (&v_hdr, &session_list, NULL, 0, 0);
	return;	


}

/**
*
*added for handle socket close
***/

void handle_ipmi_raw_resp_send(session_info_t *session )
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	unsigned int i = 0, command_count = session->send_raw_ipmi_data_count;
	
	if( session == NULL )
	{
		return;
	}

	session_list.is_new_session = 0;
	session_list.num_session = 1;
	session_list.p_session[0] = session;
	
	for(i = 0; i < command_count; i++ )
	{
		if((session->sess_ipmi_res[i].dwresplength >= 0) && (session->sess_ipmi_res[i].ipmi_resp_data != NULL))
		{
			// dwresplength + 1 is used so that sequence number can be used in addition to the ipmi command
			form_status_ctrl_pkt_hdr (&v_hdr, IVTP_IPMI_RES_COMMAND,session->sess_ipmi_res[i].dwresplength+1 ,session->sess_ipmi_res[i].raw_cmd_sucess);
			send_control_pkt (&v_hdr, &session_list, (u8 *)session->sess_ipmi_res[i].ipmi_resp_data, session->sess_ipmi_res[i].dwresplength+1, 0);
		}
		else if(session->sess_ipmi_res[i].dwresplength >= 0) 
		{
			form_status_ctrl_pkt_hdr (&v_hdr, IVTP_IPMI_RES_COMMAND,1 ,session->sess_ipmi_res[i].raw_cmd_sucess);
			send_control_pkt (&v_hdr, &session_list, (u8 *)&session->sess_ipmi_res[i].sequenceid, 1, 0);
		}
		sm_reset_raw_ipmi_resp_send(session->h_session, i);
	}
	return;	


}

/**
*
*added for handle socket close
***/

void handle_host_lock_status(session_list_t* send_list, session_list_t* session_list)
{
	ivtp_hdr_t				v_hdr;
	int status;
	sm_build_send_list (send_list, session_list);
	form_control_pkt_hdr (&v_hdr, IVTP_DISPLAY_CONTROL_STATUS,0 );
	if(g_corefeatures.host_lock_feature == ENABLED)
	{
		status = g_host_lock_auto_feature & (1<<0);
		if( (g_host_is_locked == 0) && (status ==0))
		{
			v_hdr.status =0x2;
		}
		else if( (g_host_is_locked == 1) && (status ==0))
		{
			v_hdr.status = 0x3;
		}
		else if( (g_host_is_locked == 0) && (status ==1))
		{
			v_hdr.status = 0x0;
		}
		else if( (g_host_is_locked == 1) && (status ==1))
		{
			v_hdr.status = 0x1;
		}
	}
	else
	{
		v_hdr.status = g_host_is_locked;
	}
	TINFO_4 ("About to send host_lock_status status Packet to %d clients\n", send_list->num_session);
	/* Send hsot_lock status pkt */
	send_control_pkt (&v_hdr, send_list, NULL, 0, 0);

	return;

}


/**
*
*added for handle vmedia update
***/

void handle_update_Vmedia_device_status(session_list_t* send_list, session_list_t* session_list)
{
	ivtp_hdr_t				v_hdr;
	vmedia_device_update 	media_device_free;
	session_info_header_t hdr;
	int ret=0;
	VMediaCfg_T  pVMeiaCfg;
	
	if(g_corefeatures.session_management == ENABLED)
	{
		memset(&media_device_free,0,sizeof(vmedia_device_update));
		memset(&v_hdr,0,sizeof(ivtp_hdr_t));
		sm_build_send_list (send_list, session_list);
		ret =GetVMediaCfg( &pVMeiaCfg,g_corefeatures.lmedia_support,g_corefeatures.rmedia_support,g_corefeatures.power_consumption_virtual_device_usb);
		if(ret != 0)
		{
			return;
		}
		
		ret = racsessinfo_getsessionhdr(&hdr);
		if(ret != 0)
		{
			media_device_free.cd_free_count = (g_corefeatures.kvm_media_count)? pVMeiaCfg.kvm_cd : pVMeiaCfg.num_cd;
			media_device_free.fd_free_count = (g_corefeatures.kvm_media_count)? pVMeiaCfg.kvm_fd : pVMeiaCfg.num_fd;
			media_device_free.hd_free_count = (g_corefeatures.kvm_media_count)? pVMeiaCfg.kvm_hd : pVMeiaCfg.num_hd;
			form_control_pkt_hdr (&v_hdr, IVTP_VMEDIA_USED_UPDATE,sizeof(media_device_free) );
			send_control_pkt (&v_hdr, send_list, (u8 *)&media_device_free, sizeof(media_device_free), 0);
			return;
		}
		if((pVMeiaCfg.num_cd != 0) && (pVMeiaCfg.kvm_cd != 0))
		{
			media_device_free.cd_free_count = ((g_corefeatures.kvm_media_count)? pVMeiaCfg.kvm_cd : pVMeiaCfg.num_cd) - 
				(hdr.sess_type_logins[SESSION_TYPE_VMEDIA_CD]+hdr.sess_type_logins[SESSION_TYPE_VMEDIA_LOCAL_CD]);
		}
		if( (pVMeiaCfg.num_fd != 0) && (pVMeiaCfg.kvm_fd != 0))
		{
			media_device_free.fd_free_count = ((g_corefeatures.kvm_media_count)? pVMeiaCfg.kvm_fd : pVMeiaCfg.num_fd) -
				(hdr.sess_type_logins[SESSION_TYPE_VMEDIA_FD]+hdr.sess_type_logins[SESSION_TYPE_VMEDIA_LOCAL_FD]);
		}
		if((pVMeiaCfg.num_hd != 0) && (pVMeiaCfg.kvm_hd != 0))
		{
			media_device_free.hd_free_count = ((g_corefeatures.kvm_media_count)? pVMeiaCfg.kvm_hd : pVMeiaCfg.num_hd) -
				(hdr.sess_type_logins[SESSION_TYPE_VMEDIA_HD]+hdr.sess_type_logins[SESSION_TYPE_VMEDIA_LOCAL_HD]);
		}
		form_control_pkt_hdr (&v_hdr, IVTP_VMEDIA_USED_UPDATE,sizeof(media_device_free) );
			/* Send hsot_lock status pkt */
		send_control_pkt (&v_hdr, send_list, (u8 *)&media_device_free, sizeof(media_device_free), 0);
	}
	

	return;

}

/**
 * handle_power status
**/
void
handle_host_power_status (session_info_t *session )
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	
	if( session == NULL )
	{
		return;
	}

	session_list.is_new_session = 0;
	session_list.num_session = 1;
	session_list.p_session[0] = session;
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_POWER_STATUS, 0, g_host_power_status);//has to be checked
	send_control_pkt (&v_hdr, &session_list, NULL, 0, 0);
	sm_reset_power_status_send(session->h_session);
	return;
}

/**
 * handle_stop_signal
**/
void
handle_power_cmd_status_send (session_info_t *session )
{
	session_list_t session_list;
	ivtp_hdr_t		v_hdr;
	
	if( session == NULL )
	{
		return;
	}

	session_list.is_new_session = 0;
	session_list.num_session = 1;
	session_list.p_session[0] = session;
	
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_POWER_CTRL_RESP, 0, session->host_power_cmd_status);//has to be checked
	send_control_pkt (&v_hdr, &session_list, NULL, 0, 0);
	sm_reset_power_crl_resp_send(session->h_session);
	
	return;
}


/**
 * handle_stop_signal
**/
void
handle_active_client_send (session_list_t*	send_list, session_list_t* session_list)
{
	TINFO_4 ("Going to handle the Session information\n");
	
	send_active_client( send_list,  session_list);
	sm_reset_active_client_send();
	return;
}

/**
 * handle_power_cmd_status_send_all
 * handles command to send power status to all client
**/
void
handle_power_cmd_status_send_all (session_list_t*	send_list, session_list_t* session_list)
{
	send_status_to_all_clients( send_list,  session_list,IVTP_POWER_STATUS,g_host_power_status);
	return;
}

/**
 * handle_send_usermacro
**/
void
handle_send_usermacro (session_list_t*	send_list, session_list_t* session_list)
{
	ivtp_hdr_t	v_hdr;
	FILE 	*fp = NULL;
	user_macro 	UserMacro;
	int write_ret = 0;
	int read_ret = 0;

	fp = fopen(USER_MACRO_FILE,"rb+");
	if(fp == NULL)
	{
		TCRIT("ERROR: Cannot open USER_MACRO_FILE for Read mode!\n");
		
		fp = fopen(USER_MACRO_FILE,"wb+");
		if(fp != NULL)
		{
			memset(&(UserMacro.UserDefMacro),0,sizeof(UserMacro.UserDefMacro));
			write_ret = fwrite(&(UserMacro.UserDefMacro),1,sizeof(UserMacro.UserDefMacro),fp);
			if(write_ret == 0 )
			{
				TCRIT("ERROR: Unable to write the data in Macro File\n ");
			}
			fclose(fp);
		}
		else
		{
			TCRIT("ERROR: Cannot open USER_MACRO_FILE for Write mode!\n");
			return;
		}
	}
	else
	{	
		memset(&(UserMacro.UserDefMacro),0, sizeof(UserMacro.UserDefMacro));
		read_ret = fread(&(UserMacro.UserDefMacro),1,sizeof(UserMacro.UserDefMacro),fp);
		if(read_ret == 0 )
		{
			TCRIT("ERROR: Unable to read the data in Macro File\n ");
			fclose(fp);
			return;
		}
		fclose(fp);
	}
	/* send User Macro session to particular client */
	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_GET_USER_MACRO,sizeof(UserMacro.UserDefMacro), 0);
	sm_build_user_macro_send_list (send_list, session_list);

	/* Send User Macro session pkt */
	send_control_pkt (&v_hdr, send_list, (u8*)&(UserMacro.UserDefMacro), sizeof(UserMacro.UserDefMacro), 0);
	sm_reset_user_macro_send_list();

	return;
}

/**
 * handle_keep alive status
**/
void
handle_keep_alive_status (session_list_t*	send_list, session_list_t* session_list)
{
	ivtp_hdr_t		v_hdr;

	form_status_ctrl_pkt_hdr (&v_hdr, IVTP_KEEP_ALIVE_PKT, 0, 0 );//has to be checked
	sm_build_keep_alive_send_list (send_list, session_list);
	send_control_pkt (&v_hdr, send_list, NULL, 0, 0);
	sm_reset_keep_alive_send_list(send_list);
	return;
}

/**
 * handle_control_packet
**/
BOOL
is_handle_control_packet (void)
{
	session_list_t	session_list;
	session_list_t	send_list;
	session_info_t *first_session;
	session_info_t *session;
	session_info_t *webpreview_session;
	/* check for stop signal */
	if (sm_is_set_stop_signal ())
	{
		TINFO_4 ("stop signal set\n");
		handle_stop_signal (&send_list, &session_list);
		return TRUE;
	}
	if(sm_get_webpreviewer_status())
	{
		webpreview_session = sm_is_set_webpreviewer_packet_send();
		if(webpreview_session != NULL)
		{
			handle_web_preview_data_send(webpreview_session);
			sm_set_webpreviewer_status(FALSE);
			sm_reset_webPreviewer_data_send(webpreview_session->h_session);
		}
	}
	if (services_changed)
	{
		services_changed = 0;
		send_service_info(&send_list, &session_list, service_info_data);
	}

	if(sm_is_set_media_license_send())
	{
		send_media_license_info(&send_list, &session_list);
		sm_reset_media_license_send();
	}

	if (sm_is_set_active_client_send  ())
	{
		TINFO_4 ("stop signal set\n");
		handle_active_client_send (&send_list, &session_list);
		sm_reset_active_client_send();
		return TRUE;
	}

	if (sm_is_set_power_status_send_all())
	{
		handle_power_cmd_status_send_all (&send_list, &session_list);
		sm_reset_power_status_send_all();
	}
	
	if (sm_is_mouse_media_status())
	{
		
		send_mouse_media_info(&send_list, &session_list);
		sm_reset_mouse_media_status();
		set_usb_mouse_mode(g_mouse_mode);
	}

	if (( session=sm_is_power_status_send ()) != NULL)
	{
		handle_host_power_status (session);
		
		return TRUE;
	}
	if (( session=sm_is_power_cmd_status_send( ) ) != NULL)
	{
		handle_power_cmd_status_send (session);
		
		return TRUE;
	}
	if (sm_is_set_send_encrypt_status ())
	{
		TINFO_4 ("Send Encrypt status set\n");
		handle_send_encrypt_status (&send_list, &session_list);
		return TRUE;
	}

	if (sm_is_set_keep_alive_flag ())
	{
		TINFO_4 ("Send keep alive packet status \n");
		handle_keep_alive_status (&send_list, &session_list);
		return TRUE;
		}

	if (sm_is_set_auto_bw_detect ())
	{
		TINFO_4 ("Auto Detect Bw flag set\n");
		handle_auto_bw_detect (&send_list, &session_list);
		return TRUE;
	}
	
	if (sm_is_set_send_usermacro ())
	{
		TINFO_4("Sending  User Macro signal set\n");
		handle_send_usermacro  (&send_list, &session_list);
		return TRUE;
	}
	
	if( (first_session=sm_get_kvm_priv_flag_session()) != NULL )
	{
		TINFO_4 ("KVm previlage  flag set\n");
		handle_kvm_priv_packet(first_session);	
		sm_reset_kvm_priv_flag(first_session);
		
	}

	if( (first_session=sm_get_kvm_master_change_session()) != NULL )
	{
		handle_kvm_master_packet(first_session);
		sm_reset_master_change_flag(first_session);
	}

	if(sm_is_set_pending_client_response())
	{
		handle_pending_sesssion_response(&send_list, &session_list);
		sm_reset_pending_client_response();
	}

	if( ( session=sm_is_socket_close_ctrl_packet() ) != NULL)
	{	
		TINFO_4 ("Socket close status  flag set\n");
		handle_socket_close(session);
		sm_reset_socket_cntrl_pkt(session);	
			
	}

	if( ( session=sm_is_send_raw_ipmi_resp_send() ) != NULL)
	{	
		TINFO_4 ("\n Send Raw ipmi command is set\n");
		handle_ipmi_raw_resp_send(session);
			
	}
	if(sm_is_host_lock_status())
	{
		handle_host_lock_status(&send_list, &session_list);
		sm_reset_host_lock_status();
	}
	if (sm_is_kvm_priv_resp_send())
	{
		handle_kvm_priv_resp();
		sm_reset_kvm_priv_resp_send();
	}
	if(g_corefeatures.session_management && g_corefeatures.vmedia_dynamic_free_instance_update == ENABLED)
	{
		if(sm_is_vmedia_update_status())
		{
			handle_update_Vmedia_device_status(&send_list, &session_list);
			sm_reset_vmedia_update_status();
		}
	}
	
	return FALSE;
}

//Capture screen flag
int  capturescreen = 1;

/**
 * avse_main
**/
void
avse_main (void)
{
	int 			i = 0;
	u32			data_buf_size=0, frame_hdr_size=0;
	void*			p_frame_hdr = NULL;
	void*			p_data_buf = NULL;
	session_list_t		session_list;
	session_list_t		send_list;
	int			cap_status = 0;
	BOOL			send_immediate_flag;
	int 			retval = 0;
	BOOL                    is_qlzw_init = FALSE;
	ivtp_hdr_t              v_hdr;
	int 			prev_cap_status = -1;
    	void                    *soc_handle = NULL;
    	int 	                (*send_video_data) (int, void *, void *, int, int);
	int                     no_signal_frame_cnt = 0; 
    	struct timespec sleep = {0, 1000000};	
    	prctl(PR_SET_NAME,__FUNCTION__,0,0,0);
	BOOL isValid = FALSE;
        
	while (1)
	{
		prev_cap_status = -1;

		/* This semaphore is posted once validate_video() succeeds. Until then, this thread waits */
		sem_wait(&m_start_avse);

        //Once avse thread invoked, Suspend Continuous recording if enabled
        if(g_corefeatures.record_pre_boot_or_crash_video == ENABLED)
        {
            g_sync_rec_jviewer = 1;
        }
	
		//check for media Licensing
		if(g_corefeatures.runtime_license_support == ENABLED)
		{
			isValid = getLicenseStatus(MEDIA);
			g_media_license_status = isValid;
		}

		while (1)
		{
			/**
			 * Get the session list from the session manager.
			**/
			sm_get_valid_session_list (&session_list, TRUE);

			/* If there are no active sessions avalable, send a no master response
			to the pending sessions. Otherwise do nothing */
			if (session_list.num_session == 0)
			{
				TINFO ("no session available\n");
				handle_pending_sesssion_response(&send_list, &session_list);
				break;
			}
			
			if(g_web_preview_write_file)
			{
				session_list.is_new_session = TRUE;	
			}
			
			/* Verify for web-session closure so as to close the KVM Client also */
			check_if_logout();

			if (is_handle_control_packet ())
			{
				is_qlzw_init    = TRUE;
				continue;
			}

			if ( session_list.is_new_session )
			{
				//Set capturescreen flag to capture a screen on new session
				capturescreen = 1;
				is_qlzw_init    = TRUE;
			}

			if (is_qlzw_init || (session_list.num_session > 1))
                        {
                                qlzw_init ();
                                session_list.qlzw_init_done = TRUE;
                                is_qlzw_init    = FALSE;
                        }

			/** If there are any new sessions added, send a new frame needs to all clients **/
			if ( g_send_full_screen )
			{
				session_list.is_new_session = TRUE;
				//set capture screen as fullscreen is requested.
				capturescreen = 1;
			}
			//If video recording is in progress, video is captured
			if(g_record_processing == 0) 
			{	
				if(skip_video(&session_list))
					capturescreen = 0;//set capturescreen 0 to skip video capture
			}

			setflags_before_videocapture( &session_list );

			/** If there are more than one sessions - connection manager needs to send immediately **/
			if (session_list.num_session > 1) { send_immediate_flag = TRUE;  }
			else				  { send_immediate_flag = FALSE; }
		
			if(g_corefeatures.vdd_power_check_get_video) 
			{ 
			    if(g_host_power_status == 1) //power on
			    { 
			        cap_status = capture_video ( &frame_hdr_size, &data_buf_size, &p_frame_hdr, &p_data_buf, &session_list );
			    }
			    else //power off
			    { 
			        cap_status = CAPTURE_VIDEO_NO_SIGNAL; 
			    }
			}
			else
			{
				if(capturescreen == 1)
				{
					cap_status = capture_video ( &frame_hdr_size, &data_buf_size, &p_frame_hdr, &p_data_buf, &session_list );
				}
			}

			if(g_corefeatures.capture_bsod_jpeg == ENABLED)
			{
				soc_handle = dlopen(JPEGCONVERT_LIBRARY , RTLD_LAZY);

				// This will call definition to update the soc session hdr
				if(!soc_handle)
				{
					TCRIT("%s\n",dlerror());
				}
				else
				{
					send_video_data = dlsym(soc_handle, SEND_VIDEO_DATA);
					if (send_video_data)
					{
						if (send_video_data (cap_status, p_frame_hdr, p_data_buf, (int)data_buf_size, (int)frame_hdr_size) < 0)
						{
							TINFO("Unable to send the Data to convert as jpeg file\n");
						}
					}
					dlclose(soc_handle);
				}
			}

			soc_handle = dlopen(VIDEORECORD_LIBRARY, RTLD_LAZY);

        		// This will call definition to update the soc session hdr
			if(!soc_handle)
			{
				TCRIT("%s\n",dlerror());
			}
			else
        		{
                		send_video_data = dlsym(soc_handle, SEND_VIDEO_DATA);
                		if (send_video_data)
                		{
                        		if (send_video_data (cap_status, p_frame_hdr, p_data_buf, (int)data_buf_size, (int)frame_hdr_size) < 0)
					{
						TINFO("Unable to send the Data to Dump file\n");
					}
                		}

                		dlclose(soc_handle);
        		}

			if(g_web_preview_write_file)
			{
				if(session_list.is_new_session) 
				{
					On_write_File_web_preview(cap_status, p_frame_hdr, p_data_buf, (int)data_buf_size, (int)frame_hdr_size);
					sm_set_webpreviewer_status(TRUE);
					g_web_preview_write_file = false;
				}
			}
			switch (cap_status)
			{
				case CAPTURE_VIDEO_SUCCESS:
					no_signal_frame_cnt =0;
					break;
				case CAPTURE_VIDEO_NO_CHANGE:
					no_signal_frame_cnt =0;
					// Just return on NO_CHANGE. If NO_CHANGE persists for more than 50 captures
					// Then, we delay the capture further on, until status changes
					break;		
				case CAPTURE_VIDEO_NO_SIGNAL:
					// Send the NO_SIGNAL packet to client only once, when Screen just went blank
					// No need to send anymore.
					// If this condition persists for more than 50 captures
					// Then, we delay the capture further on, until status changes
					
					if (prev_cap_status != cap_status || session_list.is_new_session || (no_signal_frame_cnt > 50)) // || (g_send_full_screen || session_list.is_new_session))
					{
						no_signal_frame_cnt =0;
						form_status_ctrl_pkt_hdr (&v_hdr, IVTP_BLANK_SCREEN, 0, STATUS_HOST_POWERED_OFF);
						sm_build_send_list (&send_list, &session_list);
						send_control_pkt (&v_hdr, &send_list, NULL, 0, 0);
					}
					no_signal_frame_cnt++;
		                        if(g_corefeatures.vdd_power_check_get_video) 
		                        { 
		                        	nanosleep(&sleep, NULL);
		                        } 

					break;
				default:
					break;
			}

			//depends on video capture status postpone next video capture
			setflags_after_videocapture( &prev_cap_status, cap_status );

			if (cap_status != CAPTURE_VIDEO_NO_SIGNAL)
			{
				//build active sessions list
				sm_build_send_list (&send_list, &session_list);
				//send non video frames(keybd led,cursor pos,color pallette etc) to active clients
				send_non_video_frame_data( &send_list, p_frame_hdr );
			}

			if (cap_status != CAPTURE_VIDEO_SUCCESS)
			{
				is_qlzw_init = TRUE;
                                if(g_corefeatures.vdd_power_check_get_video) 
                                { 
                                	nanosleep(&sleep, NULL);
                                }
				continue;
			}

			retval = cm_send_video_frame (p_frame_hdr, frame_hdr_size, p_data_buf, data_buf_size, &send_list, send_immediate_flag);
	
			if (retval == ERR_ABORTED)
			{
				TINFO ("Capture aborted\n");
				is_qlzw_init 	= TRUE;
				continue;
			}
			if (retval == ERR_CONNECTION_LOST)
			{
				TINFO ("CONNECTION LOST: Should close the session\n");
				for (i = 0; i < send_list.num_session; i++)
				{
					acquire_mutex (send_list.p_session [i]->p_access_guard);
					if (send_list.p_session [i]->transfer_status == ERR_CONNECTION_LOST)
					{
						if (session_list.p_session [i]->is_used)
						{
							session_list.p_session [i]->clean = TRUE;
							sm_set_transfer_status(session_list.p_session [i]->h_session,ERR_CONNECTION_LOST);
							if(g_corefeatures.kvm_reconnect_support == ENABLED)
							{
								if(&session_list.p_session [i]->failed_time == 0)
								{
									time(&session_list.p_session [i]->failed_time);
								}
							}
							
						}
					}
					release_mutex (send_list.p_session [i]->p_access_guard);
				}
				continue;
			}
		}

		TINFO ("AVSE Exit\n");
        //Reset the variable to restart RecordBootCrash thread
        if( (g_corefeatures.record_pre_boot_or_crash_video == ENABLED) && (g_sync_rec_jviewer) )
        {
            g_sync_rec_jviewer = 0;
            sem_post(&m_wait_rec);
        }
	}
}


