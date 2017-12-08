--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/SoftKeyboard.java Wed Aug 22 16:43:47 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/SoftKeyboard.java Wed Aug 22 17:55:36 2012
@@ -833,6 +833,7 @@
 		}
 		
 		syncKbdLED();// Synchronize Softkeyboard LED status with Host LED status;
+		syncHoldKey();
 		
 		// this.toFront();
 		this.setContentPane(JDialogContentPane);
@@ -1011,15 +1012,26 @@
 						// For AltGr to be off when normal keys are pressed
 						if (m_toggleKey[6].isSelected() == true)// Right Alt
 						{
-							if (butid <= 76 && butid != 14 && butid != 65
-									&& butid != 66 && butid != 67
-									&& butid != 68 && butid != 69
-									&& butid != 72 && butid != 73
-									&& butid != 74) {
-								// System.out.println("Right Altgr Release");
-								OnRightAltRelease();
-								m_toggleKey[6].setSelected(false);
-								OnModifier(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_RIGHT, KeyEvent.KEY_RELEASED);
+							if(JViewerApp.getInstance().getM_wndFrame().getM_status().getRightAlt().isSelected() == false) {
+								if (butid <= 76 && butid != 14 && butid != 65
+										&& butid != 66 && butid != 67
+										&& butid != 68 && butid != 69
+										&& butid != 72 && butid != 73
+										&& butid != 74) {
+									// System.out.println("Right Altgr Release");
+									OnRightAltRelease();
+									m_toggleKey[6].setSelected(false);
+									OnModifier(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_RIGHT, KeyEvent.KEY_RELEASED);
+								}
+							}
+							else {
+								if ((lngindex != JVMenu.LANGUAGE_ENGLISH_US && lngindex != JVMenu.LANGUAGE_RUSSIAN &&
+									lngindex != JVMenu.LANGUAGE_JAPANESE)){
+									altGr_enable = true;
+									OnAltGrOn(altgr[lngindex]);
+								}
+								m_toggleKey[6].setSelected(true);
+								OnModifier(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_RIGHT, KeyEvent.KEY_PRESSED);
 							}
 						}
 						// For disabling & releasing alt or shift on the event
@@ -1135,42 +1147,76 @@
 
 							if (m_toggleKey[5].isSelected() == true){ // Left Alt
 								if (butid != 64 && butid != 0) {
-									// System.out.println("Inside Alt Release");
-									OnModifier(KeyEvent.VK_ALT,
-											KeyEvent.KEY_LOCATION_LEFT,
-											KeyEvent.KEY_RELEASED);
-									m_toggleKey[5].setSelected(false);
+									if(JViewerApp.getInstance().getM_wndFrame().getM_status().getLeftAlt().isSelected() == false) {
+										// System.out.println("Inside Alt Release");
+										OnModifier(KeyEvent.VK_ALT,
+												KeyEvent.KEY_LOCATION_LEFT,
+												KeyEvent.KEY_RELEASED);
+										m_toggleKey[5].setSelected(false);
+									}
+									
+									else {
+										OnModifier(KeyEvent.VK_ALT,
+												KeyEvent.KEY_LOCATION_LEFT,
+												KeyEvent.KEY_PRESSED);
+									}
 								}
 							}
 
 							if (m_toggleKey[6].isSelected() == true) {// Right Alt
 								if (butid != 64 && butid != 0) {
-									// System.out.println("Inside Alt Release");
-									OnModifier(KeyEvent.VK_ALT,
-											KeyEvent.KEY_LOCATION_RIGHT,
-											KeyEvent.KEY_RELEASED);
-									m_toggleKey[6].setSelected(false);
+									if(JViewerApp.getInstance().getM_wndFrame().getM_status().getRightAlt().isSelected() == false) {
+										// System.out.println("Inside Alt Release");
+										OnModifier(KeyEvent.VK_ALT,
+												KeyEvent.KEY_LOCATION_RIGHT,
+												KeyEvent.KEY_RELEASED);
+										m_toggleKey[6].setSelected(false);
+									}
+									else {
+										if ((lngindex != JVMenu.LANGUAGE_ENGLISH_US && lngindex != JVMenu.LANGUAGE_RUSSIAN &&
+											lngindex != JVMenu.LANGUAGE_JAPANESE)){
+											altGr_enable = true;
+											OnAltGrOn(altgr[lngindex]);
+										}
+
+										OnModifier(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_RIGHT, 
+												KeyEvent.KEY_PRESSED);
+									}
 								}
 							}
 
 							if (m_toggleKey[3].isSelected() == true) { // Left Ctrl
-								// System.out.println("Inside Left Ctrl
-								// Release");
-								butid = new Integer(m_toggleKey[3].getName().substring(3));
-								keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_LEFT, false);
-								m_KVMClnt.sendKMMessage(keyRep);
-								m_toggleKey[3].setSelected(false);
-								m_toggleKey[8].setSelected(false);
+								if(JViewerApp.getInstance().getM_wndFrame().getM_status().getLeftCtrl().isSelected() == false) {
+									// System.out.println("Inside Left Ctrl
+									// Release");
+									butid = new Integer(m_toggleKey[3].getName().substring(3));
+									keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_LEFT, false);
+									m_KVMClnt.sendKMMessage(keyRep);
+									m_toggleKey[3].setSelected(false);
+									//m_toggleKey[8].setSelected(false);
+								}
+								else {
+									butid = new Integer(m_toggleKey[3].getName().substring(3));
+									keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_LEFT, true);
+									m_KVMClnt.sendKMMessage(keyRep);
+								}
 							}
 
 							if (m_toggleKey[8].isSelected() == true) { // Right Ctrl
-								// System.out.println("Inside Right Ctrl
-								// Release");
-								butid = new Integer(m_toggleKey[3].getName().substring(3));
-								keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_RIGHT, false);
-								m_KVMClnt.sendKMMessage(keyRep);
-								m_toggleKey[8].setSelected(false);
-								m_toggleKey[3].setSelected(false);
+								if(JViewerApp.getInstance().getM_wndFrame().getM_status().getRightCtrl().isSelected() == false) {
+									// System.out.println("Inside Right Ctrl
+									// Release");
+									butid = new Integer(m_toggleKey[8].getName().substring(3));
+									keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_RIGHT, false);
+									m_KVMClnt.sendKMMessage(keyRep);
+									m_toggleKey[8].setSelected(false);
+									//m_toggleKey[3].setSelected(false);
+								}
+								else {
+									butid = new Integer(m_toggleKey[8].getName().substring(3));
+									keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_RIGHT, true);
+									m_KVMClnt.sendKMMessage(keyRep);								
+								}	
 							}
 
 						}
