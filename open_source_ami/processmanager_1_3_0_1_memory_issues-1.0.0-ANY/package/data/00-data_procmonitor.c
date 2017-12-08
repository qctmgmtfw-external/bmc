--- .pristine/processmanager-1.3.0-src/data/procmonitor.c Wed Sep 14 14:28:49 2011
+++ source/processmanager-1.3.0-src/data/procmonitor.c Wed Sep 14 14:53:36 2011
@@ -197,6 +197,7 @@
             if(sizeof(PROC_MON_T) != fwrite((char *)procdetails,sizeof(char),sizeof(PROC_MON_T),fd))
             {
                 TCRIT("Error in writing to file %s \n",PROCESS_MONITOR);
+                fclose(fd);
                 return -1;
             }
             break;
@@ -209,6 +210,7 @@
         if(sizeof(PROC_MON_T) != fwrite((char *)procdetails,sizeof(char),sizeof(PROC_MON_T),fd))
         {
             TCRIT("Error in writing to file %s \n",PROCESS_MONITOR);
+            fclose(fd);
             return -1;
         }
     }
@@ -228,14 +230,14 @@
 {
     int Size,Err=0,i=0,existupdate=0;
     int j=0,pos=0,slpregisterd=0;
-    char pBuf[256];
+    char pBuf[sizeof(PROC_MON_T)];
     PROC_MON_T *procdetails;
     prctl(PR_SET_NAME,__FUNCTION__,0,0,0);
 
     while(1)
     {
         Size = sizeof(PROC_MON_T);
-        memset(pBuf,0,256);
+        memset(pBuf,0,sizeof(PROC_MON_T));
         READ_PIPE(pBuf,Size,fd,&Err);
         procdetails = (PROC_MON_T*)pBuf;
 
@@ -579,13 +581,14 @@
             if(sizeof(PROC_MON_T) != fread((char *)proctable[pindex],sizeof(char),sizeof(PROC_MON_T),fp))
             {
                 TCRIT("Unable to read from file %s \n",PROCESS_MONITOR);
+                fclose(fp);
                 return -1;
             }
             pindex++;
         }
+    }
 
         fclose(fp);
-    }
 
     if (0 !=CreatePipe())
     {
@@ -602,6 +605,7 @@
     if (-1 == fd)
     {
         TCRIT("Error opening named pipe %s\n", PROC_MON);
+        fclose(fp);
         return -1;
     }
 
