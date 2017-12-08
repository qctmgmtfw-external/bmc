--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/kvmpkts/ConfPkt.java Mon Sep  5 17:06:11 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/kvmpkts/ConfPkt.java Thu Sep  8 18:40:35 2011
@@ -29,9 +29,11 @@
 import com.ami.kvm.jviewer.gui.JViewerApp;
 
 public class ConfPkt {
-	private char[] serviceName = new char[16];
+	public static final int  NAME_MAX_LENGTH		= 17;
+
+	private char[] serviceName = new char[NAME_MAX_LENGTH];
 	private byte currentState;
-	private char[] interfaceName = new char[16];
+	private char[] interfaceName = new char[NAME_MAX_LENGTH];
 	private int nonSecureAccessPort;
 	private int secureAccessPort;
 	private int sessionInactivityTimeout;
@@ -44,24 +46,24 @@
 	public static final String SERV_FD_MEDIA="fd-media";
 	public static final String SERV_HD_MEDIA="hd-media";
 
-	public static final int CONF_PKT_SIZE 				= 55;
+	public static final int CONF_PKT_SIZE			= 57;
 	public static final short CONF_SERVICE_COUNT		= 5;
-	
-	public static final short CHANGE_CURR_STATE 		= 0;
-	public static final short CHANGE_IFACE_NAME 		= 1;
-	public static final short CHANGE_NON_SEC_PORT 		= 2;
-	public static final short CHANGE_SEC_PORT 			= 3;
-	public static final short CHANGE_TIME_OUT 			= 4;
+
+	public static final short CHANGE_CURR_STATE		= 0;
+	public static final short CHANGE_IFACE_NAME		= 1;
+	public static final short CHANGE_NON_SEC_PORT		= 2;
+	public static final short CHANGE_SEC_PORT		= 3;
+	public static final short CHANGE_TIME_OUT		= 4;
 	public static final short CHANGE_MAX_SESSION 		= 5;
 	public static final short CHANGE_CURR_ACTIVE_SESS 	= 6;
 	public static final short CHANGE_MAX_TIME_OUT 		= 7;
 	public static final short CHANGE_MIN_TIME_OUT 		= 8;
-	
+
 	public static boolean confModified = false;
 	public ConfPkt(byte[] dataBuf){
 		ByteBuffer dataBuffer = ByteBuffer.wrap(dataBuf);
 		dataBuffer.order(ByteOrder.LITTLE_ENDIAN);
-		byte[] temp = new byte[16];
+		byte[] temp = new byte[NAME_MAX_LENGTH];
 		dataBuffer.get(temp);
 		serviceName = new String(temp).toCharArray();
 		currentState = dataBuffer.get();
