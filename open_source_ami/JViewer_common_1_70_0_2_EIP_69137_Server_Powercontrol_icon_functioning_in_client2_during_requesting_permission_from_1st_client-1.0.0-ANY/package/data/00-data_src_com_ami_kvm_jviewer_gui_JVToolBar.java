--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java Mon Sep  5 17:06:11 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java Thu Sep  8 15:53:30 2011
@@ -396,7 +396,7 @@
 		   if(JViewerApp.getInstance().getM_frame().getMenu().getMenuEnable(JVMenu.POWER_ON_SERVER)){
 			   JViewerApp.getInstance().onSendPowerControlCommand(JVMenu.POWER_ON_SERVER);
 		   }
-		   else{
+		   else if(JViewerApp.getInstance().getM_frame().getMenu().getMenuEnable(JVMenu.POWER_OFF_ORDERLY)){
 			   JViewerApp.getInstance().onSendPowerControlCommand(JVMenu.POWER_OFF_ORDERLY);
 		   }
 	   }
