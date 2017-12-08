--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVFrame.java Fri Sep 16 14:25:30 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVFrame.java Fri Sep 16 16:00:50 2011
@@ -252,15 +252,16 @@
     		} 	
     	}   
 		
-	//If the KVM Share Response Dialog is present when system time is modified,and the second user is waiting 
+    	//If the KVM Share Response Dialog is present when system time is modified,and the second user is waiting 
     	//for permission, allow full access to the second user before closing the JViewer.
-     	if(SystemTimeMonitor.isTimeChanged()){	
-		if(KVMShareDialog.getInstance()!= null){
-			KVMSharing.KVM_REQ_GIVEN = KVMSharing.KVM_REQ_ALLOWED;
-			JViewerApp.getInstance().OnSendKVMPrevilage(KVMSharing.KVM_REQ_ALLOWED);
-			KVMShareDialog.getInstance().disposeUserOneDialog();
-			
-		}
+    	if(SystemTimeMonitor.isTimeChanged()){	
+    		if(KVMShareDialog.getInstance()!= null){
+    			if(KVMShareDialog.getInstance().getUserOneDialog()!=null){
+    				KVMSharing.KVM_REQ_GIVEN = KVMSharing.KVM_REQ_ALLOWED;
+        			JViewerApp.getInstance().OnSendKVMPrevilage(KVMSharing.KVM_REQ_ALLOWED);
+        			KVMShareDialog.getInstance().disposeUserOneDialog();
+    			}
+    		}
     	}  
     	
     	try {
