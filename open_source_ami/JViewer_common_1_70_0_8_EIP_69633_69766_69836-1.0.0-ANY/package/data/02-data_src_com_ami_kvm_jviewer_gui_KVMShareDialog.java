--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/KVMShareDialog.java Fri Sep 16 14:25:30 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/KVMShareDialog.java Fri Sep 16 16:00:31 2011
@@ -268,9 +268,11 @@
 
 	public void disposeUserOneDialog()
 	{
-		kvms_timer.stop();
+		if(kvms_timer != null)
+			kvms_timer.stop();
 		kvms_decision_duration = 30;
-		kvms_userone_dialog.dispose();
+		if(kvms_userone_dialog != null)
+			kvms_userone_dialog.dispose();
 	}
 
 	public void disposeUserTwoDialog()
