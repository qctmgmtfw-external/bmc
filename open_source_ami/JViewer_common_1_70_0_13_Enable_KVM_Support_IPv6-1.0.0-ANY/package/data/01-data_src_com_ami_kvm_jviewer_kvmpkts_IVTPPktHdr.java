--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/kvmpkts/IVTPPktHdr.java Thu Apr 12 18:30:47 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/kvmpkts/IVTPPktHdr.java Fri Apr 13 11:38:57 2012
@@ -85,6 +85,8 @@
 	public static final byte SESSION_TOKEN_LEN						= 16;
 	public static final byte HASH_SIZE								= 16;
 	public static final short VALIDATE_VIDEO_PACKET					= 140;
+	public static final byte WEB_HASH_SIZE                          = 16;
+	public static final short WEB_VALIDATE_VIDEO_PACKET_SIZE        = 164;
 
 	// member variables provide public access
 	public short 	type;
