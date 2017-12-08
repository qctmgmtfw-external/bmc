/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * PMConfig.h
 * Platform management configuration structures
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *          Basavaraj Astekar   <basavaraja@ami.com>
 *          Ravinder Reddy      <bakkar@ami.com>
 ******************************************************************/
#ifndef PMCONFIG_H
#define PMCONFIG_H
#include "Types.h"
#include "OSPort.h"
#include "IPMI_AppDevice.h"
#include "IPMI_ChassisDevice.h"
#include "IPMI_PEF.h"
#include "IPMI_LANConfig.h"
#include "SerialModem.h"
#include "BridgeMgmt.h"
#include "Support.h"
#include "IPMI_AMISmtp.h"
#include "pmcfg.h"
#include "OemDefs.h"
#include "lanchcfg.h"
#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
#include "SSIDefs.h"
#endif
#include "IPMI_AMISmtp.h"

/*** External Definitions ***/
#define MAX_PRIVILEGE_TYPES                 7
#define MAX_CHFILE_NAME                         50

#define MAX_EVT_FILTER_ENTRIES              40
#define ALERT_POLICY_SET_SIZE		    4
#define ALERT_POLICY_PER_CHANNEL	    15
#define LAN_DESTINATIONS_PER_CHANNEL	    15
#define NUM_LAN_DESTINATION                 LAN_DESTINATIONS_PER_CHANNEL
#define MAX_ALERT_POLICY_ENTRIES            (ALERT_POLICY_PER_CHANNEL * 4)

//Added to Support 20 Alert Strings in Set/Get Pef Config.Parms ../
/* It should equal to number of Event filter entries */
#define MAX_ALERT_STRINGS                  MAX_EVT_FILTER_ENTRIES
#define PEF_MAX_OEM_PARAMETERS              0x20
#define MAX_SIZE_PET_GUID                   0x11
#define MAX_PLD_ENABLES_TYPES               4
#define MAX_FF_CMD_CFGS                     255
/* RMCP+ Definitions */
#define MAX_ALGORITHMS						2
#define MAX_PAYLOADS						3
#define HASH_KEY_LEN						20
#define MAX_ID_PAIRS 7


#define SERIAL_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"serialcfg.ini");

#define CHASSIS_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"chassiscfg.ini");

#define PEF_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"pefcfg.ini");

#define WDT_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"wdogcfg.ini");

#define BRIDGE_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"bridgecfg.ini");

#define SYSINFO_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"systeminfo.ini");

#define FIREWALL_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"fwfirewallcfg.ini");

#define OPMA_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"opmacfg.ini");

#define GENERIC_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"gencfg.ini");

#define LAN_CONFIG_FILE(Instance,filename,ChIndex)    \
sprintf(filename,"%s%d/lancfg%d.ini",NV_DIR_PATH,Instance,ChIndex);

#define RMCP_CONFIG_FILE(Instance,filename,ChIndex)    \
sprintf(filename,"%s%d/rmcpcfg%d.ini",NV_DIR_PATH,Instance,ChIndex);

#define SOL_CONFIG_FILE(Instance,filename,ChIndex)    \
sprintf(filename,"%s%d/solcfg%d.ini",NV_DIR_PATH,Instance,ChIndex);

#define SMTP_CONFIG_FILE(Instance,filename,ChIndex)    \
sprintf(filename,"%s%d/smtpcfg%d.ini",NV_DIR_PATH,Instance,ChIndex);

#define USERCONFIG_FILE(Instance,filename) \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"UserConfig.ini");

#define IPMBCHCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"IpmbChCfg.ini");

#define SYSCHCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"SysifcChCfg.ini");

#define LANCHCONFIG1_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"LanChcfg1.ini");

#define LANCHCONFIG2_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"LanChcfg2.ini");

#define LANCHCONFIG3_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"LanChcfg3.ini");

#define SERIALCHCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"SerialChcfg.ini");

