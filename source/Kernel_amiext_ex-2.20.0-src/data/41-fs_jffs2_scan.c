--- linux.pristine/fs/jffs2/scan.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/fs/jffs2/scan.c	2014-02-03 15:11:00.043029093 -0500
@@ -742,7 +742,13 @@
 			if ((err = jffs2_scan_dirty_space(c, jeb, 4)))
 				return err;
 			ofs += 4;
+
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
+			return -EIO;
+#else
 			continue;
+#endif
+
 		}
 		/* We seem to have a node of sorts. Check the CRC */
 		crcnode.magic = node->magic;
@@ -761,7 +767,13 @@
 			if ((err = jffs2_scan_dirty_space(c, jeb, 4)))
 				return err;
 			ofs += 4;
+
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
+			return -EIO;
+#else
 			continue;
+#endif
+
 		}
 
 		if (ofs + je32_to_cpu(node->totlen) > jeb->offset + c->sector_size) {
@@ -981,6 +993,13 @@
 				 struct jffs2_raw_inode *ri, uint32_t ofs, struct jffs2_summary *s)
 {
 	struct jffs2_inode_cache *ic;
+
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
+	struct jffs2_full_dnode fd;
+	unsigned char *buf;
+	int ret;
+#endif
+
 	uint32_t crc, ino = je32_to_cpu(ri->ino);
 
 	jffs2_dbg(1, "%s(): Node at 0x%08x\n", __func__, ofs);
@@ -1015,7 +1034,23 @@
 	}
 
 	/* Wheee. It worked */
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
+
+	fd.raw = jffs2_link_node_ref(c, jeb, ofs | REF_UNCHECKED, PAD(je32_to_cpu(ri->totlen)), ic);
+
+	buf = kmalloc(ri->dsize.v32, GFP_KERNEL);
+	if (buf == NULL)
+		return -ENOMEM;
+
+	ret = jffs2_read_dnode(c, NULL, &fd, buf, 0, ri->dsize.v32);
+
+	kfree(buf);
+	if (ret < 0)
+		return -EIO;
+
+#else
 	jffs2_link_node_ref(c, jeb, ofs | REF_UNCHECKED, PAD(je32_to_cpu(ri->totlen)), ic);
+#endif
 
 	jffs2_dbg(1, "Node is ino #%u, version %d. Range 0x%x-0x%x\n",
 		  je32_to_cpu(ri->ino), je32_to_cpu(ri->version),
@@ -1052,7 +1087,13 @@
 		/* We believe totlen because the CRC on the node _header_ was OK, just the node itself failed. */
 		if ((err = jffs2_scan_dirty_space(c, jeb, PAD(je32_to_cpu(rd->totlen)))))
 			return err;
+
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
+		return -EIO;
+#else
 		return 0;
+#endif
+
 	}
 
 	pseudo_random += je32_to_cpu(rd->version);
@@ -1081,7 +1122,13 @@
 		/* We believe totlen because the CRC on the node _header_ was OK, just the name failed. */
 		if ((err = jffs2_scan_dirty_space(c, jeb, PAD(je32_to_cpu(rd->totlen)))))
 			return err;
+
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
+                return -EIO;
+#else
 		return 0;
+#endif
+
 	}
 	ic = jffs2_scan_make_ino_cache(c, je32_to_cpu(rd->pino));
 	if (!ic) {
