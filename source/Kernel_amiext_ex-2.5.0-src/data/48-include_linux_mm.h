--- linux-3.14.17/include/linux/mm.h	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/include/linux/mm.h	2014-08-21 12:28:24.761615751 -0400
@@ -1040,6 +1040,7 @@
 extern void show_free_areas(unsigned int flags);
 extern bool skip_free_areas_node(unsigned int flags, int nid);
 
+void shmem_set_file(struct vm_area_struct *vma, struct file *file);
 int shmem_zero_setup(struct vm_area_struct *);
 
 extern int can_do_mlock(void);
