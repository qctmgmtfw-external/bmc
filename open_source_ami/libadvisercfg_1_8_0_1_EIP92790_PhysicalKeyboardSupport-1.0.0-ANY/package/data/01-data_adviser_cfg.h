--- .pristine/libadvisercfg-1.8.0-src/data/adviser_cfg.h	Tue Jul 10 13:48:34 2012
+++ source/libadvisercfg-1.8.0-src/data/adviser_cfg.h	Wed Jul 11 12:27:55 2012
@@ -17,13 +17,16 @@
 #define STR_SECURE_CHANNEL      "secure_channel"    /** Key string that represents secure channel */
 #define STR_MOUSE_MODE      	"mouse_mode"    /** Key string that represents mouse mode */
 #define STR_DISABLE_LOCAL      	"disable_local"    /** Key string that represents local console */
+#define STR_KEYBOARD_LAYOUT     "keyboard_layout"  
 
 //#define KVMSERVER_RESTART "/etc/init.d/kvm_servers restart"
 #define ADVISER_RESTART		"/etc/init.d/adviserd.sh restart"
 #define ADVISER_RESTART_CONF	"/etc/init.d/adviserd.sh restart_conf"
+#define KEYBOARD_LANG_SIZE 3
 
 typedef struct {
 	unsigned int mouse_mode;   /** Says whether to use Absolute mouse or relative mouse */
+    char   keyboard_layout[KEYBOARD_LANG_SIZE];
 } AdviserCfg_T;
 
 
@@ -31,6 +34,8 @@
 int SetAdviserCfg(AdviserCfg_T* pAdviserCfg);
 int GetAdviserSecureStatus(void);
 int SetAdviserSecureStatus(int secure);
+int SetMouseModeCfg(unsigned int mouse_mode);
+int SetKeyLayoutCfg(char *keyboard_layout);
 
 #endif
 
