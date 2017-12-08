--- .pristine/goahead-1.62.0-src/data/hapi.c	Tue Jul 10 13:48:37 2012
+++ source/goahead-1.62.0-src/data/hapi.c	Wed Jul 11 10:41:47 2012
@@ -472,6 +472,20 @@
 				websWrite(wp, "<argument>EN</argument>\n");
 				printf("<argument>EN</argument>\n");
 			}
+
+			websWrite(wp, "<argument>%s</argument>\n", JAVA_ARG_KEYBOARD_LAYOUT);
+#ifdef CONFIG_SPX_FEATURE_KB_LANG_SELECT_SUPPORT	
+			{
+				printf("CONFIG_SPX_FEATURE_KB_LANG_SELECT_SUPPORT\n");
+				websWrite(wp, "<argument>%s</argument>\n", cfg_Adviser.keyboard_layout);
+				printf("<argument>%s</argument>\n", cfg_Adviser.keyboard_layout);
+			}
+#else
+			{
+				websWrite(wp, "<argument>EN</argument>\n");
+				printf("<argument>EN</argument>\n");
+			}
+#endif			
 		}
 
 		if(ExtractSessionCookie(((webs_t)wp)->cookie, "SessionCookie", currentToken) == 0)
