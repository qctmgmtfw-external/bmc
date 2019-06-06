#ifndef __LIBIPMI_PEF_H__
#define __LIBIPMI_PEF_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

/* command specific header files */
#pragma pack(1)
#include "IPMI_PEF.h"
#pragma pack()

#include "IPMI_SensorEvent.h"


#define SET_IN_PROGRESS_PARAM 0x00
#define SET_IN_PROGRESS_MASK   0x03
#define SET_COMPLETE			0x00
#define SET_IN_PROGRESS 		0x01
#define COMMIT_WRITE 			0x02


#ifdef  __cplusplus
extern "C" {
#endif

/* Get PEF Capabilities Command */
LIBIPMI_API uint16	IPMICMD_GetPEFCapabilities( IPMI20_SESSION_T *pSession,
							GetPEFCapRes_T *pResGetPEFCap,
							int timeout);

/* Arm PEF Postpone Timer Command */
LIBIPMI_API uint16	IPMICMD_ArmPEFTimer( IPMI20_SESSION_T *pSession/*in*/,
							uint8*	pReqTmrTimeout/*in*/,
							ArmPEFTmrRes_T *pResArmPEFTmr/*out*/,
							int timeout/*in*/);

/* Set PEF Configuration Parameters Command */
LIBIPMI_API uint16	IPMICMD_SetPEFConf( IPMI20_SESSION_T *pSession/*in*/,
							uint8 *pReqSetPEFConf/*in*/,
							uint32 dwReqBufLen,
							uint8 *pResSetPEFConf/*out*/,
							int timeout/*in*/);

/* Get PEF Configuration Parameters Command */
LIBIPMI_API uint16	IPMICMD_GetPEFConf( IPMI20_SESSION_T *pSession/*in*/,
							GetPEFConfigReq_T *pReqGetPEFConf/*in*/,
							uint8 *pResGetPEFConf/*out*/,
							int timeout/*in*/);

/* Set Last Processed Event ID Command */
LIBIPMI_API uint16	IPMICMD_SetLastEventID( IPMI20_SESSION_T *pSession/*in*/,
							SetLastEvtIDReq_T *pReqSetLastEvt/*in*/,
							uint8 *pResSetLastEvt/*out*/,
							int timeout/*in*/);

/* Get Last Processed Event ID Command */
LIBIPMI_API uint16	IPMICMD_GetLastEventID( IPMI20_SESSION_T *pSession/*in*/,
							GetLastEvtIDRes_T *pResGetLastEvt/*out*/,
							int timeout/*in*/);

/* Alert Immediate Command */
LIBIPMI_API uint16	IPMICMD_AlertImmediate( IPMI20_SESSION_T *pSession/*in*/,
							AlertImmReq_T *pReqAlertImmediate/*in*/,
							AlertImmRes_T *pResAlertImmediate/*out*/,
							int timeout/*in*/);

/* PET Acknowledge Command */
LIBIPMI_API uint16	IPMICMD_PETAck( IPMI20_SESSION_T *pSession/*in*/,
							PETAckReq_T *pReqPETAck /*out*/,
							uint8 *pResPETACK /*out*/,
							int timeout/*in*/);

/*---------- PEF Higher level routines -----------*/
/*********** Alert Table **********/
LIBIPMI_API uint16 LIBIPMI_HL_GetNumOfAlertPolicyEntries(IPMI20_SESSION_T *pSession,
					INT8U* pNumOfAlertPolicyEntries,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetAnAlertEntry(IPMI20_SESSION_T *pSession,
					INT8U AlertEntryNumber,
					AlertPolicyTblEntry_T* pAlertEntry,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_SetAnAlertEntry(IPMI20_SESSION_T *pSession,
					INT8U AlertEntryNumber,
					AlertPolicyTblEntry_T* pAlertEntry,
					int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_GetAlertTable(IPMI20_SESSION_T *pSession,
					AlertPolicyTblEntry_T* pAlertTable,
					INT8U* pNumOfAlertEntries,
					int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_SetGraspAlertLevel(IPMI20_SESSION_T *pSession,
					INT8U IndexNum,
					INT8U AlertLevel, 
					INT8U Channel, 
					int timeout);


LIBIPMI_API uint16 LIBIPMI_HL_GetGraspAlertLevel(IPMI20_SESSION_T *pSession,
					INT8U IndexNum,
					INT8U* AlertLevel, 
					INT8U* DestIndex,
					int timeout);
/*********** PEF Table ********/
LIBIPMI_API uint16 LIBIPMI_HL_GetPEFTable(IPMI20_SESSION_T *pSession,
					EvtFilterTblEntry_T* pPEFTable,
					INT8U* pNumOfPEFEntries,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetPEFEntry(IPMI20_SESSION_T *pSession,
					INT8U PEFEntryNumber,
					EvtFilterTblEntry_T* pPEFEntry,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_SetPEFEntry(IPMI20_SESSION_T *pSession,
					INT8U PEFEntryNumber,
					EvtFilterTblEntry_T* pPEFEntry,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetNumOfPEFEntries(IPMI20_SESSION_T *pSession,
					INT8U* pNumOfPEFEntries,
					int timeout);

#ifdef  __cplusplus
}
#endif


#endif