#define ICMBCHCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"IcmbChcfg.ini");

#define SMBUSCHCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"Smbuschcfg.ini");

#define SMLINKIPMBCHCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"Smlinkipmbchcfg.ini");

#define DCMICONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"DCMIConfig.ini");

#define SMMCHCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"Smmchcfg.ini");

#define TRIGGEREVTCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"TriggerEvtConfig.ini");

#define LOGINAUDITCONFIG_FILE(Instance,filename)   \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"loginadtcfg.ini");

#define AMI_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"amicfg.ini");

#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
#define SSI_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"ssicfg.ini");
#endif

#ifdef CONFIG_SPX_FEATURE_IPMI_CONFIG_VER_CHECK
#define VERSION_CONFIG_FILE(Instance,filename)    \
sprintf(filename,"%s%d/%s",NV_DIR_PATH,Instance,"version.ini");
#endif

#define IPMI_CONFIG_VERSION                 4

/*  The IPMI configuration version is maintained in IPMI_CONFIG_VERSION macro and the version 
      number should be incremented in counts of '1' whenever there are changes in CDF files used in 
      MDS and in CDF related structures used in this file.One should make sure that the version number incremented
      in IPMI_CONFIG_VERSION and version number maintained in libipmipar package should be same.
      The CDF version number for libipmipar should be configured using MDS. The IPMI Config version check
      will be done only when CONFIG_SPX_FEATURE_IPMI_CONFIG_VER_CHECK feature is enabled 
      in PRJ file*/
#pragma pack( 1 )

/**
 * @struct WDTConfig_T
 * @brief Watchdog Timer configuration information.
**/
typedef struct
{
    INT8U   TmrUse;
    INT8U   TmrActions;
    INT8U   PreTimeOutInterval;
    INT8U   ExpirationFlag;
    INT16U InitCountDown;
    INT8U   PreTimeoutActionTaken;

} PACKED  WDTConfig_T;



/**
 * @struct PEFConfig_T
 * PEF Configuration structure.
**/
typedef struct
{
    INT8U                   PEFControl;
    INT8U                   PEFActionGblControl;
    INT8U                   PEFStartupDly;
    INT8U                   PEFAlertStartupDly;
    EvtFilterTblEntry_T     EvtFilterTblEntry [MAX_EVT_FILTER_ENTRIES];
    AlertPolicyTblEntry_T   AlertPolicyTblEntry [MAX_ALERT_POLICY_ENTRIES];
    INT8U                   SystemGUID [MAX_SIZE_PET_GUID];
    AlertStringTbl_T        AlertStringEntry [MAX_ALERT_STRINGS];
    INT8U                   OEMParams [PEF_MAX_OEM_PARAMETERS];
    INT8U                   NumAlertPolicyEntries;
    INT16U                  LastBMCProcessedEventID;
    INT16U                  LastSWProcessedEventID;
    INT16U                  LastSELRecordID;
    INT32U                  LastProcessedTimestamp;
    INT8U                   LastActionDone;

} PACKED  PEFConfig_T;


/**
 * @struct ChannelUserInfo_T
 * Structure for user information per channel.
**/
typedef struct
{
    INT32U  ID;
    INT8U   UserId;
    INTU    AccessLimit:4;
    INTU    LinkAuth:1;
    INTU    UserAccessCallback:1;
    INTU    UserAccessCBCPCallback:1;
    INT8U   UserCallbackCapabilities;
    INT8U   CBCPNegOptions;
    INT8U   CallBack1;
    INT8U   CallBack2;
    INT8U   CallBack3;
    INT8U   FailureAttempts;
    INT8U   Lock;
    INT32U LockedTime;
    INT8U  PayloadEnables [MAX_PLD_ENABLES_TYPES];

    INT8U  IPMIMessaging;
    INT8U  ActivatingSOL;

} PACKED  ChannelUserInfo_T;


