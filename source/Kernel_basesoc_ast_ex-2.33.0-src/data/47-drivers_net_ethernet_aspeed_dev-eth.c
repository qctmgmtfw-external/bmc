--- linux.org/drivers/net/ethernet/aspeed/dev-eth.c	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/drivers/net/ethernet/aspeed/dev-eth.c	2014-02-20 17:50:43.262319877 -0500
@@ -0,0 +1,114 @@
+/********************************************************************************
+* File Name     : linux/arch/arm/plat-aspeed/dev-eth.c
+* Author        : Ryan Chen
+* Description   : Aspeed Ethernet Device
+*
+* Copyright (C) ASPEED Technology Inc.
+* This program is free software; you can redistribute it and/or modify
+* it under the terms of the GNU General Public License as published by the Free Software Foundation;
+* either version 2 of the License, or (at your option) any later version.
+* 
+* This program is distributed in the hope that it will be useful,  but WITHOUT ANY WARRANTY;
+* without even the implied warranty of MERCHANTABILITY or
+* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
+* 
+* You should have received a copy of the GNU General Public License
+* along with this program; if not, write to the Free Software
+* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
+
+*   History      :
+*    1. 2012/08/24 Ryan Chen initial
+*
+********************************************************************************/
+#include <linux/kernel.h>
+#include <linux/string.h>
+#include <linux/platform_device.h>
+#include <linux/etherdevice.h>
+
+#include <mach/irqs.h>
+#include <mach/platform.h>
+#include "ast_gmac.h"
+
+extern unsigned long enetaddr[][6];
+
+static struct ftgmac100_eth_data ast_eth0_data = 
+{
+	.dev_addr = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},		
+};
+
+static u64 ast_eth_dmamask = 0xffffffffUL;
+static struct resource ast_mac0_resources[] = 
+{
+	[0] = {
+		.start = AST_MAC1_BASE,
+		.end = AST_MAC1_BASE + SZ_128K - 1,
+		.flags = IORESOURCE_MEM,
+	},
+	[1] = {
+		.start = IRQ_MAC1,
+		.end = IRQ_MAC1,
+		.flags = IORESOURCE_IRQ,
+	},
+};
+
+static struct platform_device ast_eth0_device = 
+{
+	.name		= "astgmac",
+	.id		= 0,
+	.dev		= {
+				.dma_mask		= &ast_eth_dmamask,
+				.coherent_dma_mask	= 0xffffffff,
+				.platform_data = &ast_eth0_data,
+	},
+	.resource	= ast_mac0_resources,
+	.num_resources = ARRAY_SIZE(ast_mac0_resources),
+};
+
+#if (CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT > 1)	
+static struct ftgmac100_eth_data ast_eth1_data = 
+{
+	.dev_addr = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},		
+};
+
+static struct resource ast_mac1_resources[] = 
+{
+	[0] = {
+		.start = AST_MAC2_BASE,
+		.end = AST_MAC2_BASE + SZ_128K - 1,
+		.flags = IORESOURCE_MEM,
+	},
+	[1] = {
+		.start = IRQ_MAC2,
+		.end = IRQ_MAC2,
+		.flags = IORESOURCE_IRQ,
+	},
+};
+
+static struct platform_device ast_eth1_device = 
+{
+	.name		= "astgmac",
+	.id		= 1,
+	.dev		= {
+				.dma_mask		= &ast_eth_dmamask,
+				.coherent_dma_mask	= 0xffffffff,
+				.platform_data = &ast_eth1_data,
+	},
+	.resource	= ast_mac1_resources,
+	.num_resources = ARRAY_SIZE(ast_mac1_resources),
+};
+#endif
+
+
+void __init ast_add_device_gmac(void)
+{
+
+	memcpy(ast_eth0_data.dev_addr, enetaddr[0], ETH_ALEN);
+	platform_device_register(&ast_eth0_device);
+
+
+#if (CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT > 1)	
+	memcpy(ast_eth1_data.dev_addr, enetaddr[1], ETH_ALEN);
+	platform_device_register(&ast_eth1_device);
+#endif
+}
+
