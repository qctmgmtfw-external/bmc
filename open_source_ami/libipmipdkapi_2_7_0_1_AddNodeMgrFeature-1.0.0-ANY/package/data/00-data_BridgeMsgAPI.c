--- .pristine/libipmipdkapi-2.7.0-src/data/BridgeMsgAPI.c Tue Dec 20 22:01:52 2011
+++ source/libipmipdkapi-2.7.0-src/data/BridgeMsgAPI.c Wed Dec 21 00:56:53 2011
@@ -30,63 +30,71 @@
 
 /*-----------------------------------------------------------------
  * @fn API_BridgeInternal
- * @brief API adds an entry in Pending Bridge Table if response tracking is requested.
- *          Then post message to IPMBIfc Queue.
- * @param pMsgPkt       - Pointer to the message packet
- * @param QueueName  - Response Queue Name. 
- *                                    If this value is NULL, Response Queue is selected based on ChNum
- * @param ChNum         - Channel Number. This value is checked only if QueueName value is NULL
- *                                    If this value is NULL, Response Queue is selected based on IPMBBusNum
- * @param IPMBBusNum - IPMB Bus Number. (Currently not supported)
- * @param ResponseTracking - Specifies whether response tracking is required or not
- * @param BMCInst       - BMC Instance
- * @return      0   if Success
- *                  -1  on failure.
+ * 
+ * @brief This API bridges the Message to the requested interface and 
+ *        adds an entry in Pending Bridge Table if response tracking 
+ *        is requested.
+ *
+ * @param pMsgPkt    - Pointer to the message packet to be bridged
+ * @param DestAddr   - Slave address of the destination
+ * @param ChNum      - Channel Number of the bridge 
+ * @param ResponseTracking - Specifies whether response tracking 
+ *                           is required or not
+ * @param BMCInst    - BMC Instance
+ * @return      0  if Success
+ *             -1  on failure.
  *-----------------------------------------------------------------*/
 int