/**
 * @struct UserInfo_T
 * Structure for user information.
**/
typedef struct
{
    INT32U  ID;
    INT8U   UserId;
    INT8U   UserName [MAX_USERNAME_LEN];
    INT8U   UserPassword [MAX_PASSWORD_LEN];
    INT8U   MaxPasswordSize;				/**< Maximum password size					*/
    INT8U   UserShell;	                    /**< user shell type	                    */		
    INT8U	UserEMailID [EMAIL_ADDR_SIZE];	/**< Email-ID registered for the user		*/
    INT8U   MaxSession;             		/**< max No of session allowed for User.    */
    INT8U   CurrentSession;         		/**< No Current session for the User.       */
    INTU    UserStatus:1;           		/**< User Enabled/Disabled Status.          */
    INTU    FixedUser:1;            		/**< Is this user name fixed                */
    INT8U  EmailFormat[EMAIL_FORMAT_SIZE]; /** Email Alert format                       */

} PACKED  UserInfo_T;


/**
 * @struct DefaultChCfg_T
 * Structure of channel Information.
**/
typedef struct
{
    INT32U          ID;
    INTU            ChannelIndex;
    INTU            ChannelNumber:4;
    INTU            ChannelType:7;     /* Channel Type */
    INTU            ChannelMedium:7;
    INTU            ChannelProtocol:5;
    INTU            SessionSupport:2;               /**< session support. */
    INTU            ActiveSession:6;                /**< No of active session in this channel. */
    INT8U           AuthType [MAX_PRIVILEGE_TYPES]; /**< Authentication according to privilege. */
    INT8U           ProtocolVendorId [3];
    INT8U           AuxiliaryInfo [2];
    INTU            ReceiveMsgQ:1;            /**< enable/disable  message to Receive Message Queue. */
    INTU            AccessMode:4;             /**< channel access mode - disabled/perboot/always/shared. */
    INTU            AccessModeSupported:4;    /**< access mode supported by this channel. */
    INTU            Alerting:1;               /**< enable/disable alerting through this channel. */
    INTU            PerMessageAuth:1;         /**< enable/disable per message Auth through this channel. */
    INTU            UserLevelAuth:1;          /**< enable/disable userlevel Auth through this channel. */
    INTU            MaxPrivilege:4;           /**< max privilage level allowed by this channel. */
    INTU            reserved:1;
    INTU            SetUserAccess:1;          /**< set user access command not allowed. */
    INTU            SetChPrivilege:2;
    INTU            SessionLimit:6;
    INTU            LoginStatusNUP:1;
    INTU            LoginStatusNUNP:1;
    INTU            LoginStatusNNU:1;
    INTU            MaxUser:6;
    INTU            NoCurrentUser:6;
    INTU            NoFixedUser:6;
    INTU            ChannelIndexRam:4;
    INTU            ChannelIndexNvRam:4;
  ChannelUserInfo_T ChannelUserInfo [MAX_NUM_CH_USERS_MDS];

} PACKED  ChannelInfo_T;


/**
 * @struct ChassisConfig_T
 * Chassis Configuration.
**/
typedef struct
{
   INT8U                        SysRestartCause;
   INT8U                        PowerRestorePolicy;
   ChassisPowerState_T          ChassisPowerState;
   ChassisCapabilities_T        ChassisCapabilities;
   INT8U						PowerCycleInterval;
   AMI_BootOpt_T                OemBootOpt;  /**< AMI OEM BootOptions */

} PACKED  ChassisConfig_T;


/**
 * @struct VLANDestTags_T
 * VLAN Destination Tags.
**/
typedef struct
{
    INT8U   AddressFormat;
    INT16U  VLANTag;

} PACKED  VLANDestTags_T;


