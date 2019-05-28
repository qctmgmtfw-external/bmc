--- u-boot_orig/arch/arm/cpu/astcommon/ast_p2x.h	1969-12-31 19:00:00.000000000 -0500
+++ u-boot_mod/arch/arm/cpu/astcommon/ast_p2x.h	2015-12-23 20:00:16.780568782 -0500
@@ -0,0 +1,31 @@
+/********************************************************************************
+* File Name     : arch/arm/mach-aspeed/include/plat/ast-scu.h
+* Author        : Ryan Chen
+* Description   : AST SCU Service Header
+*
+* Copyright (C) 2012-2020  ASPEED Technology Inc.
+* This program is free software; you can redistribute it and/or modify
+* it under the terms of the GNU General Public License as published by the Free Software Foundation;
+* either version 2 of the License, or (at your option) any later version.
+* This program is distributed in the hope that it will be useful,  but WITHOUT ANY WARRANTY;
+* without even the implied warranty of MERCHANTABILITY or
+* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
+* You should have received a copy of the GNU General Public License
+* along with this program; if not, write to the Free Software
+* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
+
+*   History      :
+*      1. 2012/08/03 Ryan Chen create this file
+*
+********************************************************************************/
+
+#ifndef __AST_P2X_H_INCLUDED
+#define __AST_P2X_H_INCLUDED
+
+extern void ast_pcie_cfg_read(u8 type, u32 bdf_offset, u32 *value);
+//extern void ast_pcie_cfg_write(u8 type, u32 bdf_offset, u32 data);
+extern void ast_pcie_cfg_write(u8 type, u8 byte_en, u32 bdf_offset, u32 data);
+extern void ast_mctp_addr_map(u32 mask, u32 addr);
+extern void ast_p2x_addr_map(u32 mask, u32 addr);
+
+#endif
