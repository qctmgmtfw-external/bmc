#ifndef LIBIPMI_OPMA_H_
#define LIBIPMI_OPMA_H_

/* LIBIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"



/* command specific header files */
#include "IPMI_OPMA.h"
#include "IPMI_AppDevice.h"
#include "IPMI_AppDevice+.h"

#ifdef __cplusplus
extern "C" {
#endif

LIBIPMI_API  uint16 OPMAIPMICMD_ClearCMOS ( IPMI20_SESSION_T *pSession,  ClearCMOSRes_T *pClearCMOS, int timeout );
LIBIPMI_API  uint16 OPMAIPMICMD_SetLocalAccessLock ( IPMI20_SESSION_T *pSession,  SetLocalAccessLockOutStateReq_T  *pLockstateReq ,
                                          SetLocalAccessLockOutStateRes_T  *pLockstateRes, int timeout );
LIBIPMI_API uint16 OPMAIPMICMD_GetLocalAccessLock ( IPMI20_SESSION_T *pSession, GetLocalAccessLockOutStateRes_T  *pLockstateRes, int timeout );

LIBIPMI_API uint16 OPMAIPMICMD_SetSensorReadingOffset  ( IPMI20_SESSION_T *pSession,
                             SetSensorReadingOffsetReq_T  *pSetSensorStateReq , SetSensorReadingOffsetRes_T  *pSetSensorStateRes,
                             int timeout );
LIBIPMI_API  uint16 OPMAIPMICMD_GetSensorReadingOffset ( IPMI20_SESSION_T *pSession,
                             GetSensorReadingOffsetReq_T  *pGetSensorStateReq ,GetSensorReadingOffsetRes_T  *pGetSensorStateRes,
                             int timeout );

LIBIPMI_API uint16 OPMAIPMICMD_SetSystemTypeIdentifier  ( IPMI20_SESSION_T *pSession,
                             SetSystemTypeIdentifierReq_T  *pSetSysIdReq, SetSystemTypeIdentifierRes_T  *pSetSysIdRes,
                             int timeout );

LIBIPMI_API uint16 OPMAIPMICMD_GetSystemTypeIdentifier  ( IPMI20_SESSION_T *pSession,
                             GetSystemTypeIdentifierRes_T  *pGetSysIdRes,int timeout );

LIBIPMI_API  uint16 OPMAIPMICMD_GetmCardCapabilities  ( IPMI20_SESSION_T *pSession,
                             GetmCardCapabilitiesRes_T  *pmCardRes,int timeout );

LIBIPMI_API  uint16 OPMAIPMICMD_GetSupportedHostIDs ( IPMI20_SESSION_T *pSession,
                             GetSupportedHostIDsRes_T  *pGetSuppHostId,int timeout );

#ifdef __cplusplus
}
#endif

#endif //LIBIPMI_OPMA_H_
