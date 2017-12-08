--- .pristine/libubenv-1.3.0-src/data/ubenv.c Wed Sep 14 14:28:17 2011
+++ source/libubenv-1.3.0-src/data/ubenv.c Wed Sep 14 15:03:04 2011
@@ -288,12 +288,14 @@
         fprintf (stderr,
             "seek error on %s: %s\n",
             "/dev/mtd0", strerror (errno));
+        free(condata);
         return 1;
     }
     if (write (fd, condata, conlen) != conlen) {
         fprintf (stderr,
             "write error on %s: %s\n",
             "/dev/mtd0", strerror (errno));
+        free(condata);
         return 1;
     }
 
@@ -313,8 +315,15 @@
     unsigned long RetVal;
     FILE* SysFile = fopen (TagName, "r");
 
-    if ((!SysFile) || (!SysVal))
-        return 1;
+    if(SysFile == NULL)
+    {
+        return 1;
+    }
+    else if(!SysVal)
+    {
+        fclose(SysFile);
+        return 1;
+    }
 
     fscanf (SysFile, "%lu", &RetVal);
     fclose (SysFile);
@@ -518,19 +527,26 @@
 
     BootVars = malloc(400);
     if(BootVars == NULL)
-		printf("Error in Mem alloc of Bootargsize \n");
+    {
+        TCRIT("Error in Mem alloc of Bootargsize \n");
+        return EGENERAL;
+    }
 
     temp = ReadBootVars(&environment,count,BootVars,BootVarlen);		
     if(temp != 0)
-	  	printf("Error in ReadBootVars \n"); 	
+    {
+        TCRIT("Error in ReadBootVars \n");
+        free(BootVars);
+        return EGENERAL;
+    }
 
   
     memcpy(VarName,BootVars,400);
 
   
     CloseEnv(&environment,mtdDev);
-    
-    return 0;
-
-}
-
+    free(BootVars);
+    return 0;
+
+}
+
