--- .pristine/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Mon Mar  5 14:37:50 2012
+++ source/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Mon Mar  5 15:36:06 2012
@@ -958,15 +958,20 @@
     {
        if(pBMCInfo->NMConfig.NMDevSlaveAddress == pIPMIMsgHdr->ResAddr)
        {
-         if(g_BMCInfo[BMCInst].Msghndlr.CurPrivLevel != PRIV_ADMIN)
-         {
-            printf("Insufficient Privilege\n");
-            *pRes = CC_INSUFFIENT_PRIVILEGE;
-            return sizeof(*pRes);
-         }
+  	      if( (pBMCInfo->RMCPLAN1Ch == pBMCInfo->Msghndlr.CurChannel) ||  
+   	          (pBMCInfo->RMCPLAN2Ch == pBMCInfo->Msghndlr.CurChannel) ||  
+   	          (pBMCInfo->RMCPLAN3Ch == pBMCInfo->Msghndlr.CurChannel) ||     	          
+   	          (pBMCInfo->SERIALch   == pBMCInfo->Msghndlr.CurChannel) )  
+   	      {  
+   	          if(PRIV_ADMIN != pBMCInfo->Msghndlr.CurPrivLevel)  
+   	          {  
+   	              TDBG("Insufficient Privilege\n");  
+   	              *pRes = CC_INSUFFIENT_PRIVILEGE;  
+   	              return sizeof(*pRes);  
+   	          }  
+   	      }  
        } 
     }
-
 
     if((Channel == PRIMARY_IPMB_CHANNEL) && pBMCInfo->IpmiConfig.PrimaryIPMBSupport == 1)
     {