/**
 * @struct LANConfig_T
 * LAN Configuration.
**/
typedef struct {

    INT8U               AuthTypeSupport;
    AuthTypeEnables_T   AuthTypeEnables;
    INT8U               IPAddr [IP_ADDR_LEN];
    INT8U               IPAddrSrc;
    INT8U               MACAddr [MAC_ADDR_LEN];
    INT8U               SubNetMask [4];
    IPv4HdrParams_T     Ipv4HdrParam;
    INT16U              PrimaryRMCPPort;
    INT16U              SecondaryPort;
    INT8U               BMCGeneratedARPControl;
    INT8U               GratitousARPInterval;
    INT8U               DefaultGatewayIPAddr  [IP_ADDR_LEN];
    INT8U               DefaultGatewayMACAddr [MAC_ADDR_LEN];
    INT8U               BackupGatewayIPAddr  [IP_ADDR_LEN];
    INT8U               BackupGatewayMACAddr [MAC_ADDR_LEN];
    INT8U               CommunityStr [MAX_COMM_STRING_SIZE];
    INT8U               NumDest;
    LANDestType_T       DestType [NUM_LAN_DESTINATION];
    LANDestAddr_T       DestAddr [NUM_LAN_DESTINATION];
    LANDestv6Addr_T       Destv6Addr [NUM_LAN_DESTINATION];
    INT16U              VLANID;
    INT8U               VLANPriority;
    INT8U               CipherSuitePrivLevels [MAX_NUM_CIPHER_SUITE_PRIV_LEVELS];
    VLANDestTags_T      VLANDestTags [NUM_LAN_DESTINATION];
    BadPassword_T BadPasswd;
    INT8U               IPv6_Enable;
    INT8U               IPv6_IPAddrSrc;
    INT8U               IPv6_IPAddr [IP6_ADDR_LEN];
    INT8U               IPv6_PrefixLen;
    INT8U               IPv6_GatewayIPAddr[IP6_ADDR_LEN];
    V6DNS_CONFIG    V6DNSConfig;
    
} PACKED  LANConfig_T;


/**
 * @struct PayloadSupport_T
 * Payload support.
**/
typedef struct
{
    INT8U   StandPldtype1;
    INT8U   StandPldtype2;
    INT8U   SessStpPldtype1;
    INT8U   SessStpPldtype2;
    INT8U   OemPldtype1;
    INT8U   OemPldtype2;

} PACKED  PayloadSupport_T;


/**
 * @struct PayloadInfo_T
 * Payload information.
**/
typedef struct
{
    INT8U   Type;
    INT8U   OemPldIANA [3];
    INT8U   OemPldID [2];
    INT8U   Version;

} PACKED  PayloadInfo_T;


/**
 * @struct RMCPPlus_T
 * RMCP+ information.
**/
typedef struct
{
    BOOL                LockKey [2];
    INT8U               KGHashKey [HASH_KEY_LEN];
    INT8U               PseudoGenKey [HASH_KEY_LEN];
    INT8U               Algorithm [MAX_PAYLOADS] [MAX_PRIVILEGE_TYPES];
    PayloadSupport_T    PayloadSupport;
    PayloadInfo_T       PayloadInfo [MAX_PYLDS_SUPPORT];

} PACKED  RMCPPlus_T;


/**
 * @struct FFCmdConfigTbl_T
 * Firmware Firewall Command Configuration Tbl.
**/
typedef struct
{
    INT8U   NetFn;
    INT8U   Cmd;
    INT8U   Config;

} PACKED  FFCmdConfigTbl_T;


/**
 * @struct SOLConfig_T
 * SOL Configuration.
**/
typedef struct
{
    INT8U   SOLEnable;
    INT8U   SOLAuth;
    INT16U  CharAccThresh;
    INT8U   SOLRetryCount;
    INT8U   SOLRetryInterval;
    INT8U   NVBitRate;
    INT8U   VBitRate;
    INT8U   PayldChannel;
    INT16U  PayldPortNum;

} PACKED  SOLConfig_T;


