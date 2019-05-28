--- linux.org/drivers/video/ast/ast_fb_ioctl.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/drivers/video/ast/ast_fb_ioctl.h	2014-01-31 15:42:24.563921267 -0500
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
