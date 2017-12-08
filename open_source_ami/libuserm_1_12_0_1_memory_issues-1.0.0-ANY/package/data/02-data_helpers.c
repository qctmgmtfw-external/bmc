--- .pristine/libuserm-1.12.0-src/data/helpers.c Wed Sep 14 14:28:24 2011
+++ source/libuserm-1.12.0-src/data/helpers.c Wed Sep 14 15:41:44 2011
@@ -389,6 +389,7 @@
         if(asciiz == NULL)
         {
             TCRIT("Memory allocation error\n");
+            free(asciiz);
             return NULL;
         }
         strncpy(asciiz+curindex,list[i],tmplen+1); //copy the null also
@@ -529,6 +530,7 @@
 	{
 		TCRIT("Error setting webgo should refresh cache file");
 	}
+	else
 	fclose(fp);
 
 	return 0;
