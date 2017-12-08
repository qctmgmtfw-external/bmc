--- .pristine/libipmisol-2.8.0-src/data/SOL.c Tue Jan 10 19:44:06 2012
+++ source/libipmisol-2.8.0-src/data/SOL.c Wed Jan 11 13:45:56 2012
@@ -61,6 +61,10 @@
 static int  DeactivateSOL (int BMCInst);
 void*       SOLTimer (void* pData);
 
+/*-------------------- Module Variables -------------------------*/ 
+int m_activate_sol = 0;
+int m_deactivate_sol = 1; 
+
 /*
  * Close SOL Port
  */
@@ -282,9 +286,9 @@
     int             BMCInst;
     INT8U           *SendBuf,*RecvBuf;
     char            keyInstance[MAX_STR_LENGTH];
-    INT32U          SessionID;
-    _FAR_   MsgPkt_T	SOLPkt;
-    _FAR_   SessionInfo_T*	pSessInfo = NULL;
+    //INT32U          SessionID;
+    //_FAR_   MsgPkt_T	SOLPkt;
+    //_FAR_   SessionInfo_T*	pSessInfo = NULL;
     
     /*This needs to be done first as BMCInst is needed*/
     BMCArg *IPMIArgs = (BMCArg *)Addr;
@@ -362,7 +366,7 @@
                 ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->ack_seq_num         = 0;
                 ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->accepted_char_count = 0;
                 ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->status              = 0;
-                ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->status              = 0;
+                ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->status              |= (m_deactivate_sol?BIT4:0);
                 if (SS_IsPktReady (BMCInst))
                 {
                     /* Form the packet */
@@ -390,6 +394,8 @@
                     else
                     {
                         SendSOLPkt (pBMCInfo->SOLConfig.SOLBuf, size,BMCInst);
+                        if(m_deactivate_sol)
+                            break;
                     }
                 }
             }
@@ -410,31 +416,7 @@
                 ActivateSOL (BMCInst);
             }
             else if ((0 == TimeOut) && (DEACTIVATE_SOL == ReqPkt.Param))
-            {                
-                SessionID = BMC_GET_SHARED_MEM (BMCInst)->SOLSessID;
-                pSessInfo = getSessionInfo (SESSION_ID_INFO, &SessionID, BMCInst);
-
-                /* Frame a Deactivate packet and send to LAN_IFC_Q, for deactivate the 
-                established SOL session. This packet is the last packet */
-                ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->seq_num               = 1;                
-                ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->ack_seq_num           = 1;
-                ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->accepted_char_count   = 1;
-                ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->status                = BMC_2_RC_SOL_DEACTIVATED;
-
-                SOLPkt.Param = BRIDGING_REQUEST;
-                SOLPkt.Cmd = PAYLOAD_SOL;
-                SOLPkt.Data[0] = pSessInfo->SessionHandle;
-                memcpy (&SOLPkt.Data[1], pBMCInfo->SOLConfig.SOLBuf, 4);
-                SOLPkt.Size = 5;
-
-                IPMI_DBG_PRINT ("Posting to LAN_IFC_Q\n");
-
-                if (0 != PostMsg (&SOLPkt, LAN_IFC_Q,BMCInst))
-                {
-                        printf("Fail to post\n");
-                        IPMI_ASSERT (FALSE);
-                }
-            	
+            {     	
                 pBMCInfo->SOLConfig.SOLSessionActive = 0;
                 DeactivateSOL (BMCInst);
                 if (pBMCInfo->IpmiConfig.OPMASupport == 1)
@@ -469,7 +451,7 @@
                     ((SOLPayLoad_T*)(pBMCInfo->SOLConfig.SOLBuf))->ack_seq_num         = 0;
                     ((SOLPayLoad_T*)(pBMCInfo->SOLConfig.SOLBuf))->accepted_char_count = 0;
                     ((SOLPayLoad_T*)(pBMCInfo->SOLConfig.SOLBuf))->status              = 0;
-                    ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->status               = 0;
+                    ((SOLPayLoad_T*) pBMCInfo->SOLConfig.SOLBuf)->status               |= (m_deactivate_sol?BIT4:0);
 
                     if (SS_IsPktReady (BMCInst))
                     {
@@ -522,6 +504,8 @@
     pBMCInfo->SOLConfig.cur_tick        = 0;
     pBMCInfo->SOLConfig.activate_sol    = 1;
 
+    m_deactivate_sol = 0;
+
     WakeupSOLTask (BMCInst);
 
     /* Switch the serial mux for SOL */
@@ -550,6 +534,7 @@
     DeAssertSOLCTS (BMCInst);
 
     pBMCInfo->SOLConfig.activate_sol = 0;
+    m_deactivate_sol = 1;
     WakeupSOLTask (BMCInst);
 
     BMC_GET_SHARED_MEM (BMCInst)->SerialMUXMode = MUX_2_SYS;
@@ -608,6 +593,21 @@
         IPMI_ASSERT (FALSE);
     }
 
+    if(m_deactivate_sol)
+    {
+		SOLPkt.Param  = BRIDGING_REQUEST;
+		SOLPkt.Cmd    = PAYLOAD_IPMI_MSG;
+		SOLPkt.Data[0]= pSessInfo->SessionHandle;
+		SOLPkt.Size = 1;
+		
+		usleep (1);
+		
+		if (0 != PostMsg (&SOLPkt, LAN_IFC_Q,BMCInst))
+		{
+				IPMI_ASSERT (FALSE);
+		}
+	}
+
     return 0;
 }
 
