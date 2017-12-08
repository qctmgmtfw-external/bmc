--- .pristine/libvmedia-1.13.0-src/data/vmedia_instance.c	Tue Jun  3 12:11:51 2014
+++ source/libvmedia-1.13.0-src/data/vmedia_instance.c	Wed Jun  4 16:01:37 2014
@@ -562,6 +562,22 @@
 
 }
 
-
-
-
+int Checkanyinstancerunning(int gWasInstanceUsed[],int NumInstances)
+{
+	int Index=0;
+	
+	int retval = 0;
+	for(Index= 0; Index < NumInstances;Index++)
+	{
+		if(gWasInstanceUsed[Index] == 1)
+		{
+			retval = 1;
+			break;
+		}
+	}
+	return retval;
+}
+
+
+
+