/**
 * @struct SystemInfoConfig_T
 * Get / Set System Info Parameters info.
**/
typedef struct
{
    INT8U		SysFWVersion[MAX_FW_VER_SELECTOR][MAX_BLOCK_SIZE];
    INT8U		SysName[MAX_SYS_NAME_SELECTOR][MAX_BLOCK_SIZE];
    INT8U		PrimaryOSName[MAX_OS_NAME_SELECTOR][MAX_BLOCK_SIZE];
}PACKED SystemInfoConfig_T;


typedef struct
{
    BOOL	IsValid;
    INT8U	ActivePowerLimit;
    INT8U	ExceptionAction;
    INT16U	PwrLimitInWatts;
    INT32U	CorrectionTimeLimitInMsec;
    INT16U	MangSamplingTimeInSecs;

} PACKED DCMICfg_T;


typedef struct
{
    INT8U    CurrentNoUser;
    INT32U    POHCounterReading;
    /* SEL Time UTC Offset information */
    INT16U    SELTimeUTCOffset;
    INT32U    SDREraseTime;

}PACKED GENConfig_T;


/**
 * @struct ChcfgInfo_T
 * Channel Configuration Information taken from respective channel .i files.
**/
typedef struct
{
    INT8U ChType;
    ChannelInfo_T ChannelInfo;
} PACKED ChcfgInfo_T;

/**
 * @struct AuthTypeEnable_T
 * @brief Authentication Enables.
**/
typedef struct
{
    INT8U Callback;
    INT8U User;
    INT8U Operator;
    INT8U Admin;
    INT8U oem;

} PACKED  AuthTypeEnable_T;


/**
 * @struct ChannelCallbackCtrl_T
 * @brief Channel Callback Control.
**/
typedef struct 
{
    INT8U CallBackEnable;
    INT8U CBCPnegopt;
    INT8U CallBackDes1;
    INT8U CallBackDes2;
    INT8U CallBackDes3;

} PACKED  ChannelCallbackCtrl_T;


/**
 * @struct IpmiMsgCommSettings_T
 * @brief Communication Settings.
**/
typedef struct 
{
    INT8U  FlowCtrl;
    INT8U  BitRate;

} PACKED  IpmiMsgCommSettings_T;


/** 
 * @struct MuxSwitchCtrl_T
 * @brief MUX Switch Control.
**/
typedef struct 
{
    INT8U Data1;
    INT8U Data2;

} PACKED  MuxSwitchCtrl_T;


/**
 * @struct ModemRingTime_T
 * @brief Modem Ring Time.
**/
typedef struct 
{
    INT8U RingDuration;
    INT8U RingDeadTime;

} PACKED  ModemRingTime_T;


/**
 * @struct DestInfo_T
 * @brief Destination Information.
**/
typedef struct 
{
    INT8U DesType;
    INT8U AlertAckTimeout;
    INT8U Retries;
    INT8U DesTypeSpecific;

} PACKED  DestInfo_T;


/**
 * @struct ModemDestCommSettings_T
 * @brief Modem Destination Communication Settings.
**/
typedef struct 
{
    INT8U FlowControl;
    INT8U BitRate;

} PACKED  ModemDestCommSettings_T;


/**
 * @struct DestIPAddr_T
 * @brief Destination IP Address.
**/
typedef struct 
{
    INT8U ip [IP_ADDR_LEN];

} PACKED  DestIPAddr_T;


/**
 * @struct TAPServiceSettings_T
 * @brief TAP Service Settings.
**/
typedef struct 
{
    INT8U   TAPConfirmation;
    INT8U   TAPServiceTypeChars [TAP_SERVICE_TYPE_FIELD_SIZE];
    INT32U  TAPCtrlESCMask;
    INT8U   TimeOutParam1;
    INT8U   TimeOutParam2;
    INT8U   TimeOutParam3;
    INT8U   RetryParam1;
    INT8U   RetryParam2;

} PACKED  TAPServiceSettings_T;


