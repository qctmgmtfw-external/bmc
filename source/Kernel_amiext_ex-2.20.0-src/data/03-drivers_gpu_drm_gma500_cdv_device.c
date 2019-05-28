--- linux-3.14.17/drivers/gpu/drm/gma500/cdv_device.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/drivers/gpu/drm/gma500/cdv_device.c	2014-08-21 11:05:58.693522896 -0400
@@ -55,7 +55,8 @@
 	cdv_disable_vga(dev);
 
 	cdv_intel_crt_init(dev, &dev_priv->mode_dev);
-	cdv_intel_lvds_init(dev, &dev_priv->mode_dev);
+// AMI Temp fix- DVI not supported
+//	cdv_intel_lvds_init(dev, &dev_priv->mode_dev); 
 
 	/* These bits indicate HDMI not SDVO on CDV */
 	if (REG_READ(SDVOB) & SDVO_DETECTED) {
