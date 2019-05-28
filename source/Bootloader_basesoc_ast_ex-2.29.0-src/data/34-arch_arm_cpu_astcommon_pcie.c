--- u-boot_orig/arch/arm/cpu/astcommon/pcie.c	1969-12-31 19:00:00.000000000 -0500
+++ u-boot_mod/arch/arm/cpu/astcommon/pcie.c	2015-12-23 20:00:41.932568573 -0500
@@ -0,0 +1,234 @@
+/*
+ * (C) Copyright 2001
+ * Stefan Roese, esd gmbh germany, stefan.roese@esd-electronics.com
+ *
+ * See file CREDITS for list of people who contributed to this
+ * project.
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ */
+
+#include <common.h>
+#include <pci.h>
+#include <asm/processor.h>
+#include <asm/io.h>
+#include "regs-pcie.h"
+#include "ast_p2x.h"
+#include "ast-ahbc.h"
+#include "ast_hw.h"
+#include "ast_scu.h"
+
+//#define AST_PCIE_DEBUG 1 
+
+#ifdef AST_PCIE_DEBUG
+#define PCIEDBUG(fmt, args...) printf("%s() " fmt, __FUNCTION__, ## args)
+#else
+#define PCIEDBUG(fmt, args...)
+#endif
+
+DECLARE_GLOBAL_DATA_PTR;
+
+static u32 ast_pcie_base = AST_PCIE_BASE;
+
+static inline u32 
+ast_pcie_read(u32 reg)
+{
+	u32 val;
+
+	val = readl(ast_pcie_base + reg);
+	PCIEDBUG("ast_pcie_read : reg = 0x%08x, val = 0x%08x\n", reg, val);
+	return val;
+}
+
+static inline void
+ast_pcie_write(u32 val, u32 reg) 
+{
+	PCIEDBUG("ast_pcie_write : reg = 0x%08x, val = 0x%08x\n", reg, val);
+
+	writel(val, ast_pcie_base + reg);
+}
+
+/*-----------------------------------------------------------------------------+
+|  Subroutine:  ast_read_config_dword
+|  Description: Read a PCI configuration register
+|  Inputs:
+|               hose            PCI Controller
+|               dev             PCI Bus+Device+Function number
+|               offset          Configuration register number
+|               value           Address of the configuration register value
+|  Return value:
+|               0               Successful
++-----------------------------------------------------------------------------*/
+int ast_read_config_dword(struct pci_controller *hose,
+			      pci_dev_t bdf, int offset, u32* value)
+{
+ 	PCIEDBUG("Bus %d, Dev %d, Fun %d, offset %x\n", PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), offset);
+
+	if(!(ast_pcie_read(AST_PCIE_LINK) & PCIE_LINK_STS)) {
+		*value = 0xffffffff;
+		return 0;		
+	}
+
+	if((PCI_DEV(bdf) > 0) && (PCI_BUS(bdf) < 2)) {
+		*value = 0xffffffff;
+		return 0;		
+	} else if(PCI_BUS(bdf) == 0) {
+ 		ast_pcie_cfg_read(0, 
+						(PCI_BUS(bdf) << 24) |
+						(PCI_DEV(bdf) << 19) |
+						(PCI_FUNC(bdf) << 16) | 
+						offset, value);
+	} else {
+ 		ast_pcie_cfg_read(1, 
+						(PCI_BUS(bdf) << 24) |
+						(PCI_DEV(bdf) << 19) |
+						(PCI_FUNC(bdf) << 16) | 
+						offset, value);	
+	}
+
+	return 0;
+}
+
+/*-----------------------------------------------------------------------------+
+|  Subroutine:  ast_write_config_dword
+|  Description: Write a PCI configuration register.
+|  Inputs:
+|               hose            PCI Controller
+|               dev             PCI Bus+Device+Function number
+|               offset          Configuration register number
+|               Value           Configuration register value
+|  Return value:
+|               0               Successful
+| Updated for pass2 errata #6. Need to disable interrupts and clear the
+| PCICFGADR reg after writing the PCICFGDATA reg.
++-----------------------------------------------------------------------------*/
+int ast_write_config_dword(struct pci_controller *hose,
+			       pci_dev_t bdf, int offset, u32 value)
+{
+ 	PCIEDBUG("Bus %d, Dev %d, Fun %d, offset %x: w(%x)\n", PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf), offset, value);	
+	if(PCI_BUS(bdf) == 0) {
+		ast_pcie_cfg_write(0, 0xf,
+					(PCI_BUS(bdf) << 24) |
+					(PCI_DEV(bdf) << 19) |
+					(PCI_FUNC(bdf) << 16) | 
+					(offset & ~3), value);		
+	} else {
+		ast_pcie_cfg_write(1, 0xf,
+					(PCI_BUS(bdf)<< 24) |
+					(PCI_DEV(bdf) << 19) |
+					(PCI_FUNC(bdf) << 16) | 
+					(offset & ~3), value);
+	}
+	
+	return 0;
+
+}
+
+/*-----------------------------------------------------------------------
+ */
+
+void init_pcie_rc_bridge(void)
+{
+	ast_scu_init_pcie();
+	ast_ahbc_peie_mapping(1);
+
+	//plda enable 
+	ast_pcie_write(PCIE_UNLOCK, AST_PCIE_LOCK);
+	ast_pcie_write(PCIE_CFG_CLASS_CODE(0x60400) | PCIE_CFG_REV_ID(4), AST_PCIE_CFG2);
+	ast_pcie_write(ROOT_COMPLEX_ID(0x3), AST_PCIE_GLOBAL);
+
+	//px2 decode addr
+	ast_p2x_addr_map(0xF0000000, AST_PCIE_WIN_BASE);
+
+	//VGA Init for memory cycle
+	writel(0xacedbd1f, 0x1e78c000);
+	writel(0x00010004, 0x1e78c004);
+	writel(0x00000002, 0x1e78c008);	//vgamm_enable
+	writel(0x00010010, 0x1e78c004);
+	writel(0x80000000, 0x1e78c008);	//vgamm_bar
+	
+	writel(readl(0x1e6e0008) & ~(1<<16), 0x1e6e0008);		
+
+}
+
+#ifndef CONFIG_PCI_PNP
+static struct pci_config_table pcie_ast_config_table[] = {
+	{ PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, 0x0f, PCI_ANY_ID,
+	  pci_cfgfunc_config_device, { AST_PCIE_WIN_BASE,
+				       PCI_COMMAND_PARITY | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER }},
+	{ }
+};
+#endif /* CONFIG_PCI_PNP */
+
+struct pci_controller ast_pcie_host = {
+#ifndef CONFIG_PCI_PNP
+	config_table: pcie_ast_config_table,
+#endif
+};
+
+void pci_init_board(void)
+{
+	struct pci_controller *hose = &ast_pcie_host;
+
+	//init rc bridge
+	init_pcie_rc_bridge();
+
+	/*
+	 * Release PCI RST Output signal.
+	 * Power on to RST high must be at least 100 ms as per PCI spec.
+	 * On warm boots only 1 ms is required, but we play it safe.
+	 */
+
+	udelay(100000);
+	
+	printf("AST PCIE: ");
+	
+	if(ast_pcie_read(AST_PCIE_LINK) & PCIE_LINK_STS)
+		printf("Link\n");
+	else {
+		printf("No link\n");
+		return;
+	}
+
+	/*
+	 * Configure and register the PCI hose
+	 */
+	hose->first_busno = 0;
+	hose->last_busno = 0xff;
+
+	/* System memory space, window 0 256 MB non-prefetchable */
+	pci_set_region(hose->regions + 0,
+		       AST_PCIE_WIN_BASE, AST_PCIE_WIN_BASE,
+		       AST_PCIE_WIN_SIZE,
+		       PCI_REGION_MEM);
+
+	hose->region_count = 1;
+
+	pci_set_ops(hose,
+			pci_hose_read_config_byte_via_dword,
+			pci_hose_read_config_word_via_dword,
+			ast_read_config_dword,
+			pci_hose_write_config_byte_via_dword,
+			pci_hose_write_config_word_via_dword,
+			ast_write_config_dword);
+
+	pci_register_hose(hose);
+	pciauto_config_init(hose);
+	pciauto_config_device(hose, 0);
+	hose->last_busno = pci_hose_scan(hose);
+
+}
+
