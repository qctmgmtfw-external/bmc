--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java	Mon Jun 11 11:50:33 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java	Wed Jun 13 11:59:22 2012
@@ -863,7 +863,8 @@
 			{
 				removeMouseMotionListener(m_mouseListener);
 				robbie.mouseMove(viewPositionX+Mouse_X, currMousePos.y);
-				robbie.delay(30);// set a 30 millisecond delay between consecutive mouse movements.
+				if(System.getProperty("os.name").startsWith("Mac"))
+					robbie.delay(30);// set a 30 millisecond delay between consecutive mouse movements.
 				currMousePos = MouseInfo.getPointerInfo().getLocation();
 				Point AftercurrMousePos = MouseInfo.getPointerInfo().getLocation();
 				ret=1;
@@ -875,7 +876,8 @@
 				removeMouseMotionListener(m_mouseListener);
 				currMousePos = MouseInfo.getPointerInfo().getLocation();
 				robbie.mouseMove(currMousePos.x,viewPositionY+Mouse_Y);
-				robbie.delay(30);// set a 30 millisecond delay between consecutive mouse movements.
+				if(System.getProperty("os.name").startsWith("Mac"))
+					robbie.delay(30);// set a 30 millisecond delay between consecutive mouse movements.
 				Point AftercurrMousePos = MouseInfo.getPointerInfo().getLocation();
 				ret=1;
 				lastY =AftercurrMousePos.y-viewPositionY ;
@@ -976,14 +978,16 @@
 					{
 						Mouse_X = 0;
 						robbie.mouseMove(p.x, currMousePos.y);
-						robbie.delay(30);// set a 30 millisecond delay between consecutive mouse movements.
+						if(System.getProperty("os.name").startsWith("Mac"))
+							robbie.delay(30);// set a 30 millisecond delay between consecutive mouse movements.
 					}
 
 					if(Mouse_Y <= 0)
 					{
 						Mouse_Y = 0;
 						robbie.mouseMove(currMousePos.x,p.y);
-						robbie.delay(30);// set a 30 millisecond delay between consecutive mouse movements.
+						if(System.getProperty("os.name").startsWith("Mac"))
+							robbie.delay(30);// set a 30 millisecond delay between consecutive mouse movements.
 					}
 
 					if(Mouse_X >= m_cur_width ) {
