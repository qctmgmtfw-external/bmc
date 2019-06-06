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

#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dlfcn.h>
#include <sys/time.h>

#include "dbgout.h"
#include "coreTypes.h"
#include "token.h"
#include "videopkt.h"
#include "video_interface.h"
#include "video_misc.h"
#include "sessionmgr.h"
#include "reader.h"
#include "conmgr.h"
#include "utils.h"
#include "sync.h"

#include "hidtypes.h"
#include "iusb.h"
#include "hidconf.h"
#include "hid_usb.h"
#include "usb_ioctl.h"
#include "hid_errno.h"
#include "hid_cmdhandlr.h"
#include<sys/prctl.h>
#include "featuredef.h"
#include "adviser_cfg.h"
#include "racsessioninfo.h"
#include "avse.h"
#include "unix.h"

//###########################################################//

extern int is_LED_from_Client;
extern u8 gBlankScreen;
extern int     rPipeFD;
extern int     CmdPipeFD;
extern CoreFeatures_T g_corefeatures;
extern u8		*service_info_data;

unsigned int gChangeLED = 0;
usb_keybd_led_t pkt_keybd_led;

unsigned int g_SendCursImmediate = 0;

//To inform continuous recording about crash/reset event 
//and to set pre crash/reset video filename accordingly  
unsigned char g_EventOccurred = 0;
unsigned char g_EventSet = 0;
/* threads */
pthread_t       keybd_th;
pthread_t*	soc_th_threads;
soc_threads_info_t		m_soc_th;
BOOL		g_media_license_status = TRUE;
extern int g_fps_diff;

#define UPDATE_ADVISER                  0xE2

extern int do_usb_ioctl(int cmd, u8 *data);
extern void form_status_ctrl_pkt_hdr (ivtp_hdr_t* p_pkt, int type, int size, u16 status);
extern void form_control_pkt_hdr (ivtp_hdr_t* p_pkt, int type, int size);
extern int send_control_pkt (ivtp_hdr_t* p_pkt, session_list_t* p_session_list, u8* data, int size, int pkt_type);

void keybd_main (void);
int ResetKbdLEDStatus();

int soc_create_threads (void)
{
	int i = 0;
	if (0 != pthread_create (&keybd_th, NULL, (void*)keybd_main, NULL))
	{
		TEMERG("Unable to create the Keyboard LED Thread.\n");
		return -1;
        }

	get_soc_threads_info (&m_soc_th);
	soc_th_threads = (pthread_t*) malloc (m_soc_th.num_threads * sizeof(pthread_t));
	if(soc_th_threads==NULL)
	{
		TCRIT("Memory not allocated to soc_th_threads\n");
		return -1;
	}
	for (i=0; i<m_soc_th.num_threads; i++)
		if (0 != pthread_create (&soc_th_threads[i], NULL, (void*)m_soc_th.thread_fns[i], NULL))
		{
			TEMERG("Unable to create the Video HW Thread %d.\n", i);
			return -1;
		}
	return 0;
}


int soc_wait_for_threads_to_exit (void)
{
	int i =0;
	if(0 != pthread_join (keybd_th, NULL))
        {
                TEMERG ("Error in pthread_join for KERYBD LED Thread.\n");
                return -1;
        }

	for (i=0; i<m_soc_th.num_threads; i++)
		if (0 != pthread_join (soc_th_threads[i], NULL))
		{
			TEMERG("Error in pthread_join for Video HW thread %d.\n", i);
			return -1;
		}
	return 0;
}


void setflags_before_videocapture( session_list_t* sess_list)
{
	if (sess_list->is_new_session ||  g_send_full_screen )
	{
		g_SendCursImmediate = 1;
		g_send_full_screen = 0;
	}
}


struct timeval   startskip ={0, 0};
extern int  capturescreen ;

