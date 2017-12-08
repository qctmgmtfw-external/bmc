--- .pristine/Kernel_soc_ast2300-1.19.0-src/data/05-drivers_net_arm_ast_ether.h Wed Aug  1 15:05:16 2012
+++ source/Kernel_soc_ast2300-1.19.0-src/data/05-drivers_net_arm_ast_ether.h Thu Aug 30 16:04:00 2012
@@ -1,17 +1,25 @@
---- linux/drivers/net/arm/ast_ether.h	1970-01-01 08:00:00.000000000 +0800
-+++ linux.new/drivers/net/arm/ast_ether.h	2011-08-17 14:47:59.071042000 +0800
-@@ -0,0 +1,289 @@
+--- linux.pristine/drivers/net/arm/ast_ether.h	1970-01-01 08:00:00.000000000 +0800
++++ linux.new/drivers/net/arm/ast_ether.h	2012-01-12 16:18:54.925308000 +0800
+@@ -0,0 +1,308 @@
 +#ifndef _AST_ETHER_H_
 +#define _AST_ETHER_H_
 +
++#ifdef CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT
++#define AST_ETHER_MAX_NUM       CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT
++#else
 +#define AST_ETHER_MAX_NUM		2
++#endif
 +#define AST_ETHER_USE_NUM		AST_ETHER_MAX_NUM
 +
 +#define AST_ETHER_REG_BASE1		0x1E660000
 +#define AST_ETHER_REG_BASE2		0x1E680000
++#define AST_ETHER_REG_BASE3     0x1E670000
++#define AST_ETHER_REG_BASE4     0x1E690000
 +
 +#define AST_ETHER_IRQ1			2
 +#define AST_ETHER_IRQ2			3
++#define AST_ETHER_IRQ3         23
++#define AST_ETHER_IRQ4         24
 +
 +#define AST_ETHER_IO_REG_SIZE	SZ_64K
 +
@@ -133,7 +141,7 @@
 +#define BROADCOM_BCM5221_OUI	0x001018
 +#define BROADCOM_BCM5241_OUI	0x0050EF
 +#define REALTEK_RTL82X1X_OUI	0x000732
-+#define MICREL_KSZ8041_OUI		0x000885
++#define MICREL_KSZ80X1_OUI		0x000885
 +
 +/* addresses of PHY common registers */
 +#define PHY_COM_REG_ADDR_CONTROL 0
@@ -175,8 +183,10 @@
 +#define REALTEK_RTL8211B_REG_ADDR_PHY_STS_SPEED_SHIFT		14
 +#define REALTEK_RTL8211B_REG_ADDR_PHY_STS_DEPLEX			0x2000 /* bit 13 */
 +
++#define MICREL_KSZ8031_REG_STATUS		0x1E
 +#define MICREL_KSZ8041_REG_STATUS		0x1F
 +
++#define MICREL_KSZ8031_REG_STATUS_MASK			0x0007 /* bits[2:0] */
 +#define MICREL_KSZ8041_REG_STATUS_MASK			0x001C /* bits[4:2] */
 +#define MICREL_KSZ8041_REG_STATUS_SHIFT			2
 +#define MICREL_KSZ8041_REG_STATUS_FULL_100		0x0006 /* 110 */
@@ -250,6 +260,7 @@
 +};
 +
 +#define AST_PHY_STATUS_CHECK_INTERVAL		3 /* for PHY without interrupt */
++#define AST_BC_CHECK_INTERVAL			2 	
 +
 +/* store this information for the driver.. */
 +struct ast_ether_priv {
@@ -286,6 +297,14 @@
 +	struct timer_list timer; /* status check timer for PHY without interrupt */
 +};
 +
++struct ast_bc{
++	unsigned long bc_count;
++	bool bc_ctr;
++	bool trans_busy_1;
++	bool trans_busy_2;
++	struct timer_list timer;
++};
++
 +extern void ast_ether_get_mac_addr(unsigned char *mac_addr);
 +extern int ast_ether_eeprom_read(unsigned int offset, unsigned char *buffer, unsigned int cnt);
 + 
