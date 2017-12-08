--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Sep 16 14:25:30 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Sep 16 15:49:59 2011
@@ -352,8 +352,8 @@
 	public ISOCCreateBuffer prepare_buf;
 	private SystemTimeMonitor systemTimeMonitor;
 	
-	public final byte SERVER_POWER_ON = 1;
-	public final byte SERVER_POWER_OFF = 0;
+	public static final byte SERVER_POWER_ON = 1;
+	public static final byte SERVER_POWER_OFF = 0;
 	private byte powerStatus;	
 
 
@@ -547,6 +547,12 @@
 		m_RedirectionState = status;
 	}
 
+	/**
+	 * @return the powerStatus
+	 */
+	public byte getPowerStatus() {
+		return powerStatus;
+	}
 	public void Ondisplayvideo(String ip,String webPort, String sessionCookies, int secureConnect, String videoPath) {
 
 		//Assigning the window frame to the common frame object