void setflags_after_videocapture(int* prev_cap_status, int cap_status)
{
	static unsigned int bCounter = 0;
	struct timeval  currtime = {0, 0};

	capturescreen = 1;

	// If the screen status has changed, then update the flag accordingly.
	// Also reset the counter
	if (*prev_cap_status != cap_status)
	{
		*prev_cap_status = cap_status;
		bCounter = 0;
		startskip.tv_usec = 0;
	}
	// Update the value of prev_cap_status before returning.
	// When the host is in power off mode, adviser keeps on sending blank screen.
	// But we need to send only one blank screen and send video screen only when there is a change in capture state
	//for solved the mouse of Remote Java View is not smooth When local sytem boot to Linux.
	//if (USING_AST_HW) {
	//	return;
	//}

	// If the screen capture status is NO_SIGNAL || NO_CHANGE, then maintain a counter
	// If same status exists too long, then sleep for a while and re-iterate
	if ((cap_status == CAPTURE_VIDEO_NO_SIGNAL) || (cap_status == CAPTURE_VIDEO_NO_CHANGE))
	{
		bCounter++;

		if (bCounter >= 10)
		{
			capturescreen = 0;		
			if (startskip.tv_usec == 0)
			{
				gettimeofday(&startskip, NULL);
			}
			else
			{
				gettimeofday(&currtime, NULL);
				if(abs(currtime.tv_usec - startskip.tv_usec) >= 10000) // delay 0.01 sec(10000 micro.s)
				{ 
					capturescreen = 1;
					usleep(100000);// sleep 100000 m.s to make cpu happy
					startskip.tv_usec = 0;
				}
			}
		}
	}
	
	//if h5viewer is active give minor delay in capturing and sending video
	if((g_fps_diff > 1) && (g_is_h5viewer_active == 1))
	{
		if (cap_status == CAPTURE_VIDEO_SUCCESS)
		{
			if (startskip.tv_usec == 0)
			{
				gettimeofday(&startskip, NULL);
			}
			else
			{
				gettimeofday(&currtime, NULL);
				if(abs(currtime.tv_usec - startskip.tv_usec) >= 100000) // delay 0.1 sec(10000 micro.s)
				{ 
					select_sleep(0, 500000);// sleep 500000 m.s to make cpu happy
					// NOTE: shouldn't update startskip value as currtime here.
					// if it is updated as such, then there will be sleep for each capture
					// if there is a sleep for each capture then the video update will not be proper.
					// so we have added sleep for alternate video captures
					// startskip = currtime;
					startskip.tv_usec = 0;
				}
			}
		}
	}
		
}


void check_if_logout()
{
    int            readsize = 0;
    int            Length = ((2 * HASH_SIZE) + 3);//Changing the session cookie len  to 35 bytes as we writting only 35 bytes in web side 
    session_list_t session_list;
    char           stoken[Length];
    ivtp_hdr_t     v_hdr;

	if (!rPipeFD) return;	
	
	memset(&stoken[0], 0, Length);

	readsize = read(rPipeFD, &stoken[0], Length);
	if(readsize > 0)
	{
		sm_get_session_list (&session_list, FALSE);
		if(g_corefeatures.ssi_support == ENABLED)
		{
			session_list.is_new_session = 0;
			session_list.num_session = 1;
			form_status_ctrl_pkt_hdr(&v_hdr, IVTP_STOP_SESSION_IMMEDIATE, 0, STATUS_SESS_WEB_LOGOUT);

			/* Send stop session pkt */
			send_control_pkt(&v_hdr, &session_list, NULL, 0, 0);
		}
		else
		{
			int logout_num = 0, i = 0;
			for (i = 0; i < session_list.num_session; i++)
			{
				if((memcmp(session_list.p_session [i]->websession_token, &stoken[0], Length)) == 0 )
				{
				//	session_list.num_session = 1;
					session_list.p_session[logout_num] = (session_info_t *)session_list.p_session [i];
					logout_num++;
				}
			}
			if(logout_num)
			{
				session_list.is_new_session = 0;
				session_list.num_session = logout_num;
				form_status_ctrl_pkt_hdr (&v_hdr, IVTP_STOP_SESSION_IMMEDIATE, 0, STATUS_SESS_WEB_LOGOUT);
				/* Send stop session pkt */
				send_control_pkt (&v_hdr, &session_list, NULL, 0, 0);
			}
		}
	}
	return;
}

