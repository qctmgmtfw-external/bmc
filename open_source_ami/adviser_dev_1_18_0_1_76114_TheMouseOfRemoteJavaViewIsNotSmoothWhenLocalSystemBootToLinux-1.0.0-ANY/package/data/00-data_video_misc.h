--- .pristine/adviser_dev-1.18.0-src/data/video_misc.h Thu Nov 24 15:18:10 2011
+++ source/adviser_dev-1.18.0-src/data/video_misc.h Thu Dec  1 19:38:28 2011
@@ -20,6 +20,13 @@
 //####################################################//
 // Function Prototypes
 
+//for solved the mouse of Remote Java View is not smooth When local sytem boot to Linux.
+#if defined(SOC_AST2050) || defined(SOC_AST2150) || defined(SOC_AST2300)
+#define USING_AST_HW		TRUE
+#else
+#define USING_AST_HW		FALSE
+#endif
+
 int soc_create_threads (void);
 int soc_wait_for_threads_to_exit (void);
 void check_if_logout( void );
