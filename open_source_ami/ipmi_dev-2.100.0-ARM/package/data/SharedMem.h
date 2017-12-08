/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *****************************************************************
 *
 * SharedMem.h
 * Memory shared by different tasks
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef SHARED_MEM_H
#define SHARED_MEM_H
#include "Types.h"
#include "OSPort.h"
#include "MsgHndlr.h"
#include "IPMI_Main.h"
#include "PMConfig.h"
#include "Session.h"
#include "BridgeMgmt.h"
#include "BridgeEvt.h"
#include "SerialModem.h"
#include "PDKHooks.h"
#include "Terminal.h"
#include "PEFTmr.h"
#include "IPMI_LANConfig.h"
#include "NVRAccess.h"
#include "Indicators.h"
#include "Debug.h"

#include "ChassisDevice.h"
#include "IPMI_LANConfig.h"
#include "IPMI_ChassisDevice.h"
#ifdef CONFIG_SPX_FEATURE_ENABLE_INTERNAL_SENSOR
#include "internal_sensor.h"
#endif
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
#include "SSIDefs.h"
#endif


#pragma pack( 1 )

#define DISABLED_EVENT_RECEIVER 0xFF
#define MAX_HOSTNAME_LEN    64
#define MAX_DNS_IP_ADDRESS                            0x03
#define MAX_DOMAIN_BLOCK_SIZE                      64
#define MAX_BLOCK                                               0x04

typedef struct
{
    INT8U   HostSetting;
    INT8U   HostNameLen;
    INT8U   HostName[MAX_HOSTNAME_LEN];
    INT8U   RegisterBMC[MAX_LAN_CHANNELS];
    INT8U   DomainDHCP;
    INT8U   DomainIndex;
    INT8U   Domainpriority;
    INT8U   DomainLen;
    INT8U   DNSDHCP;
    INT8U   DNSIndex;
    INT8U   DomainName[MAX_DOMAIN_BLOCK_SIZE * MAX_BLOCK];
    INT8U   IPPriority;
    INT8U   DNSIPAddr1[IP6_ADDR_LEN];
    INT8U   DNSIPAddr2[IP6_ADDR_LEN];
    INT8U   DNSIPAddr3[IP6_ADDR_LEN];
}PACKED DNS_T;

/**
 * @struct BMCSharedMem_T
 * @brief Structure of memory shared between different tasks.
**/
typedef struct
{
    INT8U               EvRcv_SlaveAddr;			/**< Event receiver slave address.  */
    INT8U               EvRcv_LUN;				/**< Event receiver LUN.            */
    INT8U               MsgFlags;				/**< Message flags.                 */
    INT8U               GlobalEnables;				/**< Global enables.                */
    AlertStringTbl_T    AlertStringEntry;			/**< Alert strings.                 */
    LANDestType_T       VolLANDestType[MAX_LAN_CHANNELS];				/**< Volatile lan destination type. */
    LANDestAddr_T       VolLANDest[MAX_LAN_CHANNELS];					/**< Volatile lan destination address.*/
    LANDestv6Addr_T       VolLANv6Dest[MAX_LAN_CHANNELS];					/**< Volatile lan destination address.*/
    INT8U               LANAlertStatus[MAX_LAN_CHANNELS];				/**< Lan alert stuatus.             */
    //INT8U               GratArpStatus;				/**< Gratuitous arp status.         */
    INT8U               ArpSuspendStatus[MAX_LAN_CHANNELS];
    VLANDestTags_T      VLANDestTag;				/**< VLAN Destination tags.         */
    INT8U               SOLBitRate[MAX_LAN_CHANNELS];					/**< SOL bit rate.                  */
    SMConfig_T          SMConfig;					/**< Serial modem configuration.    */
    INT8U               SerialMuxSetting;			/**< Serial mux setting.            */
    BOOL                SerialSessionActive;		/**< Session active status.         */
    BOOL                SerialModemMode;			/**< Modem mode.                    */
    TAPResCode_T        TAPRes;
    BridgeMgmtSHM_T     BridgeMgmt;					/**< Bridge management information. */
    BridgeEvt_T         BridgeEvt;					/**< Bridge event.                  */
    HealthState_T       HealthState;
    INT32U              SOLSessID;					/**< SOL session id.                */
    INT8U               SessionHandle;				/**< Session handle.                */
    PETAckTimeOutMgr_T  PETAckMgr [MAX_PET_ACK];	/**< PET Acknowledge Informations   */
    DeferredAlert_T     DeferredAlert [MAX_DEFERRED_ALERTS];    /**<Deferred Alert Information  */
    HTaskID_T           MsgHndlrID;                 /**< Message handler Task ID        */
    MsgHndlrTbl_T*      pMsgHndlrTbl;               /**< Pointer to Message Handler Table*/
    INT8U               MsgHndlrTblSize;            /**< Message handler Table Size      */
    INT8U               NumRcvMsg[3];                  /**< Number of message in RcvMsg_Q  */
    INT8U               NumEvtMsg;                  /**< Number of message in EvtMsg_Q  */
    INT8U               DeviceGUID [16];            /**< BMC GUID                       */
    INT8U               SystemGUID [16];            /**< System GUID                    */

    IndicatorInfo_T	    LEDInfo[MAX_LED_NUM];       /**< LED Info **/
    IndicatorInfo_T	    BeepInfo;                   /**< Beep Info **/

    PDK_SharedMem_T     PDKSharedMem;               /**< PKD Shared memory              */

    INT8U            	SerialMUXMode;             /**< Current Mux setting specfic for OEM */

    INT8U       		OSName[MAX_OS_NAME_SELECTOR][MAX_BLOCK_SIZE] ;
    INT8U       		m_Sys_SetInProgress;
    INT8U       		m_Lan_SetInProgress;
    INT8U       		m_Serial_SetInProgress;
    INT8U       		m_PEF_SetInProgress;
    INT8U       		m_SOL_SetInProgress;
    INT8U				m_ACPISysPwrState;			/**<	ACPISys Power State */
    INT8U				m_ACPIDevPwrState;			/**<	ACPIDev Power State */
    INT16U                    m_LastRecID;				/**<  Added to handle via SMM Channel */
    INT16U                    m_FirstRecID;                         /* First SEL RecID*/
    INT16U                    m_SELIndex;                          /*SEL Index value*/

    /*  Watch dog timer Info Since KCS -SMM  and Msg Hndlr using WatchDog Timer Variables */	
    INT8U		               IsWDTPresent	;
    INT8U  			  	 IsWDTRunning    ;

   /* System Event Sensor Number used for PEFAction Event logging in PEF task */
   INT8U               SysEvent_SensorNo;

    BootOptions_T       sBootOptions;      			/* semi-volatile Boot Options */
    INT8U			  IsValidBootflagSet;
    INT8U             SysRestartCaused;
    INT8U               u8MadeChange;               /* indicate the restart reason changed */
    #ifdef CONFIGURABLE_SESSION_TIME_OUT
    int 		uSessionTimeout;					/* configurable session timeout */
    #endif
    int 		gIPUDPRMCPStats;							/* Valid RMCP Packets counter */
    INT8U        InitSELDone;
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
    OpStateFruObj_T     OpStateFruObj;
    INT8U               IPMBActiveBus;
    BOOL                AdminDisableIPMBPrimary;
    BOOL                AdminDisableIPMBSecondary;
    BOOL                PowerActionInProgress;
#endif
    DNS_T               DNSconf;
} PACKED  BMCSharedMem_T;