// reads pipe to check incoming command
int check_incoming_command()
{
    int   readsize = 0;
    int   Length ;
    int update=0;
    cmd_info_t command;
    char *data;
    //To load videorecording library for continuous recording
    void *videorecordlib_handle = NULL;
    void    *soc_handle = NULL;
    int    (*start_video_to_jpeg) (int);
    int (*start_pre_event_record_th) ();
    session_disconect_record_t *disconnect_data;
    BOOL isValid = FALSE;
    session_list_t send_list;
    session_list_t session_list;
    Length = sizeof(cmd_info_t);

    if (!CmdPipeFD) 
        return 0;
    
    memset(&command, 0, Length);

    readsize = read(CmdPipeFD, &command, Length);
    if(readsize > 0)
    {
        switch(command.cmd)
        {
            case AUTO_VIDEO_RECORD_CMD:// previous handled in sigusr2
                if( g_corefeatures.record_pre_boot_or_crash_video == ENABLED )
                {
                    g_sync_pre_post_rec = TRUE;
                }
                sm_wake_up_record_thread ();
                break;
            //to start/stop continuous recording for pre-crash/pre-reset video based on event set/reset
            case START_CONT_RCD:
                //If feature selected and event set to monitor will start continuous recording thread
                if( ENABLED == g_corefeatures.record_pre_boot_or_crash_video )
                {
                    if( ENABLED == g_EventSet )
                    {
                        //If g_EventSet is already enabled,means thread running 
                        TDBG("Cont. recording thread running already\n");
                        break;
                    }
                    g_EventSet = ENABLED;
                    videorecordlib_handle = dlopen(VIDEORECORD_LIBRARY, RTLD_LAZY);

                    // This will call video record library to start continuous recording
                    if(!videorecordlib_handle)
                    {
                        TCRIT("%s\n",dlerror());
                    }
                    else
                    {
                    
                        start_pre_event_record_th = dlsym(videorecordlib_handle, START_PRE_EVENT_RECORDING_THREAD);
                        if (start_pre_event_record_th)
                        {
                            if (start_pre_event_record_th () < 0)
                            {
                                TEMERG("Unable to create the Pre event Video Record Thread\n");
                                dlclose(videorecordlib_handle);
                                break;
                            }
                        }
                        dlclose(videorecordlib_handle);
                    }
                }
                break;
            case STOP_CONT_RCD:
                //to stop continuous recording run time,as per user request
                g_EventSet = 0;
                break;
            case  BSOD_SIGNAL:
                //Inform Continuous recording about BSOD crash occurred
                if( ( g_corefeatures.record_pre_boot_or_crash_video == ENABLED ) && ( g_EventSet ))
                {
                    g_EventOccurred = BSOD_SIGNAL;
                }
                break;
            case  LPCRESET_SIGNAL:
                //Inform Continuous recording about Host Reset
                if( ( g_corefeatures.record_pre_boot_or_crash_video == ENABLED ) && ( g_EventSet ))
                    g_EventOccurred = LPCRESET_SIGNAL;
                break;
            case  BSOD_CAPTURE:
                soc_handle = dlopen(JPEGCONVERT_LIBRARY, RTLD_LAZY);
                // This will call to capture the BSOD screen
                if (!soc_handle)
                {
                    TCRIT("%s\n",dlerror());
                }
                else
                {
                    start_video_to_jpeg = dlsym(soc_handle, START_VIDEO_TO_JPEG);
                    if (start_video_to_jpeg)
                    {
                        //BSOD capture going to happen set fullscreen flag.
                        g_send_full_screen = 1;
                        if (start_video_to_jpeg (command.cmd ) < 0)
                        {
                            TDBG("Unable to capture bsod screen\n");
                        }
                    }
                    dlclose(soc_handle);
                }
                break;
            case IVTP_POWER_STATUS :
                //if power status received is power OFF, then, reset the LED status.
                if(command.status == 0)
                    ResetKbdLEDStatus();
                sm_set_power_status(command.status);
                sm_set_power_status_send_all();
                break;
            case IVTP_VMEDIA_USED_UPDATE:
                if(g_corefeatures.session_management && g_corefeatures.vmedia_dynamic_free_instance_update == ENABLED)
                {
                    if(sm_get_kvm_num_session() > 0)
                        sm_set_vmedia_update_status();
                }
                break;
            case IVTP_KVM_DISCONNECT:
                if(command.datalen > 0)
                {
                    data= malloc(command.datalen);
                    if(data != NULL)
                    {
                        readsize=read(CmdPipeFD, data, command.datalen);
                        if(readsize==-1){
                            TCRIT("error in reading");
                        }
                        disconnect_data = (session_disconect_record_t *)data;
                        Ondisconnect_session(disconnect_data->racsession_id,disconnect_data->session_type);
                        
                        if(data != NULL)
                            free(data);
                    }
                }
                break;
            case IVTP_STOP_SESSION_IMMEDIATE:
                sm_set_stop_signal(command.status);
                break;
            case CONF_UPDATED: // previous handled in sigusr1
                handleconfupdate();
                break;
            case SERV_CONF_UPDATED:// previous handled in sigalrm
                if(g_corefeatures.runtime_license_support == ENABLED)
                {
                    isValid = getLicenseStatus(MEDIA);
                    if (g_media_license_status !=isValid )
                    {
                        g_media_license_status = isValid;
                        sm_set_media_license_send();
                    }
                }
                if (sm_get_services_info(&session_list, &send_list, service_info_data) != 0)
                {
                    TINFO ("Unable to fetch the configuration and send to all the clients\n");
                }
                break;
            case UPDATE_ADVISER:
                sm_set_stop_signal (STOP_SESSION);
                update=1;
                return update;
            default :
                TDBG("Un supported command number :%d\n",command.cmd);
        }   
    }
    return 0;
}

