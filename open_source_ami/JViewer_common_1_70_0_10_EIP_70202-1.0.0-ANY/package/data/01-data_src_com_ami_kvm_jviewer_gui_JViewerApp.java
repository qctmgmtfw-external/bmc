--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Wed Sep 21 18:04:01 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Wed Sep 21 18:10:59 2011
@@ -1566,8 +1566,6 @@
 
 	public void OnSendKVMPrevilage(byte User2_status)
 	{
-		if (!m_KVMClnt.redirection()) return;
-
 		m_KVMClnt.SendKVMPrevilage(User2_status);
 	}
 	/**
@@ -1576,8 +1574,11 @@
 	public void onStopConcurrentSession(){	
 		KVMSharing.KVM_REQ_GIVEN = KVMSharing.KVM_REQ_DENIED;
 		KVMShareDialog.is_first_client = true;
-		OnChangeMenuState_KVMPartial(getJVMenu(), true);
-		JViewerApp.getInstance().getM_wndFrame().toolbar.OnChangeToolbarIconState_KVMPartial();
+		//if user has paused the session, then no need to update the controls.
+		if(m_KVMClnt.redirection()){
+			OnChangeMenuState_KVMPartial(getJVMenu(), true);
+			JViewerApp.getInstance().getM_wndFrame().toolbar.OnChangeToolbarIconState_KVMPartial();
+		}
 	}
 	/**
 	 * Send the Websessiontoken to the server