@@ -1473,14 +1519,16 @@
 
 							break;
 						case 69:// Left Alt
+							/*
 							if (m_toggleKey[6].isSelected() == true) {
 								m_toggleKey[6].setSelected(false);
 								keyRep.set(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_RIGHT, false);
 								m_KVMClnt.sendKMMessage(keyRep);
 							} else {
-								keyRep.set(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_LEFT, true);
-								m_KVMClnt.sendKMMessage(keyRep);
-							}
+							*/
+							keyRep.set(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_LEFT, true);
+							m_KVMClnt.sendKMMessage(keyRep);
+							//}
 							break;
 						case 71: // Right Alt
 							if ((lngindex != JVMenu.LANGUAGE_ENGLISH_US && lngindex != JVMenu.LANGUAGE_RUSSIAN &&
@@ -1488,23 +1536,25 @@
 								altGr_enable = true;
 								OnAltGrOn(altgr[lngindex]);
 							}
+							/*
 							if (m_toggleKey[5].isSelected() == true) {
 								m_toggleKey[5].setSelected(false);
 								OnModifier(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_LEFT,
 										KeyEvent.KEY_RELEASED);
 							} else
-								OnModifier(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_RIGHT,
-										KeyEvent.KEY_PRESSED);
+							*/
+							OnModifier(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_RIGHT,
+									KeyEvent.KEY_PRESSED);
 							break;
 						case 67:// Left Ctrl
-							m_toggleKey[8].setSelected(true);
+							m_toggleKey[3].setSelected(true);
 							butid = new Integer(m_toggleKey[3].getName().substring(3));
 							keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_LEFT, true);
 							m_KVMClnt.sendKMMessage(keyRep);							
 							break;
 						case 73:// Right Ctrl
