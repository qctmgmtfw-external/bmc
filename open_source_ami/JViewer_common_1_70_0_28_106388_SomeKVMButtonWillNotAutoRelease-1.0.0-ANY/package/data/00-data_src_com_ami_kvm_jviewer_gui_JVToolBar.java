--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java Mon Dec  3 12:43:50 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVToolBar.java Tue Dec  4 13:36:03 2012
@@ -290,6 +290,7 @@
 
 		else if(e.getSource()== fullScreenBtn)
        {
+/*
            if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.VIDEO_FULL_SCREEN).isEnabled())
            {
         	   if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.VIDEO_FULL_SCREEN).isSelected())
@@ -306,6 +307,7 @@
            }else{
         	   fullScreenBtn.setToolTipText(" Full Screen option is disabled");
            }
+*/
        }
 
 		else if(e.getSource()== altCtrlDelBtn)
@@ -341,24 +343,30 @@
        }
 		else if(e.getSource() == cdBtn)
 	   {
+/*
 		if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.DEVICE_MEDIA_DIALOG).isEnabled())
 		{
 		  JViewerApp.getInstance().OnvMedia();
 		}
+*/
 	   }
 		else if(e.getSource() == floppyBtn)
 	   {
+/*
 		if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.DEVICE_MEDIA_DIALOG).isEnabled())
 	    	{
 		  JViewerApp.getInstance().OnvMedia();
 		}
+*/
 	   }
 		else if(e.getSource() == hardddiskBtn)
 	   {
+/*	   
 		if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.DEVICE_MEDIA_DIALOG).isEnabled())
 	    	{
 		  JViewerApp.getInstance().OnvMedia();
 		}
+*/
 	   }
 	   else if(e.getSource() == videoRecordBtn)
 	   {
@@ -434,6 +442,47 @@
 					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.VIDEO_RECORD_SETTINGS, false);
 					JViewerApp.getInstance().OnVideoRecordSettings();
 				}
+			}
+		}
+		else if(e.getSource()== fullScreenBtn)
+		{
+			if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.VIDEO_FULL_SCREEN).isEnabled())
+			{
+				if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.VIDEO_FULL_SCREEN).isSelected())
+				{
+					JViewerApp.getInstance().OnVideoFullScreen(false);
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.VIDEO_FULL_SCREEN,false);
+				}
+				else
+				{
+					JViewerApp.getInstance().OnVideoFullScreen(true);
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.VIDEO_FULL_SCREEN,true);
+				}
+			}
+			else
+			{
+				fullScreenBtn.setToolTipText(" Full Screen option is disabled");
+			}
+		}
+		else if(e.getSource() == cdBtn)
+		{
+			if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.DEVICE_MEDIA_DIALOG).isEnabled())
+			{
+				JViewerApp.getInstance().OnvMedia();
+			}
+		}
+		else if(e.getSource() == floppyBtn)
+		{
+			if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.DEVICE_MEDIA_DIALOG).isEnabled())
+	    	{
+				JViewerApp.getInstance().OnvMedia();
+			}
+		}
+		else if(e.getSource() == hardddiskBtn)
+		{
+			if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.DEVICE_MEDIA_DIALOG).isEnabled())
+	    	{
+				JViewerApp.getInstance().OnvMedia();
 			}
 		}
 
