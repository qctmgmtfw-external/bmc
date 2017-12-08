--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java	Fri Jul  6 14:24:07 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java	Fri Jul  6 16:53:19 2012
@@ -843,6 +843,7 @@
 
 	    public int  splitandsend_Move(int xDisp, int yDisp, boolean isLinuxMode )
 		{
+			boolean XDiff_flag = false, YDiff_flag = false;
 	    	if(JViewerView.Lost_focus_flag)
 	    		return 0;
 	    	int ret = splitandsend(xDisp, yDisp,isLinuxMode );
@@ -870,6 +871,7 @@
 				ret=1;
 				lastX = AftercurrMousePos.x-viewPositionX ;
 				addMouseMotionListener(m_mouseListener);
+				XDiff_flag = true;
 			}
 			if((YDiff <= -2 || YDiff >= 2 ) && currMousePos.y < Endy)
 			{
@@ -882,7 +884,14 @@
 				ret=1;
 				lastY =AftercurrMousePos.y-viewPositionY ;
 				addMouseMotionListener(m_mouseListener);
-			}
+				YDiff_flag = true;
+			}
+			if( !XDiff_flag )
+				lastX = curX;
+			
+			if( !YDiff_flag )
+				lastY = curY;
+			
 			return ret;
 	    }
 
@@ -1150,8 +1159,8 @@
 					//for next mouse move.
 					if(ret == 0)
 					{
-						lastX = curX;
-						lastY = curY;
+//						lastX = curX;
+//						lastY = curY;
 					}
 				}
 				repaint();
