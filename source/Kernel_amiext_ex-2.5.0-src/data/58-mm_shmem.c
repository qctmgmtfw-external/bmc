--- linux-3.14.17/mm/shmem.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/mm/shmem.c	2014-08-21 12:24:35.149611440 -0400
@@ -3070,6 +3070,14 @@
 }
 EXPORT_SYMBOL_GPL(shmem_file_setup);
 
+void shmem_set_file(struct vm_area_struct *vma, struct file *file)
+{
+   if (vma->vm_file)
+       fput(vma->vm_file);
+   vma->vm_file = file;
+   vma->vm_ops = &shmem_vm_ops;
+}
+
 /**
  * shmem_zero_setup - setup a shared anonymous mapping
  * @vma: the vma to be mmapped is prepared by do_mmap_pgoff
