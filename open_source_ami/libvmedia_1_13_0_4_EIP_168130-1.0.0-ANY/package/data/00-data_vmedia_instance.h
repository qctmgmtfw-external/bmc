--- .pristine/libvmedia-1.13.0-src/data/vmedia_instance.h	Tue Jun  3 12:11:51 2014
+++ source/libvmedia-1.13.0-src/data/vmedia_instance.h	Wed Jun  4 15:59:58 2014
@@ -27,6 +27,7 @@
 int set_hdisk_redir(char *, char*, uint8 *);
 int	reset_hdisk_redir(char *, unsigned long *racsession_id, unsigned char *is_other_vmedia_in_use);
 int set_vmedia_racsession_id(unsigned long racsession_id);
+int Checkanyinstancerunning(int gWasInstanceUsed[],int NumInstances);
 
 #endif /* __VMEDIA_INSTANCE_H__ */
 
