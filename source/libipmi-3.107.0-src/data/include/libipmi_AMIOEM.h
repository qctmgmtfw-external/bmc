#ifndef LIBIPMI_AMIOEM_H_
#define LIBIPMI_AMIOEM_H_

/* LIBIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_StorDevice.h"
#include "libipmiifc.h"

/* command specific header files */
#include "IPMI_AMIDevice.h"
#include "sslcertificate.h" 
#include "IPMI_AMISSL.h"
#include "IPMI_AMIConf.h"
#include "IPMI_AMISyslogConf.h"
#include "IPMI_AMIFirmwareUpdate.h"
#include "IPMI_AMIUartLog.h"
#include "IPMI_RAIDInfo.h"
#include "SensorMonitor.h"
#include "AMIRestoreDefaults.h"
#include "AMIDevice.h"
#include "IPMI_HPM.h"
#include "IPMI_HPMCmds.h"
#include "IPMI_Inventory.h"

//#include "vmedia_cfg.h"
#include "AMIFirmwareRecovery.h"

#define RADIUS_USER_ID	40
#define AD_USER_ID  30
#define LDAP_USER_ID    20
#define AUTH_REQ_SERVICES   4
#define LOGIN_STATE_TYPE    4
#define AUTH_FLAG       2

#define WEB_AUDIT_CNTR     8
#define SEL_MUL_LOGIN_FAILED   8

/*Service Name's For Audit purpose*/
#define WEB_AUDIT_SERVICE_NAME      "HTTP"
#define SSH_AUDIT_SERVCE_NAME       "sshd"
#define TELNET_AUDIT_SERVCE_NAME "login"

/*Audit Recordtype's for ssh and telnet*/
#define SSH_LOGIN_RECTYPE              0x09
#define SSH_LOGOUT_RECTYPE           0x0a
#define SSH_AUTOLOGOUT_RECTYPE  0x0b
#define SSH_LOGFAILED_RECTYPE      0x0e

/*Audit RecordType's for web*/
#define WEB_LOGIN_RECTYPE              0x01
#define WEB_LOGOUT_RECTYPE           0x02
#define WEB_AUTOLOGOUT_RECTYPE  0x03
#define WEB_LOGINFAILED_RECTYPE  0x06
#define IPV4_VERSION_NUM    4
#define IPV6_VERSION_NUM    6
#define IPV_FAILED         -1

/*Audit RecordType's for kvm*/
#define KVM_START_RCTYPE 0x10
#define KVM_STOP_RCTYPE 0x11
#define KVM_AUTOLOGOUT_RECTYPE 0x12
#define KVM_AUTHFAILED_RCTYPE 0x13
#define SEL_LOGIN_FAILED_REC 0x14
#define SEL_PUBKEY_LOGIN_FAILED 6
#define SSH_PUBKEY_LOGIN_FAILED  0x0f

#define LIBIPMI_REQUEST_TIMEOUT  5

#define MAX_HPM_MSG_LENGTH	(1024 * 60)
#define MAX_SEL_RETRIEVAL_SIZE 1024 * 60
#define MAX_SENSOR_INFO_SIZE    1024 * 60
#define MAX_RAID_EVENT_SIZE     1024 * 60
#define MAX_ACCESS_LEVEL_USERS 5
#define MAX_BLOCK_DATA_SIZE 64
#define IP_ADDR_LEN             4

#define SYSLOG_DISABLE 0x00
#define LOCAL_SYSLOG	0x01
#define REMOTE_SYSLOG  0x02
#define SYSLOG_DEFAULT_PORT 514

#define RIS_PARAM_SET 0x01
#define RIS_PARAM_RESET 0x00

#define FILE_TRANSFER_SIZE  (50 * 1024)

#define BIOS_TABLE_MAX_READ_SIZE  (30 * 1024)

/*RAID Commands*/
#define GET_PROP       0
#define SET_PROP       1
#define START_RAID     0
#define STOP_RAID      1
#define PHY_DRIVE      0
#define VIR_DRIVE      1
#define START_LOCATE   0
#define STOP_LOCATE    1

LIBIPMI_API INT8U g_AMINetFn;

typedef struct
{
    INT16U yy;
    INT8U  mm;
    INT8U  dd;
}PACKED GetDate_T;

typedef struct
{
    INT8U hh;
    INT8U mm;
}PACKED GetTime_T;

typedef struct
{
    GetDate_T start_date; 
    GetTime_T start_time;
    GetDate_T stop_date;
    GetTime_T stop_time;
} PACKED GetTimeout_T;

typedef struct
{
    int type;
    int pos;
    int state;
    INT8U IPAddr1[IP_ADDR_LEN];
    INT8U IPAddr2[IP_ADDR_LEN];
    GetTimeout_T timeout;
} PACKED GetIPRule;

typedef struct
{
    int type;
    int pos;
    int state;
    INT8U IPv6Addr1[IP6_ADDR_LEN];
    INT8U IPv6Addr2[IP6_ADDR_LEN];
    GetTimeout_T timeout;
} PACKED GetIPv6Rule;

typedef struct
{
    int type;
    int pos;
    int state;
    INT8U protocol;
    INT16U port1;
    INT16U port2;
    GetTimeout_T timeout;
} PACKED GetPortRule;

typedef union
{
    GetIPRule IPRules;
    GetIPv6Rule IPv6Rules;
    GetPortRule PortRules;
}PACKED AllRules;

typedef struct{
    INT8U   CPUCount;
    INT8U   CPUPresenceCount;
    INT8U   DIMMCount;
    INT8U   DIMMPresenceCount;
    INT8U   PCICount;
}PACKED DevCount_T;

typedef struct
{
	  uint32  DataTransferHandle;
	  INT8U PLDMTransferFlag;
	  INT8U CommonPLDMTransferFlag;
	  INT16U TableSize;
}PACKED BIOSResInfo_T;

