#ifndef LIBIPMI_AMIOEM_H_
#define LIBIPMI_AMIOEM_H_

/* LIBIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_StorDevice.h"
#include "libipmiifc.h"

/* command specific header files */
#include "IPMI_AMIDevice.h"
#include "IPMI_AMIConf.h"
#include "SensorMonitor.h"

#define MAX_SEL_RETRIEVAL_SIZE 1024 * 60
#define MAX_SENSOR_INFO_SIZE    1024 * 60

#ifdef __cplusplus
extern "C" {
#endif

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

LIBIPMI_API uint16 IPMICMD_AMIGetChNum ( IPMI20_SESSION_T *pSession,
                                                     AMIGetChNumReq_T *pAMIGetChNumReq,
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

/*---------- LIBIPMI Higher level routines -----------*/
#if 0
LIBIPMI_API uint16 LIBIPMI_HL_SetSSHKey( IPMI20_SESSION_T *pSession, INT8U UID,
                             char *file_path, int timeout );
LIBIPMI_API uint16 LIBIPMI_HL_DelSSHKey( IPMI20_SESSION_T *pSession, INT8U UID, int timeout );
#endif

LIBIPMI_API uint16 LIBIPMI_HL_AMIUpgradeBlock( IPMI20_SESSION_T *pSession, BI_t *blk, int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_AMILoginAuditConf(IPMI20_SESSION_T *pSession, INT8U *pEvtCfg, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMIGetLogAuditConf(IPMI20_SESSION_T *pSession, 
                INT8U*pWebLogAuditCfg ,INT8U*pIPMILogAuditCfg , INT8U* pTelnetLogAuditCfg, INT8U* pSSHLogAuditCfg, int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetLogAuditConf(IPMI20_SESSION_T *pSession,
                INT8U WebLogAuditCfg, INT8U IPMILogAuditCfg, INT8U pTelnetLogAuditCfg, INT8U pSSHLogAuditCfg, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSELPolicy(IPMI20_SESSION_T *pSession,INT8U *pSELPolicy , int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_AMISetSELPolicy(IPMI20_SESSION_T *pSession,INT8U SELPolicy , int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U EnableDisableFlag, void  *Data,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetTriggerEvent(IPMI20_SESSION_T *pSession, INT8U TriggerParam, INT8U *EnableDisableFlag, INT32U *Data,int timeout);

#ifdef CONFIG_SPX_FEATURE_SNMP_SUPPORT

LIBIPMI_API uint16 IPMICMD_AMIGetSNMPConf( IPMI20_SESSION_T *pSession,
               AMIGetSNMPConfReq_T *pGetSNMPConfReq,
         AMIGetSNMPConfRes_T *pGetSNMPConfRes,
                             int timeout );

LIBIPMI_API uint16 IPMICMD_AMISetSNMPConf(IPMI20_SESSION_T *pSession,AMISetSNMPConfReq_T *pAMISetSNMPconfReq
                                          ,AMISetSNMPConfRes_T *pAMISetSNMPConfRes,int timeout);

#endif//SNMP_SUPPORT

LIBIPMI_API uint16 IPMICMD_AMIGetSELEntires(IPMI20_SESSION_T *pSession,AMIGetSELEntriesReq_T *pAMIGetSelEntriesReq,
                                                   AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSELEntires(IPMI20_SESSION_T *pSession,SELEventRecordWithSensorName_T *pSELEntriesBuffer,
                                                   uint32 *nNumSelEntries,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMISetPreserveConfStatus(IPMI20_SESSION_T *pSession, INT8U selector, INT8U status, int timeout);

LIBIPMI_API uint16 IPMICMD_AMIGetSensorInfo(IPMI20_SESSION_T *pSession,
                                                   AMIGetSensorInfoRes_T *pAMIGetSensorInfoRes,int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSensorInfo(IPMI20_SESSION_T *pSession,SenInfo_T *pSensorInfo,uint32 *nNumSensor,int timeout);

#ifdef __cplusplus
}
#endif

#endif
