--- .pristine/Kernel_soc_ast2300-1.19.0-src/data/05-drivers_net_arm_ast_ether.h	Thu Mar 13 18:46:50 2014
+++ source/Kernel_soc_ast2300-1.19.0-src/data/05-drivers_net_arm_ast_ether.h	Fri Mar 14 11:07:10 2014
@@ -1,6 +1,6 @@
---- linux.pristine/drivers/net/arm/ast_ether.h	1970-01-01 08:00:00.000000000 +0800
-+++ linux.new/drivers/net/arm/ast_ether.h	2012-01-12 16:18:54.925308000 +0800
-@@ -0,0 +1,308 @@
+--- linux-2.6.28.10/drivers/net/arm/ast_ether.h	1970-01-01 08:00:00.000000000 +0800
++++ linux/drivers/net/arm/ast_ether.h	2014-03-14 11:00:52.963353597 +0800
+@@ -0,0 +1,309 @@
 +#ifndef _AST_ETHER_H_
 +#define _AST_ETHER_H_
 +
@@ -295,6 +295,7 @@
 +    u32 supports_gmii : 1; /* are GMII registers supported? */
 +    
 +	struct timer_list timer; /* status check timer for PHY without interrupt */
++	struct timer_list issueNPTXPDTimer;	/* A Kernel Timer to periodically issue MAC18 */
 +};
 +
 +struct ast_bc{