/**
 * @struct TermConfig_T
 * @brief Terminal Configuration Data.
**/
typedef struct 
{
    INT8U Data1;    /* Config Data1 deals with feature configuration */
    INT8U Data2;    /* Config Data2 deals with i/o termination sequence */

} PACKED  TermConfig_T;


/**
 * @struct PPPProtocolOptions_T
 * @brief PPP Protocol Options.
**/
typedef struct 
{
    INT8U  SnoopControl;
    INT8U  NegControl;
    INT8U  NegConfig;

} PACKED  PPPProtocolOptions_T;


/**
 * @struct PPPAccm_T
 * @brief PPP ACCM.
**/
typedef struct 
{
    INT32U  ReceiveACCM;
    INT32U  TransmitACCM;

} PACKED  PPPAccm_T;


/**
 * @struct PPPSnoopAccm_T
 * @brief PPP Snoop ACCM
**/
typedef struct 
{
    INT32U  ReceiveACCM;

} PACKED  PPPSnoopAccm_T;


/**
 * @struct PPPUDPProxyIPHeaderData_T
 * @brief PPP UDP Proxy Header Data.
**/
typedef struct 
{
    INT8U SrcIPAddress [IP_ADDR_LEN];
    INT8U DestIPAddress [IP_ADDR_LEN];

} PACKED  PPPUDPProxyIPHeaderData_T;


/**
 * @struct SMConfig_T
 * @brief Serial/Modem Configuration.
**/
typedef struct
{
    INT8U                       SetInProgress;
    INT8U                       AuthTypeSupport;
    AuthTypeEnable_T            AuthTypeEnable;
    INT8U                       ConnectionMode;
    INT8U                       SessionInactivity;
    ChannelCallbackCtrl_T       ChannelCallBackCtrl;
    INT8U                       SessionTermination;
    IpmiMsgCommSettings_T       IpmiMsgCommSet;
    MuxSwitchCtrl_T             MUXSwitchCtrl;
    ModemRingTime_T             RingTime;
    INT8U                       ModemInitString [MAX_MODEM_INIT_STR_BLOCKS] [MAX_MODEM_INIT_STR_BLOCK_SIZE];
    INT8U                       ModemEscapeSeq [MAX_MODEM_ESC_SEQ_SIZE + 1];   /*+1 for NULL Termination when */
    INT8U                       ModemHangup [MAX_MODEM_HANG_UP_SEQ_SIZE + 1];  /*full non null chars are provided*/
    INT8U                       ModemDialCmd [MAX_MODEM_DIAL_CMD_SIZE + 1];
    INT8U                       PageBlockOut;
    INT8U                       CommunityString [MAX_COMM_STRING_SIZE + 1];
    INT8U                       TotalAlertDest;
    DestInfo_T                  DestinationInfo [MAX_SERIAL_ALERT_DESTINATIONS];
    INT8U                       CallRetryInterval;
    ModemDestCommSettings_T     DestComSet [MAX_SERIAL_ALERT_DESTINATIONS];
    INT8U                       TotalDialStr;
    INT8U                       DestDialStrings [MAX_MODEM_DIAL_STRS] [MAX_MODEM_DIAL_STR_BLOCKS] [MAX_MODEM_DIAL_STR_BLOCK_SIZE];
    INT8U                       TotalDestIP;
    DestIPAddr_T                DestAddr [MAX_MODEM_ALERT_DEST_IP_ADDRS];
    INT8U                       TotalTAPAcc;
    INT8U                       TAPAccountSelector [MAX_MODEM_TAP_ACCOUNTS];
    INT8U                       TAPPasswd [MAX_MODEM_TAP_ACCOUNTS] [TAP_PASSWORD_SIZE + 1];
    INT8U                       TAPPagerIDStrings [MAX_MODEM_TAP_ACCOUNTS] [TAP_PAGER_ID_STRING_SIZE + 1];
    TAPServiceSettings_T        TAPServiceSettings [MAX_MODEM_TAP_ACCOUNTS];
    TermConfig_T                Termconfig;
    PPPProtocolOptions_T        PPPProtocolOptions;
    INT16U                      PPPPrimaryRMCPPort;
    INT16U                      PPPSecondaryRMCPPort;
    INT8U                       PPPLinkAuth;
    INT8U                       CHAPName [MAX_MODEM_CHAP_NAME_SIZE];
    PPPAccm_T                   PPPACCM;
    PPPSnoopAccm_T              PPPSnoopACCM;
    INT8U                       TotalPPPAcc;
    INT8U                       PPPAccDialStrSel [MAX_MODEM_PPP_ACCOUNTS];
    INT8U                       PPPAccIPAddress [MAX_MODEM_PPP_ACCOUNTS] [IP_ADDR_LEN];
    INT8U                       PPPAccUserNames [MAX_MODEM_PPP_ACCOUNTS] [PPP_ACC_USER_NAME_DOMAIN_PASSWD_SIZE + 1 ];
    INT8U                       PPPAccUserDomain [MAX_MODEM_PPP_ACCOUNTS] [PPP_ACC_USER_NAME_DOMAIN_PASSWD_SIZE + 1];
    INT8U                       PPPAccUserPasswd [MAX_MODEM_PPP_ACCOUNTS] [PPP_ACC_USER_NAME_DOMAIN_PASSWD_SIZE + 1];
    INT8U                       PPPAccAuthSettings [MAX_MODEM_PPP_ACCOUNTS];
    INT8U                       PPPAccConnHoldTimes [MAX_MODEM_PPP_ACCOUNTS];
    PPPUDPProxyIPHeaderData_T   PPPUDPProxyIPHeadData;
    INT16U                      PPPUDPProxyTransmitBuffSize;
    INT16U                      PPPUDPProxyReceiveBuffSize;
    INT8U                       PPPRemoteConsoleIPAdd [IP_ADDR_LEN];
    BadPassword_T               BadPasswd;

} PACKED  SMConfig_T;

