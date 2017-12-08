--- .pristine/libautovideorecord-1.7.0-src/data/video_record.c Tue Jan  7 02:35:55 2014
+++ source/libautovideorecord-1.7.0-src/data/video_record.c Thu Jan  9 22:43:51 2014
@@ -355,6 +355,7 @@
 	time_t	max_time = {0};
 	int	time_valid = 0;
 	char command[256] = {0};
+	int full_screen_flag = 0; 
 	
 	// Get the configurable parameters from the config file. Load default, if failed for some reason
 	if (0 != GetAutoRecordCfg(&AutoRecordCfg))
@@ -399,6 +400,7 @@
 			continue;
 		}
 
+		full_screen_flag = 1; 
 		// Designate filename to capture the Dump
 		set_filename_for_dump();
 
@@ -420,6 +422,12 @@
 			{
 				// a session is active. Setting up the flag
 				g_active_session = 1;
+
+				if (full_screen_flag)
+				{
+					g_send_full_screen = 1;
+					full_screen_flag = 0; 
+				}
 			}
 			else
 			{
