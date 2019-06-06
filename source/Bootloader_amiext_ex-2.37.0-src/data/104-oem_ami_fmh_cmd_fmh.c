--- uboot/oem/ami/fmh/cmd_fmh.c	2017-03-14 13:21:43.544172274 +0800
+++ uboot/oem/ami/fmh/cmd_fmh.c.new	2017-03-14 13:26:35.828170404 +0800
@@ -795,6 +795,7 @@
 	int rootisinitrd = 0; /*indicates root is actually an initrd*/
     char *side = NULL;
     u8 golden_side;
+	unsigned long  reg32 = 0; // Quanta
 
 #ifdef CONFIG_YAFU_SUPPORT
 	int  boot_fwupd = 0;
@@ -1236,6 +1237,19 @@
 		/* If module is a U-Boot Format Linux Image */
 		if (ExecuteType == MODULE_PIMAGE)
 		{
+			// Quanta, disable MAC after leaving boot code
+			// read SCU04, modify bit11,12 to 1
+			reg32 = *((volatile u_long *) 0x1e6e2004);
+			reg32 = reg32 | (1<<11);
+			reg32 = reg32 | (1<<12);
+			// unlock SCU
+			*((volatile u_long *) 0x1e6e2000) = 0x1688a8a8;
+			// write SCU04
+			*((volatile u_long *) 0x1e6e2004) = reg32;
+			// lock SCU
+			*((volatile u_long *) 0x1e6e2000) = 0x0;
+			printf("SCU04 = 0x%x\n", (unsigned int)(*((volatile u_long *) 0x1e6e2004)));
+
 			printf("Booting from MODULE_PIMAGE ...\n");
 			printf("Bootargs = [%s]\n",bootargs);
 			setenv("bootargs",bootargs);
