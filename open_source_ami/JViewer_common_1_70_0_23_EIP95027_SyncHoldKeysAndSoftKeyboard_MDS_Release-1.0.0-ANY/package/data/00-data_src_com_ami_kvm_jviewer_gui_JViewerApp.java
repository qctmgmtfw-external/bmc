--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Wed Aug 22 16:43:47 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Wed Aug 22 17:23:03 2012
@@ -991,6 +991,11 @@
 			this.getM_wndFrame().getM_status().getRightCtrl().setForeground(Color.red);
 		else
 			this.getM_wndFrame().getM_status().getRightCtrl().setForeground(Color.gray);
+		this.getM_wndFrame().getM_status().getRightCtrl().setSelected(state);
+
+		if(getSoftKeyboard() != null) {
+			getSoftKeyboard().syncHoldKey();
+		}
 	}
 
 	/**
@@ -1013,6 +1018,10 @@
 		else
 			this.getM_wndFrame().getM_status().getRightAlt().setForeground(Color.gray);
 		this.getM_wndFrame().getM_status().getRightAlt().setSelected(state);
+
+		if(getSoftKeyboard() != null) {
+			getSoftKeyboard().syncHoldKey();
+		}
 	}
 
 	/**
@@ -1034,6 +1043,11 @@
 			this.getM_wndFrame().getM_status().getLeftCtrl().setForeground(Color.red);
 		else
 			this.getM_wndFrame().getM_status().getLeftCtrl().setForeground(Color.gray);
+		this.getM_wndFrame().getM_status().getLeftCtrl().setSelected(state);
+
+		if(getSoftKeyboard() != null) {
+			getSoftKeyboard().syncHoldKey();
+		}		
 	}
 
 	/**
@@ -1055,6 +1069,11 @@
 			this.getM_wndFrame().getM_status().getLeftAlt().setForeground(Color.red);
 		else
 			this.getM_wndFrame().getM_status().getLeftAlt().setForeground(Color.gray);
+		this.getM_wndFrame().getM_status().getLeftAlt().setSelected(state);
+				
+		if(getSoftKeyboard() != null) {
+			getSoftKeyboard().syncHoldKey();
+		}
 	}
 
 	/**
