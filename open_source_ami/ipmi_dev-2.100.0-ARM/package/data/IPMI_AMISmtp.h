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
 ******************************************************************
 *
 * IPMI_AMIsmtp.h
 * AMI specific Smtp  Commands
 *
 *
 ******************************************************************/

#ifndef __IPMI_AMISMTP_H__
#define __IPMI_AMISMTP_H__

#include "IPMI_LANConfig.h"

#define OEM_ENABLE_DISABLE_SMTP 		0
#define OEM_PARAM_SMTP_SERVR_ADDR		1
#define OEM_PARAM_SMTP_USER_NAME		2
#define OEM_PARAM_SMTP_PASSWD			3
#define OEM_PARAM_SMTP_NO_OF_DESTINATIONS    	4
#define OEM_PARAM_SMTP_USERID		5
#define OEM_PARAM_SMTP_SUBJECT			6
#define OEM_PARAM_SMTP_MSG				7
#define OEM_PARAM_SMTP_SENDER_ADDR      8
#define OEM_PARAM_SMTP_HOST_NAME        9
#define OEM_PARAM_SMTP_PORT				0xA
#define OEM_ENABLE_DISABLE_SMTP_AUTH	0xB
#define OEM_PARAM_SMTP_IPV6_SERVR_ADDR 0x0C

#define OEM_ENABLE_DISABLE_SMTP2 		0x0D
#define OEM_PARAM_SMTP_2_SERVR_ADDR		0x0E
#define OEM_PARAM_SMTP_2_USER_NAME		0x0F
#define OEM_PARAM_SMTP_2_PASSWD			0x10
#define OEM_PARAM_SMTP_2_SENDER_ADDR      0x11
#define OEM_PARAM_SMTP_2_HOST_NAME        0x12
#define OEM_PARAM_SMTP_2_PORT				0x13
#define OEM_ENABLE_DISABLE_SMTP_2_AUTH	0x14
#define OEM_PARAM_SMTP_2_IPV6_SERVR_ADDR 0x15


#define MAX_EMAIL_ADDR_BLOCKS  4
#define MAX_EMAIL_BLOCK_SIZE   16
#define MAX_SUB_BLOCKS  	2
#define MAX_SUB_BLOCK_SIZE   	16
#define MAX_MSG_BLOCKS  	4
#define MAX_MSG_BLOCK_SIZE   	16
#define MAX_SRV_NAME_BLOCKS	2
#define MAX_SRV_NAME_BLOCK_SIZE	16
#define MAX_EMAIL_DESTINATIONS    15
#define MAX_SMTP_USERNAME_LEN  65
#define MAX_SMTP_PASSWD_LEN       65
#define MAX_SMTP_PORT_SIZE	4
#define MAX_SMTP_AUTH_FLAG_LEN 1
#define OEM_SMTP_SERVER_DISABLED -2

#pragma pack (1)

typedef union
{
    INT8U EnableDisableSMTP;
    INT8U EnableDisableSmtpAuth;
    INT8U ServerAddr [ IP_ADDR_LEN ];
    INT8U IP6_ServerAddr [ IP6_ADDR_LEN ];
    INT8U UserName [ MAX_SMTP_USERNAME_LEN ];
    INT8U Passwd   [MAX_SMTP_PASSWD_LEN];
    INT8U NoofDestinations;
    INT8U Subject	 [MAX_SUB_BLOCK_SIZE ];
    INT8U Msg	 [MAX_MSG_BLOCK_SIZE ];
    INT8U SenderAddr[MAX_EMAIL_BLOCK_SIZE];
    INT8U  Servername[MAX_SRV_NAME_BLOCK_SIZE];
    INT16U SmtpPort;
    INT8U UserID;
    INT8U EnableDisableSMTP2;
    INT8U EnableDisableSmtp2Auth;
    INT8U Server2Addr [ IP_ADDR_LEN ];
    INT8U IP6_Server2Addr [ IP6_ADDR_LEN ];
    INT8U UserName2 [ MAX_SMTP_USERNAME_LEN ];
    INT8U Passwd2 [MAX_SMTP_PASSWD_LEN];
    INT8U Sender2Addr[MAX_EMAIL_BLOCK_SIZE * MAX_EMAIL_ADDR_BLOCKS];
    INT8U  Server2name[MAX_SRV_NAME_BLOCK_SIZE * MAX_SRV_NAME_BLOCKS];
    INT16U Smtp2Port;
} PACKED Smtp_ConfigUn_T;


typedef struct
{
    INT8U   CompletionCode;
} PACKED  SetSMTPConfigRes_T;

typedef struct
{
    INT8U   Channel;
    INT8U   ParameterSelect;
    INT8U   SetSelector;
    INT8U   BlockSelector;
    Smtp_ConfigUn_T ConfigData;

} PACKED  SetSMTPConfigReq_T;


typedef struct
{
    INT8U   CompletionCode;
    Smtp_ConfigUn_T ConfigData;

} PACKED  GetSMTPConfigRes_T;



typedef struct
{
    INT8U   Channel;
    INT8U   ParameterSelect;
    INT8U   SetSelector;
    INT8U   BlockSelector;

} PACKED  GetSMTPConfigReq_T;

typedef struct
{
    INT8U UserName [ MAX_SMTP_USERNAME_LEN ];
    INT8U Passwd   [MAX_SMTP_PASSWD_LEN];
    uint8 SenderAddr[MAX_EMAIL_BLOCK_SIZE * MAX_EMAIL_ADDR_BLOCKS];
    uint8 MachineName[MAX_SRV_NAME_BLOCK_SIZE * MAX_SRV_NAME_BLOCKS];
    INT8U EnableDisableSmtpAuth;
    INT16U SmtpPort;
} PACKED SmtpInfoParam_T;

#pragma pack ()


#endif //__IPMI_AMISMTP_H__
