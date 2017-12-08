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
 * Session_mgr.h
 * Session Manager
**/

#ifndef SESSION_MGR_H
#define SESSION_MGR_H

#include <semaphore.h>
#include <sys/socket.h>
#include "authenticate.h"
#include "coreTypes.h"
#include "advisererrno.h"
#include "advisercfg.h"
#include "sessionlogin.h"
#include "session_data.h"

/*------------------------------ Definitions ------------------------*/
#define INVALID_SESSION_ID		-1
#define CD_PID_FILE			"/var/run/cdserver.pid"
#define FD_PID_FILE			"/var/run/fdserver.pid"
#define HD_PID_FILE			"/var/run/hdserver.pid"

#define SET                     1       
#define REMOVE                  2

#define STOP_SESSION		2
#define INVALID_VIDEO_TOKEN	3
#define STOP_SESSION_CONF	5
#define STOP_SESSION_WEB_LOGOUT	7

#define PEND_STATE_NORMAL		0
#define PEND_STATE_WAIT_IN_PROGRESS	1

#define	PENDING_SESSION_TIMEOUT 30 //30 seconds

#define KVM_SERVICE_LIST	5

//added for kvm sharing

typedef struct {
	unsigned char pending_state;
	time_t start_time;
	session_info_t	 *master_session;
	session_info_t	 *slave_session;
	char user_name[TOKENCFG_MAX_USERNAME_LEN];
	char client_ip[TOKENCFG_MAX_CLIENTIP_LEN];
	auth_t auth_pkt;
} PENDING_SESSION;

typedef int hsession_t;

/**
 * @fn adviser_signal_handler 
 * @brief Handles the signals recieved by the process
**/
extern void adviser_signal_handler(int id);


/**
 * @fn init_session_mgr
 * @brief Initialises the session lists.
**/
extern int init_session_mgr (void);

/**
 * @fn sm_reserve_session
 * @brief Reserves a new video redirection session. Returns the handle to the
 * new session created. Returns -1 if a session could not be created. Not thread safe.
**/
extern hsession_t sm_reserve_session (int sock, int con_port);

/**
 * @fn sm_grant_session
 * @brief Grants a new video redirection session. Returns the handle to the
 * new session created. Returns -1 if a session could not be created. Not thread safe.
**/
extern int sm_grant_session (hsession_t hsession);


/**
 * @fn sm_delete_session
 * @brief Deletes an existing session. The worker thread associated with the
 * session is stopped, and the session is freed.
**/
extern int sm_delete_session (int thread_id);


/**
 * @fn sm_wake_up_record_thread
 * @brief Wakes up the auto-video recording thread which is waiting on this semaphore
**/
extern void sm_wake_up_record_thread (void);


/**
 * @sm_set_session_bw
 * @brief Sets the session's bandwidth. Returns sm_success if the bandwidth
 * is set successfully, sm_no_session if no such session exists, sm_error
 * if there is an error setting the session bandwidth.
**/
extern int set_session_bw (int thread_id, u32 bandwidth);


/**
 * @fn sm_get_session_list
 * @brief Returns the list of all the sessions that are currently
 * present.
**/
extern int sm_get_session_list (session_list_t* p_session_list,
										BOOL clear_new_flag);

/**
 * @fn sm_get_free_session
 * @brief Gets a session that is unused
**/
extern session_info_t* get_free_session (hsession_t hsession);


/**
 * @fn sm_clean_sessions
 * @brief Cleans up the session
**/
extern void sm_clean_sessions (void);
int
sm_get_num_valid_sessions (void);

/**
 * @fn sm_get_num_session
 * @brief Gets the number of active sessions
**/
extern int sm_get_num_session (void);

/**
 * @fn sm_get_kvm_num_session
 * @brief Gets the number of active kvm sessions
**/
extern int sm_get_kvm_num_session (void);


/**
 * @fn sm_build_send_list
 * @brief builds the send list
**/
extern void sm_build_send_list (session_list_t* send_list, session_list_t* session_list);


/**
 * @fn sm_wait_for_transfer_complete
 * @brief waits for the transfer to be complete
**/
extern void sm_wait_for_transfer_complete (void);