/*
* SensorOffsetInfo_T Structure
*/
typedef struct {

    INT8U SensorLUN;
    INT8U SensorNo;
    INT8S Offset;
    INT8U valid;

} PACKED SensorOffsetInfo_T;


/*
* OPMA_IDPair_T Structure
*/
typedef struct
{
    INT16U OemId;
    INT16U ImplementationId;

} PACKED OPMA_IDPair_T;


/*
 * OPMA Config Structure
 * The OPMA specific Data are stored in this Structure
 */
typedef struct
{
    SensorOffsetInfo_T SensorOffsetInfo [256];
    INT8U         SupportedSysCount;
    OPMA_IDPair_T OPMA_IdPairs [ MAX_ID_PAIRS ];
    OPMA_IDPair_T SystemIdentifier ;
    INT8U SysLocalAccessLockout;
    INT16U OPMASpecComplaince;

} PACKED OPMA_Config_T;


typedef struct
{
    INT8U EnableDisableSMTP;
    INT8U EnableDisableSmtpAuth;
    INT8U ServerAddr [ IP_ADDR_LEN ];
    INT8U IP6_ServerAddr [ IP6_ADDR_LEN ];
    INT8U UserName [ MAX_SMTP_USERNAME_LEN ];
    INT8U Passwd   [MAX_SMTP_PASSWD_LEN];
    INT8U NoofDestinations;
    INT8U Subject	 [MAX_EMAIL_DESTINATIONS+1][MAX_SUB_BLOCK_SIZE * MAX_SUB_BLOCKS];
    INT8U Msg	 [MAX_EMAIL_DESTINATIONS+1][MAX_MSG_BLOCK_SIZE * MAX_MSG_BLOCKS];
    INT8U SenderAddr[MAX_EMAIL_BLOCK_SIZE * MAX_EMAIL_ADDR_BLOCKS];
    INT8U  Servername[MAX_SRV_NAME_BLOCK_SIZE * MAX_SRV_NAME_BLOCKS];
    INT16U SmtpPort;
    INT8U UserID[MAX_EMAIL_DESTINATIONS+1];
    INT8U EnableDisableSMTP2;
    INT8U EnableDisableSmtp2Auth;
    INT8U Server2Addr [ IP_ADDR_LEN ];
    INT8U IP6_Server2Addr [ IP6_ADDR_LEN ];
    INT8U UserName2 [ MAX_SMTP_USERNAME_LEN ];
    INT8U Passwd2 [MAX_SMTP_PASSWD_LEN];
    INT8U Sender2Addr[MAX_EMAIL_BLOCK_SIZE * MAX_EMAIL_ADDR_BLOCKS];
    INT8U  Server2name[MAX_SRV_NAME_BLOCK_SIZE * MAX_SRV_NAME_BLOCKS];
    INT16U Smtp2Port;
} PACKED Smtp_Config_T;


