--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/jvvideo/JVVideo.java	Wed Jun 13 14:18:13 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/jvvideo/JVVideo.java	Thu Jun 14 18:22:27 2012
@@ -164,7 +164,7 @@
 			Debug.out.println("Invalid resolution");
 			return false;
 		}
-        else if(JViewer.isWebPreviewer()){
+        else if(JViewer.isWebPreviewer() || JViewer.isplayerapp() || JViewer.isdownloadapp()){ 
             m_view.setRCView(m_frameHdr.getresX(), m_frameHdr.getresY(), m_frameHdr.getFrameType());
             return true;
         }