int send_non_video_frame_data( session_list_t* send_list, void* p_frame_hdr )
{
	int retval = 0;
	
	if (send_list == NULL || p_frame_hdr == NULL)
	{
		return -1;
	}

	if (gChangeLED)
	{
		form_control_pkt_hdr (&pkt_keybd_led.hdr, IVTP_GET_KEYBD_LED, 1);
		retval = send_control_pkt(&(pkt_keybd_led.hdr), send_list, &(pkt_keybd_led.keybd_led), 1, KEYBD_INFO_CODE);
		gChangeLED = 0;
	}

	send_soc_specific_data(send_list, p_frame_hdr);

	return retval;
}

/**
 * Resets the LED status to 0.
 */
int ResetKbdLEDStatus()
{
	int ioctl_request_code;

	char pBuffer[sizeof(IUSB_HID_PACKET)];
	IUSB_HID_PACKET* pHIDPacket = (IUSB_HID_PACKET*)&pBuffer[0];

	memset(pBuffer, 0, sizeof(pBuffer) );

	pHIDPacket->DataLen   = 1;
	pHIDPacket->Data   = 0;

	ioctl_request_code = USB_KEYBD_LED_RESET;
	if (0 != do_usb_ioctl(ioctl_request_code, (u8 *)pHIDPacket) ) 
	{
		TDBG("Error Resetting LED status in the driver\n");
		return -1;
	}

	gChangeLED = 1;
	return 0;
}
int GetIUSBLEDPkt(u8 *LEDstatus, int no_wait)
{
        int ioctl_request_code;

        char pBuffer[sizeof(IUSB_HID_PACKET)];
        IUSB_HID_PACKET* pHIDPacket = (IUSB_HID_PACKET*)&pBuffer[0];

        memset(pBuffer, 0, sizeof(pBuffer) );

        pHIDPacket->DataLen   = 1;
        pHIDPacket->Data   = 0;

        ioctl_request_code = no_wait ? USB_KEYBD_LED_NO_WAIT : USB_KEYBD_LED;
        if (0 != do_usb_ioctl(ioctl_request_code, (u8 *)pHIDPacket) ) 
	{
                TDBG("Error retrieving LED status from the driver\n");
                return -1;
        }

        *LEDstatus = pHIDPacket->Data;
		gChangeLED = 1;

        return 0;
}

void keybd_main (void)
{
	prctl(PR_SET_NAME,__FUNCTION__,0,0,0);
        while (1)
        {
                /* Block indefinitely until LED event occurs */
        	if(GetIUSBLEDPkt(&(pkt_keybd_led.keybd_led), 0) == -1)/* get the keyboard LED status and return immediately */
        		pkt_keybd_led.keybd_led = 0;

                if (is_LED_from_Client)
                {
                        is_LED_from_Client = 0;
                        continue;
                }
		
		gChangeLED = 1;
                pkt_keybd_led.hdr.type = IVTP_GET_KEYBD_LED;
                pkt_keybd_led.hdr.pkt_size = 1;
                pkt_keybd_led.hdr.status = 0;
        }
}

/**
 * Handle sending the configurations like license support status, host display lock status, and mouse mode info.
 */
void handleconfupdate()
{
    if(g_corefeatures.runtime_license_support == ENABLED)
    {
        if (TRUE !=getLicenseStatus("KVM"))
        {
            sm_set_stop_signal (STOP_SESSION_LICENSE_EXP );
        }
    }
    if(g_corefeatures.host_auto_lock == ENABLED)
    {
        sm_set_host_lock_info();
    }

    sm_set_mouse_media_status ();
}

