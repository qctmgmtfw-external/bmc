/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * IPMI_AMIResetPassword.h
 * AMI Reset Password specific IPMI Commands
 *
 * Author: Gokula Kannan. S <gokulakannans@amiindia.co.in>
 *
 *****************************************************************/

#ifndef IPMI_AMIRESETPASS_H_
#define IPMI_AMIRESETPASS_H_

#include <Types.h>
#include "OemDefs.h"
//#define USER_EMAIL_DB "/conf/UserEmail.db"
//#define MAX_USERS       64
//#ifndef EMAIL_ADDR_SIZE
#define EMAIL_ADDR_SIZE 64

//#endif

#pragma pack (1)

/**
 * *@struct AMISetUserEmailReq_T
 * *@brief Request Structure for setting the Email address 
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U	UserID;
	char	EMailID[EMAIL_ADDR_SIZE];
}PACKED AMISetUserEmailReq_T;

/**
 * *@struct AMISetUserEmailRes_T
 * *@brief Response Structure for setting the Email address 
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U	CompletionCode;
}PACKED AMISetUserEmailRes_T;

/**
 * *@struct AMIGetUserEmailReq_T
 * *@brief Request Structure for getting the Email address 
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U	UserID;
}PACKED AMIGetUserEmailReq_T;

/**
 * *@struct AMIGetUserEmailRes_T
 * *@brief Response Structure for getting the Email address 
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U	CompletionCode;
	char	EMailID[EMAIL_ADDR_SIZE];
}PACKED AMIGetUserEmailRes_T;

/**
 * *@struct AMIResetPasswordReq_T
 * *@brief Request Structure to Reset the Password  
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U   UserID;
	INT8U   Channel;
}PACKED AMIResetPasswordReq_T;

/**
 * *@struct AMIResetPasswordReq_T
 * *@brief Response Structure to Reset the Password  
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U	CompletionCode;
}PACKED AMIResetPasswordRes_T;

/**
 * *@struct AMISetUserEmailFormatReq_T
 * *@brief Request Structure for setting the Email Format 
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U	UserID;
	INT8U	EMailFormat[EMAIL_FORMAT_SIZE];
}PACKED AMISetUserEmailFormatReq_T;

/**
 * *@struct AMISetUserEmailFormatRes_T
 * *@brief Response Structure for setting the Email Format 
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U	CompletionCode;
}PACKED AMISetUserEmailFormatRes_T;
/**
 * *@struct AMIGetUserEmailFormatRes_T
 * *@brief Response Structure for getting the Email Format 
 * *		for a particular user ID.
 * */
typedef struct
{
	INT8U	CompletionCode;
	INT8U 	EMailFormat[EMAIL_FORMAT_SIZE];
}PACKED AMIGetUserEmailFormatRes_T;

#pragma pack ()

#endif

