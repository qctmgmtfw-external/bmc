--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Wed Jul  4 15:49:03 2012
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Wed Jul  4 16:55:51 2012
@@ -1222,6 +1222,7 @@
     int EthIndex = 0;
     INT8U CurrentIfaceState = 0x00;
 
+    pAMIIfaceStateRes->CompletionCode = CC_SUCCESS;
     switch(pAMIIfaceStateReq->Params)
     {
         case AMI_IFACE_STATE_ETH:
