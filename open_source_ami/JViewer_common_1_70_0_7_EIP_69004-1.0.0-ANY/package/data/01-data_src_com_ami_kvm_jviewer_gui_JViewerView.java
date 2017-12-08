--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java Fri Sep 16 14:25:42 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java Fri Sep 16 15:52:54 2011
@@ -243,6 +243,34 @@
 	}
 
 	/**
+	 * @return the m_cur_width
+	 */
+	public short getM_cur_width() {
+		return m_cur_width;
+	}
+
+	/**
+	 * @param m_cur_width the m_cur_width to set
+	 */
+	public void setM_cur_width(int m_cur_width) {
+		this.m_cur_width = (short) m_cur_width;
+	}
+
+	/**
+	 * @return the m_cur_height
+	 */
+	public short getM_cur_height() {
+		return m_cur_height;
+	}
+
+	/**
+	 * @param m_cur_height the m_cur_height to set
+	 */
+	public void setM_cur_height(int m_cur_height) {
+		this.m_cur_height = (short) m_cur_height;
+	}
+
+	/**
 	 * Component update override. This override is to optimize painting. update
 	 * is allowed to do only painting.
 	 *
@@ -1078,52 +1106,52 @@
 					return;
 				}
 			}
-			 if(mousevent != null ){
-					curX = mousevent.getX();
-					curY = mousevent.getY();
-
-					if (curX < 0)
-						curX = 0;
-					if (curY < 0)
-						curY = 0;
-
-					if( m_USBMouseMode == USBMouseRep.ABSOLUTE_MOUSE_MODE )
+			if(mousevent != null ){
+				curX = mousevent.getX();
+				curY = mousevent.getY();
+
+				if (curX < 0)
+					curX = 0;
+				if (curY < 0)
+					curY = 0;
+
+				if( m_USBMouseMode == USBMouseRep.ABSOLUTE_MOUSE_MODE ){
+
+					//  do not send the mouse move if it is in grey area.
+					if( curX < (m_cur_width*JVMenu.m_scale) && curY < (m_cur_height*JVMenu.m_scale) && 
+							JViewerApp.getInstance().getPowerStatus() == JViewerApp.SERVER_POWER_ON)
 					{
-					   //  do not send the mouse move if it is in grey area.
-					   // if( curX < m_act_width && curY < m_act_height)
-						if( curX < (m_cur_width*JVMenu.m_scale) && curY < (m_cur_height*JVMenu.m_scale))
-					    {
-							KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
-		                    m_USBMouseRep.set(mouseButtonStatus, curX/JVMenu.m_scale, curY/JVMenu.m_scale, 
+						KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
+						m_USBMouseRep.set(mouseButtonStatus, curX/JVMenu.m_scale, curY/JVMenu.m_scale, 
 								m_USBMouseMode, m_cur_width, m_cur_height, (byte) 0);
-							kvmClnt.sendKMMessage(m_USBMouseRep);
-							JViewerApp.getInstance().getRCView().setSentmouse_event(mousevent);
-
-					    }
-					    return;
-					}
-					else if (m_USBMouseMode == USBMouseRep.RELATIVE_MOUSE_MODE ){
-
-						// calculate displacement
-					    int xDisp = curX - lastX;
-					    int yDisp = curY - lastY;
-					    int ret = 0;
-					    //  do not send the mouse move if it is in grey area.
-						// if( curX < m_act_width && curY < m_act_height)
-					    if(curX < (m_cur_width*JVMenu.m_scale) && curY < (m_cur_height*JVMenu.m_scale))
-					    	ret = JViewerApp.getInstance().getRCView().m_mouseListener.splitandsend_Move(xDisp,yDisp,true);
-					    JViewerApp.getInstance().getRCView().setSentmouse_event(mousevent);
-					    // store current mouse location to find the displacement
-					    // for next mouse move.
-
-						if(ret == 0)
-						{
-							lastX = curX;
-							lastY = curY;
-						}
-					}
-		            repaint();
-			 }
+						kvmClnt.sendKMMessage(m_USBMouseRep);
+						JViewerApp.getInstance().getRCView().setSentmouse_event(mousevent);
+					}
+					return;
+				}
+				else if (m_USBMouseMode == USBMouseRep.RELATIVE_MOUSE_MODE ){
+
+					// calculate displacement
+					int xDisp = curX - lastX;
+					int yDisp = curY - lastY;
+					int ret = 0;
+					//  do not send the mouse move if it is in grey area.
+					if( curX < (m_cur_width*JVMenu.m_scale) && curY < (m_cur_height*JVMenu.m_scale) && 
+							JViewerApp.getInstance().getPowerStatus() == JViewerApp.SERVER_POWER_ON)
+					{
+						ret = JViewerApp.getInstance().getRCView().m_mouseListener.splitandsend_Move(xDisp,yDisp,true);
+					}
+					JViewerApp.getInstance().getRCView().setSentmouse_event(mousevent);
+					// store current mouse location to find the displacemen
+					//for next mouse move.
+					if(ret == 0)
+					{
+						lastX = curX;
+						lastY = curY;
+					}
+				}
+				repaint();
+			}
 
 		}
 
