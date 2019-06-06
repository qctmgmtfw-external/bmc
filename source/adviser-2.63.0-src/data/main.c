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
* @file   	Main.c
* @author 	Varadachari Sudan Ayanam <varadacharia@ami.com>
* @brief  	Adviser Video Server Main file
****************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/ioctl.h>
#include "cmworker.h"
#include "avse.h"
#include "adviserdbg.h"
#include "sync.h"
#include "sessionmgr.h"
#include "conmgr.h"
#include "reader.h"
#include "threadmgr.h"
#include "dlfcn.h"

#include "vmedia_cfg.h"
#include "usb_ioctl.h"
#include "qlzw.h"
#include "hid_cmdhandlr.h"
#include "video_interface.h"
#include "video_misc.h"
#include "unix.h"
#include "token.h"
#include "procreg.h"
#include "ncml.h"
#include "featuredef.h"
#include "adviser_cfg.h"
#include "drvload.h"

char *ModuleList[]={
                  "helper",
                  "usbe",
                  "iUSB",
                  "usb0_hw",
                  "usb1_hw",
                  "hid",
                  "hid2",
                  "ps2hw",
                  "luncombo",
                  "videocap",
};

#define MOUSE_PORT           (5121)
#define KVM_TIMER_TICKLE 30	//30 seconds

u8			gDaemonize = 1;

/* Communication Pipes between Video Server and Web Server*/
int			PipeFD;
int      		rPipeFD;
int      		CmdPipeFD;
int			max_session = 0;
SERVICE_CONF_STRUCT	AdviserConf;

/* externs */
extern int save_pid(char *process_name);
extern pthread_mutex_t	*session_guard;
extern sem_t		*worker_wait_event;
extern session_info_t	*m_session;
extern pthread_mutex_t	*m_session_guard;
extern sem_t		*m_ev_wakeup;
extern pthread_mutex_t	*m_session_auto_bw;
extern pthread_t	*worker_list_th;
extern u8		*service_info_data;
extern PENDING_SESSION *m_pending_session;
extern  pthread_mutex_t	*m_pending_session_guard;
extern int do_usb_ioctl(int cmd, u8 *data);
CoreFeatures_T g_corefeatures;
CoreMacros_T g_coremacros;
u8			g_isSinglePortEnabled = 0;
extern int g_TimerTickSeconds ; //the number of seconds interval for timers
extern void start_own_timer_thread();
unsigned int g_kvm_retry_count;
unsigned int g_kvm_retry_interval;
/* prototypes */
int daemon_init (void);
void adviser_unload_resource(void);
void unmount_videoremote_path(void);
volatile sig_atomic_t g_sigup = 0, g_sigusr1 = 0, g_sigusr2 = 0, g_sigalrm = 0;

void DisconnectAdviserSignals()
{
	/*------- Disconnect the Signals for Adviser----------------*/
	disconnect_signal(SIGUSR1,  NULL);
	disconnect_signal(SIGUSR2,  NULL);
	disconnect_signal(SIGALRM,  NULL);
}

void adviser_signal_handler(int signum)
{
    switch (signum)
    {
        // This signal is for, when we are stopping this video server under non-configuration changes
        case SIGHUP:            
            g_sigup++;
            break;
        // This signal is for, when any configuration change occurs in BMC
        case SIGUSR1:
            g_sigusr1++;
            break;
        // This signal is for, when we want to start auto-video recording
        case SIGUSR2:
            g_sigusr2++;
            break;
        // This signal is for, when we receive any service information change and to send it to clients
        case SIGALRM:
            g_sigalrm++;
            break;
        default:
        break;
    }

    return ;
}

void adviser_process_signal()
{
    int status = STOP_SESSION, num_session;
    if((g_sigup == 0) && (g_sigusr1 == 0) && (g_sigusr2 == 0) && (g_sigalrm == 0))
    {
         print_backtrace();
         adviser_unload_resource();
    }
    else if(g_sigup > 0)
    {
        DisconnectAdviserSignals();
        if(g_corefeatures.runtime_license_support == ENABLED)
        {
            if (TRUE !=getLicenseStatus("KVM"))
                status = STOP_SESSION_LICENSE_EXP;
        }
        sm_set_stop_signal (status );
        //unmount video mount path
        if( g_corefeatures.auto_video_recording == ENABLED)
        {
            unmount_videoremote_path();
        }
        ProcMonitorDeRegister("/usr/local/bin/adviserd");
        g_sigup--;
        num_session = sm_get_kvm_num_session();
        if (num_session == 0)
        {
            adviser_unload_resource();
        }
    }
    //vmscript restart will invoke conf update from script so handle here too
    else if(g_sigusr1 > 0)
    {
        handleconfupdate();
        g_sigusr1--;
    }
    else if(g_sigusr2 > 0)
    {
        TINFO(" Adviser received SIGUSR2 . Adviser will not act for SGUSR2\n");
        g_sigusr2--;
    }
    else if(g_sigalrm > 0)
    {
        TINFO(" Adviser received SIGALRM . Adviser will not act for SIGALRM\n");
        g_sigalrm--;
        return ;
    }
}

