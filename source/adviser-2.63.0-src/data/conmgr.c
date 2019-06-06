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
* @file		Conmgr.c
* @author	Govind Kothandapani		<govindk@ami.com>
*		Baskar Parthiban		<bparthiban@ami.com>
*		Varadachari Sudan Ayanam	<varadacharia@ami.com>
* @brief	contains functions to establish new sessions and to
*			send video data to all the sessions
****************************************************************/

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <errno.h>
#include "conmgr.h"
#include "sessionmgr.h"
#include "sync.h"
#include "cmdhndlr.h"
#include "reader.h"
#include "adviserdbg.h"
#include "ncml.h"
#include "adviser_cfg.h"
#include "hid_cmdhandlr.h"
#include "featuredef.h"
#include "racsessioninfo.h"
#include "avse.h"

/* for ipv6 */
#include <netdb.h>
#include "dlfcn.h"

#define SINGLE_PORT_APP_HOOKS_PATH	"/usr/local/lib/libsingleportapp.so"
#define INITILIZE_UDS_SERVER			"InitializeUDSServer"

#define MAX_RETRY 3
#define BOTH_INTERFACES		"both"
#define LOOP_BACK_IP		"lo"

#define KVM_ENC_ENABLED		1

/**----------------------- Module Variables ------------------------------**/
extern sem_t	m_start_cmdhndlr;
extern int	max_session;
SERVICE_CONF_STRUCT     AdviserConf;
extern CoreFeatures_T g_corefeatures;
extern u8 g_isSinglePortEnabled;
int			kvm_max_session = 0;

int generate_sock_desc(int port, char* interface);
extern volatile sig_atomic_t g_sigup;

int 
updateSinglePort()
{
	int	(*IsSinglePortEnabled) ();
	void	*singleport = NULL;
	if(g_corefeatures.single_port_app == ENABLED)
	{
		if(g_corefeatures.runtime_singleport_support == ENABLED)
		{
			singleport = dlopen(SINGLE_PORT_APP_HOOKS_PATH, RTLD_LAZY);

			// This will call definition to update singleport status
			if (singleport)
			{
				IsSinglePortEnabled = dlsym(singleport, IS_SINGLE_PORT_ENABLED);
				if (IsSinglePortEnabled)
				{
					g_isSinglePortEnabled= IsSinglePortEnabled();
				}
				else
				{
					TEMERG("%s : dlsym failed  errno:%d  . Closing Video server\n",IS_SINGLE_PORT_ENABLED,errno);
					dlclose(singleport);
					exit(1);
				}
				dlclose(singleport);
			}
			else
			{
				TEMERG("%s : dlopen failed  errno:%d . Closing Video server\n",SINGLE_PORT_APP_HOOKS_PATH,errno);
				exit(1);
			}
		}
		else
		{
			g_isSinglePortEnabled = ENABLED;
		}
	}
	return 0;
}

