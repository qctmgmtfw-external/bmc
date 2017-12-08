--- .pristine/libipmimsghndlr-2.159.0-src/data/MsgHndlr.c Wed Dec 21 21:08:57 2011
+++ source/libipmimsghndlr-2.159.0-src/data/MsgHndlr.c Wed Dec 21 22:00:05 2011
@@ -1309,7 +1309,12 @@
 {
     INT8U	i;
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
-
+    char LANQueueName[MAX_STR_LENGTH],PrimaryIPMBQueueName[MAX_STR_LENGTH],SecondaryIPMBQueueName[MAX_STR_LENGTH],QueueName[MAX_STR_LENGTH];
+
+    memset(LANQueueName,0,sizeof(LANQueueName));
+    memset(PrimaryIPMBQueueName,0,sizeof(PrimaryIPMBQueueName));
+    memset(SecondaryIPMBQueueName,0,sizeof(SecondaryIPMBQueueName));
+    memset(QueueName,0,sizeof(QueueName));
     /* Check for any pending responses */
     for (i = 0; i < sizeof (m_PendingBridgedResTbl)/sizeof (m_PendingBridgedResTbl[0]); i++)
     {
@@ -1324,6 +1329,10 @@
                 /* Fill the response packet */
                 SwapIPMIMsgHdr (&m_PendingBridgedResTbl[i].ReqMsgHdr, pIPMIMsgHdr);
 
+                sprintf(LANQueueName,"%s%d",LAN_IFC_Q,BMCInst);
+                sprintf(PrimaryIPMBQueueName,"%s%d",IPMB_PRIMARY_IFC_Q,BMCInst);
+                sprintf(SecondaryIPMBQueueName,"%s%d",IPMB_SECONDARY_IFC_Q,BMCInst);
+
                 if(m_PendingBridgedResTbl[i].ChannelNum == pBMCInfo->PrimaryIPMBCh)
                 {
                     pIPMIMsgHdr->ReqAddr = pBMCInfo->IpmiConfig.PrimaryIPMBAddr;
@@ -1346,7 +1355,7 @@
 
                 Timeout.Param = BRIDGING_REQUEST;
 
-                if (0 == strcmp ((char *)m_PendingBridgedResTbl[i].DestQ, LAN_IFC_Q))
+                if (0 == strcmp ((char *)m_PendingBridgedResTbl[i].DestQ, LANQueueName))
                 {
                     int j;
                     for (j = Timeout.Size - 1; j >= 0; --j)
@@ -1357,8 +1366,9 @@
                     Timeout.Data[0] = m_PendingBridgedResTbl[i].SrcSessionHandle;
                     Timeout.Size++;
                     Timeout.Cmd = PAYLOAD_IPMI_MSG;
-                }
-                else if (pBMCInfo->IpmiConfig.PrimaryIPMBSupport == 1 && 0 == strcmp ((char *)m_PendingBridgedResTbl[i].DestQ, IPMB_PRIMARY_IFC_Q))
+                    strcpy(QueueName, LAN_IFC_Q);
+                }
+                else if (pBMCInfo->IpmiConfig.PrimaryIPMBSupport == 1 && 0 == strcmp ((char *)m_PendingBridgedResTbl[i].DestQ, PrimaryIPMBQueueName))
                 {
                     int j;
                     for (j = Timeout.Size - 1; j >= 0; --j)
@@ -1368,8 +1378,9 @@
                     _fmemcpy (Timeout.Data, &m_PendingBridgedResTbl[i].ResMsgHdr, sizeof (IPMIMsgHdr_T));
                     Timeout.Data[sizeof (IPMIMsgHdr_T)] = CC_NORMAL;
                     Timeout.Size++;
-                }
-                else if (pBMCInfo->IpmiConfig.SecondaryIPMBSupport == 1 && 0 == strcmp ((char *)m_PendingBridgedResTbl[i].DestQ, IPMB_SECONDARY_IFC_Q))
+                    strcpy(QueueName, IPMB_PRIMARY_IFC_Q);
+                }
+                else if (pBMCInfo->IpmiConfig.SecondaryIPMBSupport == 1 && 0 == strcmp ((char *)m_PendingBridgedResTbl[i].DestQ, SecondaryIPMBQueueName))
                 {
                     int j;
                     for (j = Timeout.Size - 1; j >= 0; --j)
@@ -1379,6 +1390,7 @@
                     _fmemcpy (Timeout.Data, &m_PendingBridgedResTbl[i].ResMsgHdr, sizeof (IPMIMsgHdr_T));
                     Timeout.Data[sizeof (IPMIMsgHdr_T)] = CC_NORMAL;
                     Timeout.Size++;
+                    strcpy(QueueName, IPMB_SECONDARY_IFC_Q);
                 }
                 else 
                 {
@@ -1389,10 +1401,11 @@
                     }
                     _fmemcpy (Timeout.Data, &m_PendingBridgedResTbl[i].ResMsgHdr, sizeof (IPMIMsgHdr_T));
                     Timeout.Data[sizeof (IPMIMsgHdr_T)] = CC_TIMEOUT;
+                    sprintf (QueueName,"%s",m_PendingBridgedResTbl[i].DestQ);
                 }
 
                 /* Post the data to Destination Interface queue */
-                PostMsg (&Timeout,(INT8S *) m_PendingBridgedResTbl[i].DestQ,BMCInst);
+                PostMsg (&Timeout,QueueName,BMCInst);
 
                 m_PendingBridgedResTbl[i].Used = FALSE;
                 IPMI_DBG_PRINT_1( "MsgHndlr: clean pending index = %d.\n", i );
