--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Mon Sep 24 17:18:42 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Tue Sep 25 14:46:26 2012
@@ -2481,7 +2481,7 @@
 		m_wndFrame.toolbar.powerBtn.setEnabled(state);		
 		if(state){
 			if(powerStatus == SERVER_POWER_OFF){
-				String exceptmenu[]={JVMenu.VIDEO_EXIT, JVMenu.HELP_ABOUT_RCONSOLE, JVMenu.POWER_ON_SERVER};
+				String exceptmenu[]={JVMenu.VIDEO_EXIT, JVMenu.HELP_ABOUT_RCONSOLE, JVMenu.POWER_ON_SERVER, JVMenu.DEVICE_MEDIA_DIALOG};
 				m_frame.getMenu().enableMenu(exceptmenu, false);// disable menus when server power off;	
 				m_wndFrame.toolbar.changeToolbarButtonStateOnPowerStatus(false);
 				m_wndFrame.toolbar.turnOnPowerButton(false);
