--- .pristine/libifc-1.98.0-src/data/webifc_StorDevice.c Thu Sep 15 22:27:15 2011
+++ source/libifc-1.98.0-src/data/webifc_StorDevice.c Thu Sep 15 22:29:00 2011
@@ -119,14 +119,14 @@
 	switch(gColToSortOn)
 	{
 		case SORT_RECID:
-			return pE1->EventRecord.hdr.ID - pE2->EventRecord.hdr.ID;
+			return pE1->EventRecord.EvtRecord.hdr.ID - pE2->EventRecord.EvtRecord.hdr.ID;
 
 		case SORT_TIMESTAMP:
-			val = ipmitoh_u32(pE1->EventRecord.hdr.TimeStamp) - ipmitoh_u32(pE2->EventRecord.hdr.TimeStamp);
+			val = ipmitoh_u32(pE1->EventRecord.EvtRecord.hdr.TimeStamp) - ipmitoh_u32(pE2->EventRecord.EvtRecord.hdr.TimeStamp);
 			if(val == 0)
 			{
 				//if timestamps are equal then do it on event ID
-				return pE1->EventRecord.hdr.ID - pE2->EventRecord.hdr.ID;
+				return pE1->EventRecord.EvtRecord.hdr.ID - pE2->EventRecord.EvtRecord.hdr.ID;
 			}
 			else
 				return val;
@@ -134,7 +134,7 @@
 		case SORT_SENSORNAME:
 			return strcmp((char *)pE1->SensorName,(char *)pE2->SensorName);
 		case SORT_SENSORTYPE:
-			return pE1->EventRecord.SensorType - pE2->EventRecord.SensorType;
+			return pE1->EventRecord.EvtRecord.SensorType - pE2->EventRecord.EvtRecord.SensorType;
 	}
 
 	return 0;
