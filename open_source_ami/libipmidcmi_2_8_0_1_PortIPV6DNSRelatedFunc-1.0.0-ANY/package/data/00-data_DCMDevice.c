--- .pristine/libipmidcmi-2.8.0-src/data/DCMDevice.c Fri Feb  3 16:18:39 2012
+++ source/libipmidcmi-2.8.0-src/data/DCMDevice.c Wed Jan 18 21:49:31 2012
@@ -716,47 +716,29 @@
     }
 }
 
-
-
-
-
-
-
-
-
-
-
-
-/*******************************************************************************
-Function:       GetManagementControllerIdString
-Classification: UNCLASSIFIED
-Property of:    American Megatrends Inc.
-Description:    Called to Get Management Controller Identifier String
-Language:       ANSI C++
-Arguments:      INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst
-Return Values:  int
-Unit Test:      none
-*******************************************************************************/
+/*
+ * @fn GetManagementControllerIdString
+ * @brief This function retrieves DCMI ManagementControllerIDString
+ * @params IPMI Request data.
+ * @return Returns CC_NORMAL
+ */
 int GetManagementControllerIdString (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst)
 {
+
     GetManagementControllerIdStringReq_T* pGetManagementControllerIdStringReq = (GetManagementControllerIdStringReq_T*) pReq;
     GetManagementControllerIdStringRes_T* pGetManagementControllerIdStringRes = (GetManagementControllerIdStringRes_T*) pRes;
-    uint8 *pRReq = NULL;
-    AMIGetDNSConfRes_T  AMIGetDNSConfRes; 
     char *hostname;
+    HOSTNAMECONF    HostConfig;
+    DOMAINCONF          DomainConfig;
+    DNSCONF             DnsIPConfig;
+    INT8U               regBMC_FQDN[ MAX_CHANNEL ];
 
     IPMI_DBG_PRINT("Get Management controller ID string command is received\n");
 
     /* Get DNS parameters */
-    AMIGetDNSConf((_NEAR_ INT8U*)pRReq, 0, (_NEAR_ INT8U*)&AMIGetDNSConfRes, BMCInst);
-	
-    if(AMIGetDNSConfRes.CompletionCode != CC_SUCCESS)
-	{
-    	pGetManagementControllerIdStringRes->CompletionCode = CC_ERR_HANDLING_COMMAND_FAILURE;
-    	return sizeof (INT8U);
-	}
-    
-    hostname = (char *)&(AMIGetDNSConfRes.dnscfg.DNSBMCHostName);
+    nwGetAllDNSConf(&HostConfig,&DomainConfig,&DnsIPConfig,regBMC_FQDN);
+
+    hostname = (char *)&(HostConfig.HostName);
         
     if (pGetManagementControllerIdStringReq->OffsetToRead > MAX_ASSET_TAG_LEN)
     {
@@ -783,58 +765,37 @@
     
     pGetManagementControllerIdStringRes->CompletionCode     = CC_NORMAL;
     pGetManagementControllerIdStringRes->GroupExtnID        = IPMI_GROUP_EXTN_CODE_DCMI;
-    pGetManagementControllerIdStringRes->TotalLen           = AMIGetDNSConfRes.dnscfg.DNSBMCHostNameLen;
+    pGetManagementControllerIdStringRes->TotalLen           = HostConfig.HostNameLen;
 
     return(sizeof (GetManagementControllerIdStringRes_T) - 
                    READ_LIMIT_IN_ONE_TRANSACTION +
                    pGetManagementControllerIdStringReq->NumBytesToRead);
 }
 
-
-
-
-
-
-
-/*******************************************************************************
-Function:       SetManagementControllerIdString
-Classification: UNCLASSIFIED
-Property of:    American Megatrends Inc.
-Description:    Called to Set Management Controller Identifier String
-Language:       ANSI C++
-Arguments:      INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst
-Return Values:  int
-Unit Test:      none
-*******************************************************************************/
+/*
+ * @fn SetManagementControllerIdString
+ * @brief This function Sets DCMI Management Controller ID string
+ * @params IPMI Request data.
+ * @return Returns CC_NORMAL
+ */
 int SetManagementControllerIdString (INT8U* pReq, INT8U ReqLen, INT8U* pRes,_NEAR_ int BMCInst)
 {
     SetManagementControllerIdStringReq_T* pSetManagementControllerIdStringReq = (SetManagementControllerIdStringReq_T*) pReq;
     SetManagementControllerIdStringRes_T* pSetManagementControllerIdStringRes = (SetManagementControllerIdStringRes_T*) pRes;
-    uint8 *pRReq = NULL; 
-    AMIGetDNSConfRes_T  AMIGetDNSConfRes; 
-    AMISetDNSConfReq_T  AMISetDNSConfReq;
-    AMISetDNSConfRes_T  AMISetDNSConfRes;
-    DNS_CONFIG* dnscfgRes = &AMIGetDNSConfRes.dnscfg;
-    DNS_CONFIG* dnscfgReq = &AMISetDNSConfReq.dnscfg;
-    
+    HOSTNAMECONF    HostConfig;
+    DOMAINCONF          DomainConfig;
+    DNSCONF             DnsIPConfig;
+    INT8U               regBMC_FQDN[ MAX_CHANNEL ];
+
     IPMI_DBG_PRINT("Set Management controller ID string command is received\n");
 
     /* Get DNS parameters */
-    AMIGetDNSConf((_NEAR_ INT8U*)pRReq, 0, (_NEAR_ INT8U*)&AMIGetDNSConfRes, BMCInst);
-    /* Save the DNS response values returned into DNS request configuration*/
-
-    if(AMIGetDNSConfRes.CompletionCode != CC_NORMAL)
-    {
-    	pSetManagementControllerIdStringRes->CompletionCode = CC_ERR_HANDLING_COMMAND_FAILURE;
-    	return sizeof (INT8U);
-    }
-
-    /* Copy current settings to new settings struct */
-    memcpy(dnscfgReq,dnscfgRes,sizeof(DNS_CONFIG));
+    nwGetAllDNSConf(&HostConfig,&DomainConfig,&DnsIPConfig,regBMC_FQDN);
+
 
     if (pSetManagementControllerIdStringReq->OffsetToWrite > MAX_ASSET_TAG_LEN)
     {
-    	pSetManagementControllerIdStringRes->CompletionCode = CC_INV_DATA_FIELD;
+        pSetManagementControllerIdStringRes->CompletionCode = CC_INV_DATA_FIELD;
         return sizeof (INT8U);
     }
 
@@ -847,42 +808,32 @@
     if ((pSetManagementControllerIdStringReq->OffsetToWrite + pSetManagementControllerIdStringReq->DataLen) >
          MAX_ASSET_TAG_LEN)
     {
-    	pSetManagementControllerIdStringRes->CompletionCode = CC_INV_DATA_FIELD;
-        return sizeof (INT8U);
-    }
-   	
+        pSetManagementControllerIdStringRes->CompletionCode = CC_INV_DATA_FIELD;
+        return sizeof (INT8U);
+    }
+
     /* Set hostsetting to manual mode */
-    dnscfgReq->DNSBMCHostSetting = HOSTNAME_SETTING_MANUAL;
-    
+    HostConfig.HostSetting = HOSTNAME_SETTING_MANUAL;
+
     /* Copy new DNS HostName */
-    memcpy(dnscfgReq->DNSBMCHostName+pSetManagementControllerIdStringReq->OffsetToWrite,
+    memcpy(&HostConfig.HostName+pSetManagementControllerIdStringReq->OffsetToWrite,
            pSetManagementControllerIdStringReq->Data,
            pSetManagementControllerIdStringReq->DataLen);
 
     /* To Ensure the first Charactor is not NULL */
-    if (dnscfgReq->DNSBMCHostName[0] == 0)
-    {
-    	pSetManagementControllerIdStringRes->CompletionCode = CC_INV_CMD;
+    if (HostConfig.HostName[0] == 0)
+    {
+        pSetManagementControllerIdStringRes->CompletionCode = CC_INV_CMD;
         return sizeof (INT8U);
     }    
 
     /* Store new hostname to DNS HostName */
-    AMISetDNSConf((_NEAR_ INT8U*)&AMISetDNSConfReq, sizeof(AMISetDNSConfReq_T), (_NEAR_ INT8U*)&AMISetDNSConfRes, BMCInst);
-	
-    if(AMIGetDNSConfRes.CompletionCode != CC_NORMAL)
-    {
-     	pSetManagementControllerIdStringRes->CompletionCode = CC_COULD_NOT_PROVIDE_RESP;
-     	return sizeof (INT8U);
-    }
+    nwSetAllDNSConf( &HostConfig, &DomainConfig,&DnsIPConfig,regBMC_FQDN);
 
     pSetManagementControllerIdStringRes->CompletionCode = CC_NORMAL;
     pSetManagementControllerIdStringRes->GroupExtnID    = IPMI_GROUP_EXTN_CODE_DCMI;
     return sizeof (SetManagementControllerIdStringRes_T);
 }
-
-
-
-
 
 
 /*******************************************************************************
