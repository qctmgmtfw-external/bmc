--- .pristine/Kernel_soc_ast2300-1.19.0-src/data/04-drivers_net_arm_ast_ether.c Wed Aug  1 15:05:16 2012
+++ source/Kernel_soc_ast2300-1.19.0-src/data/04-drivers_net_arm_ast_ether.c Thu Aug 30 16:04:00 2012
@@ -1,6 +1,6 @@
---- linux/drivers/net/arm/ast_ether.c	1970-01-01 08:00:00.000000000 +0800
-+++ linux.new/drivers/net/arm/ast_ether.c	2011-08-17 17:46:42.039042000 +0800
-@@ -0,0 +1,1119 @@
+--- linux.pristine/drivers/net/arm/ast_ether.c	1970-01-01 08:00:00.000000000 +0800
++++ linux.new/drivers/net/arm/ast_ether.c	2012-01-12 16:17:34.390704000 +0800
+@@ -0,0 +1,1269 @@
 +/*
 + * drivers/net/ast_ether.c - AST2050/AST2100/AST2150/AST2200 ethernet MAC driver
 + */
@@ -34,12 +34,28 @@
 +
 +#include "ast_ether.h"
 +
++#if (AST_ETHER_MAX_NUM == 4)
++const unsigned long ast_ether_phys_base[AST_ETHER_MAX_NUM] = { AST_ETHER_REG_BASE1, AST_ETHER_REG_BASE2,
++                                                               AST_ETHER_REG_BASE3, AST_ETHER_REG_BASE4 };
++const unsigned long ast_ether_irq[AST_ETHER_MAX_NUM] = { AST_ETHER_IRQ1, AST_ETHER_IRQ2,
++                                                         AST_ETHER_IRQ3, AST_ETHER_IRQ4 };
++#elif (AST_ETHER_MAX_NUM == 3)
++const unsigned long ast_ether_phys_base[AST_ETHER_MAX_NUM] = { AST_ETHER_REG_BASE1, AST_ETHER_REG_BASE2, 
++                                                               AST_ETHER_REG_BASE3 };
++const unsigned long ast_ether_irq[AST_ETHER_MAX_NUM] = { AST_ETHER_IRQ1, AST_ETHER_IRQ2, 
++                                                         AST_ETHER_IRQ3 };
++#else
 +const unsigned long ast_ether_phys_base[AST_ETHER_MAX_NUM] = {AST_ETHER_REG_BASE1, AST_ETHER_REG_BASE2};
 +const unsigned long ast_ether_irq[AST_ETHER_MAX_NUM] = {AST_ETHER_IRQ1, AST_ETHER_IRQ2};
++#endif
 +
 +static struct net_device *ast_ether_netdev[AST_ETHER_MAX_NUM];
 +
 +extern unsigned long enetaddr[][6];
