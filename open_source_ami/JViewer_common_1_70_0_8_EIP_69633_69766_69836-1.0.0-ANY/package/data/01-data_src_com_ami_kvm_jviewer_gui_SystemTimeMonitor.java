--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/SystemTimeMonitor.java Fri Sep 16 14:25:30 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/SystemTimeMonitor.java Fri Sep 16 16:00:16 2011
@@ -26,14 +26,21 @@
 	
 	private static boolean timeChanged = false;
 	private Date currentTime = null;
-	private Date pastTime = null;
+	private Date pastTime = null;	
+	private final long SLEEP_TIME = 5000;
+	//Sleep time shift due to thread scheduling problems.
+	private final long SLEEP_TIME_SHIFT = 1000;
 	public void run(){
 		while(true){
 			currentTime = new Date(System.currentTimeMillis());
 			if(pastTime != null){
 				long timeDeviation = currentTime.getTime() - pastTime.getTime();	
-				if(timeDeviation < 5000 || timeDeviation >= 6000 ){// difference should be around 5000 millisecond(5 sec)
-					timeChanged = true;//Set time changed flag as true.
+				// deviation should be around 5000 millisecond(5 sec).
+				// + or - 1000 millisecs(1 sec) variation is considered to avoid 
+				// inconsistencies which are caused due to java thread scheduling problems.			
+				if(timeDeviation <= (SLEEP_TIME - SLEEP_TIME_SHIFT) || timeDeviation >= (SLEEP_TIME + SLEEP_TIME_SHIFT) ){
+					//Set time changed flag as true.
+					timeChanged = true;
 					//Set redirection status as REIDR_STOPPED, this will avoid unwanted message dialogs, while window closing.
 					JViewerApp.getInstance().setRedirectionStatus(JViewerApp.REDIR_STOPPED); 
 					// Exit the application.
@@ -41,7 +48,7 @@
 				}
 			}
 			try {
-				Thread.sleep(5000);//5 second sleep
+				Thread.sleep(SLEEP_TIME);//5 second sleep
 			} catch (InterruptedException e) {
 				//If interrupted from sleep, catch excexption and continue.
 				Debug.out.println("Thread.Sleep interrupted\n"+e);