int 
fetchServiceConfigurations()
{
	SERVICE_CONF_STRUCT	WebConf;
	int web_max_session = 0;
	int save_conf = 0;
	int sec_port= -1;
	int port_num = ADVISER_PORT;

	if (get_service_configurations(KVM_SERVICE_NAME, &AdviserConf) != 0)
	{
		TCRIT("Unable to Fetch the KVM Configuration\n");
		return -1;
	}

	if (get_service_configurations(WEB_SERVICE_NAME, &WebConf) != 0)
	{
		TCRIT("Unable to Fetch the Web Configuration\n");
		return -1;
	}


	/*Checking for KVM Non-secure port*/
	if (isNotApplicable((unsigned char *)&(AdviserConf.NonSecureAccessPort), 
		sizeof(AdviserConf.NonSecureAccessPort)) )
	{
		TCRIT("KVM Session, Non Secure Port data is not applicable\n");
		return -1;
	} else {
		save_conf ++;
		getNotEditableData((unsigned char *)&(AdviserConf.NonSecureAccessPort), 
			sizeof(AdviserConf.NonSecureAccessPort), 
			(unsigned char *)&(port_num));
		TDBG("KVM Session, Non Secure Port value::%d\n", port_num);
	}
	//If single port feature is enabled set MSB bit of port number to make it as not editable field
	if(g_isSinglePortEnabled == ENABLED)
	{
		//Setting MSB bit to make port as non editable
		getNotEditableMaskData((unsigned char *)&(AdviserConf.NonSecureAccessPort),sizeof(AdviserConf.NonSecureAccessPort),NULL);

		if (isNotApplicable((unsigned char *)&(AdviserConf.SecureAccessPort), 
				sizeof(AdviserConf.SecureAccessPort)) ||
			isNotEditable((unsigned char *)&(AdviserConf.SecureAccessPort),
						 sizeof(AdviserConf.SecureAccessPort))) 
		{
	
		}
		else {
			save_conf ++;
			getNotEditableData((unsigned char *)&(AdviserConf.SecureAccessPort), 
				sizeof(AdviserConf.SecureAccessPort), 
				(unsigned char *)&(sec_port));

			 //Setting MSB bit to make port as non editable
			getNotEditableMaskData((unsigned char *)&(AdviserConf.SecureAccessPort),sizeof(AdviserConf.SecureAccessPort),NULL);
			TDBG("KVM Session,  Secure Port value::%d\n", sec_port);
		}
	}
	//If single port feature is Disabled reset MSB bit of port number to make it as Editable field
	else{
		if(isNotEditable((unsigned char *)&(AdviserConf.NonSecureAccessPort),sizeof(AdviserConf.NonSecureAccessPort)) ){
			AdviserConf.NonSecureAccessPort = port_num;
			save_conf ++;
		}
		if(isNotEditable((unsigned char *)&(AdviserConf.SecureAccessPort),sizeof(AdviserConf.SecureAccessPort)) ){
			getNotEditableData((unsigned char *)&(AdviserConf.SecureAccessPort), 
				sizeof(AdviserConf.SecureAccessPort), 
				(unsigned char *)&(sec_port));
			AdviserConf.SecureAccessPort = sec_port;
			save_conf ++;
		}
	}

	/*Checking for KVM MaxAllowSession*/
	if (isNotApplicable((unsigned char *)&(AdviserConf.MaxAllowSession), 
		sizeof(AdviserConf.MaxAllowSession))) {
		TCRIT("KVM Session, MaxAllowSession data is not applicable\n");
		return -1;
	} else {
		getNotEditableData((unsigned char *)&(AdviserConf.MaxAllowSession), 
			sizeof(AdviserConf.MaxAllowSession), 
			(unsigned char *)&(kvm_max_session));
		TDBG("KVM Session, MaxAllowSession value::%d\n", kvm_max_session);
	}

	if(save_conf >=1)
	{
		if (set_service_configurations(KVM_SERVICE_NAME, &AdviserConf,g_corefeatures.timeoutd_sess_timeout) != 0)
			TCRIT("KVM Session, Unable to Set the KVM Configuration\n");
	}

	/*Checking for WEB MaxAllowSession*/
	 if (isNotApplicable((unsigned char *)&(WebConf.MaxAllowSession), 
		sizeof(WebConf.MaxAllowSession))) {
		TCRIT("WEB Session, MaxAllowSession data is not applicable\n");
		return -1;
	} else {
		getNotEditableData((unsigned char *)&(WebConf.MaxAllowSession), 
			sizeof(WebConf.MaxAllowSession), 
			(unsigned char *)&(web_max_session));
		TDBG("WEB Session, MaxAllowSession value::%d\n", web_max_session);
	}
	
	max_session = (kvm_max_session + web_max_session);
	if( g_corefeatures.session_management != ENABLED )
	{
		if(set_active_session(KVM_SERVICE_ID_BIT,0) != 0)
		{
			TCRIT("Adviser:Unable to get the active session count");
		}
	}
	return 0;

}
static void UpdateSockDesc(int *server_sock,int *loopback_sock,int *UDS_server_sock,int iPort,void *soc_handle)
{

	if(*loopback_sock > 0){
		close(*loopback_sock);
		*loopback_sock = 0;
	}
	if(*server_sock >0){
		close(*server_sock);
		*server_sock=0;
	}
	if(*UDS_server_sock > 0){
		close(*UDS_server_sock);
		*UDS_server_sock = 0;
	}
		
	if(g_isSinglePortEnabled== ENABLED)
	{

		int	(*InitializeUDSServer) (char *sock);
		soc_handle = dlopen(SINGLE_PORT_APP_HOOKS_PATH, RTLD_LAZY);

		// This will call definition to update the soc session hdr
		if (!soc_handle)
		{
			TCRIT("%s\n",dlerror());
		}
		else
		{
			InitializeUDSServer = dlsym(soc_handle, INITILIZE_UDS_SERVER);
			if(InitializeUDSServer == NULL)
				TCRIT("dlsym failed errno:%d\n",errno);

			if (InitializeUDSServer)
			{
				*UDS_server_sock = InitializeUDSServer(ADVISER_SOCKET_PATH);
				if (*UDS_server_sock < 0)
				{
					TCRIT("Unable to get sock to communicate\n");
				}
			}
		}
		dlclose(soc_handle);
	}

	if (get_service_configurations(KVM_SERVICE_NAME, &AdviserConf) != 0)
	{
		TCRIT("Unable to Fetch the KVM Configuration\n");
		return;
	}

	// if either singleport or kvm encryp*server_socktion is enabled, the adviser will be listening to loopback ip.
	if(KVM_ENC_ENABLED == GetAdviserSecureStatus() || (g_isSinglePortEnabled== ENABLED))
	{
		*server_sock = generate_sock_desc(iPort,LOOP_BACK_IP);
	}
	else
	{
		// if both singleport and kvm encryption are disabled, the adviser will be listening to kvm configured interface.
		// incase of html5 kvm support, in addition to kvm configured interface, adviser will also be listening to loopback interface.
		// if the interface name is "both", adviser will be listening to all the interfaces.
		if (strncasecmp(AdviserConf.InterfaceName, BOTH_INTERFACES,strlen(BOTH_INTERFACES)) != 0)
		{
			if(g_corefeatures.html5_kvm_support == ENABLED)
			{
				*loopback_sock = generate_sock_desc(iPort,LOOP_BACK_IP);
				if(*loopback_sock == -1)
				{
				TCRIT("Couldn't create socket.\n");
				return ;
				}
			}
		}
		*server_sock = generate_sock_desc(iPort,AdviserConf.InterfaceName);
	}

	if(*server_sock == -1)
	{
		TCRIT("Couldn't create socket.\n");
		close(*loopback_sock);
		return ;
	}

	TINFO_1 ("[Main Thread] : Listening on Port = %d.\n", iPort);
}
/**
 * @brief Accepts connection requests and reads any incoming packets.
 * @return returns 0
 **/
