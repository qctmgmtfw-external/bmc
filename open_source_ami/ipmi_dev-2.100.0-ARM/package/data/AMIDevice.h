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
 *****************************************************************
 *
 * AMICommands.h
 * AMI specific Commands
 *
 * Author: Basavaraj Astekar <basavaraja@ami.com>
 *
 *****************************************************************/
#ifndef AMI_DEVICE_H
#define AMI_DEVICE_H

#include "Types.h"

/**
 * @defgroup acf AMI Device commands
 * @ingroup acf
 * IPMI AMI-specific Command Handlers for
 * @{
 **/

#define SSH_TEMP_FILE   "/tmp/ssh_temp_key"
#define MAX_BOOTVAR_LENGTH 400
#define MAX_BOOTVAL_LENGTH 400 
#define MAX_FMHLENGTH 1024

#define PAM_MODULE_LEN 64
#define PAM_BUF_LEN   128
#define  ERR_RETRN                  -1


typedef struct
{
    INT8U  SqnceNo;
    char     PamModule[PAM_MODULE_LEN];
}PamOrder_T;


#define PAM_IPMI   "pam_ipmi.so"
#define PAM_LDAP  "pam_custom_ldap.so"
#define PAM_AD      "pam_custom_ad.so"
#define PAM_WOUNIX_FILE       "/conf/pam_wounix"
#define PAM_WITHUNIX_FILE   "/conf/pam_withunix"
#define PAM_NSSWITCH_FILE   "/conf/nsswitch.conf"

/*** Function Prototypes ***/
extern void FlashTimerTask (int BMCInst);

/** @} */

extern int AMIYAFUGetFlashInfo ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUGetFirmwareInfo ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUGetFMHInfo ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUGetStatus ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUActivateFlashMode ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,_NEAR_ int BMCInst );
extern int AMIYAFUAllocateMemory ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,_NEAR_ int BMCInst ); 
extern int AMIYAFUFreeMemory ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,_NEAR_ int BMCInst ); 
extern int AMIYAFUReadFlash ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUWriteFlash ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst);
extern int AMIYAFUEraseFlash ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUProtectFlash ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUEraseCopyFlash ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUGetECFStatus( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst);
extern int AMIYAFUGetVerifyStatus  ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst);
extern int AMIYAFUVerifyFlash ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUReadMemory ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUWriteMemory ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst);
extern int AMIYAFUCopyMemory ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUCompareMemory ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,_NEAR_ int BMCInst );
extern int AMIYAFUClearMemory ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,_NEAR_ int BMCInst ); 
extern int AMIYAFUGetBootConfig ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUSetBootConfig ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUGetBootVars ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUDeactivateFlash ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIYAFUResetDevice ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIGetChNum ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst); 
extern int AMIGetEthIndex ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,_NEAR_ int BMCInst) ;
extern int AMIGetFruDetails( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst );
extern int AMIGetRootUserAccess (_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int AMISetRootPassword   (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,_NEAR_ int BMCInst);
extern int AMIGetUserShelltype   (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,_NEAR_ int BMCInst);
extern int AMISetUserShelltype   (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,_NEAR_ int BMCInst);

extern int AMISetTriggerEvent( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,int BMCInst );
extern int AMIGetTriggerEvent( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,int BMCInst );
extern int AMIGetSolConf( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,int BMCInst );
extern int AMISetLoginAuditConfig( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,int BMCInst );
extern int AMIGetLoginAuditConfig( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,int BMCInst );
extern int AMISetPamOrder ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst);
extern int AMIGetPamOrder ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst);
int GetUBootParam (char*, char* );
int SetUBootParam (char*, char* );
int GetAllUBootParam();
#endif  /* AMI_CMDS_H */

