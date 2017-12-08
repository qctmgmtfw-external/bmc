--- .pristine/libipmi-2.38.0-src/data/libipmi_AMIOEM.c Fri Sep 16 23:41:46 2011
+++ source/libipmi-2.38.0-src/data/libipmi_AMIOEM.c Fri Sep 16 23:57:16 2011
@@ -3197,3 +3197,61 @@
     return (wRet);
 }
 
+/*
+*@fn IPMICMD_AMIGetSensorInfo
+*@brief
+*@param pSession - IPMI Session Handle
+*@param pAMIGetSensorInfoReq - Pointer to the request of AMI Get SensorInfo command
+*@param pAMIGetSensorInfoRes - Pointer to the response of AMI Get SensorInfo command
+*@param timeout - Timeout Value
+**/
+uint16 IPMICMD_AMIGetSensorInfo(IPMI20_SESSION_T *pSession, AMIGetSensorInfoRes_T *pAMIGetSensorInfoRes,int timeout)
+{
+    uint16 wRet;
+    uint32 dwResLen;
+    uint8 Req[20];
+    dwResLen = sizeof(AMIGetSensorInfoRes_T);
+
+    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
+                        (NETFN_AMI << 2), CMD_AMI_GET_SENSOR_INFO,
+                        Req, 0,
+                        (uint8*)pAMIGetSensorInfoRes, &dwResLen,
+                        timeout);
+
+    return (wRet);
+}
+
+uint16 LIBIPMI_HL_AMIGetSensorInfo(IPMI20_SESSION_T *pSession,SenInfo_T *pSensorInfo,uint32 *nNumSensor,int timeout)
+{
+    uint16 wRet = 0;
+    AMIGetSensorInfoRes_T *pAMIGetSensorInfoRes;
+    uint8 *SensorEntries;
+    int i;
+
+    SensorEntries = (uint8 *)malloc(MAX_SENSOR_INFO_SIZE);
+    if(SensorEntries == NULL)
+    {
+        TCRIT("Memory allocation error in LIBIPMI_HL_AMIGetSELEntires \n");
+        wRet = 0xFF;
+        return wRet;
+    }
+
+    memset(SensorEntries,0,MAX_SENSOR_INFO_SIZE);
+    pAMIGetSensorInfoRes = (AMIGetSensorInfoRes_T *)SensorEntries;
+    wRet = IPMICMD_AMIGetSensorInfo(pSession,(AMIGetSensorInfoRes_T *)SensorEntries,timeout);
+    if(wRet == LIBIPMI_E_SUCCESS)
+    {
+        *nNumSensor = pAMIGetSensorInfoRes->Noofentries;
+        for(i = 0; i < pAMIGetSensorInfoRes->Noofentries; i++)
+        {
+            memcpy((uint8 *)&pSensorInfo[i],&SensorEntries[sizeof(AMIGetSensorInfoRes_T) + (i * sizeof(SenInfo_T))],sizeof(SenInfo_T) );
+        }
+    }
+    else
+    {
+        TCRIT("error getting sensor info %d\n",wRet);
+    }
+    free(SensorEntries);
+    return (wRet);
+}
+
