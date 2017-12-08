/**
 * @file   libipmi_serial_session.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains data structures required to connect
 *			to BMC thru Serial medium.
 *			
 */

#ifndef __LIBIPMI_SERIAL_SESSION_H__
#define __LIBIPMI_SERIAL_SESSION_H__

#include "Types.h"

#pragma pack( 1 )

/**
	@def GetSessionChallengeReq_T
	@brief Get Session Challenge Request
	@ref IPMI Spec Table 22-20
*/
typedef struct {
	uint8	AuthType;
	char	UserName[16];
}PACKED GetSessionChallengeReq_T;

/**
	@def GetSessionChallengeRes_T
	@brief Get Session Challenge Response
	@ref IPMI Spec Table 22-20
*/
typedef struct {
	uint8	CompletionCode;
	uint32	SessionID;
	uint8	Challenge[16];
}PACKED GetSessionChallengeRes_T;

/**
	@def ActivateSessionReq_T
	@brief Activate Session Request
	@ref IPMI Spec Table 22-21
*/
typedef struct {
	uint8	AuthType;
	uint8	PrivLevel;
	uint8	Challenge[16];
	uint32	OutboundSeqNum;
}PACKED ActivateSessionReq_T;

/**
	@def ActivateSessionRes_T
	@brief Activate Session Response
	@ref IPMI Spec Table 22-21
*/
typedef struct {
	uint8	CompletionCode;
	uint8	AuthType;
	uint32	SessionID;
	uint32	InboundSeqNum;
	uint8	PrivLevel;
}PACKED ActivateSessionRes_T;

#pragma pack( )


#endif




