--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Thu Sep 15 12:23:59 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Thu Sep 15 14:21:53 2011
@@ -53,6 +53,8 @@
 #ifdef CONFIG_SPX_FEATURE_CIRCULAR_SEL
 #include "SEL.h"
 #endif
+
+#define MAX_FULL_SEL_ENTRIES 0x4000
 
 /*
  * Service commands
@@ -1670,3 +1672,52 @@
 
 }
 
+
+/*
+*@fn AMIGetSELEntires
+*@param This function retrieves the SEL entries
+*@return Returns CC_NORMAL
+*/
+int AMIGetSELEntires(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst)
+{
+    AMIGetSELEntriesReq_T *pAMIGetSelEntriesReq = (AMIGetSELEntriesReq_T *)pReq;
+    AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes = (AMIGetSELEntriesRes_T *)pRes;
+    BMCInfo_t *pBMCInfo = &g_BMCInfo[BMCInst];
+    SELRepository_T* m_sel = (SELRepository_T*)GetSDRSELNVRAddr((pBMCInfo->IpmiConfig.SDRAllocationSize * 1024), BMCInst);
+
+    OS_THREAD_MUTEX_ACQUIRE(&pBMCInfo->SELMutex, WAIT_INFINITE);
+    if(((m_sel->NumRecords - pAMIGetSelEntriesReq->Noofentretrieved) * sizeof(SELRec_T)) < MAX_FULL_SEL_ENTRIES)
+    {
+        pAMIGetSelEntiresRes->Status = FULL_SEL_ENTRIES;
+        pAMIGetSelEntiresRes->Noofentries = m_sel->NumRecords - pAMIGetSelEntriesReq->Noofentretrieved;
+    }
+    else if(m_sel->NumRecords == SEL_EMPTY_REPOSITORY)
+    {
+        pAMIGetSelEntiresRes->CompletionCode = OEMCC_SEL_EMPTY_REPOSITORY;
+        return sizeof(*pRes);
+    }
+    else if (m_sel->NumRecords < pAMIGetSelEntriesReq->Noofentretrieved)
+    {
+        pAMIGetSelEntiresRes->CompletionCode = OEMCC_SEL_CLEARED;
+        return sizeof(*pRes);
+    }
+    else
+    {
+        pAMIGetSelEntiresRes->Status = PARTIAL_SEL_ENTRIES;
+        pAMIGetSelEntiresRes->Noofentries = ((MAX_FULL_SEL_ENTRIES - sizeof(AMIGetSELEntriesRes_T))/sizeof(SELRec_T));
+    }
+
+    memcpy((INT8U*)(pAMIGetSelEntiresRes + 1),(INT8U*)&m_sel->SELRecord[pAMIGetSelEntriesReq->Noofentretrieved],
+                  sizeof(SELRec_T)*pAMIGetSelEntiresRes->Noofentries);
+
+    LOCK_BMC_SHARED_MEM(BMCInst);
+    pAMIGetSelEntiresRes->LastRecID = BMC_GET_SHARED_MEM(BMCInst)->m_LastRecID;
+    UNLOCK_BMC_SHARED_MEM (BMCInst);
+
+    pAMIGetSelEntiresRes->CompletionCode = CC_NORMAL;
+    OS_THREAD_MUTEX_RELEASE(&pBMCInfo->SELMutex);
+
+    return sizeof(AMIGetSELEntriesRes_T) + (sizeof(SELRec_T) * pAMIGetSelEntiresRes->Noofentries);
+}
+
+
