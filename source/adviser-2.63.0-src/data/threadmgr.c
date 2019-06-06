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
 * threadmgr.c
 * Thread Manager
**/
#include <pthread.h>
#include "sync.h"
#include "adviserdbg.h"
#include "advisercfg.h"
#include "sessionmgr.h"
#include "cmworker.h"
#include "avse.h"
#include "conmgr.h"
#include "video_misc.h"
#include "dlfcn.h"
#include "video_interface.h"
#include "featuredef.h"
#include "cmdhndlr.h"
//Macro to send Get trigger Event Cmd to IPMI
#define IPMI_OEM_NETFN              0x32
#define GET_TRIGGEREVENT_CMD        0x95
#define GET_TRIGGEREVENT_PARAMETER  0x0b //To get pre crash/pre reset event status
#define PRIV_LEVEL_ADMIN            0x04
/* Thread Variables */
pthread_t				*worker_list_th = NULL;
pthread_t				avse_th;
pthread_t				cmdhndlr_th;
void 					*videorecord_handle = NULL;
extern int				max_session;
void 					*jpegconversion_th = NULL;
extern CoreFeatures_T g_corefeatures;

/**
 * wait_for_threads_to_exit
**/
int
wait_for_threads_to_exit (void)
{
    int i;
    void        *soc_handle = NULL;
    void        (*wait_on_video_record_th) (void);
    void        (*wait_on_jpeg_conv_th) (void);

    TINFO ("KILLING THREADS\n");
    soc_handle = dlopen(VIDEORECORD_LIBRARY, RTLD_LAZY);

    // This will call definition to update the soc session hdr
    if (!soc_handle)
    {
    	TCRIT("%s\n",dlerror());
    }
    else
    {
        wait_on_video_record_th = dlsym(soc_handle, WAIT_ON_VIDEO_RECORD_THREAD);   
        if (!wait_on_video_record_th)
        {
        	TCRIT("%s\n",dlerror());	
        }
            wait_on_video_record_th ();

        dlclose(soc_handle);
    }
    if (videorecord_handle)
        dlclose(videorecord_handle);

    if(g_corefeatures.capture_bsod_jpeg == ENABLED)
    {
        soc_handle = dlopen(JPEGCONVERT_LIBRARY, RTLD_LAZY);

        // This will call to exit jpeg conversion thread.
        if (soc_handle)
        {
            wait_on_jpeg_conv_th = dlsym(soc_handle, WAIT_ON_JPEG_CONV_THREAD);
            if (wait_on_jpeg_conv_th)
            {
                wait_on_jpeg_conv_th ();
            }
            dlclose(soc_handle);
        }

        if (jpegconversion_th)
            dlclose(jpegconversion_th);
    }
    
    if(0 != pthread_join (cmdhndlr_th, NULL))
    {
    perror ("pthread_join cmdhndlr_thread");
    return -1;
    }
    
    /* Wait for the threads to exit  */
    if(0 != soc_wait_for_threads_to_exit ())
    {
        perror ("soc_wait_for_threads_to_exit soc-specific threads");
        return -1;
    }

    if(0 != pthread_join (avse_th, NULL))
    {
        perror ("pthread_join video server engine");
        return -1;
    }

    for (i = 0; i < max_session; i++)
    {
        if (0 != pthread_join (worker_list_th[i], NULL))
        {
            perror ("pthread_join worker list");
            return -1;
        }
    }

    return 0;
}

/**
 * create_threads
 **/