/**
 * @brief unmount_videoremote_path() - un mounts mounted remote path
 * @return None
**/

void unmount_videoremote_path()
{
	void   *unmountvideopath = NULL;
	int 	(*unmountpath) (void);

	//unmount remote path for autovideorecord
	unmountvideopath = dlopen(VIDEORECORD_LIBRARY, RTLD_LAZY);

	// This will call definition to update the video thread hndlr
	
	if (!unmountvideopath)
	{
		TCRIT("%s\n",dlerror());
	}
	else
	{
		unmountpath = dlsym(unmountvideopath, UNMOUNT_REMOTE_PATH);
		if (unmountpath)
		{
			unmountpath ();
		}
	}
	if (unmountvideopath)
		dlclose(unmountvideopath);
}

/**
 * @brief adviser_unload_resource() - close descriptors and Deregister
 * @return None
**/
void adviser_unload_resource()
{
	close_descriptors();
	TINFO(" Adviser Stopping\n");
	exit(0);
}
/**
 * @brief print_usage () - prints the usage to run Adviserd
 * @return None
**/
void print_usage ()
{
	printf ("Usage: Adviserd -p <port no>\n");
    printf ("Default port <7578> will be used, if port not specified\n");
}


static int open_adviser_pipe (char *pipeName)
{
        int pipeDesp;
        if(unlink(pipeName)==-1)
        	printf("could not unlink the pipe");
        if (-1 == mkfifo(pipeName, 0777))
        {
                printf("Could not create pipe : %s\n",pipeName);
        }

        pipeDesp = open(pipeName, O_RDWR|O_NONBLOCK);
        if (pipeDesp < 0)
        {
                printf("could not open the pipe : %s\n",pipeName);
                if(unlink(pipeName)==-1)
                   printf("could not unlink the pipe");
                pipeDesp = -1;
        }
        return(pipeDesp);
}


void close_adviser_pipe (int fd)
{
        if (fd > 0)     close(fd);
}

int ReleaseResources (void)
{
	if (session_guard)
	{
		free (session_guard);
	}

	if (worker_wait_event)
	{
		free (worker_wait_event);
	}

	if(m_session->sess_ipmi_res)
	{
		free(m_session->sess_ipmi_res);
	}

	if (m_session)
	{
		free (m_session);
	}

	if (m_session_guard)
	{
		free (m_session_guard);
	}

	if (m_ev_wakeup)
	{
		free (m_ev_wakeup);
	}

	if (m_session_auto_bw)
	{
		free (m_session_auto_bw);
	}

	if (worker_list_th)
	{
		free (worker_list_th);
	}

	if (service_info_data)
	{
		free (service_info_data);
	}

	if(m_pending_session)
	{
		free(m_pending_session);
	}

	if(m_pending_session_guard)
	{
		free(m_pending_session_guard);
	}

	return 0;
}

void close_descriptors()
{
	close_adviser_pipe(PipeFD);
	close_adviser_pipe(rPipeFD);
	close_adviser_pipe(CmdPipeFD);
	ReleaseResources();
	release_video_resources();
	release_usb_resources();
}

