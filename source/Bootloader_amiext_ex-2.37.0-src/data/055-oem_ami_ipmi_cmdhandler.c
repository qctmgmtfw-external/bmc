--- uboot.org/oem/ami/ipmi/cmdhandler.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot/oem/ami/ipmi/cmdhandler.c	2014-03-11 11:56:38.282110850 -0400
@@ -0,0 +1,1422 @@
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+ *****************************************************************
+ * $Header: $
+ *
+ * $Revision: $
+ *
+ * $Date: $
+ *
+ ******************************************************************
+ ******************************************************************
+ * 
+ * cmdhandler.c
+ * IPMI Command Handler Functions
+ *
+ *  Author: Rama Rao Bisa <ramab@ami.com>
+ *			Modified by Anurag Bhatia for SP porting		
+ ******************************************************************/
+
+#include <common.h>
+#include <config.h>
+#include <environment.h>
+#include <net.h>
+#include <command.h>
+#include <malloc.h>
+#include <flash.h>
+
+#include <oem/ami/ipmi/IPMIDefs.h>
+#include <oem/ami/ipmi/cmdhandler.h>
+#include <oem/ami/fmh/fmh.h>
+
+extern int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
+extern flash_info_t flash_info[];
+
+/* Global Variables */
+static const unsigned char      m_DualResBits [] = {2,1,2,1,2,1,1,2};
+
+
+
+static INT32U			gSizeToErase = 0;
+static unsigned int 	gSessionSeqNum = 0;
+static unsigned int 	gSessionID     = 0;
+static int  			FMHComp;
+static int 				ActivateFlashStatus =0X00 ;
+static int 				LastStatCode = 0x00;
+static unsigned char 	*pNewFirmwareAddr; 
+
+
+/* The following are used by fwupdate.c */
+int 					gReset	= 0;
+int 					gStartFirmwareUpdation = 0;
+INT32U					gEraseBlock = 0;
+unsigned char			gFWUploadSelector = IMAGE_1;
+
+
+/************************************************************************
+*Gets all the environment variable names
+*/
+static
+int Getenvvar(unsigned char *count,unsigned char *VarName,int *BootVarlen)
+{
+     int i=0,Counter=0;
+     unsigned char *Nextenv;
+     uchar *env,*nxt = NULL;
+
+     uchar *env_data = (uchar *)env_get_addr(0);
+     Nextenv = VarName;
+     for (env=env_data; *env; env=nxt+1)
+     {
+        i=0;
+        for (nxt=env; *nxt; ++nxt)
+        {
+            if(*nxt != '\0')
+            {
+                  if(i==0)
+                  {
+                      *count += 1;
+                   while((*Nextenv++ = *nxt++) != '=' )
+                   {
+                        Counter ++;
+                   };
+                   Nextenv --;
+                   *Nextenv++ = '\0';
+                       i =1;
+                  }
+            }
+              }
+      }
+     *BootVarlen = Counter + *count;
+     return 0;
+}
+
+
+/*---------------------------------------
+ * GetChAuthCaps
+ *---------------------------------------*/
+int
+GetChAuthCaps (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+
+    GetChAuthCapReq_T* pGetChAuthCapReq = (GetChAuthCapReq_T*)pReq;
+    GetChAuthCapRes_T* pGetChAuthCapRes = (GetChAuthCapRes_T*)pRes;
+    unsigned char              PrivLevel, ChannelNum;
+
+    PrivLevel   = pGetChAuthCapReq->PrivLevel & 0x0F;
+    ChannelNum  = pGetChAuthCapReq->ChannelNum & 0x0F;
+	if (ChannelNum == 0x0e) ChannelNum = 1;
+
+    pGetChAuthCapRes->CompletionCode    = CC_NORMAL; /* completion code */
+    pGetChAuthCapRes->ChannelNum        = ChannelNum; /* channel No */
+
+    if ((0 == PrivLevel) || (PrivLevel > 5))
+    {
+        pGetChAuthCapRes->CompletionCode = CC_INV_DATA_FIELD;/* Privilege is Reserved */
+        return sizeof (*pRes);
+    }
+    /* Authentication Type Supported for given Privilege */
+    pGetChAuthCapRes->AuthType = 0x01;
+
+
+    pGetChAuthCapRes->PerMsgUserAuthLoginStatus = 0;
+
+	/* Null User Null Password Enabled  */
+	pGetChAuthCapRes->PerMsgUserAuthLoginStatus |= 0x01;
+	/* Null User with Password Disabled */
+	pGetChAuthCapRes->PerMsgUserAuthLoginStatus &= ~0x02;
+
+    /* User level Authentication */
+    pGetChAuthCapRes->PerMsgUserAuthLoginStatus |= 0x08;
+
+    /* PerMessage Authentication */
+    pGetChAuthCapRes->PerMsgUserAuthLoginStatus |= 0x10;
+    return sizeof (GetChAuthCapRes_T);
+
+}
+
+
+
+/*---------------------------------------
+ * GetSessionChallenge
+ *---------------------------------------*/
+int
+GetSessionChallenge (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+
+    GetSesChallengeRes_T* pGetSesChalRes = (GetSesChallengeRes_T*)pRes;
+	unsigned char         ChallengeString[16] = { 1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6 };
+    unsigned int          TempSessId = 0x12345678;
+
+	printf ("Get Session Challenge\n");
+   	pGetSesChalRes->CompletionCode = CC_NORMAL;
+    pGetSesChalRes->TempSessionID  = TempSessId;
+    memcpy(pGetSesChalRes->ChallengeString, ChallengeString, 16);
+
+    return sizeof (GetSesChallengeRes_T);
+}
+
+
+/*---------------------------------------
+ * ActivateSession
+ *---------------------------------------*/
+int
+ActivateSession (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+     ActivateSesReq_T*   pAcvtSesReq = (ActivateSesReq_T*)pReq;
+     ActivateSesRes_T*   pAcvtSesRes = (ActivateSesRes_T*)pRes;
+     unsigned char       /*AuthType, */Privilege;
+
+    Privilege = pAcvtSesReq->Privilege & 0x0F;
+
+    if ((0 == Privilege) || (Privilege > 5))
+    {
+        pAcvtSesRes->CompletionCode = CC_INV_DATA_FIELD;
+        return sizeof (*pRes);
+    }
+
+    pAcvtSesRes->CompletionCode = CC_NORMAL;
+    pAcvtSesRes->AuthType       = 0x00;
+    pAcvtSesRes->SessionID      = 0x12345678;
+	pAcvtSesRes->InboundSeq		= 0x00;
+    pAcvtSesRes->Privilege      = 0x04;
+
+	gSessionID					= pAcvtSesRes->SessionID;
+	gSessionSeqNum				= pAcvtSesRes->InboundSeq;
+
+    return sizeof (ActivateSesRes_T);
+}
+
+
+/*---------------------------------------
+ * SetSessionPrivLevel
+ *---------------------------------------*/
+int
+SetSessionPrivLevel (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+    SetSesPrivLevelRes_T*   pSetSesPrivLevelRes = (SetSesPrivLevelRes_T*)pRes;
+
+    pSetSesPrivLevelRes->CompletionCode = CC_NORMAL;
+    pSetSesPrivLevelRes->Privilege      = *pReq & 0x0F;;
+
+    return sizeof (SetSesPrivLevelRes_T);
+}
+
+/*---------------------------------------
+ * CloseSession
+ *---------------------------------------*/
+int
+CloseSession (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+
+	CloseSesReq_T*	pCloseSesReq = (CloseSesReq_T*) pReq;
+
+	printf ("Close Session ID Receive is %x\n", pCloseSesReq->SessionID);
+    /* The SessionInfo is deleted form session table  from interface */
+    *pRes = CC_NORMAL;
+	if (gSessionID == pCloseSesReq->SessionID)
+	{
+		gSessionSeqNum = 0;
+		gSessionID	 = 0;
+	}
+	else
+	{
+		*pRes = CC_CLOSE_INVALID_SESSION_ID;
+	}	
+
+    return sizeof (*pRes);
+}
+/*---------------------------------------
+ * GetDevID
+ *---------------------------------------*/
+int
+GetDevID (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+	flash_info_t *pFlashInfo;
+ 	pFlashInfo = &flash_info[0];
+	FMH *FMHPtr = NULL;
+	MODULE_INFO *ModulePtr = NULL;
+    GetDevIDRes_T*  pGetDevIDRes = (GetDevIDRes_T*) pRes;
+    unsigned int Major,Minor;
+	unsigned char MfgID [3] = MFG_ID_AMI;
+
+	FMHPtr = ScanforFMH((unsigned char*)pFlashInfo->start[(CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE)-1], 
+			(unsigned long)CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE);
+	if (FMHPtr == NULL)
+	{
+		printf ("FMH not found\n");
+	}
+	ModulePtr = &(FMHPtr->Module_Info);
+	Major = ModulePtr->Module_Ver_Major;
+	Minor = ModulePtr->Module_Ver_Minor;
+
+    pGetDevIDRes->CompletionCode      = CC_NORMAL;
+    pGetDevIDRes->DeviceID            = DEVICE_ID;
+    pGetDevIDRes->DevRevision         = IPMI_DEV_REVISION;
+    pGetDevIDRes->FirmwareRevision1   = Major;
+    pGetDevIDRes->FirmwareRevision2   = Minor;
+    pGetDevIDRes->IPMIVersion         = IPMI_VERSION;
+    pGetDevIDRes->DevSupport          = DEV_SUPPORT;
+    pGetDevIDRes->ProdID              = htoipmi_u16 (0xaabb);
+    pGetDevIDRes->AuxFirmwareRevision = 0x00;
+    memcpy (pGetDevIDRes->MfgID, MfgID, sizeof (MfgID));
+
+    return (sizeof (GetDevIDRes_T));
+}
+
+/*---------------------------------------
+ * CalculateChksum
+ *---------------------------------------*/
+static
+unsigned long
+CalculateChksum (char *data, unsigned long size)
+{
+    unsigned long crc32val = 0;
+    unsigned int i = 0;
+
+    BeginCRC32 (&crc32val);
+
+    /*  Begin calculating CRC32 */
+	for(i = 0;i < size;i++)
+    {
+		DoCRC32 (&crc32val, data[i]);
+    }
+
+    EndCRC32 (&crc32val);
+
+	return crc32val;
+}
+
+/*---------------------------------------
+ * AMIYAFUGetFlashInfo
+ *---------------------------------------*/
+int AMIYAFUGetFlashInfo ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+
+
+	AMIYAFUGetFlashInfoReq_T *pAMIYAFUFlashInfoReq = (AMIYAFUGetFlashInfoReq_T *)pReq;
+    AMIYAFUGetFlashInfoRes_T* pAMIYAFUGetFlashInfo = (AMIYAFUGetFlashInfoRes_T*)pRes;  
+
+     	
+	pAMIYAFUGetFlashInfo->CompletionCode = YAFU_CC_NORMAL;
+    pAMIYAFUGetFlashInfo->FlashInfoRes.Seqnum = pAMIYAFUFlashInfoReq->FlashInfoReq.Seqnum;
+	pAMIYAFUGetFlashInfo->FlashInfoRes.YafuCmd= pAMIYAFUFlashInfoReq->FlashInfoReq.YafuCmd;
+	
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashSize = CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE;
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashAddress = CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_START;
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashEraseBlkSize = CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE;
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashProductID = 0; 
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashWidth = 8;  
+	pAMIYAFUGetFlashInfo->FlashInfo.FMHCompliance = 0x01;
+
+    if(pAMIYAFUGetFlashInfo->FlashInfo.FMHCompliance == 0x01)
+		FMHComp = 1;
+	
+    pAMIYAFUGetFlashInfo->FlashInfo.Reserved = 0;
+	pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks = CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE; 
+
+    if((pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks * pAMIYAFUGetFlashInfo->FlashInfo.FlashEraseBlkSize) == pAMIYAFUGetFlashInfo->FlashInfo.FlashSize)
+    	pAMIYAFUGetFlashInfo->FlashInfoRes.Datalen= 0x20; 
+    else	   
+     	pAMIYAFUGetFlashInfo->FlashInfoRes.Datalen = 0x20 + pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks;
+
+	pAMIYAFUGetFlashInfo->FlashInfoRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetFlashInfo->FlashInfo,sizeof(pAMIYAFUGetFlashInfo->FlashInfo));				 	   
+	
+    return( sizeof( AMIYAFUGetFlashInfoRes_T ) );
+}
+
+
+/*---------------------------------------
+ * AMIYAFUGetFMHInfo
+ *---------------------------------------*/
+ int AMIYAFUGetFMHInfo ( unsigned char* pReq, unsigned char ReqLen, unsigned char *pRes ) 
+{
+      DWORD i=0,m=0;
+      INT8U *Buf;
+      FlashMH      FlashModHeader;
+      ALT_FMHead    AltFmh;  
+      char *FMHDetails;	  
+      DWORD StartFlashAddr = 0;
+    
+
+      AMIYAFUGetFMHInfoReq_T *pAMIYAFUGetFMHInfoReq = (AMIYAFUGetFMHInfoReq_T *)pReq;
+      AMIYAFUGetFMHInfoRes_T* pAMIYAFUGetFMHInfo = (AMIYAFUGetFMHInfoRes_T*)pRes;
+
+	  pAMIYAFUGetFMHInfo->NumFMH = 0x00;
+
+      FMHDetails = malloc(1200);
+      if(FMHDetails == NULL)
+      {
+             printf("Error in malloc of FMHDetails");
+	      return -1;		 
+      }
+
+      if(gFWUploadSelector == IMAGE_2)	
+	  	StartFlashAddr =  CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE;
+
+	for(i=0;i<(CONFIG_SPX_FEATURE_GLOBAL_USED_FLASH_SIZE/CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE);i++) 			
+   {
+	Buf= (INT8U *)malloc(64);
+
+	if(Buf == NULL)
+	{
+	      printf("Error in alloc\n");
+	      AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+	      pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+	      pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	      pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	      pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+	      pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_ALLOC_ERR;
+	      LastStatCode = YAFU_CC_ALLOC_ERR;
+
+             return (sizeof(AMIYAFUNotAck));
+   	}
+
+
+       if(flash_read( StartFlashAddr + CONFIG_SYS_FLASH_BASE+(i*CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE),64,(char *)Buf) != 0)
+	{
+
+		AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+		pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+		pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+		pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+		pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+		pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_READ_ERR;
+			 
+		return (sizeof(AMIYAFUNotAck));
+	}
+    
+       
+      memcpy((char*)&FlashModHeader,Buf,64);
+
+      if(strncmp((char*)FlashModHeader.FmhSignature,"$MODULE$",(sizeof(FlashModHeader.FmhSignature) -1)) == 0 )	//Check FMH Signature
+      {
+
+            if(FlashModHeader.EndSignature != 21930)
+	      	  	continue;
+
+	     memcpy((FMHDetails + m),(char *)&FlashModHeader,64);
+           
+       
+              printf("Module Name = %s\n",FlashModHeader.ModuleName);
+
+	     m +=64;
+	     pAMIYAFUGetFMHInfo->NumFMH += 0x1;
+
+	     free(Buf);
+
+	}
+      else
+      {
+
+         if(flash_read(StartFlashAddr + CONFIG_SYS_FLASH_BASE+((i*CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE) +(CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE -sizeof(AltFmh))),sizeof(AltFmh),(char*)Buf) != 0)
+	  {
+
+		AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+		pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+		pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+		pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+		pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+		pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_READ_ERR;
+		 
+		return (sizeof(AMIYAFUNotAck));
+          }	
+
+	  
+	      memcpy((char *)&AltFmh,Buf,sizeof(AltFmh));
+             if(strncmp((char *)AltFmh.Signature,"$MODULE$",(sizeof(AltFmh.Signature) -1)) == 0 )	//Check FMH Signature
+             	{
+			 
+	         	if(flash_read( StartFlashAddr + CONFIG_SYS_FLASH_BASE +(i*CONFIG_SPX_FEATURE_GLOBAL_ERASE_BLOCK_SIZE) +AltFmh.LinkAddress,64,(char *)Buf) != 0)
+	  		{
+
+				AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+				pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+				pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+				pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+				pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+				pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_READ_ERR;
+					 
+				return (sizeof(AMIYAFUNotAck));
+         		 }
+
+					  
+	      		memcpy((char*)&FlashModHeader,Buf,64);	
+
+	      		if(strncmp((char *)FlashModHeader.FmhSignature,"$MODULE$",(sizeof(FlashModHeader.FmhSignature) -1)) == 0 )	//Check FMH Signature
+            		{
+
+	      			if(FlashModHeader.EndSignature != 21930)
+	      	  			continue;
+
+                            memcpy((FMHDetails +m),(char *)&FlashModHeader,64);
+           
+              		printf("Module Name = %s\n",FlashModHeader.ModuleName);
+	
+	     			m +=64;
+	     			pAMIYAFUGetFMHInfo->NumFMH += 0x1;
+
+	     			
+
+	   		}	
+          		
+             	}	
+	  free(Buf);
+      	}
+    }
+
+
+      if(pAMIYAFUGetFMHInfoReq->FMHReq.Datalen == 0x00)
+		pAMIYAFUGetFMHInfo->FMHRes.Datalen= 0x04 + (pAMIYAFUGetFMHInfo->NumFMH * 64);
+	else
+	{
+		pAMIYAFUGetFMHInfo->CompletionCode = CC_INV_DATA_FIELD;
+		return sizeof (*pRes);
+	}
+
+      memcpy (( INT8U*) (pAMIYAFUGetFMHInfo + 1),
+                   ( INT8U*)FMHDetails,(pAMIYAFUGetFMHInfo->NumFMH * 64) );	
+
+      pAMIYAFUGetFMHInfo->CompletionCode = YAFU_CC_NORMAL;
+      pAMIYAFUGetFMHInfo->FMHRes.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+      pAMIYAFUGetFMHInfo->FMHRes.YafuCmd= pAMIYAFUGetFMHInfoReq->FMHReq.YafuCmd;
+      pAMIYAFUGetFMHInfo->Reserved = 0x00; 	  
+      pAMIYAFUGetFMHInfo->FMHRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetFMHInfo->Reserved,pAMIYAFUGetFMHInfo->FMHRes.Datalen);
+     
+      return( sizeof( AMIYAFUGetFMHInfoRes_T ) + pAMIYAFUGetFMHInfo->NumFMH * 64 );
+}
+ 
+/*---------------------------------------
+ * AMIYAFUGetStatus
+ *---------------------------------------*/
+
+int AMIYAFUGetStatus  (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+    char *s;
+    int Mode = 0;
+
+    AMIYAFUGetStatusReq_T *pAMIYAFUGetStatusReq =(AMIYAFUGetStatusReq_T *)pReq;
+    AMIYAFUGetStatusRes_T* pAMIYAFUGetStatus = (AMIYAFUGetStatusRes_T*)pRes;
+
+    pAMIYAFUGetStatus->CompletionCode = YAFU_CC_NORMAL;
+    pAMIYAFUGetStatus->GetStatusRes.Seqnum = pAMIYAFUGetStatusReq->GetStatusReq.Seqnum;
+    pAMIYAFUGetStatus->GetStatusRes.YafuCmd= pAMIYAFUGetStatusReq->GetStatusReq.YafuCmd;
+    pAMIYAFUGetStatus->LastStatusCode =(INT16U) LastStatCode;
+    pAMIYAFUGetStatus->YAFUState = 0x00;
+
+
+    s = getenv("mode");
+    if (s != NULL)
+    {
+		Mode = (int)simple_strtoul(s,NULL,0);
+		printf("Mode is %d\n",Mode);
+    }
+
+
+    pAMIYAFUGetStatus->Mode = Mode;
+    pAMIYAFUGetStatus->Reserved = 0x00;
+    pAMIYAFUGetStatus->GetStatusRes.Datalen= 8;
+    pAMIYAFUGetStatus->Message[0] = 0;
+    pAMIYAFUGetStatus->GetStatusRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetStatus->LastStatusCode,(INT32U)pAMIYAFUGetStatus->GetStatusRes.Datalen);
+
+     return ( sizeof( AMIYAFUGetStatusRes_T ) );
+}
+
+/*---------------------------------------
+ * AMIYAFUActivateFlashMode
+ *---------------------------------------*/
+int AMIYAFUActivateFlashMode ( unsigned char *pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+    AMIYAFUActivateFlashModeReq_T *pAMIYAFUActivateFlashReq = (AMIYAFUActivateFlashModeReq_T *)pReq;
+    AMIYAFUActivateFlashModeRes_T* pAMIYAFUActivateFlash = (AMIYAFUActivateFlashModeRes_T*)pRes;
+
+   if(CalculateChksum((char *)&pAMIYAFUActivateFlashReq->Mode,sizeof(INT16U)) != pAMIYAFUActivateFlashReq->ActivateflashReq.CRC32chksum)
+  {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUActivateFlashReq->ActivateflashReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+  }
+
+	if(pAMIYAFUActivateFlashReq->ActivateflashReq.Datalen == 0x02)
+		pAMIYAFUActivateFlash->ActivateflashRes.Datalen= 0x02;
+	else
+	{
+	     AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+            pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUActivateFlashReq->ActivateflashReq.Seqnum;
+	     pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	     pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	     pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+            pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+            LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+            return (sizeof(AMIYAFUNotAck));
+	}
+
+    ActivateFlashStatus = 0x01;
+    pNewFirmwareAddr = (unsigned char*)YAFU_IMAGE_UPLOAD_LOCATION;
+
+    pAMIYAFUActivateFlash->CompletionCode = YAFU_CC_NORMAL;
+    pAMIYAFUActivateFlash->ActivateflashRes.Seqnum = pAMIYAFUActivateFlashReq->ActivateflashReq.Seqnum;
+    pAMIYAFUActivateFlash->ActivateflashRes.YafuCmd= pAMIYAFUActivateFlashReq->ActivateflashReq.YafuCmd;
+
+    pAMIYAFUActivateFlash->ActivateflashRes.CRC32chksum = 0x00;
+    pAMIYAFUActivateFlash->Delay = 0x00;
+
+    return ( sizeof( AMIYAFUActivateFlashModeRes_T ) );
+}
+
+/*---------------------------------------
+ * AMIYAFUProtectFlash
+ *---------------------------------------*/
+int AMIYAFUProtectFlash ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes ) 
+{
+
+	char *argv [4];
+    AMIYAFUProtectFlashReq_T *pAMIYAFUProtectFlashReq = (AMIYAFUProtectFlashReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+   AMIYAFUProtectFlashRes_T* pAMIYAFUProtectFlash = (AMIYAFUProtectFlashRes_T*)pRes;
+
+   if(CalculateChksum((char *)&pAMIYAFUProtectFlashReq->Blknum,pAMIYAFUProtectFlashReq->ProtectFlashReq.Datalen) != pAMIYAFUProtectFlashReq->ProtectFlashReq.CRC32chksum)
+   {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+   }
+
+    if(pAMIYAFUProtectFlashReq->ProtectFlashReq.Datalen== 0x05)
+          pAMIYAFUProtectFlash->ProtectFlashRes.Datalen= 0x01;
+    else
+    {
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+	   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+	   return (sizeof(AMIYAFUNotAck));
+   }
+
+    argv[0] = "protect";
+    argv[1] = "off";
+     argv[2] = "all";
+     argv[3] = NULL;
+
+     if (0 != do_protect (NULL,0,3,argv))
+     {
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+	   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_PROTECT_ERR;
+	   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+	   return (sizeof(AMIYAFUNotAck));
+
+     }
+
+    pNewFirmwareAddr = (unsigned char*)YAFU_IMAGE_UPLOAD_LOCATION;
+
+    pAMIYAFUProtectFlash->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = pAMIYAFUProtectFlash->CompletionCode;
+    pAMIYAFUProtectFlash->ProtectFlashRes.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+    pAMIYAFUProtectFlash->ProtectFlashRes.YafuCmd= pAMIYAFUProtectFlashReq->ProtectFlashReq.YafuCmd;
+    pAMIYAFUProtectFlash->ProtectFlashRes.CRC32chksum =  CalculateChksum((char *)&(pAMIYAFUProtectFlash->Status),sizeof(INT8U));
+
+
+   return (sizeof(AMIYAFUProtectFlashRes_T));
+}
+else
+{
+     AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+     pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+     pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+     pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+     pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+     pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+     LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+     return (sizeof(AMIYAFUNotAck));
+}
+
+}
+
+/*---------------------------------------
+ * AMIYAFUEraseCopyFlash
+ *---------------------------------------*/
+int AMIYAFUEraseCopyFlash ( unsigned char *pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+
+    AMIYAFUEraseCopyFlashReq_T *pAMIYAFUEraseCopyFlashReq = (AMIYAFUEraseCopyFlashReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+
+    AMIYAFUEraseCopyFlashRes_T* pAMIYAFUEraseCopyFlash = (AMIYAFUEraseCopyFlashRes_T*)pRes;
+
+    if(CalculateChksum((char *)&pAMIYAFUEraseCopyFlashReq->Memoffset,pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Datalen) != pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.CRC32chksum)
+    {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+    }
+
+    if(pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Datalen == 0x0c)
+          pAMIYAFUEraseCopyFlash->EraseCpyFlashRes.Datalen = 0x04;
+    else
+    {
+
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+          pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode = YAFU_CC_INVALID_DATLEN;
+
+	   return (sizeof(AMIYAFUNotAck));
+    }
+  
+    gEraseBlock = pAMIYAFUEraseCopyFlashReq->Flashoffset;
+    gSizeToErase = 	pAMIYAFUEraseCopyFlashReq->Sizetocopy;
+    pNewFirmwareAddr = (unsigned char *)YAFU_IMAGE_UPLOAD_LOCATION;
+
+
+   gStartFirmwareUpdation = 0x01;
+
+    pAMIYAFUEraseCopyFlash->Sizecopied =  pAMIYAFUEraseCopyFlashReq->Sizetocopy;
+    pAMIYAFUEraseCopyFlash->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode =  (INT16U)pAMIYAFUEraseCopyFlash->CompletionCode;
+    pAMIYAFUEraseCopyFlash->EraseCpyFlashRes.Seqnum = pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Seqnum;
+    pAMIYAFUEraseCopyFlash->EraseCpyFlashRes.YafuCmd= pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.YafuCmd;
+    pAMIYAFUEraseCopyFlash->EraseCpyFlashRes.CRC32chksum = CalculateChksum((char *)&(pAMIYAFUEraseCopyFlash->Sizecopied),sizeof(INT32U));
+
+    return (sizeof(AMIYAFUEraseCopyFlashRes_T));
+}
+else
+{
+    AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+    pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Seqnum;
+    pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+    pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+    pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+    pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+    LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+    return (sizeof(AMIYAFUNotAck));
+}
+
+}
+
+
+
+/*---------------------------------------
+ * AMIYAFUWriteMemory
+ *---------------------------------------*/
+int AMIYAFUWriteMemory ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+
+    AMIYAFUWriteMemoryReq_T *pAMIYAFUWriteMemoryReq = (AMIYAFUWriteMemoryReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+    INT8U *OffsetToWrite;
+
+    AMIYAFUWriteMemoryRes_T* pAMIYAFUWriteMemory = (AMIYAFUWriteMemoryRes_T*)pRes;
+
+    if(CalculateChksum((char *)&pAMIYAFUWriteMemoryReq->Memoffset,pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen) != pAMIYAFUWriteMemoryReq->WriteMemReq.CRC32chksum)
+   {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUWriteMemoryReq->WriteMemReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+         return (sizeof(AMIYAFUNotAck));
+
+   }
+
+
+   OffsetToWrite = pNewFirmwareAddr ;
+   
+    memcpy (( INT8U*) OffsetToWrite,
+	 	 (pReq + sizeof(AMIYAFUWriteMemoryReq_T)), 
+		 (pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen - 5)); 
+
+   pNewFirmwareAddr += pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen - 5;
+
+    pAMIYAFUWriteMemory->SizeWritten = (pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen - 5);
+
+
+    pAMIYAFUWriteMemory->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U) pAMIYAFUWriteMemory->CompletionCode;
+    pAMIYAFUWriteMemory->WriteMemRes.Seqnum= pAMIYAFUWriteMemoryReq->WriteMemReq.Seqnum;
+    pAMIYAFUWriteMemory->WriteMemRes.YafuCmd= pAMIYAFUWriteMemoryReq->WriteMemReq.YafuCmd;
+    pAMIYAFUWriteMemory->WriteMemRes.Datalen=0x02;
+    pAMIYAFUWriteMemory->WriteMemRes.CRC32chksum =  CalculateChksum((char *)&pAMIYAFUWriteMemory->SizeWritten,sizeof(INT16U));
+
+    return (sizeof(AMIYAFUWriteMemoryRes_T));
+}
+else
+{
+   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUWriteMemoryReq->WriteMemReq.Seqnum;
+   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+   return (sizeof(AMIYAFUNotAck));
+}
+
+
+}
+
+/*---------------------------------------
+ * AMIYAFUGetBootConfig
+ *---------------------------------------*/
+int AMIYAFUGetBootConfig ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+
+    AMIYAFUGetBootConfigReq_T *pAMIYAFUGetBootConfigReq = (AMIYAFUGetBootConfigReq_T *)pReq;
+  
+if( ActivateFlashStatus == 0x01)
+{
+     int len = 0;
+     char *Buffer;		 
+
+    AMIYAFUGetBootConfigRes_T* pAMIYAFUGetBootConfig = (AMIYAFUGetBootConfigRes_T*)pRes;
+
+    if(CalculateChksum((char *)&pAMIYAFUGetBootConfigReq->VarName[0],pAMIYAFUGetBootConfigReq->GetBootReq.Datalen) != pAMIYAFUGetBootConfigReq->GetBootReq.CRC32chksum)
+   {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetBootConfigReq->GetBootReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+         return (sizeof(AMIYAFUNotAck));
+
+   }
+
+    if(pAMIYAFUGetBootConfigReq->GetBootReq.Datalen == 0x41)
+          pAMIYAFUGetBootConfig->GetBootRes.Datalen= 0x42;
+    else
+    {
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+          pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetBootConfigReq->GetBootReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode =  pAMIYAFUNotAcknowledge->ErrorCode;
+						
+	   return (sizeof(AMIYAFUNotAck)); 
+
+    }
+
+    Buffer = getenv((char *)&pAMIYAFUGetBootConfigReq->VarName);
+
+    len = strlen(Buffer);
+    len++; 	
+	
+
+    pAMIYAFUGetBootConfig->Status = 0x01;
+
+    pAMIYAFUGetBootConfig->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U)pAMIYAFUGetBootConfig->CompletionCode;	
+    pAMIYAFUGetBootConfig->GetBootRes.Seqnum = pAMIYAFUGetBootConfigReq->GetBootReq.Seqnum;
+    pAMIYAFUGetBootConfig->GetBootRes.YafuCmd=pAMIYAFUGetBootConfigReq->GetBootReq.YafuCmd;
+      
+    memcpy (( INT8U*) (pAMIYAFUGetBootConfig + 1),
+                   ( INT8U*)Buffer,len );
+    pAMIYAFUGetBootConfig->GetBootRes.Datalen = len + 1;  
+    pAMIYAFUGetBootConfig->GetBootRes.CRC32chksum= CalculateChksum((char *)&pAMIYAFUGetBootConfig->Status,pAMIYAFUGetBootConfig->GetBootRes.Datalen);
+
+    return (sizeof(AMIYAFUGetBootConfigRes_T) + len);
+}
+else
+{
+    AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+    pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetBootConfigReq->GetBootReq.Seqnum;
+    pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+    pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+    pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+    pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+    LastStatCode =   pAMIYAFUNotAcknowledge->ErrorCode;	
+						
+    return (sizeof(AMIYAFUNotAck));   
+}
+	
+}
+
+/*---------------------------------------
+ * AMIYAFUSetBootConfig
+ *---------------------------------------*/
+int AMIYAFUSetBootConfig ( unsigned char *pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+    AMIYAFUSetBootConfigReq_T *pAMIYAFUSetBootConfigReq = (AMIYAFUSetBootConfigReq_T *)pReq;
+
+if( ActivateFlashStatus == 0x01)
+{
+   
+    char *BootVal = (char *)YAFU_TEMP_SECTOR_BUFFER;	
+	
+    AMIYAFUSetBootConfigRes_T* pAMIYAFUSetBootConfig = (AMIYAFUSetBootConfigRes_T*)pRes;
+  
+    if(CalculateChksum((char *)&pAMIYAFUSetBootConfigReq->VarName[0],pAMIYAFUSetBootConfigReq->SetBootReq.Datalen) != pAMIYAFUSetBootConfigReq->SetBootReq.CRC32chksum)
+    {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUSetBootConfigReq->SetBootReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+	  LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+    }
+
+   
+    pReq += sizeof(AMIYAFUSetBootConfigReq_T);
+    memcpy(BootVal,pReq,(pAMIYAFUSetBootConfigReq->SetBootReq.Datalen - 65));
+    memset(pReq,0,(pAMIYAFUSetBootConfigReq->SetBootReq.Datalen - 65)); 	
+    pReq -= sizeof(AMIYAFUSetBootConfigReq_T);	
+
+    if(strcmp((char *)pAMIYAFUSetBootConfigReq->VarName,"baudrate") != 0)
+    	    setenv((char *)pAMIYAFUSetBootConfigReq->VarName,BootVal);
+  
+    saveenv();
+
+    pAMIYAFUSetBootConfig->Status = 0x01;
+
+    pAMIYAFUSetBootConfig->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U)pAMIYAFUSetBootConfig->CompletionCode;	
+    pAMIYAFUSetBootConfig->SetBootRes.Seqnum = pAMIYAFUSetBootConfigReq->SetBootReq.Seqnum;
+    pAMIYAFUSetBootConfig->SetBootRes.YafuCmd = pAMIYAFUSetBootConfigReq->SetBootReq.YafuCmd;
+    pAMIYAFUSetBootConfig->SetBootRes.Datalen = 0x01;	
+    pAMIYAFUSetBootConfig->SetBootRes.CRC32chksum= CalculateChksum((char *)&pAMIYAFUSetBootConfig->Status,pAMIYAFUSetBootConfig->SetBootRes.Datalen ); 	
+
+    return (sizeof(AMIYAFUSetBootConfigRes_T));
+}
+else
+{
+   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUSetBootConfigReq->SetBootReq.Seqnum;
+   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+   LastStatCode =  pAMIYAFUNotAcknowledge->ErrorCode;
+							   
+   return (sizeof(AMIYAFUNotAck)); 
+}
+   
+}
+
+/*---------------------------------------
+ * AMIYAFUGetBootVars
+ *---------------------------------------*/
+int AMIYAFUGetBootVars ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes ) 
+{
+    AMIYAFUGetBootVarsReq_T *pAMIYAFUGetBootVarsReq = (AMIYAFUGetBootVarsReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+   // int RetVal;
+    int BootVarlen =0;	
+    char *Buffer = (char *)YAFU_TEMP_SECTOR_BUFFER;	
+
+    AMIYAFUGetBootVarsRes_T* pAMIYAFUGetBootVars = (AMIYAFUGetBootVarsRes_T*)pRes;	 
+    pAMIYAFUGetBootVars->VarCount = 0X00;
+
+
+    Getenvvar ((uchar *)&pAMIYAFUGetBootVars->VarCount,(uchar *)Buffer,&BootVarlen); 
+
+    memcpy (( INT8U*) (pAMIYAFUGetBootVars + 1),
+                   ( INT8U*)Buffer,BootVarlen );
+
+    pAMIYAFUGetBootVars->GetBootRes.Datalen = BootVarlen + 1; 
+
+    pAMIYAFUGetBootVars->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U)pAMIYAFUGetBootVars->CompletionCode;	
+    pAMIYAFUGetBootVars->GetBootRes.Seqnum = pAMIYAFUGetBootVarsReq->GetBootReq.Seqnum;
+    pAMIYAFUGetBootVars->GetBootRes.YafuCmd = pAMIYAFUGetBootVarsReq->GetBootReq.YafuCmd;
+    pAMIYAFUGetBootVars->GetBootRes.CRC32chksum= CalculateChksum((char *)&pAMIYAFUGetBootVars->VarCount,pAMIYAFUGetBootVars->GetBootRes.Datalen);
+     
+    return (sizeof(AMIYAFUGetBootVarsRes_T)+ BootVarlen);
+}
+else
+{
+   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetBootVarsReq->GetBootReq.Seqnum;
+   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+							   
+   return (sizeof(AMIYAFUNotAck)); 
+} 
+
+}
+
+
+
+
+/*---------------------------------------
+ * AMIYAFUDeactivateFlash
+ *---------------------------------------*/
+int AMIYAFUDeactivateFlash (unsigned char*pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+
+    AMIYAFUDeactivateFlashReq_T *pAMIYAFUDeactivateFlashReq = (AMIYAFUDeactivateFlashReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+    AMIYAFUDeactivateFlashRes_T* pAMIYAFUDeactivateFlash = (AMIYAFUDeactivateFlashRes_T*)pRes;
+
+
+    if(pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.Datalen== 0x00)
+          pAMIYAFUDeactivateFlash->DeactivateFlashRes.Datalen= 0x01;
+    else
+    {
+
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+          pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+	   return (sizeof(AMIYAFUNotAck));
+
+    }
+
+    ActivateFlashStatus = 0x00;
+
+#ifndef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
+   if (gFWUploadSelector == IMAGE_1)
+    {
+        setenv("recentlyprogfw","1");
+	#ifdef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+            /*clearing error codes if any generated by failsafe boot*/
+            if( getenv("fwimage1corrupted") )
+            {
+               setenv("fwimage1corrupted",NULL);
+            }
+        #endif
+
+        saveenv();
+    }
+    else if (gFWUploadSelector == IMAGE_2)
+    {
+        setenv("recentlyprogfw","2");
+	#ifdef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+           /*clearing error codes if any generated by failsafe boot*/
+           if( getenv("fwimage2corrupted") )
+           {
+                  setenv("fwimage2corrupted",NULL);
+           }
+        #endif
+
+        saveenv();
+    }
+   
+#endif
+
+    pAMIYAFUDeactivateFlash->Status = 0x00;
+    pAMIYAFUDeactivateFlash->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U) pAMIYAFUDeactivateFlash->CompletionCode;
+    pAMIYAFUDeactivateFlash->DeactivateFlashRes.Seqnum = pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.Seqnum;
+    pAMIYAFUDeactivateFlash->DeactivateFlashRes.YafuCmd = pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.YafuCmd;
+    pAMIYAFUDeactivateFlash->DeactivateFlashRes.CRC32chksum= CalculateChksum((char *)&pAMIYAFUDeactivateFlash->Status,sizeof(INT8U));
+
+
+    return (sizeof(AMIYAFUDeactivateFlashRes_T));
+}
+else
+{
+    AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+    pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.Seqnum;
+    pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+    pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+    pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+    pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+    LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+    return (sizeof(AMIYAFUNotAck));
+}
+
+}
+
+
+/*---------------------------------------
+ * AMIYAFUResetDevice
+ *---------------------------------------*/
+int AMIYAFUResetDevice ( unsigned char *pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+
+    AMIYAFUResetDeviceReq_T *pAMIYAFUResetDeviceReq = (AMIYAFUResetDeviceReq_T *)pReq;
+    AMIYAFUResetDeviceRes_T* pAMIYAFUResetDevice = (AMIYAFUResetDeviceRes_T*)pRes;
+
+     if(CalculateChksum((char *)&pAMIYAFUResetDeviceReq->WaitSec,sizeof(INT16U)) != pAMIYAFUResetDeviceReq->ResetReq.CRC32chksum)
+   {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUResetDeviceReq->ResetReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+   }
+
+    if(pAMIYAFUResetDeviceReq->ResetReq.Datalen== 0x02)
+          pAMIYAFUResetDevice->ResetRes.Datalen= 0x01;
+    else
+    {
+
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+          pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUResetDeviceReq->ResetReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode =   pAMIYAFUNotAcknowledge->ErrorCode;
+
+	   return (sizeof(AMIYAFUNotAck));
+
+    }
+
+    gReset = 1;
+
+    pAMIYAFUResetDevice->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U) pAMIYAFUResetDevice->CompletionCode;
+    pAMIYAFUResetDevice->ResetRes.Seqnum = pAMIYAFUResetDeviceReq->ResetReq.Seqnum;
+    pAMIYAFUResetDevice->ResetRes.YafuCmd = pAMIYAFUResetDeviceReq->ResetReq.YafuCmd;
+    pAMIYAFUResetDevice->ResetRes.CRC32chksum=CalculateChksum((char *)&pAMIYAFUResetDevice->Status,sizeof(INT8U));
+
+    return (sizeof(AMIYAFUResetDeviceRes_T));
+}
+
+/**
+ * @fn CheckForBootOption
+ * @brief Check for valid boot selector option.
+ * @param[in] bootoption - boot selector option.
+ * @retval      0 - for valid boot selector option.
+ *              -1 - invalid boot option.
+ */
+static
+int CheckForBootOption(char bootoption)
+{
+    //Checking for valid Boot selector option
+    if(  (bootoption != AUTO_HIGH_VER_FW) && 
+         (bootoption != LOWER_IMAGE_FW ) && 
+         (bootoption != HIGHER_IMAGE_FW ) && 
+         (bootoption != AUTO_LOW_VER_FW) &&
+         (bootoption != MOST_RECENTLY_PROG_FW) && 
+         (bootoption != LEAST_RECENTLY_PROG_FW) )
+    {
+        return -1;
+    }
+
+return 0;
+}
+
+/**
+ * @fn IsfwuploadSelectorValid
+ * @brief Check for valid firmware upload selector.
+ * @param[in] uploadselector - firmware upload selector option.
+ * @retval      1 - for valid firmware upload selector.
+ *              0 - invalid firmware upload selector.
+ */
+static
+int  IsfwuploadSelectorValid(char uploadselector)
+{
+   return( (uploadselector == AUTO_INACTIVE_IMAGE)
+           || (uploadselector == IMAGE_1 )
+           || (uploadselector == IMAGE_2 )
+           || (uploadselector == IMAGE_BOTH )
+         );
+}
+
+/*
+*@fn AMIDualImageSupport
+*@brief This command helps to set and get the dual image params
+*@param pReq - Request for the command   
+*@param ReqLen - Request length for the command
+*@param pRes - Respose for the command
+*@return Returns size of AMIDualImageSupRes_T
+*/
+
+int AMIDualImageSupport( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+    AMIDualImageSupReq_T *pAMIDualImageSupReq = (AMIDualImageSupReq_T *)pReq;
+    AMIDualImageSupRes_T *pAMIDualImageSupRes = (AMIDualImageSupRes_T *)pRes;
+    u8 BootSelector, FwUploadSelector;
+    char *env_name;
+    char env_value[5];
+
+    if(ReqLen != m_DualResBits[pAMIDualImageSupReq->Parameter - 1])
+    {
+        *pRes = CC_REQ_INV_LEN;
+        return sizeof(INT8U); 
+    }
+ 
+#ifndef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
+      *pRes  = CC_INV_CMD;
+      return sizeof(INT8U);
+#endif
+
+    switch (pAMIDualImageSupReq->Parameter) //validating for command sub specifier
+    {
+        case SETFWBOOTSELECTOR: //0x01 - Set FW Boot Selector
+        
+		//Checking for valid Boot selector option
+		if( CheckForBootOption(pAMIDualImageSupReq->BootSelector) != 0 )
+		{
+			printf("Invalid Bootoption");
+			*pRes = CC_INV_DATA_FIELD;
+			return sizeof(INT8U); 
+		}
+				
+                memset(env_value,0,sizeof(env_value));
+                sprintf(env_value,"%d",pAMIDualImageSupReq->BootSelector);
+
+                setenv("bootselector", env_value);
+                saveenv();
+
+                pAMIDualImageSupRes->BootSelOpt.SetBootSelector = pAMIDualImageSupReq->BootSelector;
+                pAMIDualImageSupRes->CompletionCode = CC_NORMAL;
+                return sizeof(INT8U) + sizeof(pAMIDualImageSupRes->BootSelOpt.SetBootSelector);
+                break;
+	
+        case GETFWBOOTSELECTOR: //0x02 - Get FW Boot Selector           	   
+
+                env_name = getenv("bootselector");
+                if (env_name == NULL)
+                {
+                    return CC_UNSPECIFIED_ERR;
+                }
+                BootSelector = (u8)simple_strtoul(env_name,NULL,0);
+			  
+                //Checking for valid Boot selector option
+                if( CheckForBootOption(BootSelector) !=0 ) 
+                {
+                    return CC_UNSPECIFIED_ERR;
+                }
+				pAMIDualImageSupRes->BootSelOpt.GetBootSelector= BootSelector;
+                pAMIDualImageSupRes->CompletionCode = CC_NORMAL;
+                return sizeof(INT8U) + sizeof(pAMIDualImageSupRes->BootSelOpt.GetBootSelector);
+                break;
+       
+        case SETFWUPLOADSELECTOR: //0x03 Set FW Upload  Selector			   
+
+                FwUploadSelector =  pAMIDualImageSupReq->BootSelector;
+                //Checking for valid FW upload selector option 
+                if( IsfwuploadSelectorValid(FwUploadSelector) == 0 )
+                {      
+			     *pRes  = YAFU_CC_INVALID_DATA;
+			      return sizeof(INT8U);
+                }
+
+                gFWUploadSelector = FwUploadSelector;
+				pAMIDualImageSupRes->BootSelOpt.SetUploadSelector = gFWUploadSelector;
+                pAMIDualImageSupRes->CompletionCode = CC_NORMAL;
+                return sizeof(INT8U) + sizeof(pAMIDualImageSupRes->BootSelOpt.SetUploadSelector);
+                break;
+
+        case GETFWUPLOADSELECTOR: //0x04 Get FW Upload Selector               
+
+                FwUploadSelector = gFWUploadSelector;
+
+                //Checking for valid FW upload selector option 
+                if( IsfwuploadSelectorValid(FwUploadSelector) == 0 )
+                {      
+			     *pRes  = YAFU_CC_INVALID_DATA;
+			      return sizeof(INT8U);
+                }
+
+				pAMIDualImageSupRes->BootSelOpt.GetUploadSelector = FwUploadSelector;
+                pAMIDualImageSupRes->CompletionCode = CC_NORMAL;
+                return sizeof(INT8U) + sizeof(pAMIDualImageSupRes->BootSelOpt.GetUploadSelector);
+                break;
+	case GETCURACTIVEIMG:
+                pAMIDualImageSupRes->BootSelOpt.GetCurActiveImg = IMAGE_1;
+                pAMIDualImageSupRes->CompletionCode = CC_NORMAL;
+                return sizeof(INT8U) + sizeof(pAMIDualImageSupRes->BootSelOpt.GetCurActiveImg);
+                break;
+
+        default:
+                return CC_INV_CMD;
+    }
+
+return  CC_SUCCESS;
+}
+
+
+
+/*-------------------------------------------
+ * AMIYAFUDualImgSup
+ *------------------------------------------*/
+int AMIYAFUDualImgSup(unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+    AMIYAFUDualImgSupReq_T *pAMIYAFUDualImgSupReq = (AMIYAFUDualImgSupReq_T *)pReq;
+    AMIYAFUDualImgSupRes_T *pAMIYAFUDualImgSupRes = (AMIYAFUDualImgSupRes_T *)pRes;
+
+    if(CalculateChksum((char *)&pAMIYAFUDualImgSupReq->PreserveConf,pAMIYAFUDualImgSupReq->DualImgSupReq.Datalen) != pAMIYAFUDualImgSupReq->DualImgSupReq.CRC32chksum)
+    {
+         return YAFU_INVALID_CHKSUM;
+    }
+    pAMIYAFUDualImgSupRes->CompletionCode = YAFU_CC_NORMAL;
+    
+    return sizeof(AMIYAFUDualImgSupRes_T);
+}
+
+
+
+/*-----------------------
+ * Command Handler Type
+ *----------------------*/
+typedef int (*pCmdHndlr_T) (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes);
+
+/**** Command Handlers ****/
+typedef struct
+{
+	unsigned char	NetFnLUN;
+	unsigned char	Cmd;
+	pCmdHndlr_T		pHandler;
+	unsigned char	Len;
+
+} CmdInfo_T;
+
+/* Supported Commands */
+CmdInfo_T	m_CmdInfo [] = 
+{
+    { NETFN_APP,		CMD_GET_DEV_ID,		            	GET_DEV_ID, 	        		0x00    								},
+	{ NETFN_APP,		CMD_GET_CH_AUTH_CAPS,				GET_CH_AUTH_CAPS,				sizeof (GetChAuthCapReq_T) 				},	
+    { NETFN_APP,		CMD_GET_SESSION_CHALLENGE,      	GET_SESSION_CHALLENGE,  		sizeof (GetSesChallengeReq_T) 			},
+    { NETFN_APP,		CMD_ACTIVATE_SESSION, 		    	ACTIVATE_SESSION,       		sizeof (ActivateSesReq_T) 				},
+    { NETFN_APP,		CMD_SET_SESSION_PRIV_LEVEL,     	SET_SESSION_PRIV_LEVEL, 		sizeof (unsigned char) 					},
+    { NETFN_APP,		CMD_CLOSE_SESSION,              	CLOSE_SESSION,          		sizeof (CloseSesReq_T) 					},
+
+    { NETFN_OEM_AMI,   	CMD_AMI_YAFU_GET_FLASH_INFO,       	AMI_YAFU_GET_FLASH_INFO,        sizeof(AMIYAFUGetFlashInfoReq_T)		},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_GET_FMH_INFO, 			AMI_YAFU_GET_FMH_INFO,			sizeof(AMIYAFUGetFMHInfoReq_T) 			},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_GET_STATUS,		 	AMI_YAFU_GET_STATUS,			sizeof(AMIYAFUGetStatusReq_T) 			},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_ACTIVATE_FLASH,		AMI_YAFU_ACTIVATE_FLASH,		sizeof(AMIYAFUActivateFlashModeReq_T)	},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_PROTECT_FLASH,			AMI_YAFU_PROTECT_FLASH, 		sizeof(AMIYAFUProtectFlashReq_T) 		},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_ERASE_COPY_FLASH, 		AMI_YAFU_ERASE_COPY_FLASH,		sizeof(AMIYAFUEraseCopyFlashReq_T) 		},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_WRITE_MEMORY, 			AMI_YAFU_WRITE_MEMORY,			0xff 									},
+    { NETFN_OEM_AMI,   	CMD_AMI_YAFU_GET_BOOT_CONFIG,       AMI_YAFU_GET_BOOT_CONFIG,       sizeof(AMIYAFUGetBootConfigReq_T) 		},
+    { NETFN_OEM_AMI,   	CMD_AMI_YAFU_SET_BOOT_CONFIG,       AMI_YAFU_SET_BOOT_CONFIG,       0xff									},
+    { NETFN_OEM_AMI,   	CMD_AMI_YAFU_GET_BOOT_VARS,         AMI_YAFU_GET_BOOT_VARS,         0xff									}, 
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_DEACTIVATE_FLASH_MODE,	AMI_YAFU_DEACTIVATE_FLASH_MODE, sizeof(AMIYAFUDeactivateFlashReq_T) 	},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_RESET_DEVICE, 			AMI_YAFU_RESET_DEVICE , 		sizeof(AMIYAFUResetDeviceReq_T) 		},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_DUAL_IMAGE_SUP,        AMI_YAFU_DUAL_IMG_SUP,          sizeof(AMIYAFUDualImgSupReq_T)			},
+    { NETFN_OEM_AMI, 	CMD_AMI_DUAL_IMG_SUPPORT,           AMI_DUAL_IMG_SUPPORT,   	 	0xFF}
+};
+
+/*
+ * Process IPMI Command
+ */
+unsigned long
+ProcessIPMICmd (IPMICmdData_T* pIPMICmd)
+{
+	int i;
+	unsigned char Err = CC_NORMAL;
+	unsigned long	Responselen;
+
+	/* Process this message through the SMM message handler */
+	for (i = 0; i < sizeof (m_CmdInfo) / sizeof (CmdInfo_T); i++)
+	{
+		if ((m_CmdInfo [i].NetFnLUN == (pIPMICmd->NetFnLUN >> 2)) &&
+			(m_CmdInfo [i].Cmd	    == pIPMICmd->Cmd))
+		{
+			break;
+		}
+	}
+
+	/* If we have found the handler - handle the command */
+	if (i != sizeof (m_CmdInfo) / sizeof (CmdInfo_T))
+	{
+		/* Check for the request length */
+		if (0xff != m_CmdInfo [i].Len)
+		{
+			if (pIPMICmd->ReqLen != m_CmdInfo [i].Len)
+			{
+				printf("INV LEN: Recv len = %ld,exp len = %d\n",pIPMICmd->ReqLen,m_CmdInfo [i].Len);
+				Err = CC_REQ_INV_LEN;
+			}
+			
+		}
+
+	}
+	else
+	{
+		Err = CC_INV_CMD;
+	}
+
+    if (Err == CC_NORMAL)
+	{
+		pIPMICmd->ResLen = m_CmdInfo [i].pHandler (pIPMICmd->ReqData, pIPMICmd->ReqLen, pIPMICmd->ResData); 
+		Responselen = pIPMICmd->ResLen;	 
+    }
+    else
+    {
+        pIPMICmd->ResData [0] = Err;
+		pIPMICmd->ResLen	  = 1;
+		Responselen = 1;
+    }
+	return Responselen;
+}
+
+
+
+
+
+
+
+
