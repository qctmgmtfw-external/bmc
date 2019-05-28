--- linux.org/arch/arm/mach-astevb/Makefile.boot	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/Makefile.boot	2014-01-31 15:42:23.587921284 -0500
@@ -0,0 +1,4 @@
+   zreladdr-y	:= 0x80008000
+params_phys-y	:= 0x80000100
+initrd_phys-y	:= 0x81000000
+
