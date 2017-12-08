/**
 * @file   libipmi_session.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains function definitions & data structures
 *			required for establishing a session
 *			
 */

#ifndef __LIBIPMI_SESSION_H__
#define __LIBIPMI_SESSION_H__

#include <stdio.h>
#include "libipmi_struct.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"
#include "libipmi_version.h"

#define LIBIPMI_DEBUG	0

#if LIBIPMI_IS_OS_WINDOWS()
	#if LIBIPMI_DEBUG  
		#define		DEBUG_PRINTF	printf
	#else
		#define		DEBUG_PRINTF	
	#endif
#elif LIBIPMI_IS_OS_LINUX()
	#if LIBIPMI_DEBUG  
		#define		DEBUG_PRINTF	printf
	#else
		#define		DEBUG_PRINTF(fmt,args...)	
	#endif		
#endif

#if LIBIPMI_IS_PLATFORM_X86() || LIBIPMI_IS_PLATFORM_ARM() || LIBIPMI_IS_PLATFORM_SH()
#define TO_LITTLE_SHORT(x) (x)
#define TO_LITTLE_LONG(x) (x)
#elif LIBIPMI_IS_PLATFORM_PPC() || LIBIPMI_IS_PLATFORM_MIPS()
#define TO_LITTLE_SHORT(x) (uint16)((((uint16)(x) >> 8) | ((uint16)(x) << 8)))
#define TO_LITTLE_LONG(x) ( ((x) >> 24) | ((x) << 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8))
#endif

#if LIBIPMI_IS_OS_LINUX()
#define PACK __attribute__ ((packed))
#else
#define PACK
#pragma pack(1)
#endif/* LIBIPMI_IS_OS_LINUX() */

#include "IPMI_RMCP.h"
#include "IPMIDefs.h"
#include "IPMI_App.h"
#include "IPMI_AppDevice.h"
#include "IPMI_RMCP+.h"
#include "IPMI_AMI.h"
#include "aes.h"
#include "hmac.h"
#include "sha1.h"

/* Undefine PACK so that it can be redefined in other header files */
#if defined (PACK)
#undef PACK
#endif

#if !LIBIPMI_IS_OS_LINUX()
#pragma pack()
#endif /* LIBIPMI_IS_OS_LINUX() */


