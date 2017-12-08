--- .pristine/libunix-1.5.0-src/data/proc-invok.c Tue Sep 20 11:37:44 2011
+++ source/libunix-1.5.0-src/data/proc-invok.c Tue Sep 20 12:30:03 2011
@@ -245,8 +245,8 @@
     int child;
 
     /* Make sure command is not huge */
-    if( (long)sizeof( command ) > MAX_CMDLINE )
-        return( -1 );
+    if( (long)strlen( command ) > MAX_CMDLINE )
+       return( -1 );
 
     /* Set environment to something safe */
     if( clean_environment() == -1 )
@@ -286,7 +286,7 @@
 /*@globals default_environment@*/
 {
     /* Make sure command is not huge */
-    if( (long)sizeof( command ) > MAX_CMDLINE )
+    if( (long)strlen( command ) > MAX_CMDLINE )
         return( NULL );
 
     /* Set environment to something safe */