int
create_threads (void)
{
	int i;
	session_info_t* new_session;
	int 			(*start_video_record_th) (void);
    int             (*start_pre_event_recording_th) (void);
	int 			(*start_jpeg_conversion_th) (void);
    INT8U ReqData;
    char ResData[2]; //For storing 2byte response of get trigger event cmd status
    uint32 ResLen;
	/**
	 * create_worker_threads
	**/
	for (i = 0; i < max_session; i++)
	{
		new_session = get_free_session (i);
		if (new_session == NULL)
		{
			TEMERG ("Unable to get free session [%d].\n", i);
			return -1;
		}

		if (0 != pthread_create (&worker_list_th [i], NULL, (void *) cm_worker_thread,
									(void *)(new_session)))
		{
			TEMERG ("Unable to Create Worker Thread [%d].\n", i);
			TDBG ("Thread id = %d\n", worker_list_th [i]);
			return -1;
		}

		TINFO_1 ("Created Worker Thread:%ld\n", worker_list_th [i]);
	}

	/* Create the Adaptive Video Server engine thread */
	if (0 != pthread_create (&avse_th, NULL, (void*)avse_main, NULL))
	{
		TEMERG("Unable to create Adaptive Server engine.\n");
		return -1;
	}

    	/* Create the Cmdhndlr manager thread to handle commands from Client */
    	if (0 != pthread_create (&cmdhndlr_th, NULL, (void*)cmdhndlr_main, NULL))
	{
		TEMERG("Unable to create the CmdHndlr main Thread.\n");
		return -1;
	}

	if(g_corefeatures.capture_bsod_jpeg == ENABLED)
	{

		jpegconversion_th = dlopen(JPEGCONVERT_LIBRARY, RTLD_LAZY);

		// This will call start jpeg conversion thread.
		if (!jpegconversion_th)
		{
			TCRIT("%s\n",dlerror());
		}
		else
		{
		start_jpeg_conversion_th= dlsym(jpegconversion_th, START_JPEG_CONV_THREAD);
			if (start_jpeg_conversion_th)
			{
				if (start_jpeg_conversion_th () < 0)
				{
					TEMERG("Unable to create the BSOD Record Thread\n");
					dlclose(jpegconversion_th);
					return -1;
				}
			}
		}
	}

	videorecord_handle = dlopen(VIDEORECORD_LIBRARY, RTLD_LAZY);

	// This will call definition to update the soc session hdr
    if (!(videorecord_handle))
    {
    	TCRIT("%s\n",dlerror());
    }
   
    else
    {
        if( g_corefeatures.auto_video_recording == ENABLED )
        {   start_video_record_th = dlsym(videorecord_handle, START_VIDEO_RECORD_THREAD);
            
            if (start_video_record_th)
            {
                if (start_video_record_th () < 0)
                {
                    TEMERG("Unable to create the Video Record Thread\n");
                    dlclose(videorecord_handle);
                    return -1;
                }
            }
           }
        //Start Pre-event recording if Pre-Crash/Pre-Reset Event is already set
        if( g_corefeatures.record_pre_boot_or_crash_video == ENABLED )
        {
            ReqData = GET_TRIGGEREVENT_PARAMETER;
            //Check pre-crash/pre-reset event status
            if (0 == SendRawIPMICmd( IPMI_OEM_NETFN, GET_TRIGGEREVENT_CMD, &ReqData, 1, (INT8U *)&ResData, &ResLen, PRIV_LEVEL_ADMIN ))
            {
                //If bit 1 or 2 is set in response data, means event set for pre-event recording
                if( ( ResLen > 0 ) && ( ( 0 != (ResData[1] & 0x01) ) || ( 0 != (ResData[1] & 0x02) ) ) )
                {
                    start_pre_event_recording_th = dlsym(videorecord_handle, START_PRE_EVENT_RECORDING_THREAD);

                    if ( start_pre_event_recording_th )
                    {
                        TCRIT("\nLoading PreEvent Video recording");
                        if ( start_pre_event_recording_th() < 0 )
                        {
                            TCRIT("Unable to create the Video Record Thread\n");
                            dlclose(videorecord_handle);
                            return -1;
                        }
                        //Set the global variable
                        g_EventSet = 1;
                    }
                }
            }
            else
            {
                TEMERG("Unable to check Event Status to start Pre-event video recording");
            }
        }
    }

	TINFO_1 ("Created Thread - Avse:%ld\n", avse_th);

	TINFO_1 ("Thread ids = %ld %ld %ld %ld %ld %ld\n", worker_list_th [0], worker_list_th [1],
							   worker_list_th [2], worker_list_th [3],
							   worker_list_th [4], avse_th);

	/* create threads specific to the SOC */
	if (0 != soc_create_threads ())
	{
		TEMERG("Unable to create SOC specific threads.\n");
		return -1;
	}

	return 0; /* success */
}


