--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/kvmpkts/KVMClient.java	Wed May 30 10:49:58 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/kvmpkts/KVMClient.java	Thu May 31 15:25:51 2012
@@ -351,7 +351,8 @@
             break;
 		case IVTPPktHdr.IVTP_GET_ACTIVE_CLIENTS:
 			readUserData();
-			JViewerApp.getInstance().getJVMenu().updateUserMenu();			
+			JViewerApp.getInstance().getJVMenu().updateUserMenu();		
+			sendPowerStatusRequest();// send power status request.
 			break;
 			
 		default:
@@ -458,7 +459,7 @@
 	{
 		m_redirection = true;
 		JViewerApp.getSoc_manager().getSOCApp().SOC_Session_validated();
-		sendPowerStatusRequest();// send power status request.
+//		sendPowerStatusRequest();// send power status request.
 		Debug.out.println("KVM Redirection Started!");
 		return 0;
 
