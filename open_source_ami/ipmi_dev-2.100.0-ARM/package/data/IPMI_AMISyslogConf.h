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
 * IPMI_AMISyslogConf.h
 * AMI Syslog Configuration related IPMI Commands
 *
 * Author: Gokula Kannan. S <gokulakannans@amiindia.co.in>
 *
 *****************************************************************/

#ifndef IPMI_AMISYSLOGCONF_H_
#define IPMI_AMISYSLOGCONF_H_

#include <Types.h>
#include "syslogconf.h"

#pragma pack (1)

/* Command for Set log */
#define SYSLOG	0x0
#define AUDITLOG	0x1

/* Size of the command */
#define SIZE_SYS_DISABLE (2 * sizeof(INT8U))
#define SIZE_SYS_LOCAL (SIZE_SYS_DISABLE + sizeof(INT32U) + sizeof(INT8U))
#define SIZE_SYS_REMOTE (SIZE_SYS_DISABLE + HOSTNAME_LEN)
#define SIZE_AUDIT	SIZE_SYS_DISABLE

/**
 * *@struct AMISetLogConfReq_T
 * *@brief Request Structure for setting the Log configuration.
 * */
typedef struct
{
    INT8U   Cmd;    /* Command to process */
    INT8U   Status;     /* status to set */
    union
    {
        struct
        {
            char    HostName[HOSTNAME_LEN];                  /* Name of the remote syslog server */
        }Remote;                                                             /* Request to Enable remote system log */
        struct
        {
            INT32U	Size;                                             /* Size of each file to rotate */
            INT8U	Rotate;                                                /* Number of back-up files after logrotate */
        }Local;                                                             /* Request to Enable local system log */
    }Config;
}PACKED AMISetLogConfReq_T;

/**
 * *@struct AMISetLogConfRes_T
 * *@brief Response Structure for setting the Log configuration.
 * */
typedef struct
{
    INT8U   CompletionCode;     /* Completion code */
}PACKED AMISetLogConfRes_T;

/**
 * *@struct AMIGetLogConfRes_T
 * *@brief Request Structure for getting the Log configuration.
 * */
typedef struct
{

    INT8U	CompletionCode;    /* Completion code */
    INT8U	SysStatus;       /* ststem log status */
    INT8U	AuditStatus;    /* Audit log status */
    union
    {
        struct
        {
            char	HostName[HOSTNAME_LEN];     /* Name of the remote syslog server */
        }Remote;    /* Request to Enable remote system log */
        struct
        {
            INT32U  Size;   /* Size of each file to rotate */
            INT8U	    Rotate;     /* Number of back-up files after logrotate */
        }Local;     /* Request to Enable local system log */
    }Config;
}PACKED AMIGetLogConfRes_T;

#pragma pack ()

#endif

