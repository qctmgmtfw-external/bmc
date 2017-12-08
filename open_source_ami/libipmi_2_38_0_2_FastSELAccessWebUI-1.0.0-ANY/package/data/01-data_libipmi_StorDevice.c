--- .pristine/libipmi-2.38.0-src/data/libipmi_StorDevice.c Thu Sep 15 12:23:01 2011
+++ source/libipmi-2.38.0-src/data/libipmi_StorDevice.c Thu Sep 15 14:26:44 2011
@@ -447,11 +447,8 @@
 					int timeout)
 {
 
-	GetSELReq_T GetSELReqData = {0x00,0x00,0x00,0xFF}; // Request packet to get the very
 	// first SEL record and complete record,
 	// starting at offset 0 of each record.
-	uint8 *GetSELResData = NULL;
-	SELEventRecord_T *ptrToSELRecord  = NULL;
 	uint32 DataLen = 0;
 	uint16 wRet = 0;
 	SDRRepositoryAllocInfo_T SDRAllocationInfo;
@@ -461,43 +458,20 @@
 	uint8 *pSDRBuff = NULL;
 	SDRHeader *pSDRHeader = NULL;
 	uint8 *pSDRStart = NULL;
-	int nNumSelEntries = 0;
+	uint32 nNumSelEntries = 0;
 	int q = 0;
-
-
-
 
 
 	/***********************************Get all Sel Entries now*********************************************/
 	/* We want to read full records */
 	DataLen = sizeof(GetSELRes_T) + sizeof(SELEventRecord_T);
-	GetSELResData = (uint8 *) malloc(DataLen);
-
-	/* Fill in the Record data first */
-	nNumSelEntries = 0;
-	while(1)
-	{
-		wRet = IPMICMD_GetSELEntry(pSession, &GetSELReqData, GetSELResData, DataLen,timeout);
-		if(wRet == LIBIPMI_E_SUCCESS)
-		{
-			ptrToSELRecord = (SELEventRecord_T *) (GetSELResData + sizeof(GetSELRes_T));
-			memcpy(&(pSELEntriesBuffer[nNumSelEntries].EventRecord),ptrToSELRecord,sizeof(SELEventRecord_T));
-			nNumSelEntries++;
-			GetSELReqData.RecID = ((GetSELRes_T *) GetSELResData)->NextRecID;
-			if(  ((GetSELRes_T *) GetSELResData)->NextRecID == 0xFFFF)
-			{
-				/* End of SEL reached. */
-				break;
-			}
-		}
-		else
-		{
-			TCRIT("LIBIPMI_HL_GetAllSELEntriesWithSensorName: Error getting SEL entry\n");
-			break;
-		}
-	}
-	free(GetSELResData);
-	/***********************************Get all Sel Entries now*********************************************/
+
+	wRet = LIBIPMI_HL_AMIGetSELEntires(pSession,pSELEntriesBuffer,&nNumSelEntries,timeout);
+	if(wRet != LIBIPMI_E_SUCCESS)
+	{
+		TCRIT("LIBIPMI_HL_AMIGetSELEntires : Error getting SEL Entries \n");
+		return wRet;
+	}
 
 
 	/*************************************Get The SDR once so that we can get sensor names*******************/
@@ -557,11 +531,11 @@
 
 
 
-				if((pSELEntriesBuffer[q].EventRecord.GenID[0] == *pTmp) && (pSELEntriesBuffer[q].EventRecord.GenID[1] == *(pTmp+1)))
+				if((pSELEntriesBuffer[q].EventRecord.EvtRecord.GenID[0] == *pTmp) && (pSELEntriesBuffer[q].EventRecord.EvtRecord.GenID[1] == *(pTmp+1)))
 				{
 
 					/* Now compare the sensor number */
-					if(pSELEntriesBuffer[q].EventRecord.SensorNum == *(pTmp+2))
+					if(pSELEntriesBuffer[q].EventRecord.EvtRecord.SensorNum == *(pTmp+2))
 					{
 						/* Get the Name and return */
 						if(pSDRHeader->RecordType == 0x01)
