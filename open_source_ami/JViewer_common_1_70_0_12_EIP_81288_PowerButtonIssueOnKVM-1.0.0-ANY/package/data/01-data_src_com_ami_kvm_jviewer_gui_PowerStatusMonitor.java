--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/PowerStatusMonitor.java Fri Jan 20 15:45:32 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/PowerStatusMonitor.java Fri Jan 20 15:54:06 2012
@@ -2,10 +2,12 @@
 
 import java.util.TimerTask;
 
+import com.ami.kvm.jviewer.Debug;
+
 public class PowerStatusMonitor extends TimerTask{
 	private boolean monitorRunning = false;	
 	private final Object syncObj = new Object();
-	private final long WAIT_TIME = 30000;//(30 seconds)in milliseconds
+	public static final long WAIT_TIME = 30000;//(30 seconds)in milliseconds
 	/**
 	 * @return the monitorRunning
 	 */
@@ -20,28 +22,32 @@
 	}
 	@Override
 	public void run() {
-		// TODO Auto-generated method stub
+		// On blank screen.
+		if(JViewerApp.getInstance().getKVMClient().m_isBlank){
+			// Send power power status request to the adviser, to get the server power status. 
+			JViewerApp.getInstance().getKVMClient().sendPowerStatusRequest();
+		}
 		monitorRunning = true;
 		// Wait for 30 seconds, or until notified by Video Drawing thread when a new frame arrives.
 		try
-    	{
+		{
 			synchronized(getSyncObj()) {
 				getSyncObj().wait(WAIT_TIME);
 			}
-    	} catch (Exception e1) {
+		} catch (InterruptedException ie) {
 			// TODO Auto-generated catch block
-			e1.printStackTrace();
+			Debug.out.println("PowerStatusMonitor wait Interrupted"+ie);
 		}
-    	if(JViewerApp.getInstance().getVidClnt().isNewFrame()){	
-    		monitorRunning = false;
-    		this.cancel();
+		//On new screen.
+		if(JViewerApp.getInstance().getVidClnt().isNewFrame()){	
+			monitorRunning = false;
+			// Cancel the PowerStatusMonitor timer task.
+			this.cancel();
 			if(JViewerApp.getInstance().getVidClnt().getPsMonitorTimer() != null)
-    		JViewerApp.getInstance().getVidClnt().getPsMonitorTimer().cancel();
+				// Cancel the timer which schedules this timer task.
+				JViewerApp.getInstance().getVidClnt().getPsMonitorTimer().cancel();
+			//set the power status in JViewer to SERVER_POWER_ON.
 			JViewerApp.getInstance().onGetPowerControlStatus(JViewerApp.getInstance().SERVER_POWER_ON);	
-    		
-    	}
-    	else
-    		JViewerApp.getInstance().getKVMClient().sendPowerStatusRequest();	
+		}    		
 	}
-
 }