/**
 * @fn sm_set_abort_flag
 * @brief Sets the abort flag in the session
**/
extern void sm_set_abort_flag (hsession_t hsession);


/**
 * @fn sm_prepare_session_for_transfer
 * @brief Initialises the member variables for a fresh transfer
**/
extern void sm_prepare_session_for_transfer (void);


/**
 * @fn set_session_fps
 * @brief sets the permissible frames per second for the session
**/
extern int set_session_fps (hsession_t hsession, u8 fps);


/**
 * @fn sm_set_refresh_on
 * @brief sets the refresh flag
**/
extern int sm_set_refresh_on (hsession_t hsession);


/**
 * @fn sm_pause_redirection
 * @brief pauses the redirection
**/
extern int sm_pause_redirection (hsession_t hsession);


/**
 * @fn sm_resume_redirection
 * @brief resumes the redirection
**/
extern int sm_resume_redirection (hsession_t hsession);


/**
 * @fn sm_set_stop_signal
 * @brief sets the stop signal flag
**/
extern void sm_set_stop_signal (unsigned short );


/**
 * @fn sm_is_set_stop_signal
 * @brief checks the stop signal flag
**/
extern BOOL sm_is_set_stop_signal (void);


/**
 * @fn sm_wait_for_stop_session_complete
 * @brief waits for the stop session packet send complete
**/
extern void sm_wait_for_stop_session_complete (void);

/**
 * @fn is_any_session_encrypted
 * @brief check this methos any session is encrypted
**/
extern BOOL is_any_session_encrypted ();

/**
 * @fn sm_set_encrypt_flag
 * @brief sets the encrypt flag to send the encrypt data to the client
**/
extern void sm_set_encrypt_flag (hsession_t hsession);

/**
 * @fn sm_reset_encrypt_flag
 * @brief reset the encypt status sent flag
**/
extern void sm_reset_encrypt_flag (hsession_t hsession);


/**
 * @fn sm_set_send_encrypt_status
 * @brief sets the is_send_encrypt_status
**/
extern void sm_set_send_encrypt_status (hsession_t hsession);

/**
 * @fn sm_reset_send_encrypt_key
 * @brief resets the send encrypt flag
**/
extern void sm_reset_send_encrypt_status (void);

/**
 * @fn sm_is_set_send_encrypt_key
 * @brief checks the send encrypt status flag
**/
extern BOOL sm_is_set_send_encrypt_status (void);

/**
 * @fn sm_set_send_auto_bw_detect
 * @brief sets the is_auto_bw_detect flag and the size to be sent
**/
void
sm_set_send_auto_bw_detect (hsession_t hsession, u32 auto_bw_size);

/**
 * @fn sm_is_set_auto_bw_detect
 * @brief checks the auto bandwidth detect flag
**/
extern BOOL sm_is_set_auto_bw_detect (void);

/**
 * @fn sm_build_auto_bw_detect_send_list
 * @brief builds the session list to send auto bw detect data
**/
extern void sm_build_auto_bw_detect_send_list (session_list_t* send_list, session_list_t* session_list);

/**
 * @fn sm_reset_auto_bw_detect
 * @brief resets the auto bandwidth detect flag
**/
extern void sm_reset_auto_bw_detect (session_list_t* send_list);

/**
 * @fn sm_set_send_mouse_mode
 * @brief sets the is_send_mouse_mode flag and the size to be sent
**/
void
sm_set_send_mouse_mode (hsession_t hsession);


/**
 * @fn sm_set_sessiontoken
 * @brief sets the session token field
**/
extern void sm_set_sessiontoken (hsession_t hsession, char* stoken);

/**
 * @fn sm_get_valid_session_list
 * @brief Returns the list of all the sessions that are currently
 * present.
**/
extern int sm_get_valid_session_list (session_list_t* p_session_list,
										BOOL clear_new_flag);
/**
 * @fn sm_send_session_approval
 * @brief Sends command to the client if the session is approved or not.
**/
extern int send_session_approval (int socket, BOOL approval);

extern int WriteSessionToken(session_info_t* p_si, int Status);

