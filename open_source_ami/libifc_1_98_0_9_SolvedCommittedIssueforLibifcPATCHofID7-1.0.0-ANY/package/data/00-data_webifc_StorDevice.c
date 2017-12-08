--- .pristine/libifc-1.98.0-src/data/webifc_StorDevice.c Fri Mar  2 14:20:32 2012
+++ source/libifc-1.98.0-src/data/webifc_StorDevice.c Wed Mar  7 13:53:58 2012
@@ -55,7 +55,7 @@
         char *bk_str =NULL;
 	if(str!=NULL)
 	{
-			if(str[0] != '\0')
+			if(str[0] == '\0')
 			{
 				bk_str = (char *) malloc(1);
 				bk_str[0]='\0';
