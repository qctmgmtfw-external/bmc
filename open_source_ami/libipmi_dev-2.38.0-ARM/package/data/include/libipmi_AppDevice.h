/**
 * @file   libipmi_AppDevice.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains exported AppDevice API by LIBIPMI for
 *			communicating with the BMC.
 *
 */

#ifndef __LIBIPMI_APPDEVICE_H__
#define __LIBIPMI_APPDEVICE_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

/* command specific header files */
#include "IPMI_AppDevice.h"
#include "IPMI_AppDevice+.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define TWENTY_BYTE_PWD 0x80
#define MAX_CHANNEL_NUM 15
/*---------- IPMI Command direct routines ----------*/
/* User Management functions */
uint16 IPMICMD_GetUserAccess(IPMI20_SESSION_T* pSession,
								 GetUserAccessReq_T* pReqGetUserAccess,
								 GetUserAccessRes_T* pResGetUserAccess,
								 int timeout);

uint16	IPMICMD_GetUser(IPMI20_SESSION_T *pSession/*in*/, GetUserNameReq_T	*pReqGetUser/*in*/,
							GetUserNameRes_T *pResGetUser/*out*/, int timeout/*in*/);
uint16	IPMICMD_SetUser(IPMI20_SESSION_T *pSession/*in*/, SetUserNameReq_T	*pReqSetUser/*in*/,
							SetUserNameRes_T	*pResSetUser/*out*/, int timeout/*in*/);
uint16	IPMICMD_GetSessionInfo(IPMI20_SESSION_T *pSession/*in*/, GetSesInfoReq_T	*pReqSetUser/*in*/,
							GetSesInfoRes_T	*pResSetUser/*out*/, int timeout/*in*/);
uint16 IPMICMD_GetUserAccessPayload(IPMI20_SESSION_T* pSession,
							 GetUsrPayldAccReq_T* pReqGetUserAccessPayload,
							 GetUsrPayldAccRes_T* pResGetUserAccessPayload,
							 int timeout);
uint16 IPMICMD_GetWDT(IPMI20_SESSION_T* pSession,
							 GetWDTRes_T* pResGetWDT,
							 int timeout);

uint16 IPMICMD_GetChannelInfo(IPMI20_SESSION_T* pSession,
							 GetChInfoReq_T* pReqGetChInfo,
							 GetChInfoRes_T* pResGetChInfo,
							 int timeout);

LIBIPMI_API uint16	IPMICMD_GetSystemGUID( IPMI20_SESSION_T *pSession,
                                            GetSysGUIDRes_T *pGetSystemGUID,
                                            int timeout );
LIBIPMI_API uint16 IPMICMD_SendMessage(IPMI20_SESSION_T *pSession,uint8 *pSendMsgReq,
                                                          uint8 *pSendMsgRes,uint32 SendMsgReqLen,int timeout);

/*---------- LIBIPMI Higher level routines -----------*/

/* User Management functions */
LIBIPMI_API uint16 LIBIPMI_HL_SetUser( IPMI20_SESSION_T *pSession/*in*/, char *pszUsername/*in*/,
										uint8 byID/*in*/, int timeout/*in*/);
LIBIPMI_API uint16 LIBIPMI_HL_GetUser( IPMI20_SESSION_T *pSession/*in*/, char *pszUsername/*out*/,
										uint8 byID/*in*/, int timeout/*in*/);

LIBIPMI_API uint16 LIBIPMI_HL_AddUser(IPMI20_SESSION_T *pSession/*in*/,INT8U UserID/*in*/,
                                    char* UserName/*in*/,   char* ClearPswd/*in*/,
                                    INT8U NetworkPriv/*in*/,INT8U SerialPriv/*in*/,
                                    int timeout);
uint16 LIBIPMI_HL_ModUser(IPMI20_SESSION_T *pSession,
					INT8U UserID,INT8U IsPwdChange,
					char* UserName,char* ClearPswd,
					INT8U NetworkPriv,INT8U SerialPriv,INT8U EnableUser,
					INT8U ModifyUserName, INT8U EnableSOL,
					int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_DelUser(IPMI20_SESSION_T *pSession/*in*/,INT8U UserID/*in*/,
                                    int timeout);
LIBIPMI_API uint16 LIBIPMI_HL_GetMaxNumUsers(IPMI20_SESSION_T* pSession/*in*/,
                                    INT8U *nNumUsers/*in*/, int timeout);
uint16 LIBIPMI_HL_GetAllUserInfo(IPMI20_SESSION_T *pSession,
							INT8U* nNumUsers,
							GetUserNameRes_T* pResGetUserName,
							GetUserAccessRes_T* pResGetUserAccess_Network,
							GetUserAccessRes_T* pResGetUserAccess_Serial,
                            GetUsrPayldAccRes_T* pResGetUserAccessPayload_SOL,
							int timeout);
uint16 LIBIPMI_HL_GetOneUserInfo(IPMI20_SESSION_T *pSession,
                            int UserID,
                            GetUserNameRes_T* pResGetUserName,
							GetUserAccessRes_T* pResGetUserAccess_Network,
							GetUserAccessRes_T* pResGetUserAccess_Serial,
                            GetUsrPayldAccRes_T* pResGetUserAccessPayload_SOL,
							int timeout);

uint16 LIBIPMI_HL_GetSessionInfoCurrent( IPMI20_SESSION_T *pSession, GetSesInfoRes_T* SesInfoRes, int timeout );
uint16 LIBIPMI_HL_GetChannelAccess(IPMI20_SESSION_T* pSession,INT8U ChannelMedium,GetChAccessRes_T* pChAccessRes, int timeout);
uint16 LIBIPMI_HL_SetChannelAccessPrivilege(IPMI20_SESSION_T* pSession,
                                   INT8U ChannelMedium,INT8U Privilege,
                                      SetChAccessRes_T* pChAccessRes, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_GetSystemGUID( IPMI20_SESSION_T *pSession, char *pszSystemGUID, int timeout );


#ifdef  __cplusplus
}
#endif

#endif


