--- uboot.org/arch/arm/cpu/astcommon/eth_scu.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/eth_scu.c	2014-07-18 13:36:58.143403810 -0400
@@ -0,0 +1,118 @@
+#include <common.h>
+#include <asm/processor.h>
+#include <asm/io.h>
+#include "regs-scu.h"
+
+#define AST_SCU_BASE 0x1e6e2000
+
+static u32 ast_scu_base = AST_SCU_BASE;
+
+static inline u32 
+ast_scu_read(u32 reg)
+{
+	u32 val;
+	val = readl(ast_scu_base + reg);
+	return val;
+}
+
+static inline void
+ast_scu_write(u32 val, u32 reg) 
+{
+	//unlock 
+	writel(SCU_PROTECT_UNLOCK, ast_scu_base);
+	writel(val, ast_scu_base + reg);
+	//lock
+	writel(0xaa,ast_scu_base);	
+}
+
+void
+ast_scu_init_eth(u8 num)
+{
+	//AST2300 max clk to 125Mhz, AST2400 max clk to 198Mhz
+	if(ast_scu_read(AST_SCU_HW_STRAP1) && (SCU_HW_STRAP_MAC1_RGMII | SCU_HW_STRAP_MAC0_RGMII))	//RGMII --> H-PLL/6
+		ast_scu_write((ast_scu_read(AST_SCU_CLK_SEL) & ~SCU_CLK_MAC_MASK) | SCU_CLK_MAC_DIV(2), AST_SCU_CLK_SEL);
+	else	//RMII --> H-PLL/10
+		ast_scu_write((ast_scu_read(AST_SCU_CLK_SEL) & ~SCU_CLK_MAC_MASK) | SCU_CLK_MAC_DIV(4), AST_SCU_CLK_SEL);
+
+	//Set MAC delay Timing
+	ast_scu_write(0x64F082, AST_SCU_MAC_CLK);	
+	//Set RGMII clock duty
+	ast_scu_write(0x6A6A00, AST_SCU_RGMII_CLK_DUTY_SEL);	
+
+	switch(num) {
+		case 0:
+			ast_scu_write(ast_scu_read(AST_SCU_RESET) | SCU_RESET_MAC0, 
+							AST_SCU_RESET);		
+			udelay(100);
+			ast_scu_write(ast_scu_read(AST_SCU_CLK_STOP) & ~SCU_MAC0CLK_STOP_EN, 
+							AST_SCU_CLK_STOP);		
+			udelay(1000);
+			ast_scu_write(ast_scu_read(AST_SCU_RESET) & ~SCU_RESET_MAC0, 
+							AST_SCU_RESET);		
+			
+			break;
+		case 1:
+			ast_scu_write(ast_scu_read(AST_SCU_RESET) | SCU_RESET_MAC1, 
+							AST_SCU_RESET);			
+			udelay(100);
+			ast_scu_write(ast_scu_read(AST_SCU_CLK_STOP) & ~SCU_MAC1CLK_STOP_EN, 
+							AST_SCU_CLK_STOP);		
+			udelay(1000);
+			ast_scu_write(ast_scu_read(AST_SCU_RESET) & ~SCU_RESET_MAC1, 
+							AST_SCU_RESET);			
+			break;
+			
+	}		
+}
+
+
+void
+ast_scu_multi_func_eth(u8 num)
+{
+	switch(num) {
+		case 0:
+			if(ast_scu_read(AST_SCU_HW_STRAP1) && SCU_HW_STRAP_MAC0_RGMII) {
+				ast_scu_write(ast_scu_read(AST_SCU_FUN_PIN_CTRL1) | 
+							SCU_FUN_PIN_MAC0_PHY_LINK, 
+					AST_SCU_FUN_PIN_CTRL1); 
+			} else {
+				ast_scu_write(ast_scu_read(AST_SCU_FUN_PIN_CTRL1) &
+							~SCU_FUN_PIN_MAC0_PHY_LINK, 
+					AST_SCU_FUN_PIN_CTRL1); 
+			}
+
+#ifdef AST_SOC_G5
+			ast_scu_write(ast_scu_read(AST_SCU_FUN_PIN_CTRL1) | 
+						SCU_FUN_PIN_MAC0_PHY_LINK, 
+				AST_SCU_FUN_PIN_CTRL1); 
+
+#endif
+			ast_scu_write(ast_scu_read(AST_SCU_FUN_PIN_CTRL3) | 
+						SCU_FUN_PIN_MAC0_MDIO |
+						SCU_FUN_PIN_MAC0_MDC, 
+				AST_SCU_FUN_PIN_CTRL3); 
+			
+			break;
+		case 1:
+			if(ast_scu_read(AST_SCU_HW_STRAP1) && SCU_HW_STRAP_MAC1_RGMII) {
+				ast_scu_write(ast_scu_read(AST_SCU_FUN_PIN_CTRL1) | 
+							SCU_FUN_PIN_MAC1_PHY_LINK, 
+					AST_SCU_FUN_PIN_CTRL1); 
+			} else {
+				ast_scu_write(ast_scu_read(AST_SCU_FUN_PIN_CTRL1) & 
+						~SCU_FUN_PIN_MAC1_PHY_LINK, 
+					AST_SCU_FUN_PIN_CTRL1); 
+			}
+		
+			ast_scu_write(ast_scu_read(AST_SCU_FUN_PIN_CTRL1) | 
+						SCU_FUN_PIN_MAC1_PHY_LINK, 
+				AST_SCU_FUN_PIN_CTRL1); 
+			
+			ast_scu_write(ast_scu_read(AST_SCU_FUN_PIN_CTRL5) | 
+						SCU_FUC_PIN_MAC1_MDIO,
+				AST_SCU_FUN_PIN_CTRL5); 
+
+			break;
+	}
+}
+
