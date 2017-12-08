diff -Naur linux-2.6.28.10/fs/jffs2/scan.c linux-2.6.28.10-ami/fs/jffs2/scan.c
--- linux-2.6.28.10/fs/jffs2/scan.c	2009-05-03 02:54:43.000000000 +0800
+++ linux-2.6.28.10-ami/fs/jffs2/scan.c	2010-08-26 14:26:45.116639312 +0800
@@ -721,7 +721,13 @@
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
@@ -739,7 +745,13 @@
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
@@ -949,6 +961,13 @@
 				 struct jffs2_raw_inode *ri, uint32_t ofs, struct jffs2_summary *s)
 {
 	struct jffs2_inode_cache *ic;
+	
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF			
+    struct jffs2_full_dnode fd;
+	unsigned char *buf;
+	int ret;
+#endif		
+
 	uint32_t crc, ino = je32_to_cpu(ri->ino);
 
 	D1(printk(KERN_DEBUG "jffs2_scan_inode_node(): Node at 0x%08x\n", ofs));
@@ -984,8 +1003,24 @@
 	}
 
 	/* Wheee. It worked */
-	jffs2_link_node_ref(c, jeb, ofs | REF_UNCHECKED, PAD(je32_to_cpu(ri->totlen)), ic);
+#if defined CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF			
+	
+	fd.raw = jffs2_link_node_ref(c, jeb, ofs | REF_UNCHECKED, PAD(je32_to_cpu(ri->totlen)), ic);
 
+    buf = kmalloc(ri->dsize.v32, GFP_KERNEL);
+    if (buf == NULL)
+        return -ENOMEM;
+        
+    ret = jffs2_read_dnode(c, NULL, &fd, buf, 0, ri->dsize.v32);
+    
+    kfree(buf);
+    if (ret < 0)
+        return -EIO;
+		
+#else
+	jffs2_link_node_ref(c, jeb, ofs | REF_UNCHECKED, PAD(je32_to_cpu(ri->totlen)), ic);
+#endif
+		
 	D1(printk(KERN_DEBUG "Node is ino #%u, version %d. Range 0x%x-0x%x\n",
 		  je32_to_cpu(ri->ino), je32_to_cpu(ri->version),
 		  je32_to_cpu(ri->offset),
@@ -1021,7 +1056,13 @@
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
@@ -1049,7 +1090,13 @@
 		/* We believe totlen because the CRC on the node _header_ was OK, just the name failed. */
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
 	ic = jffs2_scan_make_ino_cache(c, je32_to_cpu(rd->pino));
 	if (!ic) {
@@ -1059,7 +1106,7 @@
 
 	fd->raw = jffs2_link_node_ref(c, jeb, ofs | dirent_node_state(rd),
 				      PAD(je32_to_cpu(rd->totlen)), ic);
-
+	          
 	fd->next = NULL;
 	fd->version = je32_to_cpu(rd->version);
 	fd->ino = je32_to_cpu(rd->ino);
