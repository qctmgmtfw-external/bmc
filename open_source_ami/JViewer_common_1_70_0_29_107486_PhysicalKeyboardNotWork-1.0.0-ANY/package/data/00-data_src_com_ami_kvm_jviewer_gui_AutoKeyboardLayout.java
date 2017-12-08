--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/AutoKeyboardLayout.java Tue Dec  4 15:51:20 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/AutoKeyboardLayout.java Fri Dec 14 17:06:57 2012
@@ -91,7 +91,7 @@
 		French_Alt_gr_linuxMap.put(38,49);
 		French_Alt_gr_linuxMap.put(126,50);
 		French_Alt_gr_linuxMap.put(35,51);
-		French_Alt_gr_linuxMap.put(123,222);
+		French_Alt_gr_linuxMap.put(123,KeyEvent.VK_4);//for new, alt + 4
 		French_Alt_gr_linuxMap.put(91,53);
 		French_Alt_gr_linuxMap.put(124,54);
 		French_Alt_gr_linuxMap.put(96,55);
@@ -121,6 +121,7 @@
 		Spanish_Alt_gr_linuxMap.put(123,KeyEvent.VK_DEAD_ACUTE);
 		Spanish_Alt_gr_linuxMap.put(91,KeyEvent.VK_OPEN_BRACKET);
 		Spanish_Alt_gr_linuxMap.put(8364,KeyEvent.VK_5);
+		Spanish_Alt_gr_linuxMap.put(125,92);//for new, alt + \
 		German_Map.put(223,45);
 		German_Map.put(63,45);
 		German_Map.put(92,47);//VK_BACK_SLASH