int
connection_acceptor (int iPort)
{
	int			server_sock=0; // will be bound to kvm configured interface when singleport and kvm encryption are disabled
	int			loopback_sock=0; // will be bound to loopback ip when html kvm support is enabled.

	struct sockaddr_storage		client_sockaddr;
	fd_set			socks;
	int			highsock = -1;
	int			retval = 0;

	int			UDS_server_sock = -1;

	void	*soc_handle = NULL;
	int update = 0;
	int	(*WaitForUDSClientConn) (int UDS_sock);

	UpdateSockDesc(&server_sock,&loopback_sock,&UDS_server_sock,iPort,soc_handle);

	while (1)
	{
		update=0;
		/*check if any incoming command for adviser*/
		update=check_incoming_command();
		if(update == 1)
		{
			sleep(3); // Wait until configurations get updated
			updateSinglePort();
			fetchServiceConfigurations();
			UpdateSockDesc(&server_sock,&loopback_sock,&UDS_server_sock,iPort,soc_handle);
			continue;
		}
			
		if(g_corefeatures.session_management == ENABLED)
			clear_invalid_register_session();

		FD_ZERO (&socks);
		FD_SET (server_sock, &socks);
		highsock = server_sock;
	
		if(loopback_sock != -1)
		{
		    FD_SET (loopback_sock, &socks);
		    highsock = (highsock > loopback_sock )? highsock : loopback_sock;
		}

		if(UDS_server_sock != -1)
		{
			FD_SET (UDS_server_sock, &socks);
			highsock = (highsock > UDS_server_sock )? highsock :UDS_server_sock;
		}

		struct timeval	tv;

		tv.tv_sec   = 2;
		tv.tv_usec  = 0;

		retval = select (highsock + 1, &socks, NULL,NULL, &tv);
		if (retval < 0 || (g_sigup > 0))
		{
			if (errno == EINTR || (g_sigup > 0)) {
				//On receiving signal to process select will get interupted.
				adviser_process_signal();
				continue;
			}
			else
			{
				TCRIT ("select failed - Exiting program%d %d\n", EINTR, retval);
				perror ("select");
				close(loopback_sock);
				close(server_sock);
				return -1;
			}
		}

		/**check for pending session and send the status packet**/
		sm_check_pending_sessions();
		/* Clean up the sessions - if need be */
		sm_clean_sessions ();

		int new_sock;
		/* Check to see if there is a request for a new connection */
		if(server_sock != -1)
		{
		    if (FD_ISSET (server_sock, &socks))
		    {
			/* new connection  - accept it */
			unsigned int rlen;

			rlen = sizeof(struct sockaddr);
			new_sock = accept (server_sock, (struct sockaddr*) &client_sockaddr, &rlen);
			if(new_sock != -1)
			{
			    new_connection_acceptor(new_sock,iPort);
			}
		    }
		}
		/* Check to see if there is a request for a new connection */
		if(loopback_sock != -1)
		{
		    if (FD_ISSET (loopback_sock, &socks))
		    {
			/* new connection  - accept it */
			unsigned int rlen;

			rlen = sizeof(struct sockaddr);
			new_sock = accept (loopback_sock, (struct sockaddr*) &client_sockaddr, &rlen);
			if(new_sock != -1)
			{
			    new_connection_acceptor(new_sock,iPort);
			}
		    }
		}
		if (UDS_server_sock != -1)
		{
			if (FD_ISSET (UDS_server_sock, &socks))
			{
				new_sock = -1;
				soc_handle = dlopen(SINGLE_PORT_APP_HOOKS_PATH, RTLD_LAZY);

				// This will call definition to update the soc session hdr
				if (!soc_handle)
						{
							TCRIT("%s\n",dlerror());
						}

				else
				{
					WaitForUDSClientConn = dlsym(soc_handle, WAIT_FOR_UDS_CLIENT);

					if(WaitForUDSClientConn == NULL)
						TCRIT("wait for UDS Client : dlsym failed  errno:%d \n",errno);

					if (WaitForUDSClientConn)
					{
						new_sock = WaitForUDSClientConn(UDS_server_sock);
					}
				}
				dlclose(soc_handle);

				if(new_sock != -1)
				{
					new_connection_acceptor(new_sock,iPort);
				}
			}
		}
	}/* while(1) */
}

