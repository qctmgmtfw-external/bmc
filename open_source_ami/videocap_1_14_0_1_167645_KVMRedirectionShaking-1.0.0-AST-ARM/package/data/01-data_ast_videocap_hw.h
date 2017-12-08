--- .pristine/videocap-1.14.0-ARM-AST-src/data/ast_videocap_hw.h Fri Mar 28 17:22:23 2014
+++ source/videocap-1.14.0-ARM-AST-src/data/ast_videocap_hw.h Mon May 26 13:55:27 2014
@@ -339,7 +339,7 @@
 #define ANALOG_DIFF_THRESHOLD               (8)
 #define DIGITAL_DIFF_THRESHOLD              (0)
 #define EXTERNAL_SIGNAL_ENABLE              (0)
-#define AUTO_MODE                           (0)
+#define AUTO_MODE                           (1)
 #define DISABLE_HW_CURSOR                   (1)
 
 #define ENCODE_KEYS_LEN                     (16)
@@ -459,6 +459,7 @@
 #define COMPRESSION_TIMEOUT  0x04
 #define NO_INPUT_TIMEOUT     0x05
 #define SPURIOUS_TIMEOUT     0x06
+#define AUTOMODE_TIMEOUT     0x08
 #define OTHER_TIMEOUT        0x10
 
 #endif  /* __AST_VIDEOCAP_REG_H__ */
