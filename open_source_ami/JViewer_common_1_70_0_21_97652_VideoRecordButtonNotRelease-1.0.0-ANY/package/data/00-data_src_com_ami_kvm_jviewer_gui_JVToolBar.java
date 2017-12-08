--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java Tue Aug 14 15:27:09 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java Tue Aug 14 17:59:46 2012
@@ -362,7 +362,7 @@
 	   }
 	   else if(e.getSource() == videoRecordBtn)
 	   {
-		  
+/*
 		    if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.VIDEO_RECORD_START).isEnabled())
 		    {
 			   JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.VIDEO_RECORD_START, false);
@@ -381,7 +381,7 @@
 				   JViewerApp.getInstance().OnVideoRecordSettings();
 			   }
 		   }
-		   
+*/		   
 	   }
 	   else if(e.getSource()== usersBtn)
 	   {
@@ -415,6 +415,28 @@
 	}
 
 	public void mouseReleased(MouseEvent e) {
+		if(e.getSource() == videoRecordBtn)
+		{
+			if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.VIDEO_RECORD_START).isEnabled())
+		    {
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.VIDEO_RECORD_START, false);
+				JViewerApp.getInstance().getM_videorecord().OnVideoRecordStart();
+			}
+			else if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.VIDEO_RECORD_STOP).isEnabled())
+			{
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.VIDEO_RECORD_STOP, false);
+				JViewerApp.getInstance().getM_videorecord().OnVideoRecordStop();
+			}
+			else if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.VIDEO_RECORD_SETTINGS).isEnabled())
+			{
+				//Don't show video record settings dialog in video recording is in progress.
+				if(!VideoRecord.Recording_Started || !VideoRecord.Record_Processing){
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.VIDEO_RECORD_SETTINGS, false);
+					JViewerApp.getInstance().OnVideoRecordSettings();
+				}
+			}
+		}
+
 /*		if(e.getSource()== m_hotKeyBtn)
 			m_popupMenu.show(false);*/
 	}
