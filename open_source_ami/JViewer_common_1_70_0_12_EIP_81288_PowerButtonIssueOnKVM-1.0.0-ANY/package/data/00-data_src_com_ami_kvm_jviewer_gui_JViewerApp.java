--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Jan 20 15:45:33 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Jan 20 15:54:06 2012
@@ -2444,6 +2444,7 @@
 				m_frame.getMenu().enableMenu(exceptmenu, false);// disable menus when server power off;	
 				m_wndFrame.toolbar.changeToolbarButtonStateOnPowerStatus(false);
 				m_wndFrame.toolbar.turnOnPowerButton(false);
+				m_frame.getMenu().notifyMenuStateEnable(JVMenu.POWER_ON_SERVER, true);	
 			}
 			else{
 				changeMenuItemsStatusOnPauseResume(m_frame.getMenu(), true);// change menu staes to default state.
