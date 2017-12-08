--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/kvmpkts/KVMClient.java	Mon Jun 18 10:42:34 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/kvmpkts/KVMClient.java	Tue Jun 19 14:08:15 2012
@@ -234,6 +234,7 @@
 			break;
 		case IVTPPktHdr.IVTP_STOP_SESSION_IMMEDIATE:
 			Debug.out.println("*** Stop session");
+			JViewerApp.getInstance().OnVideoStopRedirection();
 			if(m_pktHdr.status() == STOP_SESSION_CONF_CHANGE)
 				JOptionPane.showMessageDialog( JViewerApp.getInstance().getRCView(),
 						"Redirection configuration has been changed. Redirection engine will restart.\nCurrent session has been closed by redirection service. Please connect again.",
@@ -252,7 +253,6 @@
 						"Information",
 						JOptionPane.ERROR_MESSAGE
 						);
-			JViewerApp.getInstance().OnVideoStopRedirection();
 
 			JViewerApp.getInstance().getM_frame().windowClosed();
 
@@ -352,13 +352,20 @@
 		case IVTPPktHdr.IVTP_GET_ACTIVE_CLIENTS:
 			readUserData();
 			JViewerApp.getInstance().getJVMenu().updateUserMenu();		
-			sendPowerStatusRequest();// send power status request.
+			sendFullScreenRequest();
 			break;
 			
 		default:
 			JViewerApp.getInstance().getSockvmclient().onSocControlMessage(m_pktHdr, m_ctrlMsg);
 
 		}
+	}
+	public void sendFullScreenRequest(){
+		if (!m_redirection)  {
+			return;
+		}  
+		//send Get Full Screen packet to setting mode detect
+		OnFormIVTPHdr_Send(IVTPPktHdr.IVTP_GET_FULL_SCREEN,0, (short)0);
 	}
 
 	/**
@@ -459,7 +466,7 @@
 	{
 		m_redirection = true;
 		JViewerApp.getSoc_manager().getSOCApp().SOC_Session_validated();
-//		sendPowerStatusRequest();// send power status request.
+		sendPowerStatusRequest();// send power status request.
 		Debug.out.println("KVM Redirection Started!");
 		return 0;
 
