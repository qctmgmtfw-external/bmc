--- linux-2.6.28.10-pristine/arch/arm/mach-ast2300evb/Makefile.boot	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/mach-ast2300evb/Makefile.boot	2011-05-25 14:59:15.988665944 +0800
@@ -0,0 +1,4 @@
+   zreladdr-y	:= 0x40008000
+params_phys-y	:= 0x40000100
+initrd_phys-y	:= 0x41000000
+
