--- linux-2.6.28.10/drivers/video/ast/ast_fb_ioctl.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10-ami/drivers/video/ast/ast_fb_ioctl.h	2011-02-24 15:01:32.000000000 +0800
@@ -0,0 +1,16 @@
+#ifndef __AST_FB_IOCTL_H__
+#define __AST_FB_IOCTL_H__
+
+struct astfb_dfbinfo {
+	unsigned long ulFBSize;
+	unsigned long ulFBPhys;
+
+	unsigned long ulCMDQSize;
+	unsigned long ulCMDQOffset;
+
+	unsigned long ul2DMode;
+};
+
+#define ASTFB_GET_DFBINFO	_IOR(0xF3,0x00,struct astfb_dfbinfo)
+
+#endif /* !__AST_FB_IOCTL_H__ */

