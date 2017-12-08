--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/WindowMenu.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/WindowMenu.java	Wed Jul 11 11:38:14 2012
@@ -64,7 +64,7 @@
 		m_menuBar.add(constructMouseMenu());
 		m_menuBar.add(constructOptionsMenu());
 		m_menuBar.add(constructDeviceRedirMenu());
-		m_menuBar.add(constructSoftKeyboardMenu());
+		m_menuBar.add(constructKeyboardLayoutMenu());
 		//if(JViewer.VideoRecord_Flag)
 		m_menuBar.add(constructVideoRecordMenu());
 		m_menuBar.add(constructPowerMenu());