/**
 * @fn sm_close_associates
 * @brief Closes the active Media connections which are associated with a particular session
**/
extern void sm_close_associates(void);

/**
 * @fn sm_kvm_priv_take_action
 * @brief take action on privilege
**/

void sm_kvm_priv_take_action(session_info_t* p_si, unsigned char status);

/**
 * @fn sm_get_first_session
 * @brief return the first session
**/
session_info_t *	sm_get_first_session();

/**
 * @fn sm_add_pending_session
 * @brief adds the pending session.
**/
void
sm_add_pending_session(session_info_t *master, session_info_t *slave,unsigned char * user_name,unsigned char * client_ip,auth_t *auth_pkt);



/**
 * @fn sm_kvm_priv_take_action
 * @brief take action on privilege
**/

void sm_kvm_priv_take_action(session_info_t* p_si, unsigned char status);


/**
 * @fn sm_check_pending_sessions
 * @brief check pending sessions
**/


void sm_check_pending_sessions();


/**
 * @fn sm_set_kvm_priv_flag
 * @brief check kvm privilege flag
**/
void sm_set_kvm_priv_flag (session_info_t *first_session, unsigned short status);

/**
 * @fn sm_is_kvm_priv_flag_set
 * @brief sets teh privilege flag
**/

session_info_t *
sm_get_kvm_priv_flag_session();

/**
 * @fn sm_reset_kvm_priv_flag
 * @brief reset the privilege flag
**/

void
sm_reset_kvm_priv_flag (session_info_t *p_session);

/**
 * @fn sm_get_first_session
 * @brief get teh first session
**/

session_info_t * sm_get_first_session();

/**
 * @fn sm_get_num_valid_sessions
 * @brief return the no of valid session
**/
int sm_get_num_valid_sessions(void);


/**
 * @fn sm_get_kvm_priv_flag_session
 * @brief get the KVM privelage flag is set
**/
session_info_t *
sm_get_kvm_priv_flag_session();



/**
 * @fn sm_is_socket_close_ctrl_packet
 * @brief return the session info the sokcet close packet is set
**/
session_info_t*  sm_is_socket_close_ctrl_packet();



/**
 * @fn sm_reset_socket_cntrl_pkt
 * @brief  reset the socket close member packet after send the packet to client
**/
void sm_reset_socket_cntrl_pkt (session_info_t *p_session);

/**
 * @fn sm_set_socket_close_ctrl_packet
 * @brief set the socket close member packet to send the packet to client
**/
void 
sm_set_socket_close_ctrl_packet(session_info_t *session,unsigned short status);

/**
 * @fn sm_check_pending_sessions
 * @brief check any pending session is available
**/
void 
sm_check_pending_sessions();
/**
 * @fn sm_is_power_status_send
 * @brief check any  session is available for sending the Host status
**/
session_info_t *
sm_is_power_status_send ();

void
sm_set_power_status_send (hsession_t hsession);

void
sm_reset_power_status_send (hsession_t hsession);

session_info_t *
sm_is_power_cmd_status_send( );

void
sm_set_power_crl_resp_send (hsession_t hsession);

void
sm_reset_power_crl_resp_send (hsession_t hsession);

void
sm_set_power_status_send (hsession_t hsession);

void
sm_set_power_status (int host_status);

void
sm_set_power_crl_resp (hsession_t hsession,int response);

int
sm_get_services_info (session_list_t* p_session_list, session_list_t *p_send_list, u8 *sessioninfo);

void
sm_set_mouse_media_status ();

void
sm_reset_mouse_media_status ();

BOOL 
sm_is_mouse_media_status( );
void
sm_reset_active_client_send ();
BOOL
sm_is_set_active_client_send ();

void
sm_set_active_client_send ();

void
sm_build_active_client_info (char *ipuserdata);


#ifdef CONFIG_SPX_FEATURE_GLOBAL_WEB_CONSOLE_PREVIEW
void sm_set_webpreviewer_status(BOOL status);
BOOL sm_get_webpreviewer_status();
void set_webpreivewer_capture_time();
BOOL is_valid_web_capture();
#endif

#endif	/* SESSION_MGR_H */


