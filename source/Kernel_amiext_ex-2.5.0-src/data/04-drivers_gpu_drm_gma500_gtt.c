--- linux.pristine/drivers/gpu/drm/gma500/gtt.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/drivers/gpu/drm/gma500/gtt.c	2014-02-03 15:11:17.175028950 -0500
@@ -384,7 +384,7 @@
 		(void) PSB_RVDC32(PSB_PGETBL_CTL);
 	}
 	if (dev_priv->vram_addr)
-		iounmap(dev_priv->gtt_map);
+		iounmap(dev_priv->vram_addr);
 }
 
 int psb_gtt_init(struct drm_device *dev, int resume)
@@ -411,7 +411,8 @@
 	pci_write_config_word(dev->pdev, PSB_GMCH_CTRL,
 			      dev_priv->gmch_ctrl | _PSB_GMCH_ENABLED);
 
-	dev_priv->pge_ctl = PSB_RVDC32(PSB_PGETBL_CTL);
+	//dev_priv->pge_ctl = PSB_RVDC32(PSB_PGETBL_CTL);
+	dev_priv->pge_ctl = 0xcfff0000;		/* AMI : Support for 1920x1080x32bpp*/
 	PSB_WVDC32(dev_priv->pge_ctl | _PSB_PGETBL_ENABLED, PSB_PGETBL_CTL);
 	(void) PSB_RVDC32(PSB_PGETBL_CTL);
 
@@ -434,7 +435,8 @@
 	/* CDV doesn't report this. In which case the system has 64 gtt pages */
 	if (pg->gtt_start == 0 || gtt_pages == 0) {
 		dev_dbg(dev->dev, "GTT PCI BAR not initialized.\n");
-		gtt_pages = 64;
+	//	gtt_pages = 64;
+		gtt_pages = 16;				/* AMI : Support for 1920x1080x32bpp*/
 		pg->gtt_start = dev_priv->pge_ctl;
 	}
 
