--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Fri Sep 16 23:42:32 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Sat Sep 17 00:00:09 2011
@@ -45,6 +45,8 @@
 #include <dlfcn.h>
 #include "stunnel_cfg.h"
 #include "unix.h"
+#include "Sensor.h"
+#include "SDR.h"
 
 #ifdef CONFIG_SPX_FEATURE_SNMP_SUPPORT
 #include "snmpusers.h"
@@ -1720,4 +1722,190 @@
     return sizeof(AMIGetSELEntriesRes_T) + (sizeof(SELRec_T) * pAMIGetSelEntiresRes->Noofentries);
 }
 
-
+/**
+ * @fn AMIGetSensorInfo
+ * @brief This function Gets all the Sensor Info needed for Web.
+ * @param Request message and BMC instance
+ * @return Proper completion code
+ */
+int AMIGetSenforInfo(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst)
+{
+    AMIGetSensorInfoRes_T *pAMIGetSensorInfoRes = (AMIGetSensorInfoRes_T *)pRes;
+    BMCInfo_t *pBMCInfo = &g_BMCInfo[BMCInst];
+    INT8U*   pValidSensor = NULL;
+    INT8U       SensorIndex = 0;
+    SensorInfo_T    pSensor ;
+    SenInfo_T       SensorInfo;
+    BOOL SensorIsSigned = FALSE;
+    INT16U SensorReading = 0;
+    SensorSharedMem_T*    pSMSharedMem;
+    _FAR_  SDRRecHdr_T*     pSDRRec;
+    _FAR_ FullSensorRec_T*      FullSDR;
+    _FAR_ CompactSensorRec_T*   CompSDR;
+    int i = 0;
+
+    /* Get the Sensor Shared Memory */
+    pSMSharedMem = (_FAR_ SensorSharedMem_T*)&pBMCInfo->SensorSharedMem;
+
+    if(pBMCInfo->SenConfig.ValidSensorCnt == 0)
+    {
+        pAMIGetSensorInfoRes->CompletionCode = OEMCC_SENSOR_INFO_EMPTY;
+        return sizeof(*pRes);
+    }
+
+    OS_THREAD_MUTEX_ACQUIRE(&pBMCInfo->m_hSMSharedMemMutex, SHARED_MEM_TIMEOUT);
+    pValidSensor = (INT8U*)(pAMIGetSensorInfoRes+1);
+    for(i = 0; i < pBMCInfo->SenConfig.ValidSensorCnt; i++)
+    {
+        SensorIndex = pBMCInfo->SenConfig.ValidSensorList[i];
+        pSensor = pSMSharedMem->SensorInfo[SensorIndex];
+
+        /*Copy the SDR Header*/
+        memcpy(&SensorInfo.hdr,pSensor.SDRRec,sizeof(SDRRecHdr_T));
+
+        SensorInfo.SensorNumber = pSensor.SensorNumber;
+        SensorInfo.SensorTypeCode = pSensor.SensorTypeCode;
+        SensorInfo.EventTypeCode = pSensor.EventTypeCode;
+
+        SensorInfo.Units1 = pSensor.Units1;
+        SensorInfo.Units2 = pSensor.Units2;
+        SensorInfo.Units3 = pSensor.Units3;
+
+        SensorInfo.M_LSB = pSensor.M_LSB;
+        SensorInfo.M_MSB_Tolerance = pSensor.M_MSB_Tolerance;
+        SensorInfo.B_LSB = pSensor.B_LSB;
+        SensorInfo.B_MSB_Accuracy = pSensor.B_MSB_Accuracy;
+        SensorInfo.Accuracy_MSB_Exp = pSensor.Accuracy_MSB_Exp;
+        SensorInfo.RExp_BExp = pSensor.RExp_BExp;
+
+        SensorInfo.LowerNonCritical = pSensor.LowerNonCritical;
+        SensorInfo.LowerCritical = pSensor.LowerCritical;
+        SensorInfo.LowerNonRecoverable = pSensor.LowerNonRecoverable;
+        SensorInfo.UpperNonCritical = pSensor.UpperNonCritical;
+        SensorInfo.UpperCritical = pSensor.UpperCritical;
+        SensorInfo.UpperNonRecoverable= pSensor.UpperNonRecoverable;
+
+        SensorInfo.Flags = pSensor.EventFlags & 0xe0;
+        if((pSensor.EventFlags & BIT5) != 0)
+        {
+            SensorInfo.SensorReading = 0;
+        }
+
+        SensorReading = pSensor.SensorReading;
+        SensorInfo.SensorReading = 0;
+        SensorIsSigned = ( 0 != (pSensor.InternalFlags & BIT1));
+
+        if (THRESHOLD_SENSOR_CLASS  == pSensor.EventTypeCode)
+        {
+            SensorInfo.SensorReading = (SensorReading & 0x00FF);
+            SensorInfo.ComparisonStatus = 0;
+            if((pSensor.DeassertionEventEnablesByte2 & BIT6) == BIT6 )
+            {
+                if (CompareValues(SensorIsSigned, SensorInfo.SensorReading, pSensor.UpperNonRecoverable) >= 0)
+                {
+                    SensorInfo.ComparisonStatus |= BIT5;
+                }
+            }
+            if((pSensor.DeassertionEventEnablesByte2 & BIT5) == BIT5 )
+            {
+                if (CompareValues(SensorIsSigned, SensorInfo.SensorReading, pSensor.UpperCritical) >= 0)
+                {
+                    SensorInfo.ComparisonStatus |= BIT4;
+                }
+            }
+            if((pSensor.DeassertionEventEnablesByte2 & BIT4) == BIT4 )
+            {
+                if (CompareValues(SensorIsSigned, SensorInfo.SensorReading, pSensor.UpperNonCritical) >= 0)
+                {
+                    SensorInfo.ComparisonStatus |= BIT3;
+                }
+            }
+            if((pSensor.AssertionEventEnablesByte2 & BIT6) == BIT6 )
+            {
+                if (CompareValues(SensorIsSigned, SensorInfo.SensorReading, pSensor.LowerNonRecoverable) <= 0)
+                {
+                    SensorInfo.ComparisonStatus |= BIT2;
+                }
+            }
+            if((pSensor.AssertionEventEnablesByte2 & BIT5) == BIT5 )
+            {
+                if (CompareValues(SensorIsSigned, SensorInfo.SensorReading, pSensor.LowerCritical) <= 0)
+                {
+                    SensorInfo.ComparisonStatus |= BIT1;
+                }
+            }
+            if((pSensor.AssertionEventEnablesByte2 & BIT4) == BIT4 )
+            {
+                if (CompareValues(SensorIsSigned, SensorInfo.SensorReading, pSensor.LowerNonCritical) <= 0)
+                {
+                    SensorInfo.ComparisonStatus |= BIT0;
+                }
+            }
+
+            SensorInfo.ComparisonStatus &= ((pSensor.SettableThreshMask >>  8) & 0xFF);
+            SensorInfo.OptionalStatus = 0;
+            // For Threshold sensor, [7:6] - reserved. Returned as 1b. Ignore on read.
+            SensorInfo.ComparisonStatus |= THRESHOLD_RESERVED_BIT;
+        }
+        else
+        {
+            SensorInfo.ComparisonStatus = (INT8U) (SensorReading & 0x00FF);
+            SensorInfo.OptionalStatus   = (INT8U) (SensorReading >> 8);
+            // For Discrete sensor, [7] - reserved. Returned as 1b. Ignore on read.
+            SensorInfo.OptionalStatus  |= DISCRETE_RESERVED_BIT;
+        }
+
+        if((pSensor.EventFlags & BIT7) == 0)
+        {
+            SensorInfo.AssertionEventByte1 = 0;
+            SensorInfo.AssertionEventByte2 = 0;
+            SensorInfo.DeassertionEventByte1 = 0;
+            SensorInfo.DeassertionEventByte2 = 0;
+        }
+        if((pSensor.SensorCaps & BIT6) == 0)
+        {
+             SensorInfo.AssertionEventByte1    = (pSensor.AssertionHistoryByte1 & pSensor.AssertionEventEnablesByte1);
+             SensorInfo.AssertionEventByte2    = (pSensor.AssertionHistoryByte2 & pSensor.AssertionEventEnablesByte2);
+             SensorInfo.DeassertionEventByte1    = (pSensor.DeassertionHistoryByte1 & pSensor.DeassertionEventEnablesByte1);
+             SensorInfo.DeassertionEventByte2    = (pSensor.DeassertionHistoryByte2 & pSensor.DeassertionEventEnablesByte2);
+        }
+        else
+        {
+             SensorInfo.AssertionEventByte1    = (pSensor.AssertionEventOccuredByte1 & pSensor.AssertionEventEnablesByte1);
+             SensorInfo.AssertionEventByte2    = (pSensor.AssertionEventOccuredByte2 & pSensor.AssertionEventEnablesByte2);
+             SensorInfo.DeassertionEventByte1    = (pSensor.DeassertionEventOccuredByte1 & pSensor.DeassertionEventEnablesByte1);
+             SensorInfo.DeassertionEventByte2    = (pSensor.DeassertionEventOccuredByte2 & pSensor.DeassertionEventEnablesByte2);
+        }
+        pSDRRec = GetSDRRec(pSensor.SDRRec->ID,BMCInst);
+        if(pSensor.SDRRec->Type  == FULL_SDR_REC)       /*Full SDR*/
+        {
+            FullSDR = (_FAR_ FullSensorRec_T *)pSDRRec;
+            SensorInfo.OwnerID = FullSDR->OwnerID;
+            SensorInfo.OwnerLUN= FullSDR->OwnerLUN;
+            SensorInfo.MaxReading = FullSDR->MaxReading;
+            SensorInfo.MinReading = FullSDR->MinReading;
+            SensorInfo.Linearization = FullSDR->Linearization;
+            strncpy(SensorInfo.SensorName,FullSDR->IDStr,MAX_ID_STR_LEN);
+        }
+        else if(pSensor.SDRRec->Type == COMPACT_SDR_REC)   /*Compact SDR*/
+        {
+            CompSDR = (_FAR_ CompactSensorRec_T *)pSDRRec;
+            SensorInfo.OwnerID = CompSDR->OwnerID;
+            SensorInfo.OwnerLUN= CompSDR->OwnerLUN;
+            SensorInfo.MaxReading = 0;
+            SensorInfo.MinReading = 0;
+            SensorInfo.Linearization = 0;
+            strncpy(SensorInfo.SensorName,CompSDR->IDStr,MAX_ID_STR_LEN);
+        }
+        memcpy(pValidSensor,(INT8U *)&SensorInfo,sizeof(SenInfo_T));
+        pValidSensor = pValidSensor + sizeof(SenInfo_T);
+    }
+    OS_THREAD_MUTEX_RELEASE(&pBMCInfo->m_hSMSharedMemMutex);
+
+    pAMIGetSensorInfoRes->CompletionCode = CC_NORMAL;
+    pAMIGetSensorInfoRes->Noofentries = pBMCInfo->SenConfig.ValidSensorCnt;
+
+    return sizeof(AMIGetSensorInfoRes_T) + (sizeof(SenInfo_T) * pBMCInfo->SenConfig.ValidSensorCnt);
+
+}
+