#ifdef  __cplusplus
extern "C" {
#endif

/**
\breif	Establishes a session with the BMC thru any medium
 @param	pSession			[in]Session Info handle
 @param	pszDevice			[in]Name of the device. 
								If byMedium is NETWORK_MEDIUM, pszDevice denotes IPaddress
								of BMC.
								If byMedium is SERIAL_MEDIUM, pszDevice denotes 
								comm port identifier eg:"/COM1" or "/dev/ttyS0"
								If byMedium is KCS_MEDIUM, pszDevice denotes
								kc port driver identifier
 @param	wPort				[in]socket port to connect in case of NETWORK_MEDIUM.
								It is meaning less in case of serial and kcs mediums
 @param pszUserName			[in] Username required for authencation with BMC
 @param pszPassword			[in] Password required for authencation with BMC
 @param byEncryption		[in] 1 if Encryption required and 0 if not
 @param byIntegrity			[in] 1 if Integrity required and 0 if not
 @param	byPrivLevel			[in] Role of the session,
 @param	byMedium			[in] medium to be used for establish a session
 @param	byReqAddr			[in] Requester Address (Client address)
 @param	byResAddr			[in] Responder Address (BMC)
 @param	byKG				[in] KG value required for hmac calculation in NETWORK_MEDIUM
								 should be null in serial and kcs mediums
 @param	wKGLen				[in] size of byKG data supplied.
								 should be 0 in serial and kcs mediums
 @param	timeout				[in]timeout value in seconds.

 @retval Returns LIBIPMI_E_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_Create_IPMI20_Session(IPMI20_SESSION_T *pSession, char *pszDevice, uint16 wPort,
							char *pszUserName, char *pszPassword,
							uint8 byAuthType,
							uint8 byEncryption, uint8 byIntegrity, uint8 *byPrivLevel,
							uint8 byMedium,
							uint8 byReqAddr, uint8 byResAddr,
							uint8 *pbyKG, uint16 wKGLen,
							int timeout);

LIBIPMI_API uint16 LIBIPMI_Create_IPMI_Local_Session(IPMI20_SESSION_T *pSession,
							char *pszUserName, char *pszPassword,
							uint8 *byPrivLevel,INT8U AuthFlag,int timeout);

LIBIPMI_API uint16 LIBIPMI_Create_IPMI_Local_Session_Ext(IPMI20_SESSION_T *pSession, char *pszUserName,
                                                   char *pszPassword, uint8 *byPrivLevel, char *xtUserName, 
                                                   INT8U *ipaddr, INT8U IPVer, INT8U AuthFlag,int timeout);

/**
This function is used to get the UserID for given Username and Password.
It compares all IPMI username with the IN argument Username.
If it matches, then it checks for the password.
If password also matches, it will return SUCCESS and corresponding User ID.
Since duplicate username is supported in IPMI specification,
It will executes unless it gets SUCCESS or reaches MAX_USERS_COUNT.
Username and Password are IN arguments, UserID is OUT argument.
**/
LIBIPMI_API int LIBIPMI_GetUserIDForUserName(IPMI20_SESSION_T *pSession, 
							char *UserName, char *Password, 
							INT8U *UserID, INT8U AuthEnable,int timeout);


/**
\breif	Sends RAW IPMI Command to the BMC
 @param	pSession			[in]Session Info handle
 @param	byPayLoadType		[in]Pay Load Type in NETWORK_MEDIUM.
								shd be 0 in case of serial and kcs mediums.
 @param	byNetFnLUN			[in] Net Function and LUN for the given byCommand
 @param byCommand			[in] Command number.
 @param pszReqData			[in] Command Request Data
 @param dwReqDataLen		[in] size of Command Request data.
 @param pszResData			[in] Command Response Data
 @param	pdwResDataLen		[in] size of Command Response data.
 @param	timeout				[in]timeout value in seconds.

 @retval Returns LIBIPMI_E_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_Send_RAW_IPMI2_0_Command(IPMI20_SESSION_T *pSession,
									uint8 byPayLoadType,
									uint8 byNetFnLUN, uint8 byCommand,
									uint8 *pszReqData, uint32 dwReqDataLen,
									uint8 *pszResData, uint32 *pdwResDataLen,
									int	timeout);


/**
\breif	Closes the IPMI 2.0 Session
 @param	pSession			[in]Session Info handle

 @retval Returns LIBIPMI_E_SUCCESS on success and error codes on failure
*/
LIBIPMI_API uint16 LIBIPMI_CloseSession( IPMI20_SESSION_T *pSession );

/**
\breif	returns the medium used  by the client
 @param	pSession			[in]Session Info handle
 
 @retval returns NETWORK_MEDIUM, SERIAL_MEDIUM, KCS_MEDIUM depending
		 on the medium used.
*/
LIBIPMI_API uint8	MediumUsed(IPMI20_SESSION_T *pSession);

/**
\breif	returns status whether the session is started or not
 @param	pSession			[in]Session Info handle
 
 @retval 1 if session is started and 0 if not
*/
LIBIPMI_API uint8	IsSessionStarted(IPMI20_SESSION_T *pSession);

/**
\breif	sets the default session settings
 @param	pSession			[in]Session Info handle
*/
LIBIPMI_API void LIBIPMI_SetDefaultSerialPortSettings(IPMI20_SESSION_T *pSession);

/**
\breif	returns the settings for a session
 @param	pSession			[in]Session Info handle
*/
LIBIPMI_API void LIBIPMI_GetSerialPortSettings(IPMI20_SESSION_T *pSession, LIBIPMI_SERIAL_SETTINGS_T *);

/**
\breif	sets the settings supplied for the session.
 @param	pSession			[in]Session Info handle
*/
LIBIPMI_API void LIBIPMI_SetSerialPortSettings(IPMI20_SESSION_T *pSession, LIBIPMI_SERIAL_SETTINGS_T *);


/**
\brief  Sets the Cmd Retry Count and Interval
 @param pSession                        [in]Session Info handle
 @retval Returns LIBIPMI_E_SUCCESS on success and error codes on failure
 @note This function is not implemented 
*/

LIBIPMI_API uint16 LIBIPMI_SetRetryCfg( IPMI20_SESSION_T *pSession, 
										int nCmdRetryCount, 
										int nCmdRetryInterval);

#ifdef  __cplusplus
}
#endif


#endif /* __LIBIPMI_SESSION_H__ */
