--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Thu Apr 12 18:30:48 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Apr 13 11:37:57 2012
@@ -1594,6 +1594,7 @@
 	 * @return
 	 */
 	public int OnsendWebsessionToken() {
+		int PktLen = 0, TokenLen = 0;
 
 		String web_session_token = JViewerApp.getInstance().getM_webSession_token();
 		byte[] web_token = new byte[web_session_token.length()];
@@ -1613,12 +1614,12 @@
 			return -1;
 		}
 
+		PktLen = IVTPPktHdr.HDR_SIZE + IVTPPktHdr.WEB_VALIDATE_VIDEO_PACKET_SIZE;
+		TokenLen = IVTPPktHdr.WEB_HASH_SIZE;
 		/* Sending the session token */
-		IVTPPktHdr VideoSessTokenPkt = new IVTPPktHdr( IVTPPktHdr.IVTP_VALIDATE_VIDEO_SESSION, IVTPPktHdr.VALIDATE_VIDEO_PACKET,
-										(short) 0);
+		IVTPPktHdr VideoSessTokenPkt = new IVTPPktHdr(IVTPPktHdr.IVTP_VALIDATE_VIDEO_SESSION, PktLen, (short)0);
 		String session_token = JViewerApp.getInstance().getSessionToken();
-		ByteBuffer bf = ByteBuffer.allocate(VideoSessTokenPkt.size() + ( IVTPPktHdr.VALIDATE_VIDEO_PACKET-IVTPPktHdr.HASH_SIZE));
-
+		ByteBuffer bf = ByteBuffer.allocate(PktLen);
 
 		// Calculate digest
 		byte[] hashed_token = new byte[IVTPPktHdr.HASH_SIZE];
@@ -1633,20 +1634,6 @@
 
 		for (int i=bf.position(); i < 107; i++ )
 			bf.put((byte)0);
-
-
-
-
-		for (int i=bf.position(); i < 107; i++ )
-			bf.put((byte)0);
-
-//		try {
-//			InetAddress hostAddress = InetAddress.getByName(KVMSharing.KVM_CLIENT_OWN_IP);
-//		} catch (UnknownHostException e) {
-//			// TODO Auto-generated catch block
-//			e.printStackTrace();
-//
-//		}
 
 		bf.put(KVMSharing.KVM_CLIENT_OWN_IP.getBytes());
 		for (int i=bf.position(); i < bf.limit(); i++ )
