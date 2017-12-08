--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/jvvideo/JVVideo.java Fri Jan 20 15:45:32 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/jvvideo/JVVideo.java Fri Jan 20 15:53:59 2012
@@ -272,7 +272,8 @@
 			if(!psMonitor.isMonitorRunning()){
 				psMonitorTimer = new Timer(true);
 				try{
-					psMonitorTimer.schedule(psMonitor, 0, 30000);		
+					// schedule the psMonitor for every 30 seconds
+					psMonitorTimer.schedule(psMonitor, 0, PowerStatusMonitor.WAIT_TIME);		
 				}catch(Exception e){
 					e.printStackTrace();
 				}
