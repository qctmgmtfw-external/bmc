--- .pristine/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Tue Jun  5 14:44:22 2012
+++ source/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Tue Jun  5 17:46:25 2012
@@ -1195,6 +1195,12 @@
         return  sizeof (*pRes);
     }
 
+	if (BMC_GET_SHARED_MEM (BMCInst)->NumEvtMsg == 0)
+	{
+		pReadEvtMsgBufRes->CompletionCode = CC_EVT_MSG_QUEUE_EMPTY;
+		return  sizeof (*pRes);
+	}
+	
     BMC_GET_SHARED_MEM (BMCInst)->NumEvtMsg--;
 
 #if GET_MSG_FLAGS != UNIMPLEMENTED
