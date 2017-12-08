--- .pristine/libipmi-2.38.0-src/data/scsi.c Wed Sep 14 14:28:18 2011
+++ source/libipmi-2.38.0-src/data/scsi.c Wed Sep 14 15:27:50 2011
@@ -118,6 +118,7 @@
 			sscanf(linebuf,"%s %*s %*s - %s",module,Status);
 			if((strcasecmp(module,"sg")==0)&&(strcasecmp(Status,"LIVE")==0))
 			{
+				fclose(fp);
 				return 0;
 			}
 		}
@@ -137,6 +138,7 @@
                         sscanf(linebuf,"%s %*s %*s - %s",module,Status);
                         if((strcasecmp(module,"sg")==0)&&(strcasecmp(Status,"LIVE")==0))
                         {
+                                fclose(fp);
                                 return 0;
                         }
                 }