-							m_toggleKey[3].setSelected(true);
-							butid = new Integer(m_toggleKey[3].getName().substring(3));
+							m_toggleKey[8].setSelected(true);
+							butid = new Integer(m_toggleKey[8].getName().substring(3));
 							keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_RIGHT, true);
 							m_KVMClnt.sendKMMessage(keyRep);
 							break;
@@ -1777,31 +1827,69 @@
 							m_KVMClnt.sendKMMessage(keyRep);
 							break;
 						case 69:// Left Alt
-							OnModifier(KeyEvent.VK_ALT,
-									KeyEvent.KEY_LOCATION_LEFT,
-									KeyEvent.KEY_RELEASED);
-							// rb.keyRelease(m_ps2.get(butid));
+							if(JViewerApp.getInstance().getM_wndFrame().getM_status().getLeftAlt().isSelected() == false) {
+								OnModifier(KeyEvent.VK_ALT,
+										KeyEvent.KEY_LOCATION_LEFT,
+										KeyEvent.KEY_RELEASED);
+								// rb.keyRelease(m_ps2.get(butid));
+							}
+							else {
+								OnModifier(KeyEvent.VK_ALT,
+										KeyEvent.KEY_LOCATION_LEFT,
+										KeyEvent.KEY_PRESSED);	
+								m_toggleKey[5].setSelected(true);							
+							}
 							break;
 						case 71: // Right Alt
-							// System.out.println("Right Alt Key Released");
-							if(lngindex != 16)
-							OnRightAltRelease();
-							OnModifier(KeyEvent.VK_ALT,
-									KeyEvent.KEY_LOCATION_RIGHT,
-									KeyEvent.KEY_RELEASED);
-							// rb.keyRelease(m_ps2.get(butid));
+							if(JViewerApp.getInstance().getM_wndFrame().getM_status().getRightAlt().isSelected() == false) {
+								// System.out.println("Right Alt Key Released");
+								if(lngindex != 16)
+								OnRightAltRelease();
+								OnModifier(KeyEvent.VK_ALT,
+										KeyEvent.KEY_LOCATION_RIGHT,
+										KeyEvent.KEY_RELEASED);
+								// rb.keyRelease(m_ps2.get(butid));
+							}
+							else {
+								if ((lngindex != JVMenu.LANGUAGE_ENGLISH_US && lngindex != JVMenu.LANGUAGE_RUSSIAN &&
+									lngindex != JVMenu.LANGUAGE_JAPANESE)){
+									altGr_enable = true;
+									OnAltGrOn(altgr[lngindex]);
+								}
+
+								m_toggleKey[6].setSelected(true);
+								OnModifier(KeyEvent.VK_ALT,
+										KeyEvent.KEY_LOCATION_RIGHT,
+										KeyEvent.KEY_PRESSED);
+							}	
 							break;
 						case 67:// Left Ctrl