/**
 * @brief Accepts New socket connection requests and reads any incoming packets.
 * @return returns 0
 **/
int
new_connection_acceptor ( int new_sock ,int iPort)
{
	int	off = 1;
	int	num_sessions=0;

	if (new_sock > 0)
	{
		/* Turn off naggle */
		if(setsockopt(new_sock, SOL_TCP, TCP_NODELAY, &off, sizeof(int)) != 0)
		{
			TWARN("Unable to off naggle %s\n", strerror(errno));
		}

		int flags = 0;
		if ((flags = fcntl(new_sock, F_GETFL, 0)) == -1)
		{
			flags = 0;
			TWARN("unable to get the flags for the socket\n");
		}

		if (fcntl(new_sock, F_SETFL, flags|O_NONBLOCK) == -1)
		{
			TWARN("Unable to set the socket NON_BLOCKING\n");
		}

		num_sessions = sm_get_num_session ();
		/* If we already are out of session - close this socket */
		if ( num_sessions >= max_session)
		{
			/**
			 * Max Session reached
			 * Hence, sending a closure command to the client
			 * to disconnect the connection
			**/
			send_session_approval(new_sock, FALSE,0);
			shutdown (new_sock, SHUT_RDWR);
			close (new_sock);
		}
		else
		{
			/**
			 * Session approved
			 * Sending approval command to the client to proceed
			**/
			send_session_approval(new_sock, TRUE,0);

			if(g_corefeatures.kvm_reconnect_support == ENABLED)
			{
				int ret =-1;
				ivtp_hdr_t pkt;
				 session_info_t* p_si;p_si = NULL;

				ret = recv_bytes (new_sock,(unsigned char *)&pkt,sizeof(ivtp_hdr_t),p_si);
				if(0 !=ret )
				{
					return -1;
				}
				else
				{
					if(pkt.pkt_size > 0)
					{
						on_reconnect_pkt(new_sock,pkt.pkt_size);
						return 0;
					}
				}
			}
			hsession_t hsession;
			/* Create a new session */
			hsession = sm_reserve_session (new_sock, iPort);
			if (hsession == INVALID_SESSION_ID)
			{
				close (new_sock);
			}
			else
			{
				/* we got a first client for our session list. so program fpga now.*/
				if( num_sessions == 0 )
				{
					//TINFO("Got a fresh connection.");
					/* This semaphore is held in cmdhndlr_main(). Posting here */
					sem_post(&m_start_cmdhndlr);
				}
			}
		}
	}
	return 0;
}