++static struct ast_bc ast_ether_bc[AST_ETHER_MAX_NUM];
++static void ast_ether_free_rx_buf(struct net_device *dev);
++int phy_ad = 0 ;
++
 +static void ast_ether_get_hardware_addr(struct net_device* dev)
 +{
 +	memcpy(dev->dev_addr, enetaddr[dev->dev_id], ETH_ALEN);
@@ -102,12 +118,22 @@
 +{
 +	struct ast_ether_priv *priv = netdev_priv(dev);
 +	u32 reg_val;
++	int i, ret1, ret2;
++
++	for(i = 0 ; i < 32 ; i++){
++		ret1 = ast_ether_phy_read_register(dev, i, PHY_COM_REG_ADDR_ID_HI);
++		ret2 = ast_ether_phy_read_register(dev, i, PHY_COM_REG_ADDR_ID_LO);
++		if( ret1 != 0xffff && ret2 != 0xffff ){
++			phy_ad = i;
++			break;
++		}
++	}
 +
 +	/* OUI[23:22] are not represented in PHY ID registers */
-+	reg_val = ast_ether_phy_read_register(dev, 0, PHY_COM_REG_ADDR_ID_HI);
++	reg_val = ast_ether_phy_read_register(dev, phy_ad, PHY_COM_REG_ADDR_ID_HI);
 +	priv->phy_oui = (reg_val & 0xFFFF) << 6; /* OUI[21:6] in ID_HI[15:0] */
 +
-+	reg_val = ast_ether_phy_read_register(dev, 0, PHY_COM_REG_ADDR_ID_LO);
++	reg_val = ast_ether_phy_read_register(dev, phy_ad, PHY_COM_REG_ADDR_ID_LO);
 +	priv->phy_oui |= (reg_val & 0xFFFF) >> 10; /* OUI[5:0] in ID_LO[15:10] */
 +
 +	priv->phy_model = (reg_val & 0x03F0) >> 4; /* MODEL[5:0] in ID_LO[9:4] */
@@ -116,7 +142,7 @@
 +	case MARVELL_88E1111_OUI:
 +	case BROADCOM_BCM5221_OUI:
 +	case BROADCOM_BCM5241_OUI:
-+	case MICREL_KSZ8041_OUI:
++	case MICREL_KSZ80X1_OUI:
 +	case REALTEK_RTL82X1X_OUI:
 +		break;
 +	default:
@@ -132,10 +158,10 @@
 +int ast_mii_check_gmii_support(struct net_device *dev)                                                                                                     
 +{        
 +    uint32_t reg = 0;
-+    reg = ast_ether_phy_read_register(dev, 0, MII_BMSR);
++    reg = ast_ether_phy_read_register(dev, phy_ad, MII_BMSR);
 +
 +    if (reg & BMSR_ESTATEN) {
-+        reg = ast_ether_phy_read_register(dev, 0, MII_ESTATUS);
++        reg = ast_ether_phy_read_register(dev, phy_ad, MII_ESTATUS);
 +        if (reg & (ESTATUS_1000_TFULL | ESTATUS_1000_THALF))
 +            return 1; 
 +    }
@@ -149,12 +175,12 @@
 +	u16 reg_val;
 +
 +	/* restart auto-negotiation */
-+	reg_val = ast_ether_phy_read_register(dev, 0, PHY_COM_REG_ADDR_CONTROL);
++	reg_val = ast_ether_phy_read_register(dev, phy_ad, PHY_COM_REG_ADDR_CONTROL);
 +	reg_val |= PHY_COM_REG_CONTROL_RE_AUTO;
-+	ast_ether_phy_write_register(dev, 0, PHY_COM_REG_ADDR_CONTROL, reg_val);
++	ast_ether_phy_write_register(dev, phy_ad, PHY_COM_REG_ADDR_CONTROL, reg_val);
 +
 +	/* Waiting for complete Auto-Negotiation */
-+	while ((ast_ether_phy_read_register(dev, 0, PHY_COM_REG_ADDR_STATUS) & PHY_COM_REG_STATUS_AUTO_OK) == 0)
++	while ((ast_ether_phy_read_register(dev, phy_ad, PHY_COM_REG_ADDR_STATUS) & PHY_COM_REG_STATUS_AUTO_OK) == 0)
 +		mdelay(2);
 +}
 +
@@ -165,7 +191,7 @@
 +	unsigned int reg_val, duplex, speed;
 +
 +	if (priv->phy_oui != 0) {
-+		reg_val = ast_ether_phy_read_register(dev, 0, PHY_COM_REG_ADDR_STATUS);
++		reg_val = ast_ether_phy_read_register(dev, phy_ad, PHY_COM_REG_ADDR_STATUS);
 +		if (reg_val & PHY_COM_REG_STATUS_LINK) {
 +            if (rtnl_trylock())
 +            {
@@ -187,25 +213,25 @@
 +			/* read speed and duplex from PHY */
 +			switch (priv->phy_oui) {
 +			case MARVELL_88E1111_OUI:
-+				reg_val = ast_ether_phy_read_register(dev, 0, MARVELL_88E1111_REG_ADDR_PHY_STATUS);
++				reg_val = ast_ether_phy_read_register(dev, phy_ad, MARVELL_88E1111_REG_ADDR_PHY_STATUS);
 +				duplex = (reg_val & MARVELL_88E1111_REG_PHY_STATUS_DUPLEX_MASK) >> 13;
 +				speed = (reg_val & MARVELL_88E1111_REG_PHY_STATUS_SPEED_MASK) >> 14;
 +				break;
 +			case BROADCOM_BCM5221_OUI:
 +			case BROADCOM_BCM5241_OUI:
-+				reg_val = ast_ether_phy_read_register(dev, 0, BROADCOM_BCM5221_REG_ADDR_AUX_CTRL_STAT);
++				reg_val = ast_ether_phy_read_register(dev, phy_ad, BROADCOM_BCM5221_REG_ADDR_AUX_CTRL_STAT);
 +				duplex = (reg_val & BROADCOM_BCM5221_REG_AUX_CTRL_STAT_DEPLEX) ? PHY_DUPLEX_FULL : PHY_DUPLEX_HALF;
 +				speed = (reg_val & BROADCOM_BCM5221_REG_AUX_CTRL_STAT_SPEED) ? PHY_SPEED_100M : PHY_SPEED_10M;
 +				break;
 +			case REALTEK_RTL82X1X_OUI:
 +				switch (priv->phy_model) {
 +				case 0x01:
-+					reg_val = ast_ether_phy_read_register(dev, 0, PHY_COM_REG_ADDR_CONTROL);
++					reg_val = ast_ether_phy_read_register(dev, phy_ad, PHY_COM_REG_ADDR_CONTROL);
 +					duplex = (reg_val & PHY_COM_REG_CONTROL_DUPLEX) ? PHY_DUPLEX_FULL : PHY_DUPLEX_HALF;
 +					speed = (reg_val & PHY_COM_REG_CONTROL_SPEED) ? PHY_SPEED_100M : PHY_SPEED_10M;
 +					break;
 +				case 0x11:
-+					reg_val = ast_ether_phy_read_register(dev, 0, REALTEK_RTL8211B_REG_ADDR_PHY_STAT);
++					reg_val = ast_ether_phy_read_register(dev, phy_ad, REALTEK_RTL8211B_REG_ADDR_PHY_STAT);
 +					duplex = (reg_val & REALTEK_RTL8211B_REG_ADDR_PHY_STS_DEPLEX) ? PHY_DUPLEX_FULL : PHY_DUPLEX_HALF;
 +					speed = (reg_val & REALTEK_RTL8211B_REG_ADDR_PHY_STS_SPEED_MASK) >> REALTEK_RTL8211B_REG_ADDR_PHY_STS_SPEED_SHIFT;
 +					break;
@@ -214,9 +240,21 @@
 +					duplex = 1;
 +				}
 +				break;
-+			case MICREL_KSZ8041_OUI:
-+				reg_val = ast_ether_phy_read_register(dev, 0, MICREL_KSZ8041_REG_STATUS);
-+				switch ((reg_val & MICREL_KSZ8041_REG_STATUS_MASK) >> MICREL_KSZ8041_REG_STATUS_SHIFT) {
++			case MICREL_KSZ80X1_OUI:
++				switch (priv->phy_model) {
++				case 0x11://8041
++					reg_val = ast_ether_phy_read_register(dev, phy_ad, MICREL_KSZ8041_REG_STATUS);
++					reg_val = (reg_val & MICREL_KSZ8041_REG_STATUS_MASK) >> MICREL_KSZ8041_REG_STATUS_SHIFT;
++					break;
++				case 0x15://8031
++					reg_val = ast_ether_phy_read_register(dev, phy_ad, MICREL_KSZ8031_REG_STATUS);
++					reg_val = reg_val & MICREL_KSZ8031_REG_STATUS_MASK;
++					break;
++				default:
++					reg_val = 0;
++					break;
++				}
++				switch (reg_val) {
 +				case MICREL_KSZ8041_REG_STATUS_HALF_10:
 +					duplex = PHY_DUPLEX_HALF;
 +					speed = PHY_SPEED_10M;
@@ -272,7 +310,7 @@
 +		/* default setting: 100M, duplex */
 +		speed = PHY_SPEED_100M;
 +		duplex = 1;
-+		netif_carrier_on(dev);
++		//netif_carrier_on(dev);
 +	}
 +
 +	reg_val = ioread32(ioaddr + AST_ETHER_MACCR);
@@ -303,6 +341,30 @@
 +	iowrite32(reg_val, ioaddr + AST_ETHER_MACCR);
 +}
 +
++static void ast_ether_bc_checker(unsigned long data)
++{
++	struct net_device *dev;
++	struct ast_ether_priv *priv;
++
++	dev = (struct net_device *) data;
++	unsigned int ioaddr = dev->base_addr;
++	unsigned int reg_value ;
++	priv = netdev_priv(dev);
++
++	if( ast_ether_bc[dev->dev_id].bc_ctr ){
++		ast_ether_bc[dev->dev_id].bc_count = 0 ;  
++		ast_ether_bc[dev->dev_id].bc_ctr = 0 ;
++		reg_value = ioread32(ioaddr + AST_ETHER_IER);
++		reg_value |= (ISR_RPKT2B) ;
++		iowrite32(reg_value, ioaddr + AST_ETHER_IER);
++	}
++	else
++		ast_ether_bc[dev->dev_id].bc_count = 0 ;
++
++	ast_ether_bc[dev->dev_id].timer.expires = jiffies + (AST_BC_CHECK_INTERVAL * HZ);
++	add_timer(&ast_ether_bc[dev->dev_id].timer);
++}
++
 +static void ast_ether_phy_status_checker(unsigned long data)
 +{
 +	struct net_device *dev;
@@ -340,32 +402,31 @@
 +static void ast_ether_init_descriptor(struct net_device *dev)
 +{
 +	struct ast_ether_priv *priv = netdev_priv(dev);
-+	struct sk_buff *skb;
 +	int i;
 +
 +	/* initializes Rx descriptors */
 +	priv->cur_rx = 0;
 +	priv->dirty_rx = 0;
-+	for (i = 0; i < RX_DES_NUM; i ++) {
-+		priv->rx_desc[i].status &= ~RX_DESC_RXPKT_RDY; /* haredware owns descriptor */
-+		priv->rx_skbuff[i] = NULL;
-+	}
-+
 +	/* allocate Rx buffers */
 +	for (i = 0; i < RX_DES_NUM; i ++) {
-+		skb = dev_alloc_skb(RX_BUF_SIZE + 16);
-+		priv->rx_skbuff[i] = skb;
-+		if (skb == NULL) {
-+			printk(KERN_ERR "%s: allocate Rx buffer failed.\n", dev->name);
-+			break;
-+		}
-+		skb->dev = dev; /* Mark as being used by this device. */
-+		skb_reserve(skb, NET_IP_ALIGN); /* 16 byte align the IP header. */
-+
-+		priv->rx_skb_dma[i] = dma_map_single(NULL, skb->data, RX_BUF_SIZE, DMA_FROM_DEVICE);
++        struct sk_buff *rx_skbuff = priv->rx_skbuff[i];
++        if (rx_skbuff == NULL) {
++            if (!(rx_skbuff = priv->rx_skbuff[i] = dev_alloc_skb (RX_BUF_SIZE + 16))) {
++                printk(KERN_ERR "%s: allocate Rx buffer failed.\n", dev->name);
++                ast_ether_free_rx_buf(dev);
++                return;
++            }
++            rx_skbuff->dev = dev; /* Mark as being used by this device. */
++            skb_reserve(rx_skbuff, NET_IP_ALIGN); /* 16 byte align the IP header. */
++        }
++        
++        rmb();
++		priv->rx_skb_dma[i] = dma_map_single(NULL, rx_skbuff->data, RX_BUF_SIZE, DMA_FROM_DEVICE);
 +		priv->rx_desc[i].buffer = priv->rx_skb_dma[i];
-+		priv->rx_desc[i].status = 0;
-+	}
++        wmb();
++        priv->rx_desc[i].status &= ~RX_DESC_RXPKT_RDY; /* haredware owns descriptor */
++	}
++
 +	priv->rx_desc[RX_DES_NUM - 1].status |= RX_DESC_EDORR; /* last descriptor */
 +
 +	/* initializes Tx descriptors */
@@ -460,12 +521,25 @@
 +	/* enable interrupts */
 +	reg_value = ISR_AHB_ERR | ISR_TPKT_LOST | ISR_TPKT2E | ISR_RPKT_LOST | ISR_RXBUF_UNAVA | ISR_RPKT2B;
 +	if (priv->phy_oui != 0) {
++		if(priv->timer.function != NULL)
++			del_timer_sync(&priv->timer);
 +		init_timer(&priv->timer);
 +		priv->timer.data = (unsigned long) dev;
 +		priv->timer.function = ast_ether_phy_status_checker;
 +		priv->timer.expires = jiffies + (AST_PHY_STATUS_CHECK_INTERVAL * HZ);
 +		add_timer(&priv->timer);
 +	}
++	
++	ast_ether_bc[dev->dev_id].bc_count = 0 ;	
++	ast_ether_bc[dev->dev_id].bc_ctr = 0 ;
++	ast_ether_bc[dev->dev_id].trans_busy_1 = 0 ; 
++	ast_ether_bc[dev->dev_id].trans_busy_2 = 0 ; 
++	
++	init_timer(&ast_ether_bc[dev->dev_id].timer);
++	ast_ether_bc[dev->dev_id].timer.data = (unsigned long) dev;
++	ast_ether_bc[dev->dev_id].timer.function = ast_ether_bc_checker;
++	ast_ether_bc[dev->dev_id].timer.expires = jiffies + (AST_BC_CHECK_INTERVAL * HZ);
++	add_timer(&ast_ether_bc[dev->dev_id].timer);
 +	iowrite32(reg_value, ioaddr + AST_ETHER_IER);
 +
 +	/* enable trans/recv, ... */
@@ -572,7 +646,8 @@
 +			priv->rx_skbuff[entry] = skb;
 +			if (skb == NULL) {
 +				printk(KERN_ERR "%s: reallocate Rx buffer failed.\n", dev->name);
-+				break;
++                ast_ether_free_rx_buf(dev);
++                return;
 +			}
 +
 +			skb->dev = dev; /* Mark as being used by this device. */
@@ -586,6 +661,28 @@
 +		priv->dirty_rx ++;
 +	}
 +}
++
++static void ast_ether_free_rx_buf(struct net_device *dev)
++{
++	struct ast_ether_priv *priv = netdev_priv(dev);
++	int i;
++
++	/* free all allocated skbuffs */
++    priv->cur_rx = 0;
++	priv->dirty_rx = 0;
++	for (i = 0; i < RX_DES_NUM; i++) {
++		priv->rx_desc[i].status = 0;
++		wmb();		/* Make sure adapter sees owner change */
++		if (priv->rx_skbuff[i]) {
++            dma_unmap_single(NULL, priv->rx_skb_dma[i], priv->rx_skbuff[i]->len, DMA_TO_DEVICE);
++            dev_kfree_skb_irq(priv->rx_skbuff[i]);
++
++		}
++		priv->rx_skbuff[i] = NULL;
++		priv->rx_skb_dma[i] = 0;
++	}
++}
++
 +
 +static inline void ast_ether_rx_csum(struct sk_buff *skb, struct ast_eth_desc *desc)
 +{
@@ -609,6 +706,8 @@
 +	int has_error;
 +	int has_first;
 +	int entry;
++	unsigned int ioaddr = dev->base_addr;
++	unsigned int reg_value;
 +
 +	has_error = 0;
 +	has_first = 0;
@@ -629,6 +728,18 @@
 +
 +			if (status & RX_DESC_MULTICAST)
 +				priv->stats.multicast ++;
++			
++			if ((status & RX_DESC_BROADCAST)||(status & RX_DESC_MULTICAST)){
++				if( !ast_ether_bc[dev->dev_id].bc_ctr ){
++					ast_ether_bc[dev->dev_id].bc_count++;
++					if (ast_ether_bc[dev->dev_id].bc_count >= 1000) {
++						reg_value = ioread32(ioaddr + AST_ETHER_IER);
++						reg_value &= ~(ISR_RPKT2B) ;
++						iowrite32(reg_value, ioaddr + AST_ETHER_IER);
++						ast_ether_bc[dev->dev_id].bc_ctr = 1 ;
++					}
++				}
++			}
 +		}
 +
 +		skb = priv->rx_skbuff[entry];
@@ -641,6 +752,7 @@
 +				priv->stats.rx_crc_errors ++;
 +			if (status & RX_DESC_FIFO_FULL)
 +				priv->stats.rx_fifo_errors ++;
++			kfree_skb(skb);
 +		} else {
 +			pkt_len = (status & RX_DESC_VDBC) - 4; /* Omit the four octet CRC from the length. */
 +			if (pkt_len > 1518) {
@@ -662,6 +774,18 @@
 +
 +		priv->cur_rx ++;
 +	}
++	
++	if(ast_ether_bc[dev->dev_id].trans_busy_1 == 1){
++		reg_value = ioread32(ioaddr + AST_ETHER_IER);
++		reg_value |= ISR_RXBUF_UNAVA ;
++		iowrite32(reg_value, ioaddr + AST_ETHER_IER);
++	}
++
++	if(ast_ether_bc[dev->dev_id].trans_busy_2 == 1){
++		reg_value = ioread32(ioaddr + AST_ETHER_IER);
++		reg_value |= ISR_RPKT_LOST ;
++		iowrite32(reg_value, ioaddr + AST_ETHER_IER);
++	} 
 +}
 +
 +static irqreturn_t ast_ether_irq_handler(int irq, void *dev_id)
@@ -671,6 +795,7 @@
 +	unsigned int ioaddr;
 +	unsigned int intr_mask;
 +	unsigned int status;
++	unsigned int reg_value;
 +
 +	dev = (struct net_device *)dev_id;
 +	priv = netdev_priv(dev);
@@ -689,12 +814,18 @@
 +		ast_ether_realloc_rx_buf(dev);
 +	}
 +
-+	if (status & ISR_RXBUF_UNAVA)
-+		printk("ISR_RXBUF_UNAVA\n");
-+
-+	if (status & ISR_RPKT_LOST)
-+		printk("ISR_RPKT_LOST\n");
-+
++	if (status & ISR_RXBUF_UNAVA){
++		reg_value = ioread32(ioaddr + AST_ETHER_IER);
++		reg_value &= ~(ISR_RXBUF_UNAVA);
++		iowrite32(reg_value, ioaddr + AST_ETHER_IER);
++		ast_ether_bc[dev->dev_id].trans_busy_1 = 1 ;
++	}
++	if (status & ISR_RPKT_LOST){
++		reg_value = ioread32(ioaddr + AST_ETHER_IER);
++		reg_value &= ~(ISR_RPKT_LOST);
++		iowrite32(reg_value, ioaddr + AST_ETHER_IER);
++		ast_ether_bc[dev->dev_id].trans_busy_2 = 1 ;
++	}
 +	if (status & ISR_TPKT2E) { /* Tx completed */
 +		priv->stats.tx_packets ++;
 +		ast_ether_free_tx_buf(dev);
@@ -741,6 +872,7 @@
 +{
 +	int dev_id;
 +	struct ast_ether_priv *priv;
++    unsigned long flags;
 +
 +	dev_id = dev->dev_id;
 +	priv = netdev_priv(dev);
@@ -751,8 +883,24 @@
 +
 +	if (priv->timer.function != NULL)
 +	    del_timer_sync(&priv->timer);
++	
++	init_timer(&priv->timer);
++	priv->timer.data = (unsigned long)dev;
++	priv->timer.function = ast_ether_phy_status_checker;
++	priv->timer.expires = jiffies + (AST_PHY_STATUS_CHECK_INTERVAL * HZ);
++	add_timer(&priv->timer);
++
++	if( ast_ether_bc[dev->dev_id].timer.function != NULL )
++		del_timer_sync(&ast_ether_bc[dev->dev_id].timer);
++
 +
 +	free_irq(dev->irq, dev);
++	spin_lock_irqsave(&priv->lock, flags);
++
++	ast_ether_free_rx_buf(dev);
++	ast_ether_free_tx_buf(dev);
++
++	spin_unlock_irqrestore(&priv->lock, flags);
 +
 +	return 0;
 +}
@@ -826,9 +974,9 @@
 +	                  
 +	cmd->transceiver = XCVR_INTERNAL; /* only supports internal transceiver */
 +	cmd->advertising = ADVERTISED_TP | ADVERTISED_MII;
-+	advert = ast_ether_phy_read_register(dev, 0, MII_ADVERTISE);
++	advert = ast_ether_phy_read_register(dev, phy_ad, MII_ADVERTISE);
 +    if (priv->supports_gmii)
-+        advert2 = ast_ether_phy_read_register(dev, 0, MII_CTRL1000);
++        advert2 = ast_ether_phy_read_register(dev, phy_ad, MII_CTRL1000);
 +
 +	if (advert & ADVERTISE_10HALF)
 +		cmd->advertising |= ADVERTISED_10baseT_Half;
@@ -841,11 +989,11 @@
 +	if (advert2 & ADVERTISE_1000FULL)
 +		cmd->advertising |= ADVERTISED_1000baseT_Full;
 +			
-+	bmcr = ast_ether_phy_read_register(dev, 0, MII_BMCR);
-+	lpa = ast_ether_phy_read_register(dev, 0, MII_LPA);
++	bmcr = ast_ether_phy_read_register(dev, phy_ad, MII_BMCR);
++	lpa = ast_ether_phy_read_register(dev, phy_ad, MII_LPA);
 +    if (priv->supports_gmii) {
-+        bmcr2 = ast_ether_phy_read_register(dev, 0, MII_CTRL1000);
-+        lpa2 = ast_ether_phy_read_register(dev, 0, MII_STAT1000);
++        bmcr2 = ast_ether_phy_read_register(dev, phy_ad, MII_CTRL1000);
++        lpa2 = ast_ether_phy_read_register(dev, phy_ad, MII_STAT1000);
 +    }
 +	if (bmcr & BMCR_ANENABLE) {
 +		cmd->autoneg = AUTONEG_ENABLE;
@@ -884,7 +1032,7 @@
 +	uint32_t bmcr, advert, tmp;
 +	uint32_t advert2 = 0, tmp2 = 0;
 +
-+	ast_ether_phy_write_register(dev, 0, MII_BMCR, BMCR_RESET);
++	ast_ether_phy_write_register(dev, phy_ad, MII_BMCR, BMCR_RESET);
 +
 +	if (cmd->speed != SPEED_10 && 
 +        cmd->speed != SPEED_100 &&
@@ -905,11 +1053,11 @@
 +			return -EINVAL;
 +
 +		/* advertise only what has been requested */
-+		advert = ast_ether_phy_read_register(dev, 0, MII_ADVERTISE);
++		advert = ast_ether_phy_read_register(dev, phy_ad, MII_ADVERTISE);
 +		tmp = advert & ~(ADVERTISE_ALL | ADVERTISE_100BASE4);
 +		
 +        if (priv->supports_gmii) {
-+			advert2 = ast_ether_phy_read_register(dev, 0, MII_CTRL1000);
++			advert2 = ast_ether_phy_read_register(dev, phy_ad, MII_CTRL1000);
 +			tmp2 = advert2 & ~(ADVERTISE_1000HALF | ADVERTISE_1000FULL);
 +		}
 +		
@@ -929,20 +1077,20 @@
 +		}
 +		
 +		if (advert != tmp) {
-+			ast_ether_phy_write_register(dev, 0, MII_ADVERTISE, tmp);
++			ast_ether_phy_write_register(dev, phy_ad, MII_ADVERTISE, tmp);
 +		}
 +		if ((priv->supports_gmii) && (advert2 != tmp2))
-+			ast_ether_phy_write_register(dev, 0, MII_CTRL1000, tmp2);
++			ast_ether_phy_write_register(dev, phy_ad, MII_CTRL1000, tmp2);
 +			
 +		/* turn on auto negotiation, and force a re-negotiate */
-+		bmcr = ast_ether_phy_read_register(dev, 0, MII_BMCR);
++		bmcr = ast_ether_phy_read_register(dev, phy_ad, MII_BMCR);
 +		bmcr |= (BMCR_ANENABLE | BMCR_ANRESTART);
-+		ast_ether_phy_write_register(dev, 0, MII_BMCR, bmcr);
++		ast_ether_phy_write_register(dev, phy_ad, MII_BMCR, bmcr);
 +	} else {
 +        if ((cmd->speed == SPEED_1000) && (cmd->duplex == DUPLEX_HALF)) /* not supported */
 +                return -EINVAL;
 +		/* turn off auto negotiation, set speed and duplexity */
-+		bmcr = ast_ether_phy_read_register(dev, 0, MII_BMCR);
++		bmcr = ast_ether_phy_read_register(dev, phy_ad, MII_BMCR);
 +		tmp = bmcr & ~(BMCR_ANENABLE | BMCR_SPEED100 |
 +			       BMCR_SPEED1000 | BMCR_FULLDPLX);
 +
@@ -954,7 +1102,7 @@
 +		if (cmd->duplex == DUPLEX_FULL)
 +			tmp |= BMCR_FULLDPLX;
 +		if (bmcr != tmp)
-+			ast_ether_phy_write_register(dev, 0, MII_BMCR, tmp);
++			ast_ether_phy_write_register(dev, phy_ad, MII_BMCR, tmp);
 +	}
 +
 +	return 0;
@@ -1100,6 +1248,8 @@
 +			continue;
 +
 +		priv = netdev_priv(ast_ether_netdev[id]);
++		if(priv->timer.function != NULL)
++			del_timer_sync(&priv->timer);
 +		if (priv->rx_desc)
 +			dma_free_coherent(NULL, sizeof(struct ast_eth_desc) * RX_DES_NUM, priv->rx_desc, priv->rx_desc_dma);
 +		if (priv->tx_desc)
