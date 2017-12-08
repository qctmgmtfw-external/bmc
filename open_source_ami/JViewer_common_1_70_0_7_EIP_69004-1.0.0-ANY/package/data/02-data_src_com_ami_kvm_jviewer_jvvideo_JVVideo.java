--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/jvvideo/JVVideo.java Fri Sep 16 14:25:30 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/jvvideo/JVVideo.java Fri Sep 16 15:47:52 2011
@@ -251,7 +251,11 @@
 			Debug.out.println(e);
 		}
 
-		JViewerApp.getInstance().getPrepare_buf().SetImage(img);		
+		JViewerApp.getInstance().getPrepare_buf().SetImage(img);
+		//Set the value of m_cur_width and m_cur_height as the widht and height of the
+		//No Signal image respectively.
+		m_view.setM_cur_width(img.getWidth());
+		m_view.setM_cur_height(img.getHeight());
 		if (VideoRecord.Recording_Started == true)
 		{
 			synchronized(JViewerApp.getInstance().getM_videorecord().obj) {
