--- .pristine/Kernel_soc_ast2300-1.19.0-src/data/04-drivers_net_arm_ast_ether.c	Thu Mar 13 18:46:50 2014
+++ source/Kernel_soc_ast2300-1.19.0-src/data/04-drivers_net_arm_ast_ether.c	Fri Mar 14 11:06:58 2014
@@ -1,6 +1,6 @@
---- linux.pristine/drivers/net/arm/ast_ether.c	1970-01-01 08:00:00.000000000 +0800
-+++ linux.new/drivers/net/arm/ast_ether.c	2012-01-12 16:17:34.390704000 +0800
-@@ -0,0 +1,1269 @@
+--- linux-2.6.28.10/drivers/net/arm/ast_ether.c	1970-01-01 08:00:00.000000000 +0800
++++ linux/drivers/net/arm/ast_ether.c	2014-03-14 11:00:52.963353597 +0800
+@@ -0,0 +1,1296 @@
 +/*
 + * drivers/net/ast_ether.c - AST2050/AST2100/AST2150/AST2200 ethernet MAC driver
 + */
@@ -440,6 +440,20 @@
 +	priv->tx_desc[TX_DES_NUM - 1].status |= TX_DESC_EDOTR; /* last descriptor */
 +}
 +
++static void issueNPTXPDTimerFn(unsigned long data)
++{
++	struct net_device *dev;
++	struct ast_ether_priv *priv;
++
++	dev = (struct net_device *) data;
++	unsigned int ioaddr = dev->base_addr;
++	priv = netdev_priv(dev);	
++	iowrite32(0xffffffff, ioaddr + AST_ETHER_TXPD); 
++
++	priv->issueNPTXPDTimer.expires = jiffies + ((1 * HZ)/10); //100ms
++	add_timer(&priv->issueNPTXPDTimer);	
++}
++
 +static void ast_ether_up(struct net_device *dev)
 +{
 +	struct ast_ether_priv *priv = netdev_priv(dev);
@@ -540,6 +554,15 @@
 +	ast_ether_bc[dev->dev_id].timer.function = ast_ether_bc_checker;
 +	ast_ether_bc[dev->dev_id].timer.expires = jiffies + (AST_BC_CHECK_INTERVAL * HZ);
 +	add_timer(&ast_ether_bc[dev->dev_id].timer);
++
++	if(priv->issueNPTXPDTimer.function != NULL)
++			del_timer_sync(&priv->issueNPTXPDTimer);
++	init_timer(&priv->issueNPTXPDTimer);
++	priv->issueNPTXPDTimer.data = (unsigned long) dev;
++	priv->issueNPTXPDTimer.function = issueNPTXPDTimerFn;
++	priv->issueNPTXPDTimer.expires = jiffies + ((1 * HZ)/10); //100ms
++	add_timer(&priv->issueNPTXPDTimer);
++
 +	iowrite32(reg_value, ioaddr + AST_ETHER_IER);
 +
 +	/* enable trans/recv, ... */
@@ -893,6 +916,8 @@
 +	if( ast_ether_bc[dev->dev_id].timer.function != NULL )
 +		del_timer_sync(&ast_ether_bc[dev->dev_id].timer);
 +
++	if (priv->issueNPTXPDTimer.function != NULL)
++	    	del_timer_sync(&priv->issueNPTXPDTimer);
 +
 +	free_irq(dev->irq, dev);
 +	spin_lock_irqsave(&priv->lock, flags);
@@ -1250,6 +1275,8 @@
 +		priv = netdev_priv(ast_ether_netdev[id]);
 +		if(priv->timer.function != NULL)
 +			del_timer_sync(&priv->timer);
++		if (priv->issueNPTXPDTimer.function != NULL)
++		    	del_timer_sync(&priv->issueNPTXPDTimer);
 +		if (priv->rx_desc)
 +			dma_free_coherent(NULL, sizeof(struct ast_eth_desc) * RX_DES_NUM, priv->rx_desc, priv->rx_desc_dma);
 +		if (priv->tx_desc)
