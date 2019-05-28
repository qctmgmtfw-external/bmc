--- uboot/board/ast2500evb/config.mk	1970-01-01 08:00:00.000000000 +0800
+++ uboot.new/board/ast2500evb/config.mk	2014-08-14 12:04:55.389476914 +0800
@@ -0,0 +1,22 @@
+#
+# Copyright (C) 2013 American Megatrends Inc
+#
+# This program is free software; you can redistribute it and/or modify
+# it under the terms of the GNU General Public License as published by
+# the Free Software Foundation; either version 2 of the License, or
+# (at your option) any later version.
+#
+# This program is distributed in the hope that it will be useful,
+# but WITHOUT ANY WARRANTY; without even the implied warranty of
+# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+# GNU General Public License for more details.
+#
+# You should have received a copy of the GNU General Public License
+# along with this program; if not, write to the Free Software
+# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
+#
+
+
+
+#CONFIG_SYS_TEXT_BASE  = 0x80100000			# Debug Mode - Load using BDI to this address
+CONFIG_SYS_TEXT_BASE  = 0x0				# U-boot location in flash