/**
 * @struct TriggerEventCfg_T
 * Trigger Event Configuration.
**/
typedef struct
{
   INT8U CriticalFlag;
   INT8U NONCriticalFlag;
   INT8U NONRecoverableFlag;
   INT8U FanTroubled;
   INT8U WDTTimeExpire;
   INT8U SysDConFlag;
   INT8U SysDCoffFlag;
   INT8U SysResetFlag;
   INT8U LPCResetFlag;
   INT8U SpecDateTime;
   INT32U Time;
}PACKED TriggerEventCfg_T;

/**
 * @struct LoginAuditConfig_T
 * Login Audit Configuration.
**/
typedef struct
{
   INT8U WebEventMask;
   INT8U IPMIEventMask;
   INT8U TelnetEventMask;
   INT8U SSHEventMask;
}PACKED LoginAuditConfig_T;

/**
 * @struct AMIConfig_T
 * @brief AMI Specific Configuration.
**/
typedef struct
{
    INT8U CircularSEL;
    INT8U  CircularSELFlag;

} PACKED AMIConfig_T;

#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
/**
 * @struct SSIConfig_T
 * @brief SSI Configuration.
**/
typedef struct
{
    OpState CurrentState;
    INT32U  CurrentConditions;
    INT8U   CurrentPowerLevel;
    INT8U   CMMIPAddr[IP_ADDR_LEN];

} PACKED SSIConfig_T; 
#endif

#ifdef CONFIG_SPX_FEATURE_IPMI_CONFIG_VER_CHECK
typedef struct
{
    INT32U Version;
} PACKED VersionConfig_T;
#endif

#pragma pack( )

/**
 * @brief Initialize PM configuration information.
 * @return 0 if success, -1 if error.
**/
extern int InitPMConfig (int BMCInst);


/**
*@fn InitChannelConfigs
*@brief This function is Initialize to load all Channel Configs from NVRAM to RAM
*/
extern int InitChannelConfigs(int BMCInst);

/**
*@fn InitDCMIConfig
*@brief This function is invoked to Initialize all DCMI Configs from NVRAM to RAM
*/
extern int initDCMIConfig(int BMCInst);

/**
*@fn InitUserConfig
*@brief This function is invoked to Initialize UserConfigs from NVRAM to RAM
*/
extern int InitUserConfig(int BMCInst);

/**
*@fn InitTriggerEventConfig
*@brief This function is invoked to Initialize TriggerEventConfig from NVRAM to RAM
*/
extern int InitTriggerEventConfig(int BMCInst);

/**
*@fn InitLoginAuditConfig
*@brief This function is invoked to Initialize the login audit configuration from NVRAM to RAM
*/
extern int InitLoginAuditConfig(int BMCInst);


#endif /* PMCONFIG_H */

