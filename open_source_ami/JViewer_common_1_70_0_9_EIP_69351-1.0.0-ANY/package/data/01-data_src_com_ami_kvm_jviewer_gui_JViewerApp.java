--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Mon Sep 19 10:43:17 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Mon Sep 19 19:12:35 2011
@@ -328,6 +328,7 @@
 	public static byte WEB_PREVIEWER_CAPTURE_IN_PROGRESS = -2;
 	public static byte WEB_PREVIEWER_CONNECT_FAILURE = -3;
 	public static byte WEB_PREVIEWER_INVALID_SERVERIP = -4;
+	public static byte WEB_PREVIEWER_HOST_POWER_OFF = -5;
 	public static byte m_webPreviewer_cap_status = WEB_PREVIEWER_CAPTURE_IN_PROGRESS;
 	private KVMShareDialog kVMDialog =  KVMShareDialog.getInstance();
 	private int m_zoomSliderValue;// Zoom slider value
