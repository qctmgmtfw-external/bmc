--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java Wed Sep 21 18:04:00 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java Wed Sep 21 18:12:34 2011
@@ -409,15 +409,15 @@
 	}
 	
 	public void changeMacrowsStatusOnPauseResume(boolean status){
-		
-		mouseBtn.setEnabled(status);
-		kbdBtn.setEnabled(status);
-		cdBtn.setEnabled(status);
-		floppyBtn.setEnabled(status);
-		hardddiskBtn.setEnabled(status);
-		altCtrlDelBtn.setEnabled(status);
-		altTab.setEnabled(status);
-		videoRecordBtn.setEnabled(status);
+		// Remove the mouse listener from toolbar buttons while pausing 
+		// and add the mouse listener while resuming.
+		setButtonEnabled(mouseBtn, status);
+		setButtonEnabled(kbdBtn, status);
+		setButtonEnabled(cdBtn, status);
+		setButtonEnabled(floppyBtn, status);
+		setButtonEnabled(hardddiskBtn, status);
+		setButtonEnabled(m_hotKeyBtn, status);
+		setButtonEnabled(videoRecordBtn, status);
 		if(status)
 			powerBtn.addMouseListener(this);
 		else
