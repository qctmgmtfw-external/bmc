--- .pristine/libifc-1.98.0-src/data/webifc_StorDevice.c Wed Jan  4 15:11:13 2012
+++ source/libifc-1.98.0-src/data/webifc_StorDevice.c Wed Jan  4 16:03:36 2012
@@ -55,17 +55,25 @@
         char *bk_str =NULL;
 	if(str!=NULL)
 	{
-        	bk_str = (char *) malloc(2*strlen(str));
-		memset(bk_str,0,(2*strlen(str)));
-	        tmp_str = str;
-	        for(i=0;i<strlen(str);i++,c++)
-	        {
-	                if(tmp_str[i]=='\'' || tmp_str[i]=='\\')
-	                {
-	                        bk_str[c++] = '\\';
-	                }
-	                bk_str[c] = tmp_str[i];
-	        }
+			if(str[0] != '\0')
+			{
+				bk_str = (char *) malloc(1);
+				bk_str[0]='\0';
+			}
+			else
+			{	
+				bk_str = (char *) malloc(2*strlen(str));
+				memset(bk_str,0,(2*strlen(str)));
+				tmp_str = str;
+				for(i=0;i<strlen(str);i++,c++)
+				{
+						if(tmp_str[i]=='\'' || tmp_str[i]=='\\')
+						{
+								bk_str[c++] = '\\';
+						}
+						bk_str[c] = tmp_str[i];
+				}
+			}
 	        str = strdup(bk_str);
 	        free(bk_str);
 	}
