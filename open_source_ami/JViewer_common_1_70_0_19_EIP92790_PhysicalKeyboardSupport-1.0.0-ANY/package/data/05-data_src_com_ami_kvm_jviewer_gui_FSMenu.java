--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/FSMenu.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/FSMenu.java	Wed Jul 11 11:30:31 2012
@@ -60,7 +60,7 @@
 		m_popupMenu.add(constructMouseMenu());
 		m_popupMenu.add(constructOptionsMenu());
 		m_popupMenu.add(constructDeviceRedirMenu());
-		m_popupMenu.add(constructSoftKeyboardMenu());
+		m_popupMenu.add(constructKeyboardLayoutMenu());
 		m_popupMenu.add(constructVideoRecordMenu());
 		m_popupMenu.add(constructPowerMenu());
 		m_popupMenu.add(constructUserMenu());
