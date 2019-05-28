--- u-boot-2013.07/disassemble.sh	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/disassemble.sh	2013-12-05 12:17:55.199503948 -0500
@@ -0,0 +1,2 @@
+#!/bin/sh
+arm-none-linux-gnueabi-objdump -h -S -l -m armv5te --show-raw-insn -EL u-boot > u-boot.dis
