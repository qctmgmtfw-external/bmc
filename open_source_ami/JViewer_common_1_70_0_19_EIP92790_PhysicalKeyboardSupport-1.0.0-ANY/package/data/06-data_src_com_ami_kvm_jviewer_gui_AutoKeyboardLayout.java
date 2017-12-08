--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/AutoKeyboardLayout.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/AutoKeyboardLayout.java	Wed Jul 11 11:29:37 2012
@@ -18,12 +18,17 @@
 
 import com.ami.iusb.FloppyRedir;
 import com.ami.kvm.jviewer.Debug;
+import com.ami.kvm.jviewer.JViewer; 
 import com.ami.kvm.jviewer.hid.KeyProcessor;
 import com.ami.kvm.jviewer.hid.USBKeyProcessorEnglish;
+import com.ami.kvm.jviewer.hid.USBKeyProcessorEnglishHost; 
 import com.ami.kvm.jviewer.hid.USBKeyProcessorFrench;
+import com.ami.kvm.jviewer.hid.USBKeyProcessorFrenchHost;
 import com.ami.kvm.jviewer.hid.USBKeyProcessorGerman;
+import com.ami.kvm.jviewer.hid.USBKeyProcessorGermanHost;
 import com.ami.kvm.jviewer.hid.USBKeyProcessorJapanese;
 import com.ami.kvm.jviewer.hid.USBKeyProcessorSpanish;
+import com.ami.kvm.jviewer.hid.USBKeyProcessorSpanishHost; 
 import com.ami.kvm.jviewer.hid.USBKeyboardRep;
 import com.ami.kvm.jviewer.kvmpkts.KVMClient;
 
@@ -33,10 +38,11 @@
 	public static final int  KBD_TYPE_FRENCH 	= 1036;
 	public static final int  KBD_TYPE_SPANISH 	= 1034;//0x40a
 	public static final int  KBD_TYPE_GERMAN	= 1031;//0x407
-	public static final int  KBD_TYPE_ENGLISH	= 1024;//400
+	public static final int  KBD_TYPE_ENGLISH   = 1033;//409 
 	public static final int  KBD_TYPE_JAPANESE 	= 1041;//411
 	
-	private int keyboardlayout;
+	private KeyProcessor m_keyprocessor = null;
+	private int keyboardType;
 	public static  boolean DEAD_FLAG 		= false;
 	public static  boolean SHIFT_FLAG 		= false;
 	public static  int SHIFT_KEY_POSITION 	= 0;
@@ -52,9 +58,11 @@
 	public HashMap<Integer, Integer> German_Map_Alt_gr_linuxMap;
 	private byte[] bdata;
 
