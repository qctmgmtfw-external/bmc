--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/InfoDialog.java Mon Sep  5 17:06:11 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/InfoDialog.java Thu Sep  8 16:49:28 2011
@@ -31,17 +31,19 @@
  * @author shirleyh@ami.com
  */
 public class InfoDialog {
-	public JDialog InfoDialog = null;
+	public JDialog InfoDialog;
 
-	private JLabel lblInfo = null;
+	private JLabel lblInfo;
 
-	private JPanel infoPane = null;
+	private JPanel infoPane;
 
 	/**
 	 * InfoDialog constructor comment.
 	 */
 	public InfoDialog() {
-		initialize();
+		InfoDialog = null;
+		lblInfo = null;
+		infoPane = null;
 	}
 
 	/**
@@ -63,13 +65,8 @@
 				getInfoDialog().setContentPane(getJDialogContentPane());
 				InfoDialog.setUndecorated(true);
 				// InfoDialog.getRootPane().setWindowDecorationStyle(JRootPane.NONE);
-				InfoDialog.setLocationRelativeTo(null);
-				
-				new WaitThread();
-				
-				InfoDialog.setVisible(true);
-				
-				
+				InfoDialog.setLocationRelativeTo(frame);
+							
 			} catch (java.lang.Throwable Exc) {
 				Exc.printStackTrace();
 			}
@@ -174,4 +171,14 @@
 			Exc.printStackTrace();
 		}
 	}
+	
+	/**
+	 * Shows the information dialog.
+	 */
+	public void showDialog(){
+		initialize();
+		new WaitThread();		
+		InfoDialog.setVisible(true);		
+	}
 }
+