-							m_toggleKey[8].setSelected(false);
-							butid = new Integer(m_toggleKey[3].getName().substring(3));
-							keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_LEFT, false);
-							m_KVMClnt.sendKMMessage(keyRep);
+							if(JViewerApp.getInstance().getM_wndFrame().getM_status().getLeftCtrl().isSelected() == false) {
+								m_toggleKey[3].setSelected(false);
+								butid = new Integer(m_toggleKey[3].getName().substring(3));
+								keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_LEFT, false);
+								m_KVMClnt.sendKMMessage(keyRep);
+							}
+							else {
+								m_toggleKey[3].setSelected(true);
+								butid = new Integer(m_toggleKey[3].getName().substring(3));
+								keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_LEFT, true);
+								m_KVMClnt.sendKMMessage(keyRep);
+							}
 							break;
 						case 73:// Right Ctrl
-							m_toggleKey[3].setSelected(false);
-							butid = new Integer(m_toggleKey[3].getName().substring(3));
-							keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_RIGHT, false);
-							m_KVMClnt.sendKMMessage(keyRep);
+							if(JViewerApp.getInstance().getM_wndFrame().getM_status().getRightCtrl().isSelected() == false) {
+								m_toggleKey[8].setSelected(false);
+								butid = new Integer(m_toggleKey[8].getName().substring(3));
+								keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_RIGHT, false);
+								m_KVMClnt.sendKMMessage(keyRep);
+							}
+							else {
+								m_toggleKey[8].setSelected(true);
+								butid = new Integer(m_toggleKey[8].getName().substring(3));
+								keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_RIGHT, true);
+								m_KVMClnt.sendKMMessage(keyRep);								
+							}
 							break;
 						default:
 							keyRep.set(m_ps2.get(butid), KeyEvent.KEY_LOCATION_STANDARD, false);
@@ -2059,7 +2147,56 @@
 	public  void setLngindex(int lngindex) {
 		SoftKeyboard.lngindex = lngindex;
 	}
+
+	public void syncHoldKey() {	
+		//Left Ctrl
+		if(JViewerApp.getInstance().getM_wndFrame().getM_status().getLeftCtrl().isSelected() == false) {
+			m_toggleKey[3].setSelected(false);
+		}
+		else {
+			m_toggleKey[3].setSelected(true);
+		}
+
+		//Right Ctrl
+		if(JViewerApp.getInstance().getM_wndFrame().getM_status().getRightCtrl().isSelected() == false) {	
+			m_toggleKey[8].setSelected(false);			
+		}
+		else {
+			m_toggleKey[8].setSelected(true);			
+		}
+
+		//Left Alt
+		if(JViewerApp.getInstance().getM_wndFrame().getM_status().getLeftAlt().isSelected() == false) {
+			m_toggleKey[5].setSelected(false);	
+		}
+		else {
+			m_toggleKey[5].setSelected(true);				
+		}
+
+		//Right Alt
+		if(JViewerApp.getInstance().getM_wndFrame().getM_status().getRightAlt().isSelected() == false) {
 	
+			if(lngindex != 16)
+				m_skmouselistener.OnRightAltRelease();
+			m_skmouselistener.OnModifier(KeyEvent.VK_ALT,
+					KeyEvent.KEY_LOCATION_RIGHT,
+					KeyEvent.KEY_RELEASED);
+			m_toggleKey[6].setSelected(false);		
+		}
+		else {
+			if ((lngindex != JVMenu.LANGUAGE_ENGLISH_US && lngindex != JVMenu.LANGUAGE_RUSSIAN &&
+				lngindex != JVMenu.LANGUAGE_JAPANESE)){
+				altGr_enable = true;
+				m_skmouselistener.OnAltGrOn(altgr[lngindex]);
+			}
+
+			m_toggleKey[6].setSelected(true);
+			m_skmouselistener.OnModifier(KeyEvent.VK_ALT,
+					KeyEvent.KEY_LOCATION_RIGHT,
+					KeyEvent.KEY_PRESSED);
+		}			
+	}
+		
 	/**
 	 * This method synchorines Softkeyboard LED status with Host keyboard LED status.
 	 */