/**
 * cm_send_video_frame
**/
int
cm_send_video_frame (	void*		p_frame_header,
			u32		frame_hdr_size,
			void*		frame_data_ptr,
			u32		frame_size,
			session_list_t*	p_session_list,
			BOOL		send_immediate)
{
	int		i;
	u8*	p_frame_hdr = (u8*) p_frame_header;
	u8*	p_frame_data = (u8*) frame_data_ptr;

	if (p_session_list->num_session == 0)
	{
		return 0;
	}

	/* Prepare the session info for transfer */
	sm_prepare_session_for_transfer ();
	*(p_session_list->p_session [0]->p_pending_clients) = p_session_list->num_session;

	/* For all the clients in the send list */
	for (i = 0; i < p_session_list->num_session; i++)
	{
		session_info_t* p_session_info = p_session_list->p_session [i];

		/**
		 * Fill the session info structure.
		 * Some of the fields for the client info is already filled when
		 * the session is established or when the configuration is changed
		 * for the client. We do not fill them here.
		**/
		/* ... Acquire exclusive access to the Client info */
		acquire_mutex (p_session_info->p_access_guard);

		/* ... Fill the information */
		p_session_info->is_frame_pkt			= TRUE;
		p_session_info->p_frame_hdr			= p_frame_hdr;
		p_session_info->frame_hdr_size			= frame_hdr_size;
		p_session_info->p_frame_data			= p_frame_data;
		p_session_info->frame_size			= frame_size;
		p_session_info->transfer_status			= 0;

		if (send_immediate) { p_session_info->send_fragmented = FALSE; }
		else { p_session_info->send_fragmented = TRUE; }

		/* ... Release exclusive access to the client info */
		release_mutex (p_session_info->p_access_guard);

		/* Wakeup the client worker thread */
		sem_post (p_session_info->pev_wakeup);
	}


	/* Wait for the current transfer to be complete */
	sm_wait_for_transfer_complete ();

	TINFO_4 ("send done..\n");

	/* Check for any aborted conditions */
	if(g_corefeatures.kvm_reconnect_support != ENABLED)
	{
		acquire_mutex (p_session_list->p_session [0]->p_global_guard);
		if ((*(p_session_list->p_session [0]->p_abort_count)) != 0)
		{
			release_mutex (p_session_list->p_session [0]->p_global_guard);
			TINFO_2 ("transfer aborted\n");
			return ERR_ABORTED;
		}
		release_mutex (p_session_list->p_session [0]->p_global_guard);
	}

	/* check for Connection Error */
	for (i = 0; i < p_session_list->num_session; i++)
	{
		acquire_mutex (p_session_list->p_session [i]->p_access_guard);
		if (((p_session_list->p_session [i]->transfer_status) != SUCCESS) != 0)
		{
			TINFO_2 ("Transfer status->Error Connection\n");
			sm_set_transfer_status(p_session_list->p_session [i]->h_session,ERR_CONNECTION_LOST);
			release_mutex (p_session_list->p_session [i]->p_access_guard);
			return ERR_CONNECTION_LOST;
		}
		release_mutex (p_session_list->p_session [i]->p_access_guard);
	}


	return 0;
}


/**
 * cm_abort_current_frame
**/
int
cm_abort_current_frame (hsession_t hsession)
{
	sm_set_abort_flag (hsession);
	return 0;
}


//###########################################################//
/**
 *  * send_control_pkt
 *  **/
int send_control_pkt (ivtp_hdr_t* p_pkt, session_list_t* p_session_list, u8* data, int size, int pkt_type)
{
	int             i;

	//TINFO_4 ("Sending Ctrl Packet\n");

	if (p_session_list->num_session == 0)
	{
		return 0;
	}

	/* Prepare the session info for transfer */
	sm_prepare_session_for_transfer ();

	*(p_session_list->p_session [0]->p_pending_clients) = p_session_list->num_session;

	/* For all the clients in the send list */
	for (i = 0; i < p_session_list->num_session; i++)
	{
		session_info_t* p_session_info = p_session_list->p_session [i];

		/**
		 * Fill the session info structure.
		 * Some of the fields for the client info is already filled when
		 * the session is established or when the configuration is changed
		 * for the client. We do not fill them here.          **/

		/* ... Acquire exclusive access to the Client info */
		acquire_mutex (p_session_info->p_access_guard);

		/* ... Fill the information */
		p_session_info->is_ctrl_pkt             = TRUE;
		p_session_info->ctrl_pkt_type           = pkt_type;
		p_session_info->p_ctrl_pkt              = p_pkt;
		p_session_info->ctrl_transfer_status         = 0;
		p_session_info->ctrl_frame_size              = size;
		p_session_info->p_ctrl_frame_data            = data;

		/* ... Release exclusive access to the client info */
		release_mutex (p_session_info->p_access_guard);

		/* Wakeup the client worker thread */
		sem_post (p_session_info->pev_wakeup);
	}

	/* Wait for the current transfer to be complete */
	sm_wait_for_transfer_complete ();

	//TINFO_4 ("Ctrl Packet Sent\n");
	return 0;
}

