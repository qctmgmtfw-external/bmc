--- .pristine/libipmi-2.38.0-src/data/libipmi_AMIOEM.c Thu Sep 15 12:23:01 2011
+++ source/libipmi-2.38.0-src/data/libipmi_AMIOEM.c Thu Sep 15 21:46:13 2011
@@ -22,6 +22,7 @@
 
 #include "dbgout.h"
 
+#include "libipmi_AMIOEM.h"
 #include "libipmi_session.h"
 #include "libipmi_errorcodes.h"
 #include "IPMI_AMIDevice.h"
@@ -32,6 +33,9 @@
 #include "IPMI_AMIConf.h"
 #include "ncml.h"
 #include "AMIRestoreDefaults.h"
+#include "libipmi_StorDevice.h"
+#include "SELRecord.h"
+#include "SEL.h"
 
 /***** Firewall structures *****/
 
@@ -3031,5 +3035,165 @@
     return (uint16)Res.CompletionCode;
 }
 
-
-
+/*
+*@fn IPMICMD_AMIGetSELEntires
+*@brief
+*@param pSession - IPMI Session Handle
+*@param pAMIGetSelEntriesReq - Pointer to the request of AMI Get SEL Entries command
+*@param pAMIGetSelEntiresRes - Pointer to the response of AMI Get SEL Entries command
+*@param timeout - Timeout Value
+**/
+uint16 IPMICMD_AMIGetSELEntires(IPMI20_SESSION_T *pSession,AMIGetSELEntriesReq_T *pAMIGetSelEntriesReq,
+                                                   AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes,int timeout)
+{
+    uint16 wRet;
+    uint32 dwResLen;
+    dwResLen = sizeof(AMIGetSELEntriesRes_T);
+
+    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
+                        (NETFN_AMI << 2), CMD_AMI_GET_SEL_ENTIRES,
+                        (uint8 *)pAMIGetSelEntriesReq, sizeof(AMIGetSELEntriesReq_T),
+                        (uint8*)pAMIGetSelEntiresRes, &dwResLen,
+                        timeout);
+
+    return (wRet);
+}
+
+/*
+*@fn LIBIPMI_HL_AMIGetSELEntires
+*@brief High Level command to retrieve the total SEL Entries
+*@param pSession - IPMI Session Handle
+*@param pSELEntriesBuffer - Pointer to hold the retrieved SEL Entries
+*@param nNumSelEntries - Pointer which hold the no. of SEL Entries retrieved
+*@param timeout - Timeout Value
+*@return Returns LIBIPMI_E_SUCCESS on success
+*@return Returns proper completion code on error
+**/
+uint16 LIBIPMI_HL_AMIGetSELEntires(IPMI20_SESSION_T *pSession,SELEventRecordWithSensorName_T *pSELEntriesBuffer,uint32 *nNumSelEntries,int timeout)
+{
+    AMIGetSELEntriesReq_T pAMIGetSelEntriesReq = {0};
+    AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes;
+    SELEventRecordWithSensorName_T *CircSELEntries;
+    uint8 *SELEntries;
+    uint16 wRet = 0;
+    uint16 i=0,j=0,recindex=0;
+    uint16 LastRecID = 0xFFFF,MaxSELEntries=0;
+
+    SELEntries = (uint8 *)malloc(MAX_SEL_RETRIEVAL_SIZE);
+    if(SELEntries == NULL)
+    {
+        TCRIT("Memory allocation error in LIBIPMI_HL_AMIGetSELEntires \n");
+        wRet = 0xFF;
+        return wRet;
+    }
+
+    memset(SELEntries,0,MAX_SEL_RETRIEVAL_SIZE);
+    pAMIGetSelEntiresRes = (AMIGetSELEntriesRes_T *)SELEntries;
+
+    do
+    {
+        wRet = IPMICMD_AMIGetSELEntires(pSession,&pAMIGetSelEntriesReq,(AMIGetSELEntriesRes_T *)SELEntries,timeout);
+        if(wRet == LIBIPMI_E_SUCCESS)
+        {
+            for(i = 0; i < pAMIGetSelEntiresRes->Noofentries; i++)
+            {
+                memcpy((uint8 *)&pSELEntriesBuffer[i+*nNumSelEntries].EventRecord,&(SELEntries)[sizeof(AMIGetSELEntriesRes_T) + (i * sizeof(SELRec_T))],
+                               sizeof(SELRec_T));
+            }
+
+            if(pAMIGetSelEntriesReq.Noofentretrieved == 0x00)
+            {
+                LastRecID = pAMIGetSelEntiresRes->LastRecID;
+            }
+
+            if(pAMIGetSelEntiresRes->Status == FULL_SEL_ENTRIES)
+            {
+                *nNumSelEntries += pAMIGetSelEntiresRes->Noofentries;
+                break;
+            }
+            else
+            {
+                *nNumSelEntries += pAMIGetSelEntiresRes->Noofentries;
+                pAMIGetSelEntriesReq.Noofentretrieved = *nNumSelEntries;
+                memset(SELEntries,0,MAX_SEL_RETRIEVAL_SIZE);
+                continue;
+            }
+        }
+        else if((wRet & 0xFF) == LIBIPMI_E_SEL_CLEARED)
+        {
+            memset(SELEntries,0,MAX_SEL_RETRIEVAL_SIZE);
+            memset((uint8*)&pSELEntriesBuffer[0],0,sizeof(SELEventRecordWithSensorName_T) * *nNumSelEntries);
+            *nNumSelEntries =  pAMIGetSelEntriesReq.Noofentretrieved = 0;
+            continue;
+        }
+        else if((wRet & 0xFF)  == LIBIPMI_E_SEL_EMPTY)
+        {
+            /* Returning LIBIPMI_E_SUCCESS when sel repo is empty as
+                 per WebUI expectations. Will be removed once changes are made in WebUI*/
+            wRet = LIBIPMI_E_SUCCESS;
+            return wRet;
+        }
+        else
+        {
+            break;
+        }
+    }while(1);
+
+    free(SELEntries);
+
+
+    if(wRet == LIBIPMI_E_SUCCESS)
+    {
+        if(LastRecID != 0xFFFF)
+        {
+            wRet = LIBIPMI_HL_GetMaxPossibleSELEntries(pSession,&MaxSELEntries,timeout);
+            if(wRet == LIBIPMI_E_SUCCESS)
+            {
+                 if(LastRecID > MaxSELEntries)
+                {
+                    for(i = 0; i < *nNumSelEntries; i++)
+                    {
+                        if(pSELEntriesBuffer[i].EventRecord.EvtRecord.hdr.ID == LastRecID)
+                        {
+                            recindex = i;
+                            break;
+                        }
+                    }
+
+                    /* Last Record is not found if reached till end of SEL Entries*/
+                    if(i == *nNumSelEntries)
+                    {
+                        wRet = 0xFF;
+                        return wRet;
+                    }
+
+                    /*SEL Records added lastely will be displayed first */
+                    CircSELEntries = (SELEventRecordWithSensorName_T *)malloc (sizeof(SELEventRecordWithSensorName_T) * (*nNumSelEntries));
+                    if(CircSELEntries != NULL)
+                    {
+                        memset(CircSELEntries,0,sizeof(SELEventRecordWithSensorName_T) * (*nNumSelEntries));
+                        for(i=0,j=recindex;i<=recindex;i++,j--)
+                        {
+                            memcpy((uint8 *)&CircSELEntries[i],(uint8*)&pSELEntriesBuffer[j],sizeof(SELEventRecordWithSensorName_T));
+                        }
+
+                        for(j = recindex + 1,i = 1; j < *nNumSelEntries; j++,i++)
+                        {
+                            memcpy((uint8*)&CircSELEntries[j],(uint8*)&pSELEntriesBuffer[*nNumSelEntries - i ],sizeof(SELEventRecordWithSensorName_T));
+                        }
+
+                        for(i = 0,j=1;i< *nNumSelEntries; i++,j++)
+                        {
+                            memcpy((uint8*)&pSELEntriesBuffer[i],(uint8*)&CircSELEntries[*nNumSelEntries - j],sizeof(SELEventRecordWithSensorName_T));
+                        }
+                        free(CircSELEntries);
+                        
+                    }
+                }
+            }
+        }
+    }
+
+    return (wRet);
+}
+