#pragma pack( )

/**
 * @def BMC_CREATE_SHARED_MEM()
 * @brief Create shared memory.
**/
#define BMC_CREATE_SHARED_MEM() \
    (OS_CREATE_SHARED_MEM (SHMH_BMC, sizeof (BMCSharedMem_T)))

/**
 * @def BMC_GET_SHARED_MEM()
 * @brief Get shared memory access.
**/
#define BMC_GET_SHARED_MEM(BMCInst) ((_FAR_ BMCSharedMem_T*)&g_BMCInfo[BMCInst].BMCSharedMem)


/*Shared memory time out in seconds */
#define SHARED_MEM_TIMEOUT        15

#if 0
/**
 * @var hSharedMemMutex
 * @brief Shared memory mutex.
**/
//extern _FAR_ Mutex_T*    hSharedMemMutex;
extern pthread_mutex_t    g_hBMCSharedMemMutex;

/**
*@var g_hSensorSharedMemMutex
*@brief Sensor Shared memory mutex
**/
extern pthread_mutex_t  g_hSensorSharedMemMutex;

/**
*@var g_SELMutex
*@brief SEL mutex
**/
extern pthread_mutex_t m_hSELMutex;

/**
*@var m_hSensorSharedMemMutex
*@brief SEL mutex
**/
extern pthread_mutex_t m_hSensorSharedMemMutex;

/**
*@var hPEFSharedMemMutex
*@brief SEL mutex
**/
extern pthread_mutex_t hPEFSharedMemMutex;

/**
*@var g_hSessionTblMutex
*@brief Session Tbl mutex
**/
extern pthread_mutex_t g_hSessionTblMutex;


#endif

/**
 * @def LOCK_BMC_SHARED_MEM ()
 * @brief lock BMC shared memory
**/
//#define LOCK_BMC_SHARED_MEM() OS_ACQUIRE_MUTEX(hSharedMemMutex, SHARED_MEM_TIMEOUT)
#define LOCK_BMC_SHARED_MEM(BMCInst) OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].BMCSharedMemMutex, SHARED_MEM_TIMEOUT)


/**
 * @def UNLOCK_BMC_SHARED_MEM ()
 * @brief unlock BMC shared memory
**/
//#define UNLOCK_BMC_SHARED_MEM() OS_RELEASE_MUTEX(hSharedMemMutex)
#define UNLOCK_BMC_SHARED_MEM(BMCInst) OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].BMCSharedMemMutex)


/**
 * @def LOCK_BMC_SHARED_MEM () for PEF Task only
 * @brief lock BMC shared memory
**/
#define LOCK_BMC_SHARED_MEM_FOR_PEF(BMCInst) OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].PEFSharedMemMutex, SHARED_MEM_TIMEOUT)

/**
 * @def UNLOCK_BMC_SHARED_MEM () for PEF Task only
 * @brief unlock BMC shared memory
**/
#define UNLOCK_BMC_SHARED_MEM_FOR_PEF(BMCInst) OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].PEFSharedMemMutex)


/**
 * @brief Initialize Shared memory.
 * @return 0 if success, -1 if error.
**/
extern int InitBMCSharedMem (int BMCInst);

extern MsgHndlrTbl_T m_MsgHndlrTbl[15];
extern GroupExtnMsgHndlrTbl_T m_GroupExtnMsgHndlrTbl [10];
extern TimerTaskTbl_T    m_TimerTaskTbl [20];
#ifdef CONFIG_SPX_FEATURE_ENABLE_INTERNAL_SENSOR
extern InternalSensorTbl_T m_internal_sensor_tbl[3];
#endif

#endif  /* SHARED_MEM_H */