#ifdef __cplusplus
extern "C" {
#endif

LIBIPMI_API uint16 IPMICMD_AMIYAFUSwitchFlashDevice(IPMI20_SESSION_T *pSession,
	                    AMIYAFUSwitchFlashDeviceReq_T* pAMIYAFUSwitchFlashDeviceReq,
	                    AMIYAFUSwitchFlashDeviceRes_T* pAMIYAFUSwitchFlashDeviceRes,
	                    int timeout);

LIBIPMI_API uint16 IPMICMD_AMIMiscellaneousInfo(IPMI20_SESSION_T *pSession,
                           AMIYAFUMiscellaneousReq_T* pAMIYAFUMiscellaneousReq,
                           AMIYAFUMiscellaneousRes_T* pAMIYAFUMiscellaneousRes,
                           int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUActivateFlashDevice(IPMI20_SESSION_T *pSession,
                            AMIYAFUActivateFlashDeviceReq_T* pAMIYAFUActivateFlashDeviceReq,
                            AMIYAFUActivateFlashDeviceRes_T* pAMIYAFUActivateFlashDeviceRes,
                            int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetFwVersion( IPMI20_SESSION_T *pSession,
                       AMIGetFwVersionReq_T* pAMIGetFwVersionReq, INT8U ReqLen,
                       AMIGetFwVersionRes_T* pAMIGetFwVersionRes, int timeout );

LIBIPMI_API uint16 IPMICMD_AMIYAFURestoreFlashDevice(IPMI20_SESSION_T *pSession,
	                    AMIYAFUSwitchFlashDeviceReq_T* pAMIYAFUSwitchFlashDeviceReq,
	                    AMIYAFUSwitchFlashDeviceRes_T* pAMIYAFUSwitchFlashDeviceRes,
	                    int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUGetFlashInfo(IPMI20_SESSION_T *pSession,
	                    AMIYAFUGetFlashInfoReq_T* pAMIYAFUGetFlashInfoReq,
	                    AMIYAFUGetFlashInfoRes_T* pAMIYAFUGetFlashInfoRes,
	                    int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUGetFirmwareInfo(IPMI20_SESSION_T *pSession,
			   AMIYAFUGetFirmwareInfoReq_T* pAMIYAFUGetFirmwareInfoReq, 
			   AMIYAFUGetFirmwareInfoRes_T* pAMIYAFUGetFirmwareInfoRes,
			   int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUGetFMHInfo(IPMI20_SESSION_T *pSession,
                             AMIYAFUGetFMHInfoReq_T* pAMIYAFUGetFMHInfoReq,               
                             AMIYAFUGetFMHInfoRes_T* pAMIYAFUGetFMHInfoRes,
                             int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUGetStatus(IPMI20_SESSION_T *pSession,
	                      AMIYAFUGetStatusReq_T* pAMIYAFUGetStatusReq,
	                      AMIYAFUGetStatusRes_T* pAMIYAFUGetStatusRes,
	                      int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUActivateFlashMode(IPMI20_SESSION_T *pSession,
	                      AMIYAFUActivateFlashModeReq_T* pAMIYAFUActivateFlashReq,
	                      AMIYAFUActivateFlashModeRes_T* pAMIYAFUActivateFlashRes,
	                      int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUAllocateMemory(IPMI20_SESSION_T *pSession,
	                      AMIYAFUAllocateMemoryReq_T* pAMIYAFUAllocateMemoryReq,
	                      AMIYAFUAllocateMemoryRes_T* pAMIYAFUAllocateMemoryRes,
	                      int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUFreeMemory(IPMI20_SESSION_T *pSession,
	                      AMIYAFUFreeMemoryReq_T* pAMIYAFUFreeMemoryReq,
	                      AMIYAFUFreeMemoryRes_T* pAMIYAFUFreeMemoryRes,
	                      int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUReadFlash(IPMI20_SESSION_T *pSession,
	                      AMIYAFUReadFlashReq_T* pAMIYAFUReadFlashReq,
	                      AMIYAFUReadFlashRes_T* pAMIYAFUReadFlashRes,
	                      int timeout);
LIBIPMI_API uint16 IPMICMD_AMIYAFUWriteFlash(IPMI20_SESSION_T *pSession,
	                	 AMIYAFUWriteFlashReq_T* pAMIYAFUWriteFlashReq,
	                	 AMIYAFUWriteFlashRes_T* pAMIYAFUWriteFlashRes,
	                	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUEraseFlash(IPMI20_SESSION_T *pSession,
	                	 AMIYAFUErashFlashReq_T* pAMIYAFUEraseFlashReq,
	                	 AMIYAFUErashFlashRes_T* pAMIYAFUEraseFlashRes,
	                	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUProtectFlash(IPMI20_SESSION_T *pSession,
	               	 AMIYAFUProtectFlashReq_T* pAMIYAFUProtectFlashReq,
	               	 AMIYAFUProtectFlashRes_T* pAMIYAFUProtectFlashRes,
	               	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUEraseCopyFlash(IPMI20_SESSION_T *pSession,
	               	 AMIYAFUEraseCopyFlashReq_T* pAMIYAFUEraseCopyFlashReq,
	               	 AMIYAFUEraseCopyFlashRes_T* pAMIYAFUEraseCopyFlashRes,
	               	 int timeout);
LIBIPMI_API uint16 IPMICMD_AMIYAFUGetECFStatus(IPMI20_SESSION_T *pSession,
	               AMIYAFUGetECFStatusReq_T* pAMIYAFUGetECFStatusReq,
	               AMIYAFUGetECFStatusRes_T* pAMIYAFUGetECFStatusRes,
	               int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUVerifyFlash(IPMI20_SESSION_T *pSession,
	               	 AMIYAFUVerifyFlashReq_T* pAMIYAFUVerifyFlashReq,
	               	 AMIYAFUVerifyFlashRes_T* pAMIYAFUVerfyFlashRes,
	               	 int timeout);
LIBIPMI_API uint16 IPMICMD_AMIYAFUGetVerifyStatus(IPMI20_SESSION_T *pSession,
	               AMIYAFUGetVerifyStatusReq_T* pAMIYAFUGetVerifyStatusReq,
	               AMIYAFUGetVerifyStatusRes_T* pAMIYAFUGetVerifyStatusRes,
	               int timeout);
LIBIPMI_API uint16 IPMICMD_AMIYAFUReadMemory(IPMI20_SESSION_T *pSession,
	                	 AMIYAFUReadMemoryReq_T* pAMIYAFUReadMemoryReq,
	                	 AMIYAFUReadMemoryRes_T* pAMIYAFUReadMemoryRes,
	                	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUWriteMemory(IPMI20_SESSION_T *pSession,
	               	 AMIYAFUWriteMemoryReq_T* pAMIYAFUWriteMemoryReq,
	               	 AMIYAFUWriteMemoryRes_T* pAMIYAFUWriteMemoryRes,
	               	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUCopyMemory(IPMI20_SESSION_T *pSession,
	               	 AMIYAFUCopyMemoryReq_T* pAMIYAFUCopyMemoryReq,
		               AMIYAFUCopyMemoryRes_T* pAMIYAFUCopyMemoryRes,
		               int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUCompareMemory(IPMI20_SESSION_T *pSession,
		               AMIYAFUCompareMemoryReq_T* pAMIYAFUCompareMemoryReq,
	                	 AMIYAFUCompareMemoryRes_T* pAMIYAFUCompareMemoryRes,
	                	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUClearMemory(IPMI20_SESSION_T *pSession,
	               	 AMIYAFUClearMemoryReq_T* pAMIYAFUClearMemoryReq,
	               	 AMIYAFUClearMemoryRes_T* pAMIYAFUClearMemoryRes,
	               	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUGetBootConfig(IPMI20_SESSION_T *pSession,
	               AMIYAFUGetBootConfigReq_T* pAMIYAFUGetBootConfigReq,
	               AMIYAFUGetBootConfigRes_T* pAMIYAFUGetBootConfigRes,
	               int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUSetBootConfig(IPMI20_SESSION_T *pSession,
	               AMIYAFUSetBootConfigReq_T* pAMIYAFUSetBootConfigReq,
	               AMIYAFUSetBootConfigRes_T* pAMIYAFUSetBootConfigRes,
	               int timeout,uint32 ReqLen);

LIBIPMI_API uint16 IPMICMD_AMIYAFUGetAllBootVars(IPMI20_SESSION_T *pSession,
	               AMIYAFUGetBootVarsReq_T* pAMIYAFUGetBootVarsReq,
	               AMIYAFUGetBootVarsRes_T* pAMIYAFUGetBootVarsRes,
	               int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUDeactivateFlash(IPMI20_SESSION_T *pSession,
	               	 AMIYAFUDeactivateFlashReq_T* pAMIYAFUDeactivateFlashReq,
	               	 AMIYAFUDeactivateFlashRes_T* pAMIYAFUDeactivateFlashRes,
	               	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUResetDevice(IPMI20_SESSION_T *pSession,
	               	 AMIYAFUResetDeviceReq_T* pAMIYAFUResetDeviceReq,
	               	 AMIYAFUResetDeviceRes_T* pAMIYAFUResetDeviceRes,
	               	 int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUDualImageSupport(IPMI20_SESSION_T *pSession,
                    AMIYAFUDualImgSupReq_T* pAMIYAFUDualImgSupReq,
                    AMIYAFUDualImgSupRes_T* pAMIYAFUDualImgSupRes,
                    int timeout);

LIBIPMI_API uint16 IPMICMD_AMIDualImageSupport(IPMI20_SESSION_T *pSession,
                       AMIDualImageSupReq_T *pAMIDualImageSupReq,
                       AMIDualImageSupRes_T *pAMIDualImageSupRes,
                       INT8U ReqLen,int timeout);

LIBIPMI_API uint16 IPMICMD_AMISetUBootMemtest(IPMI20_SESSION_T *pSession,
                       AMISetUBootMemtestReq_T *pAMISetUBootMemtestReq,
                       AMISetUBootMemtestRes_T *pAMISetUBootMemtestRes,
                       INT8U ReqLen,int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetUBootMemtestStatus(IPMI20_SESSION_T *pSession,
                       AMIGetUBootMemtestStatusRes_T *pAMIGetUBootMemtestStatusRes,
                       int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUFirmwareSelectFlash(IPMI20_SESSION_T *pSession,
	                    AMIYAFUFWSelectFlashModeReq_T* pAMIYAFUFirmwareSelectFlashReq,
	                    AMIYAFUFWSelectFlashModeRes_T* pAMIYAFUFirmwareSelectFlashRes,
	                    int timeout);

LIBIPMI_API uint16 IPMICMD_AMIYAFUCompareMeVersion(IPMI20_SESSION_T *pSession,
                        AMIYAFUCompareMeVersionReq_T* pAMIYAFUCompareMeVersionReq,
                        AMIYAFUCompareMeVersionRes_T* pAMIYAFUCompareMeVersionRes,
                        int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetChNum ( IPMI20_SESSION_T *pSession,
                                                     AMIGetChNumRes_T* pAMIGetChNumRes,
                                                     int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetEthIndex( IPMI20_SESSION_T *pSession,
                                                  AMIGetEthIndexReq_T *pGetEthIndexReq,
                                                  AMIGetEthIndexRes_T *pGetEthIndexRes,
                                                    int timeout );

LIBIPMI_API uint16    IPMICMD_FRUDetails( IPMI20_SESSION_T *pSession,
                                    AMIGetFruDetailReq_T *pReqGetFruDet,
                                    AMIGetFruDetailRes_T *pResGetFruDet,
                                    int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetpreserveConfStatus(IPMI20_SESSION_T *pSession, GetPreserveConfigReq_T* pGetPreserveConfReq
                                               , GetPreserveConfigRes_T* pAMIGetPreserveConfRes, int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetAllPreserveConfStatus(IPMI20_SESSION_T *pSession,
                       GetAllPreserveConfigRes_T *pGetAllPreserveConfigRes,
                       INT8U ReqLen,int timeout);

LIBIPMI_API uint16 IPMICMD_AMISetAllPreserveConfStatus(IPMI20_SESSION_T *pSession,
                       SetAllPreserveConfigReq_T *pSetAllPreserveConfigReq,
                       SetAllPreserveConfigRes_T *pSetAllPreserveConfigRes,
                       INT8U ReqLen,int timeout);

LIBIPMI_API uint16  IPMICMD_AMIGetSOLConf(IPMI20_SESSION_T *pSession,
									AMIGetSOLConfRes_T    *pResGetSOLConf, 
									int timeout);

LIBIPMI_API uint16  IPMICMD_AMIGetLoginAuditConfig(IPMI20_SESSION_T *pSession,
		AMIGetLoginAuditCfgRes_T    *pResGetLoginAuditCfg, int timeout);
LIBIPMI_API uint16  IPMICMD_AMISetLoginAuditConfig(IPMI20_SESSION_T *pSession,
		 AMISetLoginAuditCfgReq_T* pReqSetLoginAuditCfg,
		   AMISetLoginAuditCfgRes_T *pResSetLoginAuditCfg,int timeout);
LIBIPMI_API uint16  IPMICMD_AMIGetSELPolicy(IPMI20_SESSION_T *pSession,
		AMIGetSELPolicyRes_T *pResGetSELPolicy, int timeout);
LIBIPMI_API uint16  IPMICMD_AMISetSELPolicy(IPMI20_SESSION_T *pSession,
		AMISetSELPolicyReq_T *  pReqAMISetSELPolicy,
		AMISetSELPolicyRes_T *  pResAMISetSELPolicy, int timeout);

uint16  IPMICMD_AMISetTriggerEvent(IPMI20_SESSION_T *pSession,
				AMISetTriggerEventReq_T    *pReqSetTriggerEvent,uint32  ReqDataLen,AMISetTriggerEventRes_T *pResSetTriggerEvent,
				int timeout);

LIBIPMI_API uint16  IPMICMD_AMIGetTriggerEvent(IPMI20_SESSION_T *pSession,
				AMIGetTriggerEventReq_T    *pReqGetTriggerEvent, AMIGetTriggerEventRes_T *pResGetTriggerEvent,
				int timeout);

uint16  IPMICMD_AMISetSOLTriggerEvent(IPMI20_SESSION_T *pSession,
				AMISetSOLTriggerEventReq_T    *pReqSetSOLTriggerEvent,uint32  ReqDataLen,AMISetSOLTriggerEventRes_T *pResSetSOLTriggerEvent,
				int timeout);

LIBIPMI_API uint16  IPMICMD_AMIGetSOLTriggerEvent(IPMI20_SESSION_T *pSession,
				AMIGetSOLTriggerEventReq_T    *pReqGetSOLTriggerEvent, AMIGetSOLTriggerEventRes_T *pResGetSOLTriggerEvent,
				int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGet_IsBlockAll( IPMI20_SESSION_T *pSession, INT8U *block, int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetFirewallRuleCount( IPMI20_SESSION_T *pSession, int    *TotalCount,
                                                                    int timeout );
LIBIPMI_API uint16 IPMICMD_AMISetAddFirewall_IPRule( IPMI20_SESSION_T *pSession, GetIPRule *SetRule,
                                                                    int timeout );
LIBIPMI_API uint16 IPMICMD_AMISetAddFirewall_PortRule( IPMI20_SESSION_T *pSession, GetPortRule *SetRule,
                                                                    int timeout );
LIBIPMI_API uint16 IPMICMD_AMISetDelFirewall_IPRule( IPMI20_SESSION_T *pSession, GetIPRule *SetRule,
                                                                    int timeout );
LIBIPMI_API uint16 IPMICMD_AMISetDelFirewall_PortRule( IPMI20_SESSION_T *pSession, GetPortRule *SetRule,
                                                                    int timeout );

LIBIPMI_API uint16 IPMICMD_AMISetAddFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession, GetIPv6Rule *SetRule,
                                                                    int timeout );
LIBIPMI_API uint16 IPMICMD_AMISetAddFirewall_IPv6_PortRule( IPMI20_SESSION_T *pSession, GetPortRule *SetRule,
                                                                    int timeout );
LIBIPMI_API uint16 IPMICMD_AMISetDelFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession, GetIPv6Rule *SetRule,
                                                                    int timeout );
LIBIPMI_API uint16 IPMICMD_AMISetDelFirewall_IPv6_PortRule( IPMI20_SESSION_T *pSession, GetPortRule *SetRule,
                                                                    int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGetFirewall_IPv6_RuleCount( IPMI20_SESSION_T *pSession, int *TotalCount,
                                                                    int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGetFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession,
								                        int    *pos, 
								                        AMIGetFirewallReq_T *GetFWConfReq, 
								                        AMIGetFirewallRes_T *GetFWConfRes,
                                                                                int timeout );

LIBIPMI_API int IPMICMD_AMIFLUSHALL_IPRULES(IPMI20_SESSION_T *pSession, int timeout );

LIBIPMI_API int IPMICMD_AMISET_BlockALL_IPRULES(IPMI20_SESSION_T *pSession, INT8U Block, INT8U Param,int timeout );

LIBIPMI_API int IPMICMD_AMISET_BlockALL_IPRULES_TIMEOUT(IPMI20_SESSION_T *pSession,INT8U Block,INT8U Param,GetTimeout_T *fwtimeout,int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGet_BlockAll_Timeout( IPMI20_SESSION_T *pSession,int *Rulecount, GetTimeout_T *fwtimeout,int timeout );

LIBIPMI_API uint16 IPMICMD_AMIGet_IPV6_BlockAll_Timeout( IPMI20_SESSION_T *pSession,int *Rulecount, GetTimeout_T *fwtimeout,int timeout );


LIBIPMI_API uint16 IPMICMD_AMIGetChannelType(IPMI20_SESSION_T *pSession, AMIGetChannelTypeReq_T *pGetChannelTypeReq,
                                                 AMIGetChannelTypeRes_T *pGetChannelTypeRes, int timeout);

#if LIBIPMI_IS_OS_LINUX()
LIBIPMI_API uint16 IPMICMD_AMIGetUDSInfo(IPMI20_SESSION_T *pSession,AMIGetUDSInfoReq_T *AMIGetUDSInfoReq
                ,AMIGetUDSInfoRes_T *AMIGetUDSInfoRes,int timeout);
#endif

/*---------- LIBIPMI Higher level routines -----------*/
#if 0
LIBIPMI_API uint16 LIBIPMI_HL_SetSSHKey( IPMI20_SESSION_T *pSession, INT8U UID,
                             char *file_path, int timeout );
LIBIPMI_API uint16 LIBIPMI_HL_DelSSHKey( IPMI20_SESSION_T *pSession, INT8U UID, int timeout );
#endif

LIBIPMI_API uint16 LIBIPMI_HL_AMIUpgradeBlock( IPMI20_SESSION_T *pSession, BI_t *blk, int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_AMILoginAuditConf(IPMI20_SESSION_T *pSession, INT8U *pEvtCfg, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetLogAuditConf(IPMI20_SESSION_T *pSession, INT8U*pWebLogAuditCfg,
                INT8U*pIPMILogAuditCfg , INT8U* pTelnetLogAuditCfg, INT8U* pSSHLogAuditCfg,INT8U* pKVMLogAuditCfg, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetLogAuditConf(IPMI20_SESSION_T *pSession,INT8U WebLogAuditCfg,
                INT8U IPMILogAuditCfg, INT8U pTelnetLogAuditCfg, INT8U pSSHLogAuditCfg, INT8U pKVMLogAuditCfg, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSELPolicy(IPMI20_SESSION_T *pSession,INT8U *pSELPolicy , int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSELPolicy(IPMI20_SESSION_T *pSession,INT8U SELPolicy , int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U EnableDisableFlag, void  *Data,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U *EnableDisableFlag, INT32U *Data,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSOLTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U EnableDisableFlag, void  *Data,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSOLTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U *EnableDisableFlag, INT32U *Data,int timeout);

uint16 LIBIPMI_HL_LoginLogoutAudit(IPMI20_SESSION_T *pSession, char *service, char*uname, char *ipstr, uint8 state, uint8 count);

LIBIPMI_API uint16 IPMICMD_AMIGetSNMPConf( IPMI20_SESSION_T *pSession,
               AMIGetSNMPConfReq_T *pGetSNMPConfReq,
         AMIGetSNMPConfRes_T *pGetSNMPConfRes,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMISetSNMPConf(IPMI20_SESSION_T *pSession,AMISetSNMPConfReq_T *pAMISetSNMPconfReq
                                          ,AMISetSNMPConfRes_T *pAMISetSNMPConfRes,int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetSELEntires(IPMI20_SESSION_T *pSession,AMIGetSELEntriesReq_T *pAMIGetSelEntriesReq,
                                                   AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes,int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetMediaInfo(IPMI20_SESSION_T *pSession,AMIGetMediaInfoReq_T *pAMIGetMediaInfoReq,
                                                   AMIGetMediaInfoRes_T *pAMIGetMediaInfoRes,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIMediaRedirectStartStop(IPMI20_SESSION_T *pSession,AMIMediaRedirctionStartStopReq_T *pAMIMediaRedirctionStartStopReq,
                                                  int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIMediaRedirectStartStop(IPMI20_SESSION_T *pSession, INT8U *ImageName, INT8U ImageType, INT8U ImageIndex,INT8U State,INT8U Media_Type,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetMediaRedirectedImagesCount(IPMI20_SESSION_T *pSession, INT8U *ImagesCount,INT8U Media_Type,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetMediaRedirectedImagesInfo(IPMI20_SESSION_T *pSession, RedirectedImageInfo_T* RedirectedImageInfo,INT8U ImagesCount,INT8U Media_Type,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetAllAvaliableImagesCount(IPMI20_SESSION_T *pSession, INT8U *ImagesCount,INT8U Media_Type,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetAllAvaliableImagesInfo(IPMI20_SESSION_T *pSession, LongImageInfo_T* AvaliableImageInfo,INT8U ImagesCount, INT8U Media_Type,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIClearImageInfo(IPMI20_SESSION_T *pSession, INT8U ImageType,INT8U ImageIndex,INT8U Media_Type,int timeout);

LIBIPMI_API uint16 IPMICMD_AMISetMediaInfo(IPMI20_SESSION_T *pSession,AMISetMediaInfoReq_T *pAMISetMediaInfoReq,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIDeleteImage(IPMI20_SESSION_T *pSession, INT8U Media_Type, INT8U ImageIndex,char *ImageName,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIUpdateImagelist(IPMI20_SESSION_T *pSession, INT8U Media_Type,int timeout);

uint16 IPMICMD_AMIGetExtendedPrivilege( IPMI20_SESSION_T *pSession, uint8 uid, INT32U *ExtendedPriv, int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSELEntires(IPMI20_SESSION_T *pSession,SELEventRecordWithSensorName_T *pSELEntriesBuffer,
                                                   uint32 *nNumSelEntries,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetPreserveConfStatus(IPMI20_SESSION_T *pSession, UINT8 selector, UINT8 *status, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetPreserveConfStatus(IPMI20_SESSION_T *pSession, INT8U selector, INT8U status, int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetSensorInfo(IPMI20_SESSION_T *pSession,
                                                   AMIGetSensorInfoRes_T *pAMIGetSensorInfoRes,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSensorInfo(IPMI20_SESSION_T *pSession,SenInfo_T *pSensorInfo,uint32 *nNumSensor,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_GetAllIPv6Address(IPMI20_SESSION_T *pSession, 
                                                   INT8U Channel, INT8U *address, INT8U *addrcnt, int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetIPMISessionTimeOut(IPMI20_SESSION_T *pSession, AMIGetIPMISessionTimeOutRes_T *pAMIGetIPMISessionTimeOutRes, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetIPMISessionTimeOut(IPMI20_SESSION_T *pSession, INT8U *pSessionTimeOut, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDualImage_FWBootSelector(IPMI20_SESSION_T *pSession,INT8U *FWBootSelector,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDualImage_FWUploadSelector(IPMI20_SESSION_T *pSession,INT8U *FWUploadSelector,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDualImage_RebootStatus(IPMI20_SESSION_T *pSession,INT8U *RebootStatus,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDualImage_CurActiveImg(IPMI20_SESSION_T *pSession,INT8U *ActiveImg,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDualImage_FWBootSelector(IPMI20_SESSION_T *pSession,INT8U FWBootSelector,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDualImage_FWUploadSelector(IPMI20_SESSION_T *pSession,INT8U FWUploadSelector,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetDualImage_RebootStatus(IPMI20_SESSION_T *pSession,INT8U RebootStatus,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDualImage_GetFWVersion(IPMI20_SESSION_T *pSession,INT8U Image, INT8U *MajVer, INT8U *MinVer,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDualImage_GetFullFWVersion(IPMI20_SESSION_T *pSession,INT8U Image, INT8U *MajVer, INT8U *MinVer, INT32U *AuxVer, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetUBootMemtest(IPMI20_SESSION_T *pSession,INT8U Enablememtest,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetUBootMemtestStatus(IPMI20_SESSION_T *pSession,INT8U *memtest,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIYAFUReplaceSignedImageKey(IPMI20_SESSION_T *pSession,INT8U *PubKey,INT32U Size,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIPublicKeyUpload(IPMI20_SESSION_T *pSession,INT8U FuncID,INT8U *PubKey,INT32U Size,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIVirtualDeviceGetStatus(IPMI20_SESSION_T *pSession,INT8U *Status,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIVirtualDeviceSetStatus(IPMI20_SESSION_T *pSession,INT8U Command,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetHostLockFeatureStatus(IPMI20_SESSION_T *pSession, INT8U *Status, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetHostAutoLockStatus(IPMI20_SESSION_T *pSession, INT8U *Status, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetHostLockFeatureStatus(IPMI20_SESSION_T *pSession, INT8U Command, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetHostAutoLockStatus(IPMI20_SESSION_T *pSession, INT8U Command, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetFirewall_IPRule( IPMI20_SESSION_T *pSession, int *IPCount,
								                        GetIPRule *IPRules,
                                                                                int timeout );
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetFirewall_PortRule( IPMI20_SESSION_T *pSession, int *PortCount,
								                        GetPortRule *PortRules,
                                                                                int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession, int *IPCount,
								                        GetIPv6Rule *IPRules,
                                                                                int timeout );
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetFirewall_IPv6_PortRule( IPMI20_SESSION_T *pSession, int *PortCount,
								                        GetPortRule *PortRules,
                                                                                int timeout);


LIBIPMI_API uint16 LIBIPMI_HL_AMISetFirewallCfg( IPMI20_SESSION_T *pSession, INT8U blockAll, 
	INT8U flushAll, int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetUserShelltype(IPMI20_SESSION_T *pSession,INT8U UserID,INT8U *ShellType,int timeout);

LIBIPMI_API uint16 IPMICMD_AMIAddLicenseKey(IPMI20_SESSION_T *pSession,
                        AMIAddLicenseKeyReq_T *pAMIAddLicenseKeyReq,
                        int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetLicenseValidity(IPMI20_SESSION_T *pSession,
                AMIGetLicenseValidityRes_T *pAMIGetLicenseValidityRes,
                int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetSSLCertStatus(IPMI20_SESSION_T *pSession,
                AMIGetSSLCertStatusReq_T *pAMIGetSSLCertStatusReq,
                AMIGetSSLCertStatusRes_T *pAMIGetSSLCertStatusRes,
                int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetSSLCert(IPMI20_SESSION_T *pSession,
                AMISetSSLCertReq_T *pAMISetSSLCertReqBuff,
                AMISetSSLCertRes_T *pAMISetSSLCertResBuff,
                int timeout); 
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSSLCerDetails(IPMI20_SESSION_T *pSession,
                SSL_Config_T *ssldtls,int timeout); 
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSSLCertValidate(IPMI20_SESSION_T *pSession,
                int *sslValid, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSSLCert(IPMI20_SESSION_T *pSession,
                SSL_T *sslCfg,int timeout); 
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSSLCertStatus(IPMI20_SESSION_T *pSession,
                INT8U *CertExits,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSSLCertInfo(IPMI20_SESSION_T *pSession,
                char *CerInfo,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSSLPrivateKeyInfo(IPMI20_SESSION_T *pSession,char *PrivateKeyInfo,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetRemoteKVMCfg(IPMI20_SESSION_T *pSession,
					AMIGetRemoteKVMCfgReq_T *pAMIGetRemoteKVMCfgReq,
					AMIGetRemoteKVMCfgRes_T *pAMIGetRemoteKVMCfgRes,
					int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetRemoteKVMCfg(IPMI20_SESSION_T *pSession,
					AMISetRemoteKVMCfgReq_T *pAMISetRemoteKVMCfgReq,
					AMISetRemoteKVMCfgRes_T *pAMISetRemoteKVMCfgRes,
					int timeout);
LIBIPMI_API uint16 IPMICMD_AMIActivedcloseSession(IPMI20_SESSION_T *pSession,
		AMIActivedcloseSessionReq_T *pActivedcloseSessionReq,
		AMIActivedcloseSessionRes_T *pActivedcloseSessionRes,
                int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRemoteKVM_Mouse_Mode(IPMI20_SESSION_T *pSession,INT8U *Mouse_Mode,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRemoteKVM_Keyboard_Layout(IPMI20_SESSION_T *pSession,char *kbd_layout,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRemoteKVM_Encryption(IPMI20_SESSION_T *pSession,INT8U *Secure_status,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRemoteKVM_Mouse_Mode(IPMI20_SESSION_T *pSession,INT8U Mouse_Mode,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRemoteKVM_Keyboard_Layout(IPMI20_SESSION_T *pSession,char *kbd_layout,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRemoteKVM_Encryption(IPMI20_SESSION_T *pSession,INT8U secure_status,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRemoteKVM_Retry_Count(IPMI20_SESSION_T *pSession,INT8U *retry_count,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRemoteKVM_Retry_Interval(IPMI20_SESSION_T *pSession,INT8U *retry_interval,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRemoteKVM_Retry_Count(IPMI20_SESSION_T *pSession, INT8U retryCount, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRemoteKVM_Retry_Interval(IPMI20_SESSION_T *pSession,INT8U retryTimeInterval, int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetRISConf( IPMI20_SESSION_T *pSession,
							AMIGetRISConfReq_T* pAMIGetRISConfReq,
							AMIGetRISConfRes_T* pAMIGetRISConfRes,
							int timeout );
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRISCfg(IPMI20_SESSION_T *pSession,INT8U MediaType, MediaConfig_T *rmediacfg,int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetRISConf( IPMI20_SESSION_T *pSession, AMISetRISConfReq_T* pAMISetRISConfReq, int timeout );
LIBIPMI_API uint16 IPMICMD_AMISetRISProgressBit(IPMI20_SESSION_T *pSession,INT8U Param,INT8U MediaType, INT8U progressbit,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRIS_State(IPMI20_SESSION_T *pSession, INT8U MediaType, INT8U State,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRISImageName(IPMI20_SESSION_T *pSession, INT8U MediaType, INT8U *Image_name,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRISClear(IPMI20_SESSION_T *pSession, INT8U MediaType,  int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRISCfg(IPMI20_SESSION_T *pSession, INT8U MediaType, MediaConfig_T *rmediacfg, INT8U Restart,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRISStartandStop(IPMI20_SESSION_T *pSession,INT8U imagetype, INT8U Status,int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetLogConf(IPMI20_SESSION_T *pSession, AMIGetLogConfRes_T *pAMIGetLogConfRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRemoteLogConf(IPMI20_SESSION_T *pSession,INT8U *Sysstatus, INT8U *Auditstatus, char *hostname, INT32U *portnum, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetLocalLogConf(IPMI20_SESSION_T *pSession,INT32U *File_size, INT8U *Rotate,int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetLogConf(IPMI20_SESSION_T *pSession, AMISetLogConfReq_T *pAMISetLogConfReq, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSysLogConf(IPMI20_SESSION_T *pSession, INT8U Sysenable, INT8U Auditenable, char *hostname, INT32U portnum,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetLocalLogConf(IPMI20_SESSION_T *pSession, INT32U filesize, INT8U Rotate,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIActivedcloseSession(IPMI20_SESSION_T *pSession,INT32U racsession_id,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetVmediaCfg(IPMI20_SESSION_T *pSession,
							AMIGetVmediaCfgReq_T *pAMIGetVmediaCfgReq,
							AMIGetVmediaCfgRes_T *pAMIGetVmediaCfgRes,
							int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetVmediaCfg(IPMI20_SESSION_T *pSession,VMediaCfgParam_T *pvmedia_cfg, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetVmedia_Encryption(IPMI20_SESSION_T *pSession,INT8U *Media_Encryption, int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetVmediaCfg(IPMI20_SESSION_T *pSession,
							AMISetVmediaCfgReq_T *pAMISetVmediaCfgReq,
							int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetVmediaCfg(IPMI20_SESSION_T *pSession,VMediaCfgParam_T *pvmedia_cfg, int timeout,INT8U Page);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetVmedia_Encryption(IPMI20_SESSION_T *pSession,INT8U Media_Encryption, int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetLDAPConf(IPMI20_SESSION_T *pSession, AMIGetLDAPReq_T *pAMIGetDLAPReq, AMIGetLDAPRes_T *pAMIGetDLAPRes, INT8U ReqLen, int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetLDAPConf(IPMI20_SESSION_T *pSession, AMISetLDAPReq_T *pAMISetDLAPReq, AMISetLDAPRes_T *pAMISetDLAPRes, INT8U ReqLen, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetLDAPConf(IPMI20_SESSION_T *pSession,IPMILDAPCONFIG * GetldapConfig,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetLDAPConf(IPMI20_SESSION_T *pSession,IPMILDAPCONFIG * SetldapConfig,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetLDAPPrivConf(IPMI20_SESSION_T *pSession,IPMI_LDAP_Config_T  GetldapPrivConfig [],int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetLDAPPrivConf(IPMI20_SESSION_T *pSession,IPMI_LDAP_Config_T*  SetldapPrivConfig,int UserID,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetRadius(IPMI20_SESSION_T *pSession,AMIGetRadiusReq_T *pAMIGetRadiusReq,AMIGetRadiusRes_T *pAMIGetRadiusRes,int ReqLen,int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetRadius(IPMI20_SESSION_T *pSession,AMISetRadiusReq_T *pAMISetRadiusReq,AMISetRadiusRes_T *pAMISetRadiusRes,int ReqLen,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRadiusConf(IPMI20_SESSION_T *pSession,IPMIRADIUSCONFIG *radius_config,int enable_ext_prvi,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRadiusprivConf(IPMI20_SESSION_T *pSession,IPMIRADIUSPRIVCONFIG radius_priv_config[],int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRadiusConf(IPMI20_SESSION_T *pSession,IPMIRADIUSCONFIG *radius_config,int enable_ext_prvi,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRadiusprivConf(IPMI20_SESSION_T *pSession,IPMIRADIUSPRIVCONFIG radius_priv_config[],int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetADConf(IPMI20_SESSION_T *pSession,AMIGetADReq_T *pAMIGetADReq,AMIGetADRes_T *pAMIGetADRes,int ReqLen,int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetADConf(IPMI20_SESSION_T *pSession,AMISetADReq_T *pAMISetADReq,AMISetADRes_T *pAMISetADRes,int ReqLen,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetADConf(IPMI20_SESSION_T *pSession,IPMI_AD_Config_T* ad_config,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetADConf(IPMI20_SESSION_T *pSession,IPMI_AD_Config_T* ad_config,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetADprivConf(IPMI20_SESSION_T *pSession,IPMI_SSAD_Config_T ad_priv_config[],int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetADprivConf(IPMI20_SESSION_T *pSession,IPMI_SSAD_Config_T *ad_priv_config,int UserID,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetVideoRcdConf(IPMI20_SESSION_T *pSession,
						AMIGetVideoRcdReq_T *pAMIGetVideoRcdReq,
						AMIGetVideoRcdRes_T *pAMIGetVideoRcdRes,int timeout);
LIBIPMI_API int16 IPMICMD_AMISetVideoRcdConf(IPMI20_SESSION_T *pSession,
						AMISetVideoRcdReq_T *pAMISetVideoRcdReq,
						AMISetVideoRcdRes_T *pAMISetVideoRcdRes,int ReqLen,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetVideoRcdConf(IPMI20_SESSION_T *pSession,IPMI_AutoRecordCfg_T *video_rcd_config,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetContVideoRcdConf(IPMI20_SESSION_T *pSession,AMIGetVideoRcdRes_T *GetVideoRcdConfRes,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetContVideoRcdConf(IPMI20_SESSION_T *pSession, AMISetVideoRcdReq_T *SetVideoRcdConfReq,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetVideoRcdConf(IPMI20_SESSION_T *pSession,IPMI_AutoRecordCfg_T *video_rcd_config,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetSinglePortFeatureStatus( IPMI20_SESSION_T *pSession, AMIGetRunTimeSinglePortStatusRes_T *pAMIGetRunTimeSinglePortStatusRes, int timeout );
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSinglePortFeatureStatus(IPMI20_SESSION_T *pSession,INT8U *Status,int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetSinglePortFeatureStatus( IPMI20_SESSION_T *pSession, AMISetRunTimeSinglePortStatusReq_T* pAMISetRunTimeSinglePortStatusReq, int timeout );
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSinglePortFeatureStatus(IPMI20_SESSION_T *pSession,INT8U Command,int timeout);
LIBIPMI_API uint16  IPMICMD_AMIGetFeatureStatus(IPMI20_SESSION_T *pSession, INT8U *FeatureName, INT8U *FeatureStatus,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetFeatureStatus(IPMI20_SESSION_T *pSession, INT8U *FeatureName, INT8U *FeatureStatus,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIFileDownload(IPMI20_SESSION_T *pSession,INT8U *pReq,uint32 ReqLen, INT8U *pRes, uint32 *ResLen,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIFileUpload(IPMI20_SESSION_T *pSession,INT8U *pReq,uint32 ReqLen,INT8U *TransID,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSELRecords (IPMI20_SESSION_T *pSession, SELRec_T *pSELRecData, uint32  *nNumSelEntries, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRestartWebService(IPMI20_SESSION_T *pSession,INT8U *Status, int timeout);
LIBIPMI_API uint16 IPMICMD_AMIRestartWebService(IPMI20_SESSION_T *pSession, AMIRestartWebServiceRes_T *pRestartWebServiceRes, int timeout );

LIBIPMI_API  uint16 IPMICMD_AMIFirmwareUpdate(IPMI20_SESSION_T *pSession,
                        INT8U *pReq, uint32 ReqLen, INT8U *pRes, uint32 *ResLen, int timeout );
LIBIPMI_API uint16 IPMICMD_AMIEnterFirmwareUpdateMode(IPMI20_SESSION_T *pSession,INT8U FwMode,INT8U *Handle,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIExitFwUpdateMode(IPMI20_SESSION_T *pSession,INT8U FwMode,INT8U *pHandle,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetAllImageInfo(IPMI20_SESSION_T *pSession, GetAllImageInfo_T *ImageInfo, uint32 *ResLen,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetUpdateComponent(IPMI20_SESSION_T *pSession, INT8S *ReqData, int ReqDataLen, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetComponentUpdateStatus(IPMI20_SESSION_T *pSession,GetAllUpdateStatusInfo_T *UpdateStatus, uint32 *ResLen, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetRemoteImageInfo(IPMI20_SESSION_T *pSession,FWRemoteImg_T *RemoteImgInfo, int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetComponentName(IPMI20_SESSION_T *pSession,INT8U DevID,INT8U *DeviceName,int *DevNameLen,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetRemoteImageInfo(IPMI20_SESSION_T *pSession,FWRemoteImg_T* FWRemoteImg,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIDiscardAllPendingUpdate(IPMI20_SESSION_T *pSession,INT8U *CancelStatus,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIValidateBundle(IPMI20_SESSION_T *pSession,INT8U *BundleName, INT8U *ValidateStatus, int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetExtendedLogConf(IPMI20_SESSION_T *pSession,AMIGetEXTtLogConfRes_T *pGetExtLogConfRes,INT8U ReqLen,int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetBackupFlag(IPMI20_SESSION_T *pSession, GetBackupConfigReq_T* pGetBackupFlagReq, uint32 dwReqLen, GetBackupConfigRes_T* pGetBackupFlagRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetBackupFlag(IPMI20_SESSION_T *pSession, INT8U *pCount, INT8U *pEnabledFlag, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetBackupList(IPMI20_SESSION_T *pSession, INT8U Selector, GetBackupList_T *pBackupConfig, int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetBackupFlag(IPMI20_SESSION_T *pSession, SetBackupFlagReq_T* pSetBackupFlagReq, SetBackupFlagRes_T* pSetBackupFlagRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetBackupFlag(IPMI20_SESSION_T *pSession, INT16U backupflag, int timeout);
LIBIPMI_API uint16 IPMICMD_AMIManageBMCConfig(IPMI20_SESSION_T *pSession, AMIManageBMCConfigReq_T* pAMIManageBMCConfigReq, AMIManageBMCConfigRes_T* pAMIManageBMCConfigRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIManageBMCConfig_BackupConf(IPMI20_SESSION_T *pSession, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIManageBMCConfig_RestoreConf(IPMI20_SESSION_T *pSession, int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetPendStatus(IPMI20_SESSION_T *pSession, AMIGetPendStatusReq_T* pAMIGetPendStatusReq, AMIGetPendStatusRes_T* pAMIGetPendStatusRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetPendStatus(IPMI20_SESSION_T *pSession, INT8U *pStatus, INT16U *pErrorCode, int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetNetworkShareOperation(IPMI20_SESSION_T *pSession,INT8U NetworkOperation,INT8U *Status,int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetBMCInstanceCount(IPMI20_SESSION_T *pSession, AMIGetBMCInstanceCountRes_T *pAMIGetBMCInstanceCountRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetBMCInstanceCount(IPMI20_SESSION_T *pSession,INT8U *BMCCount, int timeout);
LIBIPMI_API uint16 IPMICMD_AMIGetUSBSwitchSetting(IPMI20_SESSION_T *pSession, AMIGetUSBSwitchSettingRes_T *pAMIGetUSBSwitchSettingRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetUSBSwitchSetting(IPMI20_SESSION_T *pSession,INT8U *USBSwitchSetting, int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetUSBSwitchSetting(IPMI20_SESSION_T *pSession, AMISetUSBSwitchSettingReq_T *pAMISetUSBSwitchSettingReq, AMISetUSBSwitchSettingRes_T *pAMISetUSBSwitchSettingRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetUSBSwitchSetting(IPMI20_SESSION_T *pSession,INT8U USBSwitchSetting, int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetNetworkShareOperationStatus(IPMI20_SESSION_T *pSession,INT8U NetworkOperation,INT8U *Status,int timeout);

LIBIPMI_API uint16 IPMICMD_AMISwitchMUX(IPMI20_SESSION_T *pSession, AMISwitchMUXReq_T* pAMISwitchMUXReq, uint8* pAMISwitchMUXRes, int timeout); 

LIBIPMI_API uint16 IPMICMD_AMIGetRAIDConfig(IPMI20_SESSION_T *pSession, INT8U *pReq,uint32 ReqLen, INT8U *pRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDCtrlCount(IPMI20_SESSION_T *pSession,INT8U *CtrlCount, INT32U *CtrlIDList, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDCtrlInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, RAIDInfo_T *pRAIDInfo,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDPhyDevCount(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT8U *PhyCount, INT16U *PhyDevIDList, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDPhyDevInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT16U PhyDevID,PhyDevInfo_T *pPhyDevInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDLogDevCount(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT8U *LogCount, INT16U *LogDevIDList, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDLogDevInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U LogDevID, LogicalDevInfo_T *pLogDevInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDBBUInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, BBUInfo_T *pBBUInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDEventLogEntry(IPMI20_SESSION_T *pSession, INT32U RecID, RAIDEvtRcd_T *pEvtRcd, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetRAIDEvtReposInfo(IPMI20_SESSION_T *pSession, RAIDRepos_T *pRAIDReposInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIClearRAIDEventLog(IPMI20_SESSION_T *pSession, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetAllRAIDEventLog (IPMI20_SESSION_T *pSession, RAIDEvtRcd_T *pEvtRecData, uint32  *nNumRAIDEvtEntries, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_Make_PHYDRV_OnlineOffline(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U deviceId, INT8U cmdparam, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetController_Properties(IPMI20_SESSION_T *pSession,INT32U CtrlID,GetControllerProps_T *pRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_SetController_Properties(IPMI20_SESSION_T *pSession,INT32U CtrlID,ControllerProps_T config_data,int ConfigSize, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDStartInitVirDisk(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,INT8U Init_Type,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDCancelInitVirDisk(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDStartConsistancyCheck(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U DevID, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDCancelConsistancyCheck(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U DevID, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDPatrolRead(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT8U action, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_Get_Patrol_Read_Properties(IPMI20_SESSION_T *pSession,INT32U CtrlID, PatrolReadPropertiesRes_T *pRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_Set_Patrol_Read_Properties(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT8U mode, PatrolReadPropertiesRes_T *pRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDDedicatedHotspare(IPMI20_SESSION_T *pSession, DedicatedHotspare_T ld_config_data, int ConfigSize, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDStartPhyLocateDevice(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,INT8U duration,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDStopPhyLocateDevice(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDStartVirLocateDevice(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,INT8U duration,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIRAIDStopVirLocateDevice(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_SET_PD_State(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U DevID,INT8U state, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_SetLogicalDev_Properties(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID, LDproperties_T ld_config_data, int ConfigSize, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetLogicalDev_Properties(IPMI20_SESSION_T *pSession,INT32U ctrlID,INT16U devID,LDpropertiesRes_T *pRes, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetLogicalDev_Progress(IPMI20_SESSION_T *pSession,INT32U CtrlID,INT16U DevID,LDProgressRes_T *pRes, int timeout);


//LIBIPMI_API uint16 LIBIPMI_HL_PHYDRV_FormatCmd(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U deviceId, INT8U cmdparam, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_PHYDRV_RemovalCmd(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U deviceId, INT8U cmdparam, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_Manage_HotSpare(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U deviceId, INT8U cmdparam, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_Create_LogicalDev(IPMI20_SESSION_T *pSession, INT32U CtrlID, ManageConfig_T ld_config_data, int ConfigSize, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_Delete_LogicalDev(IPMI20_SESSION_T *pSession, INT32U CtrlID, INT16U DeviceID, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_Get_RAID_Level_Strip_Size(IPMI20_SESSION_T *pSession, INT32U CtrlID, RL_Strip_Info_T *pRLStripInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_Get_RAID_Level_PD_Count(IPMI20_SESSION_T *pSession, INT32U CtrlID, RL_PD_Count_T *pRLPDCount, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSASITCtrlCount(IPMI20_SESSION_T *pSession,INT8U *CtrlCount, INT32U *CtrlIDList, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSASITCtrlInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, HBAInfo_T *pHBAInfo,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSASITPhyDevCount(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT8U *PhyCount, INT16U *PhyDevIDList, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSASITPhyDevInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT16U PhyDevID,HBAPhyDevInfo_T *pPhyDevInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSASITElementStatus(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT16U EnclID, INT8U *pStatus, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSASITToplogy(IPMI20_SESSION_T *pSession,INT32U CtrlID, Topology_T *pTopology, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSASITTempSensorInfo(IPMI20_SESSION_T *pSession,INT32U CtrlID, INT16U EnclID, INT8U Index, TempSensorInfo_T *pTempSensorInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSASITEnclosureList(IPMI20_SESSION_T *pSession,INT32U CtrlID, EnclosureInfo_T *pEnclListInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSASITPSElementInfo(IPMI20_SESSION_T *pSession,PSConfig_T PSConfig, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSASITTempSenElementInfo(IPMI20_SESSION_T *pSession, SetTempSenConfig_T SetTempSenConfig, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSASITCoolingInfo(IPMI20_SESSION_T *pSession,CoolingConfig_T CoolingConfig, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSASITAlarmInfo(IPMI20_SESSION_T *pSession,AlarmConfig_T AlarmConfig, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetBIOSTable(IPMI20_SESSION_T *pSession, BIOSResInfo_T *biosInfo, INT8U *payLoad ,INT8U tableType , int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetFirmwareRecoveryInfo(IPMI20_SESSION_T *pSession, FIRMWARERECOVERYINFO * FirmwareRecoveryInfo, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetFirmwareRecoveryInfo(IPMI20_SESSION_T *pSession, FIRMWARERECOVERYINFO * pFirmwareRecoveryInfo,int timeout);

// Serial Log Configuarion related API */
LIBIPMI_API uint16 IPMICMD_AMIGetSerialLogConf(IPMI20_SESSION_T *pSession,AMIGetSerialLogReq_T *pAMIGetSerialLogCfgReq,AMIGetSerialLogRes_T *pAMIGetSerailLogCfgRes,int ReqLen,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSerialLogCfg(IPMI20_SESSION_T *pSession,IPMI_SerialLogCfg_T* SerialLog_config,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSerialLogRemoteCfg(IPMI20_SESSION_T *pSession,IPMI_SerialLogCfg_T* SerialLog_config,int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetSerialLogCfg(IPMI20_SESSION_T *pSession,AMISetSerialLogReq_T *pAMISetSerialLogCfgReq,AMISetSerialLogRes_T *pAMISetSerialLogCfgRes,int ReqLen,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSerialLogCfg(IPMI20_SESSION_T *pSession,IPMI_SerialLogCfg_T* SerialLog_config,int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSerialLogRemoteCfg(IPMI20_SESSION_T *pSession,IPMI_SerialLogCfg_T* SerialLog_config,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_GetSysInventory(IPMI20_SESSION_T *pSession, GetInventoryReq_T *pGetSysInvenotryReq, GetInventoryRes_T *pGetSysInvenotryRes,int timeout);

uint16 LIBIPMI_HL_AMIGetFirewall_IPv6_Rule( IPMI20_SESSION_T *pSession, int *IPCount,
                                                            GetIPv6Rule *IPv6Rules, int timeout );
uint16 LIBIPMI_HL_AMIGetFirewall_IPv6_PortRule( IPMI20_SESSION_T *pSession, int *PortCount,
                                                            GetPortRule *PortRules, int timeout );

// HPM Commands API
uint16 LIBIPMI_HL_GetTargetUpgCap(IPMI20_SESSION_T *pSession,GetTargetUpgradeCapablitiesRes_T *TargetUpgCapRes, int timeout);
uint16 LIBIPMI_HL_GetCompProperties_GeneralProp(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout);
uint16 LIBIPMI_HL_GetCompProperties_DescString(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout);
uint16 LIBIPMI_HL_GetCompProperties_DeferredVersion(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout);
uint16 LIBIPMI_HL_GetCompProperties_CurrVersion(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout);
uint16 LIBIPMI_HL_GetCompProperties_RollBackVersion(IPMI20_SESSION_T *pSession, INT8U CompID, INT8U *Res, int timeout);
uint16 LIBIPMI_HL_InitiateUpgAction(IPMI20_SESSION_T *pSession,INT8U Components, INT8U UpgAction , InitiateUpgActionRes_T *InitialUpgActRes, int timeout);
uint16 LIBIPMI_HL_UploadFWBlock(IPMI20_SESSION_T *pSession,INT8U BlockNumber, INT8U *FirmwareData , INT32U DataLen, UploadFirmwareBlkRes_T *UploadFWBlkRes, int timeout);
uint16 LIBIPMI_HL_FinishFWUpload(IPMI20_SESSION_T *pSession,INT8U Comp, INT32U ImgSize , FinishFWUploadRes_T *FinishUploadRes, int timeout);
uint16 LIBIPMI_HL_AbortFWUpgrade(IPMI20_SESSION_T *pSession, AbortFirmwareUpgradeRes_T *AbortFWUpgRes, int timeout);
uint16 LIBIPMI_HL_GetUpgStatus(IPMI20_SESSION_T *pSession, GetUpgradeStatusRes_T *GetUpgStatusRes, int timeout);
uint16 LIBIPMI_HL_ActivateFWUpgrade(IPMI20_SESSION_T *pSession, ActivateFWRes_T *ActFWUpgRes, int timeout);
uint16 LIBIPMI_HL_QueryRollBackStatus(IPMI20_SESSION_T *pSession, QueryRollbackStatusRes_T *RollBckRes, int timeout);
uint16 LIBIPMI_HL_InitiateManRollBack(IPMI20_SESSION_T *pSession, InitiateManualRollbackRes_T *InitateManRollBckRes, int timeout);
uint16 LIBIPMI_HL_QuerySelfTestResults(IPMI20_SESSION_T *pSession, QuerySelfTestResultsRes_T *QuerySelfTestRes, int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetFWCfg(IPMI20_SESSION_T *pSession,
					   AMIGetFWCfgReq_T* pAMIGetFWCfgReq,
					   AMIGetFWCfgRes_T* pAMIGetFWCfgRes,
                       int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetFWHostCfg(IPMI20_SESSION_T *pSession,
		               AMISetFWCfgReq_T* pAMISetFWCfgReq,
		               AMISetFWCfgRes_T* pAMISetFWCfgRes,
                       int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetFWFilePathCfg(IPMI20_SESSION_T *pSession,
		               AMISetFWCfgReq_T* pAMISetFWCfgReq,
		               AMISetFWCfgRes_T* pAMISetFWCfgRes,
                       int timeout);
LIBIPMI_API uint16 IPMICMD_AMISetFWProtocol(IPMI20_SESSION_T *pSession,
                       AMISetFWProtocolReq_T* pAMISetFWProtocolReq,
                       AMISetFWProtocolRes_T* pAMISetFWProtocolRes,
                       int timeout);


#if LIBIPMI_IS_OS_LINUX()
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetUDSInfo(INT8U *IPAddr/*IN*/,INT8U *ChannelType/*OUT*/,INT8U *ChannelNum /*OUT*/,INT8U *BMCInst/*OUT*/,int timeout/*IN*/);
#endif
#ifdef __cplusplus
}
#endif

#endif
