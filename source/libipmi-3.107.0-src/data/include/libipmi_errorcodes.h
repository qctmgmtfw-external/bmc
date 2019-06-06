/**
 * @file   libipmi_errorcodes.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   02-Sep-2004
 *
 * @brief  Contains the error codes returned by LIBIPMI API
 *		
 *			
 */

#ifndef __LIBIPMI_ERROR_CODES_H__
#define __LIBIPMI_ERROR_CODES_H__

/* Error code retrieval macros */
#define MEDIUM_ERROR_FLAG					0
#define	IPMI_ERROR_FLAG						1
#define	RMCP_RAKP_ERROR_FLAG				2

/**
	@def STATUS_CODE(x,y)
	@brief forms a 2 byte error code. Here x specifies error type and y specifies
		actual error code.
*/
#define STATUS_CODE(x,y)				((uint16)( (((uint16)(x))<<8) | ((uint16)(y)) ))

/**
	@def IS_MEDIUM_ERROR(x)
	@brief checks if error type is MEDIUM_ERROR_FLAG
*/
#define IS_MEDIUM_ERROR(x)				( ((x)>>8) ==  MEDIUM_ERROR_FLAG )

/**
	@def IS_IPMI_ERROR(x)
	@brief checks if error type is IPMI_ERROR_FLAG
*/
#define IS_IPMI_ERROR(x)				( ((x)>>8) ==  IPMI_ERROR_FLAG )

/**
	@def IS_RMCP_RAKP_ERROR(x)
	@brief checks if error type is IPMI_ERROR_FLAG
*/
#define IS_RMCP_RAKP_ERROR(x)				( ((x)>>8) ==  RMCP_RAKP_ERROR_FLAG )

/**
	@def GET_ERROR_CODE(x)
	@brief returns the actual 1 byte error code.
*/
#define	GET_ERROR_CODE(x)					( ((uint8)x & 0xff) )
/**************************/

/* Error Codes */
#define LIBIPMI_E_SUCCESS										0x0000

#define LIBIPMI_STATUS_SUCCESS									0x00

/* Medium related errors */
#define LIBIPMI_MEDIUM_E_CONNECT_FAILURE						0x01
#define LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE						0x02
#define LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE						0x03
#define LIBIPMI_MEDIUM_E_WSA_INIT_FAILURE						0x04
#define LIBIPMI_MEDIUM_E_INVALID_SOCKET							0x05
#define LIBIPMI_MEDIUM_E_TIMED_OUT								0x06
#define LIBIPMI_MEDIUM_E_UNSUPPORTED							0x07
#define LIBIPMI_MEDIUM_E_OS_UNSUPPORTED							0x08
#define LIBIPMI_MEDIUM_E_INVALID_PARAMS							0x09
#define LIBIPMI_MEDIUM_E_INVALID_DATA							0x0A
#define LIBIPMI_MEDIUM_E_TIMED_OUT_ON_SEND						0x0B
#define LIBIPMI_MEDIUM_E_RESOLVED_ADDR_FAILURE                              0x0C

/* Session related errors */
#define LIBIPMI_SESSION_E_EXPIRED								0x10
#define LIBIPMI_SESSION_E_RECONNECT_FAILURE						0x11
#define LIBIPMI_SESSION_E_HANDSHAKE_NOT_RECVD					0x12

/* RMCP reated errors*/
#define LIBIPMI_RMCP_E_INVALID_PACKET							0x20
#define LIBIPMI_RMCP_E_INVALID_PONG								0x21

#define LIBIPMI_BMC_E_IPMI2_NOT_SUPPORTED						0x30

/* AES Encryption Errors */
#define LIBIPMI_AES_CBC_E_NO_ENOUGH_MEMORY						0x40
#define LIBIPMI_ENCRYPTION_UNSUPPORTED							0x41
#define LIBIPMI_AES_CBC_E_FAILURE  					            0x42

/* Validation Errors */
#define	LIBIPMI_E_INVALID_AUTHTYPE								0x50
#define	LIBIPMI_E_INVALID_SESSIONID								0x51
#define LIBIPMI_E_PADBYTES_MISMATCH								0x52
#define LIBIPMI_E_AUTHCODE_MISMATCH								0x53
#define LIBIPMI_E_CHKSUM_MISMATCH								0x54
#define	LIBIPMI_E_AUTHTYPE_NOT_SUPPORTED						0x55

/* Session Establishment Errors */
#define	LIBIPMI_E_INVALID_OPEN_SESSION_RESPONSE					0x60
#define	LIBIPMI_E_INVALID_RAKP_MESSAGE_2						0x61
#define LIBIPMI_E_AUTH_ALG_UNSUPPORTED							0x62
#define LIBIPMI_E_INTEGRITY_ALG_UNSUPPORTED						0x63
#define LIBIPMI_E_CONFIDENTIALITY_ALG_UNSUPPORTED				0x64
#define LIBIPMI_E_AUTH_CODE_INVALID								0x65
#define LIBIPMI_E_INVALID_RAKP_MESSAGE_4						0x66
#define LIBIPMI_E_INVALID_HMAC_SIK      						0x67

/* Highlevel function errors */
#define LIBIPMI_E_INVALID_USER_ID								0x70
#define LIBIPMI_E_INVALID_USER_NAME								0x71
#define LIBIPMI_E_INVALID_PASSWORD								0x80

#define LIBIPMI_E_INVALID_INDEX 					0x72

/* IPMB errors */
#define LIBIPMI_E_NO_ENOUGH_MEMORY						0x80
#define LIBIPMI_E_INVALID_HOST_ADDR						0x81
#define LIBIPMI_E_I2C_WRITE_FAILURE						0x82
#define LIBIPMI_E_I2C_READ_FAILURE						0x83
#define LIBIPMI_E_I2C_BUS_SUSPEND						0x84
#define LIBIPMI_E_SEQ_NUM_MISMATCH						0x85
#define LIBIPMI_E_INSUFFICIENT_BUFFER_SIZE					0x86
#define LIBIPMI_E_IPMB_LOCK_ACCESS_FAILED					0x87
#define LIBIPMI_E_IPMB_COMM_FAILURE						0x88
#define LIBIPMI_E_IPMB_UNKNOWN_ERROR						0x89
#define LIBIPMI_E_IPMB_REQ_BUFF_TOO_BIG						0x8A
#define LIBIPMI_E_IPMB_RES_BUFF_TOO_BIG						0x8B
#define LIBIPMI_E_SEL_CLEARED                                              0x85
#define LIBIPMI_E_SEL_EMPTY                                                  0x86
#define LIBIPMI_E_SENSOR_INFO_EMPTY                                 0x87
#define LIBIPMI_E_RAID_EVENT_EMPTY                          0x81
#define LIBIPMI_E_RAID_EVENT_INVALID_ENTRY                  0x82

/* Last RMCP+/RAKP satus code */
#define LAST_RMCP_RAKP_STATUS_CODE	(SC_NO_CIPHER_SUITE_MATCH)
/* Error Codes for FRU*/
#define FRU_INVALID_HEADER_VERSION						0x8C
#define FRU_INVALID_AREA								0x8D
#endif

