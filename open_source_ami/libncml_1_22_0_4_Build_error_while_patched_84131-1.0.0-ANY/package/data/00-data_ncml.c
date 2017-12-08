--- .pristine/libncml-1.22.0-src/data/ncml.c Fri Mar  9 14:10:38 2012
+++ source/libncml-1.22.0-src/data/ncml.c Fri Mar  9 14:23:06 2012
@@ -30,8 +30,7 @@
 
 #include "ncml.h"
 #include "defservicecfg.h"
-#include "nwcfg.h"
-
+#include <netinet/in.h>
 #include "dictionary.h"
 #include "iniparser.h"
 
