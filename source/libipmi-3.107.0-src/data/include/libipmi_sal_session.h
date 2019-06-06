#ifndef __LIBIPMI_SAL_SESSION_H__
#define __LIBIPMI_SAL_SESSION_H__

#include "platform.h"
#include <sys/types.h>
#if LIBIPMI_IS_OS_LINUX()
#include <arpa/inet.h>
#include <dlfcn.h>
#endif

#include "coreTypes.h"
#include "libipmi_struct.h"
#include "IPMIDefs.h"



#define SAL_IP_ADDR                "127.0.0.1"
#define SAL_PORT                    6677
#define SAL_WEB_PORT                6688
#define INET_ADDRSTRLEN             16
#define SEQ( seqlun )               (( seqlun & (u8)0xFC ) >> 2 )
#define CMD_RETRY_INTERVAL          1 //1 sec
#define CMM_SAL_LIB_PATH            "/usr/local/lib/libcmmsal.so" //SAL Library Path

#if LIBIPMI_IS_OS_LINUX()
#define PACK __attribute__ ((packed))
#else
#define PACK
#pragma pack(1)
#endif/* LIBIPMI_IS_OS_LINUX() */



typedef struct IPMI20_SAL_SESSION_T_tag
{
    #define INET_ADDRSTRLEN        16

    char ip[INET_ADDRSTRLEN];

#ifdef WIN32
} IPMI20_SAL_SESSION_T;
#else
}PACK IPMI20_SAL_SESSION_T;
#endif



/**
 @def IPMI20_SESSION__SAL_T
 @brief holds info for maintaining a session
*/
typedef struct IPMI20_SESSION_SAL_T_tag
{
    /* Medium type (Network, Serial, KCS, IPMB, USB) */
    uint8                           byMediumType;

    #define NETWORK_MEDIUM_TCP      0x01
    #define NETWORK_MEDIUM_UDP      0x04

    #define NETWORK_MEDIUM          NETWORK_MEDIUM_TCP
    #define SERIAL_MEDIUM           0x02
    #define KCS_MEDIUM              0x03
    #define IPMB_MEDIUM             0x05
    #define USB_MEDIUM              0x06
    #define UDS_MEDIUM              0x07

    /* tells whether session has started or not. */
    uint8   bySessionStarted;
    #define SESSION_NOT_STARTED     0x00
    #define SESSION_STARTED         0x01

    #define NETWORK_MEDIUM_TCP_SAL  0x81
    #define NETWORK_MEDIUM_UDP_SAL  0x84
    #define NETWORK_MEDIUM_SAL      NETWORK_MEDIUM_TCP_SAL

    /* if this value is > 0, session reestablishment will be tried for byMaxRetries times. */
    uint8   byMaxRetries;
    /* LAN Eth Index for hold Eth number if multi NIc supported */
    uint8         EthIndex;

    IPMI20_NETWORK_SESSION_T        *hNetworkSession;
    IPMI20_SERIAL_SESSION_T         *hSerialSession;
    IPMI20_KCS_SESSION_T            *hKCSSession;
    IPMI20_IPMB_SESSION_T           *hIPMBSession;
    IPMI20_USB_SESSION_T            *hUSBSession;
    IPMI20_UDS_SESSION_T            *hUDSSession;

    LIBIPMI_SERIAL_SETTINGS_T       Settings;

    /* SAL specific info */
    IPMI20_SAL_SESSION_T            salInfo;
    #define SAL_SESSION_STARTED     0x02

} PACK IPMI20_SESSION_SAL_T;



int Send_RAW_IPMI2_0_SAL_SessionLessPkt_Command(IPMI20_SESSION_SAL_T *pSession,
                                                                 uint8 byPayLoadType,
                                                                 uint8 byNetFnLUN,
                                                                 uint8 byCommand,
                                                                 uint8 *pszReqData,
                                                                 uint32 dwReqDataLen,
                                                                 uint8 *pszResData,
                                                                 uint32 *pdwResDataLen,
                                                                 int timeout);



uint8 SessionLess_IPMI2_0_Send_Command(IPMI20_SESSION_SAL_T *pSession,
                                                  uint8 *pbyPayLoadType,
                                                  uint8 *pbyIPMIData,
                                                  uint32 dwIPMIDataLen,
                                                  uint8 *pszResData,
                                                  uint32 *pdwResDataLen,
                                                  int timeout);

uint16 ValidateSessionLessHeader(IPMI20_SESSION_SAL_T *pSession,
                                       IPMIMsgHdr_T  *pRq,
                                       IPMIMsgHdr_T  *pRs);


#endif //__LIBIPMI_SAL_SESSION_H__
