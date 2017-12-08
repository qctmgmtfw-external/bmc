--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyboardRep.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyboardRep.java	Wed Jul 11 11:42:42 2012
@@ -113,7 +113,7 @@
 
 		m_repBuf.put( (byte)( USB_PKT_KEYBDREP_SIZE & 0xff ) ); /* DataLen */
 		if(m_USBKeyProcessor == null)
-			m_USBKeyProcessor = JViewerApp.getInstance().getKeyprocesssor();
+			m_USBKeyProcessor = JViewerApp.getInstance().getKeyProcesssor();
 		/* Keyboard report */
 		byte [] keybd_report_data = m_USBKeyProcessor.convertKeyCode(this.keyCode, this.keyLocation, this.keyPress);
 