static int AllocateResources (void)
{
	int retval = 0;
	 unsigned int datalen = 0;

	datalen = sizeof(pthread_mutex_t) * max_session;
	if((datalen< sizeof(pthread_mutex_t)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	session_guard = (pthread_mutex_t *)malloc(datalen);
	if (session_guard == NULL)
	{
		printf("unable to allocate resource for video server\n");
		retval = -1;
		goto error_out;
	}
	memset((char *)session_guard, 0, datalen);

	datalen = sizeof(sem_t) * max_session;
	if((datalen< sizeof(sem_t)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	worker_wait_event = (sem_t *)malloc(datalen);
	if (worker_wait_event == NULL)
	{
		printf("unable to allocate resource for video server\n");
		retval = -1;
		goto error_out;
	}
	memset((char *)worker_wait_event, 0, datalen);

	datalen = sizeof(session_info_t) * max_session;
	if((datalen< sizeof(session_info_t)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	m_session = (session_info_t *)malloc(datalen);
	if (m_session == NULL)
	{
		printf("unable to allocate resource for video server\n");
		retval = -1;
		goto error_out;
	}
	memset((char *)m_session, 0, datalen);

	datalen = sizeof(pthread_mutex_t) * max_session;
	if((datalen< sizeof(pthread_mutex_t)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	m_session_guard = (pthread_mutex_t *)malloc(datalen);
	if (m_session_guard == NULL)
	{
		printf("unable to allocate resource for video server\n");
		retval = -1;
		goto error_out;
	}
	memset((char *)m_session_guard, 0, datalen);

	datalen = sizeof(sem_t) * max_session;
	if((datalen< sizeof(sem_t)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	m_ev_wakeup = (sem_t *)malloc(datalen);
	if (m_ev_wakeup == NULL)
	{
		printf("unable to allocate resource for video server\n");
		retval = -1;
		goto error_out;
	}
	memset((char *)m_ev_wakeup, 0, datalen);

	datalen = sizeof(pthread_mutex_t) * max_session;
	if((datalen< sizeof(pthread_mutex_t)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	m_session_auto_bw = (pthread_mutex_t *)malloc(datalen);
	if (m_session_auto_bw == NULL)
	{
		printf("unable to allocate resource for video server\n");
		retval = -1;
		goto error_out;	
	}
	memset((char *)m_session_auto_bw, 0, datalen);

	datalen = sizeof(pthread_t) * max_session;
	if((datalen < sizeof(pthread_t)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	worker_list_th = (pthread_t *)malloc(datalen);
	if (worker_list_th == NULL)
	{
		printf("unable to allocate resource for video server\n");
		retval = -1;
		goto error_out;	
	}
	memset((char *)worker_list_th, 0, datalen);

	datalen = sizeof(SERVICE_CONF_STRUCT) * KVM_SERVICE_LIST;
	if((datalen < sizeof(SERVICE_CONF_STRUCT)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	service_info_data = (u8 *)malloc(datalen);
	if (service_info_data == NULL)
	{
		printf("Unable to allocate resource for collecting service configuration\n");
		retval = -1;
		goto error_out;
	}
	memset((char *)service_info_data, 0, datalen);

	datalen = sizeof(PENDING_SESSION) * max_session;
	if((datalen < sizeof(PENDING_SESSION)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	m_pending_session = (PENDING_SESSION *)malloc(datalen);
	if (m_pending_session == NULL)
	{
		printf("malloc failed to allocate resource for collecting pending session info.\n");
		retval = -1;
		goto error_out;
	}
	memset((char *)m_pending_session, 0, datalen);

	datalen = sizeof(pthread_mutex_t) * max_session;
	if((datalen< sizeof(pthread_mutex_t)) || ((signed int)datalen < max_session))
	{
		TCRIT("Integer overflow");
		retval = -1;
		goto error_out;
	}
	m_pending_session_guard = (pthread_mutex_t *)malloc(datalen);
	if (m_pending_session_guard == NULL)
	{
		printf("unable to allocate resource for video server\n");
		retval = -1;
		goto error_out;
	}
	memset((char *)m_pending_session_guard, 0, datalen);

error_out:
	return retval;
}

/**
 * @brief ParseCmdLineParameters () - Parses the command line Parameters
 * @return void
**/
void ParseCmdLineParameters (int argc, char* argv[], int* port_num)
{
	char* p 		= 0;
   	int temp_arg	= argc;

	while (--argc)
	{

		p = *++argv;
		if (*p == '-')
		{
			switch(*++p)
			{
				case 'd':
					gDaemonize = 0; //changed to 0 -d means debug
					break;
				case 'h':
				case '?':
					print_usage ();
					exit (1);
				case 'p':
					if (temp_arg < 3)
					{
						print_usage ();
						exit (1);
					}
					*port_num = atoi ((*++argv));
					TDBG ("Portnum:%d\n", *port_num);
					argc--;
					break;
				default:
					print_usage ();
					exit (1);
			}

		}
	}
}

/**
* exit_function
**/
void
exit_function ()
{
	TINFO ("Exiting Main\n");
}


/**
 * @brief main () - Adaptive Video Server Daemon Entry Point
**/
int main(int argc, char* argv[])
{
	int port_num = ADVISER_PORT;
	u8 device_status=0;
	VMediaCfg_T vmediacfg;
	AdviserCfg_T  pAdviserCfg;
	
	if(IsServiceEnabled(KVM_SERVICE_NAME) == 0)
	{
		exit(1);
	}

	if(IsProcRunning("adviserd") > 1)
	{
		exit(1);
	}

	//on_exit (exit_function, NULL);
	ParseCmdLineParameters (argc, argv, &port_num);
	
	/* Daemonize the program */
	if(gDaemonize)
	{
		if(daemon_init() != 0)
		{
			TWARN("[Main Thread] : Unable To daemonize the server.\n");
		}
	}

	/* save my pid */
	save_pid ("Adviserd");

	/*------- Connect the Signals for Adviser----------------*/
	connect_signal(SIGUSR1, adviser_signal_handler, NULL);
	connect_signal(SIGUSR2, adviser_signal_handler, NULL);
	connect_signal(SIGALRM, adviser_signal_handler, NULL);
	connect_signal(SIGSEGV, adviser_signal_handler, NULL);
	connect_signal(SIGFPE, adviser_signal_handler, NULL);

	/* Retrieves the Enabled features */
	RetrieveCoreFeatures(&g_corefeatures);
	RetrieveCoreMacros(&g_coremacros,&g_corefeatures);
	
	PlatformDriverInit(&ModuleList[0], sizeof(ModuleList)/sizeof(ModuleList[0]));

	updateSinglePort();
	fetchServiceConfigurations();
	
	/* Allocates all the dynamic structures based on Max Session */
	AllocateResources ();

	/* Initializations for video device */
	if (init_video_resources() < 0)
	{
		TCRIT("Unable to Open Video Capture Driver. Terminate Client\n");
		return -1;
	}

	if (init_usb_resources() < 0)
	{
		TCRIT("Unable to Open USB Devices. Terminate Client\n");
		release_video_resources();
		return -1;
	}
	/* Initialize the session manager */
	init_session_mgr ();
	save_usb_resource();
	if(g_corefeatures.power_consumption_virtual_device_usb == ENABLED)
	{
		memset(&vmediacfg,0,sizeof(VMediaCfg_T));
		do_usb_ioctl(USB_GET_ALL_DEVICE_STATUS,&device_status);
		GetVMediaCfg(& vmediacfg,g_corefeatures.lmedia_support,g_corefeatures.rmedia_support,g_corefeatures.power_consumption_virtual_device_usb);
		if(vmediacfg.power_consumption_enable != ENABLED)
		{
			if(device_status == 0)
			{
				do_usb_ioctl(USB_ENABLE_ALL_DEVICE,NULL);
			}			   
		}
	}
	PipeFD = open_adviser_pipe(WEBTOKEN_COMM_PIPE);
	rPipeFD = open_adviser_pipe(WEBTOKEN_COMM_PIPE1);
	CmdPipeFD = open_adviser_pipe(CMD_COMM_PIPE);

	/* Create the threads */
	if(0 != create_threads())
	{
		close_descriptors();
		TEMERG("Creation of threads failed.\n");
		return -1;
	}

	init_usb_mouse_mode();
	//if kvm session timout is supported start timer thread.
	if(g_corefeatures.kvm_session_timeout)
	{
		g_TimerTickSeconds = KVM_TIMER_TICKLE;
		start_own_timer_thread();
	}

	if(g_corefeatures.kvm_reconnect_support == ENABLED)
	{
		GetAdviserCfg(& pAdviserCfg);
		g_kvm_retry_count = pAdviserCfg.retry_count;
		g_kvm_retry_interval = pAdviserCfg.retry_interval;
	}

	if(g_corefeatures.hid_device_initialize_adviser)
	{
		if ( 0 != Refresh_HID_Device())
			TCRIT("HID Device Refresh Failed...\n");
	}

	ProcMonitorRegister("/usr/local/bin/adviserd",port_num,"adviser",adviser_signal_handler,0);
	connection_acceptor (port_num);

	wait_for_threads_to_exit();

	close_descriptors();

	return 0; /* Returns only when CTRL-C is pressed */
}

