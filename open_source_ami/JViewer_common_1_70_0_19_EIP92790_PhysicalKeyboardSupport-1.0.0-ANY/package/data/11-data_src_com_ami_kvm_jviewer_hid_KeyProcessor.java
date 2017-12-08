--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/KeyProcessor.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/KeyProcessor.java	Wed Jul 11 11:39:05 2012
@@ -21,6 +21,15 @@
 
 public interface KeyProcessor
 {
+	public static final int MOD_LEFT_CTRL = 0x01;
+	public static final int MOD_RIGHT_CTRL = 0x10;
+	public static final int MOD_LEFT_SHIFT = 0x02;
+	public static final int MOD_RIGHT_SHIFT = 0x20;
+	public static final int MOD_LEFT_ALT = 0x04;
+	public static final int MOD_RIGHT_ALT = 0x40;
+	public static final int MOD_LEFT_WIN = 0x08;
+	public static final int MOD_RIGHT_WIN = 0x80;
+	
     public byte[] convertKeyCode( int keyCode, int keyLocation, boolean keyPressed );
     public void setAutoKeybreakMode( boolean state );
     public boolean getAutoKeybreakMode();
