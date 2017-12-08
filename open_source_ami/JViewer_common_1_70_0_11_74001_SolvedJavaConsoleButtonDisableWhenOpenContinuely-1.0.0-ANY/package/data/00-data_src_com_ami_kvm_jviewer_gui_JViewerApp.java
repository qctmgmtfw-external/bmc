--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Tue Nov 22 11:29:38 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Tue Nov 22 11:39:45 2011
@@ -702,6 +702,16 @@
 		sockvmclient.SetVidoclnt(vidClnt);
 		sockvmclient.SetKVMClient(m_KVMClnt);
 		sockvmclient.SOCKVM_reader();
+
+                //solved Java console button disable when open Java console continuely
+                if(!JViewer.isWebPreviewer()) {
+                        if (!m_KVMClnt.redirection()){
+                                JViewerApp.getInstance().getM_wndFrame().toolbar.changeMacrowsStatus(false);
+                                JViewerApp.getInstance().getM_wndFrame().getM_status().enableStatusBar(false);
+                        }
+                }
+
+
 		// start redirection
 		if (-1 == m_KVMClnt.startRedirection()){
 			m_RedirectionState = REDIR_STOPPED;
@@ -725,8 +735,6 @@
 		if (!m_KVMClnt.redirection()){
 			String exceptmenu[]={"VideoExit","HelpAboutJViewer","VideoFullScreen"};
 			menu.enableMenu(exceptmenu,false);
-			JViewerApp.getInstance().getM_wndFrame().toolbar.changeMacrowsStatus(false);
-			JViewerApp.getInstance().getM_wndFrame().getM_status().enableStatusBar(false);
 		}
 		//start the system time monitor thread.
 		systemTimeMonitor.start();
