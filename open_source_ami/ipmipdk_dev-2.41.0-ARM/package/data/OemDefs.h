#ifndef OEM_DEFINES_H_
#define OEM_DEFINES_H_


#include "Types.h"
#include "IPMI_LANConfig.h"
#ifdef __cplusplus
extern "C" {
#endif

#define NETFN_GENERIC_VANILLA_OEM	0x2e

/* OEM Configuration Commands */
#define CMD_RESERVED_EXT_CONFIGURATION   0x01
#define CMD_GET_EXT_CONFIGURATION        0x02
#define CMD_SET_EXT_CONFIGURATION        0x03



#define EMAIL_ADDR_SIZE   64
#define EMAIL_FORMAT_SIZE   64
#define EMAIL_MSG_SIZE    32

/* Tokens */
#define SNMP_CONFIG_TOKEN  0x05
#define EMAIL_CONFIG_TOKEN 0x08
#define MISC_CONFIG_TOKEN  0x12
#define TOKEN_VERSION      0x01


/* indexs */
#define  IDX_SNMP_AGNT_ENABLE 	0x0
#define  IDX_COMM_STR_GET	0x1
#define  IDX_COMM_STR_SET 	0x2

#define  IDX_EMAIL_ADDR_ENABLE  0x0
#define  IDX_EMAIL_ADDR 	0x1
#define  IDX_EMAIL_MSG 		0x2



/* OEM specific Completion codes */ 
#define  OEM_CC_SET_IN_PROGRESS 	 0x81
#define  OEM_CC_RESERVATION_ID_CANCELLED 0xC5
#define  OEM_CC_INV_DATA_FIELD 		 0xCC
#define  OEM_CC_REQ_INV_LEN 		 0xC7
#define  OEM_CC_PARAM_OUT_OF_RANGE 	 0xC9
#define  OEM_CC_INVALID_TOKEN_VERSION	 0xCC

#define COMMUNITY_STR_LEN 32
#define READ_ENTIRE_TOKEN 0xFF 
#define MAX_EMAIL_CONFIGS  3



/** SMtp Config Structure **/ 
typedef struct {

   INT8U SnmpAgentEnable;
   INT8U CNGetStrlen;
   INT8U CNGetStr [COMMUNITY_STR_LEN];
   INT8U CNSetStrlen;
   INT8U CNSetStr [COMMUNITY_STR_LEN];
	
} PACKED Snmp_Config_T;


/** Email Config Structure **/ 
typedef struct {

   INT8U   EmailEnable;
   INT8U   EmailAddrLength;
   INT8U   EmailAddr [EMAIL_ADDR_SIZE];
   INT8U   EmailMsgLength;
   INT8U   EmailMsg [EMAIL_MSG_SIZE];

} PACKED Email_Config_T;


/** Misc cconfigurations **/
typedef struct {

   INT8U SMTPIpAddr [IP_ADDR_LEN];

} PACKED MiscCfg_T ;

/** Oem Configurations Structure **/
typedef struct {
 	Snmp_Config_T	SnmpConfig;
	Email_Config_T  EmailConfig [MAX_EMAIL_CONFIGS] ;
	MiscCfg_T 	MiscConfig; 
} PACKED ExtCfgData_T;


/** union for the Response **/
typedef union {
	
	Snmp_Config_T	SmtpConfig;
	Email_Config_T  EmailConfig [MAX_EMAIL_CONFIGS] ;
	MiscCfg_T 	MiscConfig;

} ExtCfgDataUn_T;


/**  Reserve Ext cofigs  **/

/* ReservedExtCfg Command Req */
typedef struct
{
    INT8U   IANA [3];

} PACKED ReservedExtCfgReq_T;

/* ReservedExtCfg Command Res */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   IANA [3];
    INT8U   ReservationID;

} PACKED ReservedExtCfgRes_T;


/**  Get Ext configs  **/

/* GetExtCfg Command Req */
typedef struct
{
    INT8U   IANA [3];
    INT8U   ReservationID;
    INT8U   TokenID;
    INT8U   Index;
    INT8U   DataOffsetLSB;
    INT8U   DataOffsetMSB;
    INT8U   BytesToRead;

} PACKED GetExtCfgReq_T;



/** TOKEN_HEADER_SIZE = sizeof ( TotalSize + TotalVersion  + ValidFieldMask )
    If any change in the fields edit TOKEN_HEADER_SIZE accordingly..  **/

#define TOKEN_HEADER_SIZE 5 

/* GetExtCfg Command Res */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   IANA [3]; 
    INT8U   TokenID;
    INT8U   Index;
    INT8U   BytesRead;
/* Token Header */
    INT16U  TotalSize;
    INT8U   TokenVersion;
    INT16U  ValidFieldMask;
/* Token data */
    ExtCfgDataUn_T  CfgData;

} PACKED GetExtCfgRes_T;


/** Set Extended Configs **/ 

/* SetExtCfg Command Req */
typedef struct
{
    INT8U   IANA [3];
    INT8U   ReservationID;
    INT8U   TokenID;
    INT8U   Index;
    INT8U   DataOffsetLSB;
    INT8U   DataOffsetMSB;
    INT8U   InProgress;
/* Token Header */
    INT16U  TotalSize;
    INT8U   TokenVersion;
    INT16U  ValidFieldMask;
/* Token Data */
    ExtCfgDataUn_T  CfgData;

} PACKED SetExtCfgReq_T;

/* SetExtCfg Command Res */
typedef struct
{
    INT8U   CompletionCode; 
    INT8U   IANA [3];
    INT8U   BytesWritten;

} PACKED SetExtCfgRes_T;


#ifdef __cplusplus
}
#endif

#endif  //OEM_DEFINES_H_
