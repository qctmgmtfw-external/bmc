/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2010, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* Filename: ncml.h
*
* Description: Contains code for the network conection manager library APIs.
*
***************************************************************/

#ifndef _NCML_H
#define _NCML_H

#include <Types.h>

#define SERVICE_CONF_FILE                  "/conf/ncml.conf"  /** Location of the configuration file */
#define DEFAULT_SERVICE_CONF_FILE          "/etc/defconfig/ncml.conf"  /** Location of the default configuration file */

/* Session/Service name macros */
#define WEB_SERVICE_NAME            "web"
#define KVM_SERVICE_NAME            "kvm"
#define CDMEDIA_SERVICE_NAME        "cd-media"
#define FDMEDIA_SERVICE_NAME        "fd-media"
#define HDMEDIA_SERVICE_NAME        "hd-media"
#define SSH_SERVICE_NAME            "ssh"
#define TELNET_SERVICE_NAME         "telnet"

#define WEB_SERVICE_ID_BIT		0
#define KVM_SERVICE_ID_BIT		1
#define CDMEDIA_SERVICE_ID_BIT		2
#define FDMEDIA_SERVICE_ID_BIT		3
#define HDMEDIA_SERVICE_ID_BIT		4
#define SSH_SERVICE_ID_BIT		5
#define TELNET_SERVICE_ID_BIT		6
#define IPMI_SERVICE_ID_BIT		7

#define MAX_SERVICE_NUM             32
#define MAX_SERVICE_NAME_SIZE       16
#define MAX_SERVICE_IFACE_NAME_SIZE 16
#define MAX_TEMP_ARRAY_SIZE         64
#define MAX_NA_ARRAY_SIZE           255		
#define ASCII_F                     70

/* Key string that represents the member of the service configurations */
#define STR_SERVICE_NAME                "service_name"
#define STR_SERVICE_CURRENT_STATE       "current_state"
#define STR_SERVICE_INTERFACE_NAME      "interface_name"
#define STR_SERVICE_NONSECURE_PORT      "nonscecure_port"   
#define STR_SERVICE_SECURE_PORT         "secure_port"       
#define STR_SERVICE_SESSION_TIMEOUT     "session_timeout"
#define STR_SERVICE_MAX_SESSIONS        "max_sessions"      
#define STR_SERVICE_ACTIVE_SESSIONS     "active_sessions"   
#define STR_MAX_SESSION_INACTIVITY_TIMEOUT "MaxSessionInactivityTimeout"
#define STR_MIN_SESSION_INACTIVITY_TIMEOUT "MinSessionInactivityTimeout"

/* Return Error Code for init_service_configurations */
#define COMPLETION_SUCCESS          (0)
#define ERR_SET_CONF     (1)
#define ERR_LOAD_DEFCONF (-1)
#define ERR_GET_CONF     (-2)
#define ERR_GET_DEFCONF  (-3)
#define ERR_LOAD_CONF   (-4)

/*  Return Error Code for  Validate_SetServiceConf_Req_Parameter  */
#define NCML_ERR_READ_NCML_CONF                                         0x80
#define NCML_ERR_NCMLCONFIG_NA                                          0x81
#define NCML_ERR_NCMLCONFIG_NE                                          0x82
#define NCML_ERR_INVALID_PORT                                           0x83
#define NCML_ERR_PORT_ALREADY_IN_USE                                    0x84
#define NCML_ERR_INVALID_SESSION_INACTIVE_TIMEOUT                       0x85
#define NCML_ERR_SESSION_INVALID_COUNT                                  0x86 

#define MAX_PORT_NUMBER     49151
#define MAX_SESSION_COUNT 127
#define PORT_VAL_BYTE 0x7FFFFFFF
#define NOT_APPLICABLE_BYTE 0xFF
#define NOT_ENABLED_BYTE 0x80

#define MAX_ETHIFC_LEN (IFNAMSIZ+1)
    

#ifdef __GNUC__
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif

typedef struct
{
    INT8S   ServiceName[MAX_SERVICE_NAME_SIZE +1]; /* Service name */
    INT8U   CurrentState;                       /* 1, to enable the service; 0, to disable the service */
    /* The following fields are meaningful only when the service is enabled */
    INT8S   InterfaceName[MAX_SERVICE_IFACE_NAME_SIZE +1]; /* Interface name */
    INT32U  NonSecureAccessPort;                        /* Non-secure access port number */
    INT32U  SecureAccessPort;                           /* Secure access port number */                        
    INTU    SessionInactivityTimeout;                   /* Service session inactivity yimeout in seconds*/
    INT8U   MaxAllowSession;                            /* Maximum allowed simultaneous sessions */
    INT8U   CurrentActiveSession;                       /* Number of current active sessions */
    INTU    MaxSessionInactivityTimeout;                /*Max Allowed value for Session timeout in sec*/
    INTU    MinSessionInactivityTimeout;	            /*Min Allowed value for Session timeout in sec*/

} PACKED SERVICE_CONF_STRUCT;

#ifndef __GNUC__
#pragma pack()
#endif



extern int get_network_interface_count(int *ifcount);
extern int get_up_network_interfaces(char *up_interfaces, int *ifupcount, int ifcount);
extern int get_network_interfaces_name(char * up_interfaces, int ifcount);
extern int wait_network_state_change();
extern int get_service_configurations(char *service_name, SERVICE_CONF_STRUCT* conf);
extern int set_service_configurations(char *service_name, SERVICE_CONF_STRUCT* conf);
extern int create_service_defconf_file(void);
extern int check_service_conf_file(void);
extern int init_service_configurations(char *service_name, SERVICE_CONF_STRUCT* conf);
extern int Validate_SetServiceConfiguration(SERVICE_CONF_STRUCT *ReqConf);
extern int isNotApplicable(unsigned char *data, int dataSize);
extern int isNotEditable(unsigned char *data, int dataSize);
extern void getNotEditableData(unsigned char *maskData, int dataSize, unsigned char *validData);
extern void getNotEditableMaskData(unsigned char *validData/*IN*/, int dataSize/*IN*/, unsigned char *maskData/*OUT*/);
extern int isStringNotApplicable(char *data,int size);
#endif // _NCML_H