/**
 * generate_sock_desc - creates a socket and binds it to the interface.
 * interface - interface to be bound to.
 * port - port number
 **/
int generate_sock_desc(int port, char* interface)
{
	int			server_sock=0;
	int			reuseaddr = 1;

	//int                     num_listen_socks =0;
	int                     error;
	int			retry;

	/* variables and structures that is used for getaddrinfo call */
	struct addrinfo         hints, *res, *r;
	char			serv[6] = {0};
	int                     is_listening = 0;

	if(port <= 0 || interface == NULL)
	{
	    TCRIT("Invalid port = %d (or) interface = %s\n", port, interface);
	    return -1;
	}

	memset(&hints, 0, sizeof(hints));

	/* Use getaddrinfo call to get number of avialble socket address families those are available in the kernel.
	 * By using this call we are able to write the IPv4/IPv6 kernel independent code. so that the application will run on IPv4/IPv6 supported kernel.
	 */

	/* Fill addrinfo struct values to get the avialable services */
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ( sizeof(serv) <= (unsigned) snprintf(serv, sizeof(serv), "%d", port))
	{
		TCRIT("'serv' value truncated in snprintf");
	}

	/* Using getaddrinfo call to get available socket address families those are supported in the kernel*/
	if ((error = getaddrinfo(NULL, serv, &hints, &res)))
	{
		TCRIT("Couldn't get bind addresses: %s\n", gai_strerror(error));
		return -1;
	}

	if (res == NULL)
	{
		TCRIT("No addresses to bind to.\n");
		return -1;
	}

	/* Now bind sockets to these addresses. */
	for (r = res /*,num_listen_socks = 0*/; r != NULL; r = r->ai_next)
	{
		if(g_corefeatures.global_ipv6 == ENABLED)
		{
			/*
			* Check the Network Family before creating socket. we have to allow only IPv6
			* family becuase it is possible to create the IPv4 socket instead of IPv6.
			* So add proper validation here.
			*/
			if(r->ai_family != AF_INET6)
				continue;
		}
		if(server_sock)
		{
			close(server_sock);
			server_sock=0;
		}
		if ((server_sock = socket(r->ai_family, r->ai_socktype, IPPROTO_TCP)) < 0)
		{
			TWARN ("Couldn't make socket");
			/* If socket creation fails, don't quit. just try to create next one. */
			continue;
		}

		for (retry = 0; retry < MAX_RETRY; retry++)
		{
		    if (strncasecmp(interface, BOTH_INTERFACES,strlen(BOTH_INTERFACES)) != 0)
		    {
				if (setsockopt(server_sock, SOL_SOCKET, SO_BINDTODEVICE, interface, strlen(interface)) == -1)
					TWARN("Setsockopt(SO_BINDTODEVICE) Failed for server socket\n");
		    }

		    if (setsockopt(server_sock,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(int)) == -1)
				TWARN("Setsockopt(SO_REUSEADDR) Failed for server socket\n");

		    /* Bind to VIDEO_PORT */
		    if (bind(server_sock, r->ai_addr, r->ai_addrlen) < 0)
		    {
				TWARN ("Unable to bind server socket on port %d %s.\n", port, strerror(errno));
				sleep(5);
				continue;
		    }
		    else
		    {
				break;
		    }
		}

		if (retry >= MAX_RETRY)
		{
			continue;
		}
		/* Listen on the socket */
		if (listen(server_sock, 1) < 0)
		{
			TWARN ("Unable to listen to socket\n");
			continue;
		}
		is_listening = 1;
		break;
	}
	/* free the res struct used by getaddrinfo call */
	freeaddrinfo(res);

	if (is_listening == 0)
	{
		/* no sockets are in listen state */
		TWARN ("Could not bind any address\n");
		close(server_sock);
		return -1;
	}
	return server_sock;
}