+	private int hostKeyboardType = KBD_TYPE_ENGLISH;
 
 	public AutoKeyboardLayout() {
 		get_keybd_type();
+		getHostKeyboardType();
 		ongetKeyprocessor();
 		JViewerApp.getInstance().getM_USBKeyRep().setM_USBKeyProcessor(ongetKeyprocessor());
 		French_linuxMap = new HashMap<Integer, Integer>();
@@ -139,54 +147,81 @@
 
 	private void get_keybd_type()
 	{
+		String keybdLayout = null; 
 		FloppyRedir floppyredir = new FloppyRedir(true);
-	    String 	Keybdtype =	floppyredir.ReadKeybdType();
-	    Debug.out.println("Keybdtype"+Keybdtype);
-	    String OS_name = System.getProperty("os.name");
-	    Debug.out.println("Os_name"+OS_name);
-
-	    if(OS_name.equalsIgnoreCase("Linux") ) {
-	    	if(Keybdtype.equalsIgnoreCase("us"))
-	    		keyboardlayout = KBD_TYPE_ENGLISH;
-	    	if(Keybdtype.equalsIgnoreCase("fr"))
-	    		keyboardlayout = KBD_TYPE_FRENCH;
-	    	if(Keybdtype.equalsIgnoreCase("de"))
-	    		keyboardlayout = KBD_TYPE_GERMAN;
-	    	if(Keybdtype.equalsIgnoreCase("es"))
-	    		keyboardlayout = KBD_TYPE_SPANISH;
-	    	if(Keybdtype.equalsIgnoreCase("jp"))
-	    		keyboardlayout = KBD_TYPE_JAPANESE;
-	    }
-	    else {
-	    	try
-	    	{
-	    		Keybdtype=Keybdtype.substring(Keybdtype.length()-3, Keybdtype.length());
-	    		keyboardlayout = Integer.parseInt(Keybdtype,16);
-	    		Debug.out.println("JViewerView.KBD_TYPE"+keyboardlayout);
-	    	}
-	    	catch(Exception e)
-	    	{
-	    		Debug.out.println("Unknown Language");
-	    	}
-	    }
-	}
-
-	public int getKeyboardlayout() 
+		keybdLayout = floppyredir.ReadKeybdType();
+		Debug.out.println("Keybdtype" + keybdLayout);
+		String OS_name = System.getProperty("os.name");
+		Debug.out.println("Os_name" + OS_name);
+		if (OS_name.equalsIgnoreCase("Linux")) {
+			if (keybdLayout.equalsIgnoreCase("us"))
+				keyboardType = KBD_TYPE_ENGLISH;
+			else if (keybdLayout.equalsIgnoreCase("fr"))
+				keyboardType = KBD_TYPE_FRENCH;
+			else if (keybdLayout.equalsIgnoreCase("de"))
+				keyboardType = KBD_TYPE_GERMAN;
+			else if (keybdLayout.equalsIgnoreCase("es"))
+				keyboardType = KBD_TYPE_SPANISH;
+			else if (keybdLayout.equalsIgnoreCase("jp"))
+				keyboardType = KBD_TYPE_JAPANESE;
+			else
+				keyboardType = KBD_TYPE_ENGLISH;
+		} else {
+			try {
+				keybdLayout = keybdLayout.substring(keybdLayout.length() - 3,
+						keybdLayout.length());
+				keyboardType = Integer.parseInt(keybdLayout, 16);
+	    		Debug.out.println("JViewerView.KBD_TYPE"+keyboardType);
+			} catch (Exception e) {
+				Debug.out.println("Unknown Language");
+			}
+		}
+
+	}
+	
+	private void getHostKeyboardType(){
+		String keybdLayout = JViewer.getKeyboardLayout();
+		if (keybdLayout.equalsIgnoreCase("us"))
+			hostKeyboardType = KBD_TYPE_ENGLISH;
+		else if (keybdLayout.equalsIgnoreCase("fr"))
+			hostKeyboardType = KBD_TYPE_FRENCH;
+		else if (keybdLayout.equalsIgnoreCase("de"))
+			hostKeyboardType = KBD_TYPE_GERMAN;
+		else if (keybdLayout.equalsIgnoreCase("es"))
+			hostKeyboardType = KBD_TYPE_SPANISH;
+		else if (keybdLayout.equalsIgnoreCase("jp"))
+			hostKeyboardType = KBD_TYPE_JAPANESE;
+		else
+			hostKeyboardType = KBD_TYPE_ENGLISH;
+	}
+
+	public int getKeyboardType() 
 	{
-		return keyboardlayout;
-	}
-
-	public void setKeyboardlayout(int keyboardlayout) 
+		get_keybd_type();
+		return keyboardType;
+	}
+
+	public void setKeyboardType(int keyboardType) 
 	{
-		this.keyboardlayout = keyboardlayout;
-	}
-
+		if(this.keyboardType != keyboardType){
+			this.keyboardType = keyboardType;
+			m_keyprocessor = null;
+		}
+	}
+
+	public void setHostKeyboardType(int keyboardType) 
+	{
+		if(hostKeyboardType != keyboardType){
+			hostKeyboardType = keyboardType;
+			m_keyprocessor = null;
+		}
+	}
 	public boolean OnkeyTyped(KeyEvent e) 
 	{
 		int keycode = 0;
 		int ascii_value = e.getKeyChar();		
 		
-		switch(keyboardlayout)
+		switch(keyboardType)
 		{
 			case KBD_TYPE_SPANISH:
 				if(e.getKeyLocation() != KeyEvent.KEY_LOCATION_NUMPAD)
@@ -247,7 +282,7 @@
 
 	private void OnSendALTGR_Keyevent(int keycode) {
 		KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
-		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_view().getM_USBKeyRep();
+		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_USBKeyRep();
 		m_USBKeyRep.set(KeyEvent.VK_CONTROL,KeyEvent.KEY_LOCATION_LEFT, false );
 		kvmClnt.sendKMMessage(m_USBKeyRep);
 		m_USBKeyRep.set(KeyEvent.VK_ALT, KeyEvent.KEY_LOCATION_RIGHT, false);
@@ -266,7 +301,7 @@
 
 	private void OnSend_Keyevent(int keycode) {
 		KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
-		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_view().getM_USBKeyRep();
+		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_USBKeyRep();
 		m_USBKeyRep.set(keycode, KeyEvent.KEY_LOCATION_STANDARD, true);
 		kvmClnt.sendKMMessage(m_USBKeyRep);
 		m_USBKeyRep.set(keycode, KeyEvent.KEY_LOCATION_STANDARD, false );
@@ -276,7 +311,7 @@
 
 	private void OnSendShiftGrave_Keyevent(int keycode,int keylocation) {
 		KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
-		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_view().getM_USBKeyRep();
+		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_USBKeyRep();
 		m_USBKeyRep.set(16, keylocation, true );
 		kvmClnt.sendKMMessage(m_USBKeyRep);
 		m_USBKeyRep.set(keycode, KeyEvent.KEY_LOCATION_STANDARD, true );
@@ -292,9 +327,9 @@
 	public boolean OnkeyPressed(KeyEvent e) {
 
 		KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
-		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_view().getM_USBKeyRep();
+		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_USBKeyRep();
 		
-		switch(keyboardlayout)
+		switch(keyboardType)
 		{
 			case KBD_TYPE_FRENCH:
 			case KBD_TYPE_SPANISH:
@@ -307,7 +342,7 @@
 				if( keyModifiers == 640 )
 					return true;
 				
-				if(keyboardlayout == KBD_TYPE_GERMAN) {
+				if(keyboardType == KBD_TYPE_GERMAN) {
 					if(e.getKeyChar()== '~' ) {
 						OnSend_Keyevent(61);
 						return true;
@@ -327,15 +362,15 @@
 	@SuppressWarnings("unchecked")
 	public boolean OnkeyReleased(KeyEvent e) {
 		KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
-		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_view().getM_USBKeyRep();
+		USBKeyboardRep m_USBKeyRep =  JViewerApp.getInstance().getM_USBKeyRep();
 
 		if( ( e.getModifiersEx() & KeyEvent.ALT_DOWN_MASK ) == KeyEvent.ALT_DOWN_MASK ) {
-			if(keyboardlayout == KBD_TYPE_ENGLISH )
+			if(keyboardType == KBD_TYPE_ENGLISH )
 				return true;
 		}
 		int keyCode = e.getKeyCode();
 		
-		switch(keyboardlayout)
+		switch(keyboardType)
 		{
 			case KBD_TYPE_GERMAN:
 				if(System.getProperty("os.name").equals("Linux")) {
@@ -483,25 +518,57 @@
 	}
 
 	public KeyProcessor ongetKeyprocessor() {
-		KeyProcessor m_keyprocessor = null;
-
-		switch(keyboardlayout)
-		{
-			case KBD_TYPE_JAPANESE:
-				m_keyprocessor = new USBKeyProcessorJapanese();
-			break;
-			case  KBD_TYPE_GERMAN:
-				m_keyprocessor = new USBKeyProcessorGerman();
-			break;
-			case KBD_TYPE_FRENCH:
-				m_keyprocessor = new USBKeyProcessorFrench();
-			break;
-			case KBD_TYPE_SPANISH:
-				m_keyprocessor = new USBKeyProcessorSpanish();
-			break;
-			default:
-				m_keyprocessor = new USBKeyProcessorEnglish();
-
+		if(m_keyprocessor == null){
+			if(JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.AUTOMAIC_LANGUAGE)){
+				switch(keyboardType)
+				{
+				case KBD_TYPE_JAPANESE:
+					m_keyprocessor = new USBKeyProcessorJapanese();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_JAPANESE, true);
+					break;
+				case  KBD_TYPE_GERMAN:
+					m_keyprocessor = new USBKeyProcessorGerman();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_GERMAN_GER, true);
+					break;
+				case KBD_TYPE_FRENCH:
+					m_keyprocessor = new USBKeyProcessorFrench();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_FRENCH, true);
+					break;
+				case KBD_TYPE_SPANISH:
+					m_keyprocessor = new USBKeyProcessorSpanish();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_SPANISH, true);
+					break;
+				default:
+					m_keyprocessor = new USBKeyProcessorEnglish();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_ENGLISH_US, true);
+
+				}
+			}
+			else{
+				switch(hostKeyboardType)
+				{
+				case KBD_TYPE_JAPANESE:
+					m_keyprocessor = new USBKeyProcessorJapanese();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_JAPANESE, true);
+					break;
+				case  KBD_TYPE_GERMAN:
+					m_keyprocessor = new USBKeyProcessorGermanHost();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_GERMAN_GER, true);
+					break;
+				case KBD_TYPE_FRENCH:
+					m_keyprocessor = new USBKeyProcessorFrenchHost();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_FRENCH, true);
+					break;
+				case KBD_TYPE_SPANISH:
+					m_keyprocessor = new USBKeyProcessorSpanishHost();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_SPANISH, true);
+					break;
+				default:
+					m_keyprocessor = new USBKeyProcessorEnglishHost();
+					JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(JVMenu.PKBRD_LANGUAGE_ENGLISH_US, true);
+
+				}
+			}
 		}
 		return m_keyprocessor;
 		// TODO Auto-generated method stub