-API_BridgeInternal(MsgPkt_T* pMsgPkt, char *QueueName, int* ChNum, int* IPMBBusNum,BOOL ResponseTracking, int BMCInst)
+API_BridgeInternal(MsgPkt_T* pMsgPkt, INT8U DestAddr, int ChNum, BOOL ResponseTracking, int BMCInst)
 {
-    int i;
+    _FAR_ BMCInfo_t*  pBMCInfo    = &g_BMCInfo[BMCInst];
     
-    _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
-    IPMIMsgHdr_T    *pIPMIMsgHdr;
-    pIPMIMsgHdr = (IPMIMsgHdr_T*)pMsgPkt->Data;
+    IPMIMsgHdr_T  *pIPMIMsgHdr = (IPMIMsgHdr_T*)pMsgPkt->Data;
+    INT8U i, RqSeqNum;
 
+    if( SYS_IFC_CHANNEL == pMsgPkt->Channel )
+    {
+        memmove( &pMsgPkt->Data [sizeof (IPMIMsgHdr_T)], &pMsgPkt->Data[0], pMsgPkt->Size ); 
+        
+        pMsgPkt->Size  = pMsgPkt->Size + sizeof(IPMIMsgHdr_T) + 1; // Plus 1 for 2nd checksum
+        
+        pIPMIMsgHdr->NetFnLUN = pMsgPkt->NetFnLUN;
+        pIPMIMsgHdr->RqSeqLUN = pMsgPkt->SessionID;
+        pIPMIMsgHdr->Cmd      = pMsgPkt->Cmd;
+    }
+
+    RqSeqNum = pIPMIMsgHdr->RqSeqLUN;
+
+    /* Format message packet */
+    pMsgPkt->Channel = ChNum;
+    pMsgPkt->Param = BRIDGING_REQUEST;
+
+    /* Format IPMI message header */
+    pIPMIMsgHdr->ResAddr  = DestAddr;
+    pIPMIMsgHdr->ChkSum   = ~(pIPMIMsgHdr->ResAddr + pIPMIMsgHdr->NetFnLUN) + 1;
+
+    if(pBMCInfo->IpmiConfig.PrimaryIPMBSupport == 1)
+    {
+        pIPMIMsgHdr->ReqAddr  = pBMCInfo->IpmiConfig.PrimaryIPMBAddr;
+    }
+    else
+    {
+        pIPMIMsgHdr->ReqAddr  = pBMCInfo->IpmiConfig.SecondaryIPMBAddr;
+    }
+
+    /* Format Sequence Number */
+    pIPMIMsgHdr->RqSeqLUN = ((pBMCInfo->SDRConfig.IPMB_Seqnum++) << 2) | (pIPMIMsgHdr->NetFnLUN & 0x03);
+
+    if (pBMCInfo->SDRConfig.IPMB_Seqnum >= 0x3F)
+    {
+        pBMCInfo->SDRConfig.IPMB_Seqnum = 0xFF;
+    }
+
+    pMsgPkt->Data [pMsgPkt->Size - 1] =  CalculateCheckSum2 (pMsgPkt->Data, pMsgPkt->Size - 1);
+    
     /* Check if response tracking is required */
-    if (ResponseTracking == TRUE)
+    if (ResponseTracking)
     {
-        /* Check if QueueName is given */
-        if (QueueName == NULL)
-        {
-            if (ChNum == NULL)
-            {
-                IPMI_WARNING("BridgeMsgAPI.c : Both QueueName and ChNum are NULL \n");
-                return -1;
-            }
-            
-            /* Select QueueName based on Channel Number */
-            if ((pBMCInfo->IpmiConfig.PrimaryIPMBSupport == 1) && (*ChNum == PRIMARY_IPMB_CHANNEL))
-            {
-                strcpy (QueueName, IPMB_PRIMARY_IFC_Q);
-            }
-            else if((pBMCInfo->SecondaryIPMBCh != CH_NOT_USED) && (pBMCInfo->IpmiConfig.SecondaryIPMBSupport == 1) && (*ChNum == g_BMCInfo[BMCInst].SecondaryIPMBCh))
-            {
-                strcpy (QueueName, IPMB_SECONDARY_IFC_Q);
-            }
-            else
-            {
-                IPMI_WARNING("BridgeMsgAPI.c : Receiving Response to Interfaces other than IPMB is not supported \n");
-                return -1;
-            }
-        }
-
-        /* Check if the source queue is one of the IPMB Interface queues */
-        if (((strcmp ((char*)pMsgPkt->SrcQ, IPMB_PRIMARY_IFC_Q)) != 0) && ((strcmp ((char*)pMsgPkt->SrcQ, IPMB_SECONDARY_IFC_Q)) != 0))
-        {
-            IPMI_WARNING("BridgeMsgAPI.c : Posting to Interfaces other than IPMB is not supported \n");
-            return -1;
-        }
-        
         /* Store in the table for response tracking */
         for (i=0; i < sizeof (m_PendingBridgedResTbl)/sizeof (m_PendingBridgedResTbl[0]); i++)
         {
@@ -96,24 +104,44 @@
                 m_PendingBridgedResTbl[i].SeqNum        = pIPMIMsgHdr->RqSeqLUN >> 2;
                 m_PendingBridgedResTbl[i].ChannelNum    = pMsgPkt->Channel; /* Destination Channel number */
 
-                _fmemcpy (&m_PendingBridgedResTbl[i].ReqMsgHdr, pIPMIMsgHdr, sizeof (IPMIMsgHdr_T));
-                strcpy ((char *)m_PendingBridgedResTbl[i].DestQ, QueueName);
+                memcpy (&m_PendingBridgedResTbl[i].ReqMsgHdr, pIPMIMsgHdr, sizeof (IPMIMsgHdr_T));
+                m_PendingBridgedResTbl[i].ReqMsgHdr.RqSeqLUN = RqSeqNum;
+                strcpy ((char *)m_PendingBridgedResTbl[i].DestQ, (char *) pMsgPkt->SrcQ);
+                m_PendingBridgedResTbl[i].Used = TRUE;
+                IPMI_DBG_PRINT_1( "BridgeMsgAPI.c:  Bridged message added index = %d.\n", i );
+                break;                
             }
-            m_PendingBridgedResTbl[i].Used = TRUE;
-            IPMI_DBG_PRINT_1( "BridgeMsgAPI.c:  Bridged message added index = %d.\n", i );
-            break;
+        }
+        
+        if(i == sizeof (m_PendingBridgedResTbl)/sizeof (m_PendingBridgedResTbl[0]))
+        {
+            IPMI_WARNING ("Pending Bridge Response Table is full \n");
+            return -1;
         }
     }
+    
+    /* Select Bridge's Queue name based on Channel Number */ 
+    if ((pBMCInfo->IpmiConfig.PrimaryIPMBSupport == 1) && (ChNum == PRIMARY_IPMB_CHANNEL)) 
+    {
+        strcpy ( (char *) pMsgPkt->SrcQ, IPMB_PRIMARY_IFC_Q); 
+    }
+    else if((pBMCInfo->SecondaryIPMBCh != CH_NOT_USED) && (pBMCInfo->IpmiConfig.SecondaryIPMBSupport == 1) && (ChNum == g_BMCInfo[BMCInst].SecondaryIPMBCh)) 
+    {
+        strcpy ( (char *) pMsgPkt->SrcQ, IPMB_SECONDARY_IFC_Q); 
+    }
+    else
+    {
+        IPMI_WARNING("Invalid IPMB Channel \n"); 
+        return -1; 
+    }
 
-    /* Post Message to source queue */
-    if(0 != PostMsg(pMsgPkt,(INT8S*) pMsgPkt->SrcQ, BMCInst))
+    /* Post Message to the bridge queue */
+    if(0 != PostMsg( pMsgPkt, (char *) pMsgPkt->SrcQ, BMCInst) )
     {
         IPMI_WARNING ("BridgeMsgAPI.c : Error posting message to Queue %s \n", (char*)pMsgPkt->SrcQ);
         return -1;
     }
-    
+
     IPMI_DBG_PRINT_1( "BridgeMsgAPI.c:  Posted Successfully to Queue %s \n", (char*)pMsgPkt->SrcQ);
     return 0;
-} 
-
-
+}
