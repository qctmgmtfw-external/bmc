--- .pristine/goahead-1.62.0-src/data/default.c Tue May 13 13:16:51 2014
+++ source/goahead-1.62.0-src/data/default.c Tue May 13 13:27:42 2014
@@ -232,7 +232,8 @@
 	{
 		//websWrite(wp, T("Pragma: no-cache\r\nCache-Control: no-cache,private\r\n"));
 		websWrite(wp, T("Expires: 0\r\n"));
-		bytes = sbuf.size+2048;
+		//bytes = sbuf.size+2048;
+		bytes = 0;
 	}
 	else {
 		if ((date = websGetDateString(&sbuf)) != NULL) {
