--- .pristine/libsmtp-1.6.0-src/data/smtpclient.c Thu Oct 25 15:55:03 2012
+++ source/libsmtp-1.6.0-src/data/smtpclient.c Thu Oct 25 16:48:18 2012
@@ -800,12 +800,12 @@
 		}
 		else
 		{
+			fprintf(sfp, "\r\n");
+			fflush(sfp);
 			/*
 			*  Give out Message body.
 			*/
-			fprintf(sfp,"%s\r\n",pmail->message_body);
-			fprintf(sfp, "\r\n");
-			fflush(sfp);
+			fprintf(sfp,"%s\r\n",pmail->message_body);			
 		}
 		chat(CHECK_NORMAL, ".\r\n");
 	}