@@ -143,7 +143,7 @@
 
 WEBIFC_HAPIFN_DEFINE( WEBIFC_IPMICMD_HL_GetAllSELEntriesSorted )
 {
-	int wRet = 0;
+	int wRet = RPC_HAPI_SUCCESS;
 	unsigned short nEntries = 0;
 	SELEventRecordWithSensorName_T *pSELBuff = NULL;
 	uint8 *pStartBuff = NULL;
@@ -173,7 +173,7 @@
 	/* Get number of SEL records in the system at this time */
 	TDBG("befor num sel entries\n");
 	wRet = LIBIPMI_HL_GetNumOfSELEntries(&((wc_get_ipmi_session(wp))->IPMISession), &nEntries, DEFAULT_IPMITIMEOUT);
-	if(wRet != 0)
+	if(wRet != RPC_HAPI_SUCCESS)
 	{
 		TCRIT("Error getting num of sel recods\n");
 		goto error_out;
@@ -184,7 +184,7 @@
 		/* Now get all sel records */
 		//now ifnd maximum possible sel entries and allocate that much
 		wRet = LIBIPMI_HL_GetMaxPossibleSELEntries(&((wc_get_ipmi_session(wp))->IPMISession),&nMaxEntries,DEFAULT_IPMITIMEOUT);
-		if(wRet != 0)
+		if(wRet != RPC_HAPI_SUCCESS)
 		{
 			printf("Error getting maximum no of entries..assuming 1024\n");
 			nMaxEntries = 1024;
@@ -213,24 +213,27 @@
 		TDBG("before get all sel entried\n");
 		/* Now pass this buffer pointer to the function to get all SEL entries */
 		wRet = LIBIPMI_HL_GetAllSelEntriesWithSensorNames(&((wc_get_ipmi_session(wp))->IPMISession), pSELBuff, DEFAULT_IPMITIMEOUT);
-		if(wRet == LIBIPMI_E_SUCCESS)
+		if(wRet == RPC_HAPI_SUCCESS)
 		{
 			/* Write entries here */
 			while(nEntries)
 			{
+				if(pSELBuff[nEntries-1].EventRecord.Valid != 0xA5)
+				{
 				WEBPAGE_WRITE_JSON_RECORD(HL_GETALLSELENTRIES,
-											pSELBuff[nEntries-1].EventRecord.hdr.ID,
-											pSELBuff[nEntries-1].EventRecord.hdr.Type,
-											ipmitoh_u32(pSELBuff[nEntries-1].EventRecord.hdr.TimeStamp),
-											pSELBuff[nEntries-1].EventRecord.GenID[0],
-											pSELBuff[nEntries-1].EventRecord.GenID[1],
-											pSELBuff[nEntries-1].EventRecord.EvMRev,
-											pSELBuff[nEntries-1].EventRecord.SensorType,
-											pSELBuff[nEntries-1].SensorName,
-											pSELBuff[nEntries-1].EventRecord.EvtDirType,
-											pSELBuff[nEntries-1].EventRecord.EvtData1,
-											pSELBuff[nEntries-1].EventRecord.EvtData2,
-											pSELBuff[nEntries-1].EventRecord.EvtData3);
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.hdr.ID,
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.hdr.Type,
+								ipmitoh_u32(pSELBuff[nEntries-1].EventRecord.EvtRecord.hdr.TimeStamp),
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.GenID[0],
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.GenID[1],
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.EvMRev,
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.SensorType,
+								pSELBuff[nEntries-1].SensorName,
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.EvtDirType,
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.EvtData1,
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.EvtData2,
+								pSELBuff[nEntries-1].EventRecord.EvtRecord.EvtData3);
+				}
 				nEntries--;
 			}
 		}
@@ -244,7 +247,7 @@
 	WEBPAGE_WRITE_JSON_END(HL_GETALLSELENTRIES,wRet);
 	WEBPAGE_WRITE_END();
 
-	return( 0 );
+	return RPC_HAPI_SUCCESS;
 }
 
 WEBIFC_HAPIFN_DEFINE( WEBIFC_IPMICMD_HL_GetSELEntriesSorted )
@@ -327,21 +330,24 @@
 			/* Write entries here */
 			while(nEntries)
 			{
-				if(pSELBuff[nEntries-1].EventRecord.hdr.ID>WEBVAR_LASTEVENTID)
+				if(pSELBuff[nEntries-1].EventRecord.EvtRecord.hdr.ID>WEBVAR_LASTEVENTID)
 				{
-					WEBPAGE_WRITE_JSON_RECORD(HL_GETSELENTRIES,
-												pSELBuff[nEntries-1].EventRecord.hdr.ID,
-												pSELBuff[nEntries-1].EventRecord.hdr.Type,
-												ipmitoh_u32(pSELBuff[nEntries-1].EventRecord.hdr.TimeStamp),
-												pSELBuff[nEntries-1].EventRecord.GenID[0],
-												pSELBuff[nEntries-1].EventRecord.GenID[1],
-												pSELBuff[nEntries-1].EventRecord.EvMRev,
-												pSELBuff[nEntries-1].EventRecord.SensorType,
-												pSELBuff[nEntries-1].SensorName,
-												pSELBuff[nEntries-1].EventRecord.EvtDirType,
-												pSELBuff[nEntries-1].EventRecord.EvtData1,
-												pSELBuff[nEntries-1].EventRecord.EvtData2,
-												pSELBuff[nEntries-1].EventRecord.EvtData3);
+					if(pSELBuff[nEntries-1].EventRecord.Valid != 0xA5)
+					{
+						WEBPAGE_WRITE_JSON_RECORD(HL_GETSELENTRIES,
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.hdr.ID,
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.hdr.Type,
+							ipmitoh_u32(pSELBuff[nEntries-1].EventRecord.EvtRecord.hdr.TimeStamp),
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.GenID[0],
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.GenID[1],
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.EvMRev,
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.SensorType,
+							pSELBuff[nEntries-1].SensorName,
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.EvtDirType,
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.EvtData1,
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.EvtData2,
+							pSELBuff[nEntries-1].EventRecord.EvtRecord.EvtData3);
+					}
 				}
 				nEntries--;
 			}
