--- .pristine/libuserm-1.12.0-src/data/groupio.c Wed Sep 14 14:28:24 2011
+++ source/libuserm-1.12.0-src/data/groupio.c Wed Sep 14 15:41:33 2011
@@ -49,9 +49,15 @@
 		return NULL;
 	*gr = *grent;
 	if (!(gr->gr_name = strdup(grent->gr_name)))
-		return NULL;
+	{
+		free(gr);
+		return NULL;
+	}
 	if (!(gr->gr_passwd = strdup(grent->gr_passwd)))
-		return NULL;
+	{
+		free(gr);
+		return NULL;
+	}
 
 	for (i = 0; grent->gr_mem[i]; i++);
 
@@ -59,11 +65,18 @@
 
 
 	if (!gr->gr_mem)
-		return NULL;
+	{
+		free(gr);
+		return NULL;
+	}
 	for (i = 0; grent->gr_mem[i]; i++) {
 		gr->gr_mem[i] = strdup(grent->gr_mem[i]);
 		if (!gr->gr_mem[i])
+		{
+			free(gr->gr_mem);
+			free(gr);
 			return NULL;
+		}
 	}
 	gr->gr_mem[i] = NULL;
 	return gr;
