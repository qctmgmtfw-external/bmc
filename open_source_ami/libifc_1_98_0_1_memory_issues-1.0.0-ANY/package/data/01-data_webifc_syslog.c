--- .pristine/libifc-1.98.0-src/data/webifc_syslog.c Wed Sep 14 14:34:06 2011
+++ source/libifc-1.98.0-src/data/webifc_syslog.c Wed Sep 14 17:05:57 2011
@@ -171,13 +171,13 @@
 		
 		memset(buf, 0, filesize + 1);
 		escbuf = (char *)malloc(filesize*2 +1);
-		memset(escbuf, 0, filesize*2 +1);
+		
 		if (NULL == escbuf)
 		{
 			TCRIT("Unable to allocate space for escape buffer field\n");
 			goto error_out;
 		}
-		
+		memset(escbuf, 0, filesize*2 +1);
 		fread(buf, filesize, 1, fp);
 		Escaper((char *)buf, (char *)escbuf);
 
