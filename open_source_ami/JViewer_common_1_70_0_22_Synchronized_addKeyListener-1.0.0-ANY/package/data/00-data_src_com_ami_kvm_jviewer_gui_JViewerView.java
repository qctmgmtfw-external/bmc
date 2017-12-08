--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java Thu Aug 30 16:56:24 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java Thu Aug 30 17:00:11 2012
@@ -159,7 +159,7 @@
 	/**
 	 * Add mouse and keyboard listeners
 	 */
-	public void addKeyListener() {
+	public synchronized void addKeyListener() {
 		setkeyprocessor();
 		addKeyListener(m_keyListener);
 	}
