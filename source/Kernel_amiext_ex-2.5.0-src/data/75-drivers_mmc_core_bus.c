--- linux/drivers/mmc/core/bus.c        2014-08-14 07:08:34.000000000 +0530
+++ linux_new/drivers/mmc/core/bus.c    2015-05-18 15:26:08.683365635 +0530
@@ -18,6 +18,7 @@
 #include <linux/stat.h>
 #include <linux/pm_runtime.h>

+#include <linux/kmod.h>
 #include <linux/mmc/card.h>
 #include <linux/mmc/host.h>

@@ -26,6 +27,27 @@
 #include "bus.h"

 #define to_mmc_driver(d)       container_of(d, struct mmc_driver, drv)
+#define dev_to_mmc_card(d)  container_of(d, struct mmc_card, dev)
+
+
+static int restart_sdserver(void)
+{
+        /*
+    struct subprocess_info *sub_info;
+    char *argv[] = { "/etc/init.d/sdserver", "restart", NULL };
+    static char *envp[] = { NULL };
+
+    sub_info = call_usermodehelper_setup( argv[0], argv, envp, GFP_ATOMIC );
+    if (sub_info == NULL) return -ENOMEM;
+
+    return call_usermodehelper_exec( sub_info, UMH_WAIT_PROC );
+    */
+
+    char *argv[] = { "/etc/init.d/sdserver", "restart", NULL };
+    static char *envp[] = { NULL };
+
+    return call_usermodehelper( argv[0], argv, envp, UMH_WAIT_PROC );
+}

 static ssize_t type_show(struct device *dev,
        struct device_attribute *attr, char *buf)
@@ -368,6 +390,11 @@
        mmc_init_context_info(card->host);

        ret = device_add(&card->dev);
+
+       if(!mmc_host_is_spi(card->host)) {
+               if(restart_sdserver())
+                       printk(KERN_INFO "restart_sdserver() failure\n");
+       }
        if (ret)
                return ret;

@@ -395,6 +422,10 @@
                                mmc_hostname(card->host), card->rca);
                }
                device_del(&card->dev);
+               if(!mmc_host_is_spi(card->host)) {
+                       if(restart_sdserver())
+                               printk(KERN_INFO "restart_sdserver() failure\n");
+               }
        }

        put_device(&card->dev);
