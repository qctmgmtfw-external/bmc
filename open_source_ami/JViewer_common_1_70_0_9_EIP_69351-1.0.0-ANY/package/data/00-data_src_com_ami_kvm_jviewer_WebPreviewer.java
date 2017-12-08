--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/WebPreviewer.java Mon Sep 19 10:43:16 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/WebPreviewer.java Mon Sep 19 19:16:46 2011
@@ -95,6 +95,8 @@
 					g.drawString("Not able to connect "+serverip,REFRESH_BUTTON_WEIGHT+10, REFRESH_BUTTON_HEIGHT);
 				else if (m_capStatus == JViewerApp.WEB_PREVIEWER_INVALID_SERVERIP)
 					g.drawString("INVALID SERVER IP"+serverip,REFRESH_BUTTON_WEIGHT+10, REFRESH_BUTTON_HEIGHT);	
+				else if (m_capStatus == JViewerApp.WEB_PREVIEWER_HOST_POWER_OFF)
+					g.drawString("Host is powered OFF or is in Sleep Mode",REFRESH_BUTTON_WEIGHT+10, REFRESH_BUTTON_HEIGHT);	
 				else 
 					g.drawString(m_view.viewWidth() + " x " + m_view.viewHeight(), REFRESH_BUTTON_WEIGHT+10, REFRESH_BUTTON_HEIGHT);
 				
@@ -182,32 +184,39 @@
 			if(kvmsecure == 1)
 				useSSL = true;
 			JViewerApp.getInstance().OnConnectToServer(ip, kvmport, "asdfa", useSSL, "asdfas" );
-		JViewerApp.getInstance().getKVMClient().setM_redirection(true);
-
-		while(true) 
-		{
-			m_capStatus = JViewerApp.getInstance().getWebPreviewerCaptureStatus();
-				if(m_capStatus == JViewerApp.WEB_PREVIEWER_CAPTURE_SUCCESS || retryCount == RETRY_COUNT || m_capStatus == JViewerApp.WEB_PREVIEWER_CAPTURE_FAILURE) {
-					JViewerApp.getInstance().OnVideoStopRedirection();
-				break;
-			}else if (m_capStatus == JViewerApp.WEB_PREVIEWER_CONNECT_FAILURE) {
-				break;
+			JViewerApp.getInstance().getKVMClient().setM_redirection(true);
+
+			while(true) 
+			{
+				m_capStatus = JViewerApp.getInstance().getWebPreviewerCaptureStatus();
+				if(m_capStatus == JViewerApp.WEB_PREVIEWER_CAPTURE_SUCCESS ||
+					retryCount == RETRY_COUNT ||
+					m_capStatus == JViewerApp.WEB_PREVIEWER_CAPTURE_FAILURE ||
+					m_capStatus == JViewerApp.WEB_PREVIEWER_HOST_POWER_OFF){
+						JViewerApp.getInstance().OnVideoStopRedirection();
+						break;
+				}
+				else if (m_capStatus == JViewerApp.WEB_PREVIEWER_CONNECT_FAILURE) {
+					break;
+				}
+				// Sleep for 1000 ms
+				try {
+					retryCount++;
+					Thread.currentThread().sleep(1000);
+				} catch (InterruptedException e) {
+					e.printStackTrace();
+				}
 			}
-			// Sleep for 1000 ms
-			try {
-				retryCount++;
-				Thread.currentThread().sleep(1000);
-			} catch (InterruptedException e) {
-				e.printStackTrace();
-			}			
-		}
-		}
-		if(m_capStatus == JViewerApp.WEB_PREVIEWER_CAPTURE_FAILURE || m_capStatus == JViewerApp.WEB_PREVIEWER_CONNECT_FAILURE || m_capStatus == JViewerApp.WEB_PREVIEWER_INVALID_SERVERIP) {
+		}
+		if(m_capStatus == JViewerApp.WEB_PREVIEWER_CAPTURE_FAILURE ||
+			m_capStatus == JViewerApp.WEB_PREVIEWER_CONNECT_FAILURE ||
+			m_capStatus == JViewerApp.WEB_PREVIEWER_INVALID_SERVERIP ||
+			m_capStatus == JViewerApp.WEB_PREVIEWER_HOST_POWER_OFF) {
 				retryCount = 0;
 				Capture1.setEnabled(true);
 				repaint();
 				return;
-		}			
+		}
 		
 		// this method only use for HTTP
 		try {
@@ -217,7 +226,7 @@
 			connection.setUseCaches(false);
 			DataInputStream in = new DataInputStream(connection.getInputStream());
 			m_frameBuf = new byte[2 * MAX_SCREEN_WIDTH * MAX_SCREEN_HEIGHT];
-            m_frameByteBuf = ByteBuffer.wrap(m_frameBuf);	
+			m_frameByteBuf = ByteBuffer.wrap(m_frameBuf);	
 			/*File f = new File ("Z:\\sengud\\megarac\\spx\\workspace\\RAINIER_WEBPREVIEW\\Build\\output\\ImageTree\\usr\\local\\www\\Java\\release\\webPreview.cap");
 			DataInputStream in = new DataInputStream(new FileInputStream(f));*/
 		
