--- .pristine/libipmipef-2.11.0-src/data/PEF.c Thu Apr 26 21:17:17 2012
+++ source/libipmipef-2.11.0-src/data/PEF.c Thu Apr 26 21:27:08 2012
@@ -534,7 +534,7 @@
 
                 /* We have to run the PEF postpone time interval */
                 /* Wait until the PEF postpone timer stoped */
-                /*while (pBMCInfo->PefConfig.PEFTmrMgr.StartTmr) { OS_TIME_DELAY_HMSM (0, 0, 0, 500); }*/
+                while (pBMCInfo->PefConfig.PEFTmrMgr.StartTmr) { OS_TIME_DELAY_HMSM (0, 0, 0, 500); }
 
                 if (!(((pEvtRecord->hdr.ID < pPEFConfig->LastBMCProcessedEventID && 0xFFFF != pPEFConfig->LastBMCProcessedEventID)
                 || (pEvtRecord->hdr.ID < pPEFConfig->LastSWProcessedEventID && 0xFFFF != pPEFConfig->LastSWProcessedEventID)))
