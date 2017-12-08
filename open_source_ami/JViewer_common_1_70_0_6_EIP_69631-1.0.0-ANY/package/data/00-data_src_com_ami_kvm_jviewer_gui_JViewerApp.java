--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Sep 16 13:19:56 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Sep 16 13:44:02 2011
@@ -1701,6 +1701,7 @@
 				if(Second_Session_flag)
 				{
 					JVFrame frame = JViewerApp.getInstance().getMainWindow();
+					JViewerApp.getInstance().getKVMClient().setM_redirection(true);
 					OnVideoStopRedirection();
 
 					JOptionPane.showMessageDialog(frame,"Unable to launch Virtual Console. Only one Virtual Console session is allowed for "+""+JViewer.getIp()+" "+"from the same client.","INFORMATION",JOptionPane.INFORMATION_MESSAGE);
