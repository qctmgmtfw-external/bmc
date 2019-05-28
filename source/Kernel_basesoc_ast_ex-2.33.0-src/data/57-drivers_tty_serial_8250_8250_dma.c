--- linux-3.14.17/drivers/tty/serial/8250/8250_dma.c	2014-08-14 09:38:34.000000000 +0800
+++ linux-3.14.17-ami/drivers/tty/serial/8250/8250_dma.c	2016-04-19 22:06:13.828927980 +0800
@@ -11,227 +11,510 @@
 #include <linux/tty.h>
 #include <linux/tty_flip.h>
 #include <linux/serial_reg.h>
+#include <linux/init.h>
 #include <linux/dma-mapping.h>
+#include <linux/slab.h>
+#include <mach/hardware.h>
+#include <linux/delay.h>
 
 #include "8250.h"
 
-static void __dma_tx_complete(void *param)
+//#define CONFIG_UART_DMA_DEBUG
+
+#ifdef CONFIG_UART_DMA_DEBUG
+    #define UART_DBG(fmt, args...) printk("%s() " fmt, __FUNCTION__, ## args)
+#else
+    #define UART_DBG(fmt, args...)
+#endif
+
+#define UART_UDMA_TIMER_DEFAULT_VALUE 0x171
+
+#define AST_UART_TX_DMA_BUFFER_SIZE                        UDMA_BUFF_SIZE_4KB 
+#if AST_UART_TX_DMA_BUFFER_SIZE==UDMA_BUFF_SIZE_1KB
+  #define AST_UART_TX_DMA_BUFFER_SIZE_VAL                  1024
+#elif AST_UART_TX_DMA_BUFFER_SIZE==UDMA_BUFF_SIZE_4KB
+  #define AST_UART_TX_DMA_BUFFER_SIZE_VAL                  4*1024
+#elif AST_UART_TX_DMA_BUFFER_SIZE==UDMA_BUFF_SIZE_16KB
+  #define AST_UART_TX_DMA_BUFFER_SIZE_VAL                  16*1024
+#else /*AST_UART_TX_DMA_BUFFER_SIZE==UDMA_BUFF_SIZE_64KB*/
+  #define AST_UART_TX_DMA_BUFFER_SIZE_VAL                  64*1024
+#endif
+
+
+#define AST_UART_RX_DMA_BUFFER_SIZE                        UDMA_BUFF_SIZE_64KB 
+#if AST_UART_RX_DMA_BUFFER_SIZE==UDMA_BUFF_SIZE_1KB
+  #define AST_UART_RX_DMA_BUFFER_SIZE_VAL                  1024
+#elif AST_UART_RX_DMA_BUFFER_SIZE==UDMA_BUFF_SIZE_4KB
+  #define AST_UART_RX_DMA_BUFFER_SIZE_VAL                  4*1024
+#elif AST_UART_RX_DMA_BUFFER_SIZE==UDMA_BUFF_SIZE_16KB
+  #define AST_UART_RX_DMA_BUFFER_SIZE_VAL                  16*1024
+#else /*AST_UART_RX_DMA_BUFFER_SIZE==UDMA_BUFF_SIZE_64KB*/
+  #define AST_UART_RX_DMA_BUFFER_SIZE_VAL                  64*1024
+#endif
+
+#define SCU_HARDWARE_STRAP_REQ                      0x70
+#define SCU_HW_STRAP_REQ_SEL_UART_DEBUG_PORT        (1 << 29)
+
+static int dma_count=0;
+u32 locount=0;
+u32 errcount=0;
+
+static inline void ast_udma_bufffdone(struct uart_8250_port *p, u8 dir);
+static irqreturn_t ast_uart_udma_irq(int irq, void *dev_id);
+static int ast_uart_tx_udma_update(struct uart_8250_port *p, u16 point);
+static int ast_uart_tx_udma_enqueue(struct uart_8250_port *p);
+static int ast_uart_rx_udma_enqueue(struct uart_8250_port *p);
+static int ast_uart_rx_udma_ctrl(struct uart_8250_port *p, enum ast_uart_chan_op op);
+static int ast_uart_tx_udma_ctrl(struct uart_8250_port *p, enum ast_uart_chan_op op);
+
+static void ast_uart_udma_write(u32 value, u32 offset)
 {
-	struct uart_8250_port	*p = param;
-	struct uart_8250_dma	*dma = p->dma;
-	struct circ_buf		*xmit = &p->port.state->xmit;
-	unsigned long	flags;
+    iowrite32( value, (void * __iomem)AST_UART_UDMA_VA_BASE + offset );
+}
 
-	dma_sync_single_for_cpu(dma->txchan->device->dev, dma->tx_addr,
-				UART_XMIT_SIZE, DMA_TO_DEVICE);
+static u32 ast_uart_udma_read(u32 offset)
+{
+    return( ioread32( (void * __iomem)AST_UART_UDMA_VA_BASE + offset ) );
+}
 
-	spin_lock_irqsave(&p->port.lock, flags);
+int serial8250_tx_dma(struct uart_8250_port *p)
+{
+    struct circ_buf *xmit = &p->port.state->xmit;
+    UART_DBG("line [%d]  \n", p->dma->tx.dma_ch);
 
-	dma->tx_running = 0;
+    ast_uart_tx_udma_ctrl(p, AST_UART_DMAOP_PAUSE);
+    dma_sync_single_for_device(p->port.dev,
+                               p->dma->tx.dma_virt_addr,
+                               AST_UART_TX_DMA_BUFFER_SIZE_VAL,
+                               DMA_TO_DEVICE);
+    //update xmit->head -->fot tx 
+    ast_uart_tx_udma_update(p, xmit->head);
+    ast_uart_tx_udma_ctrl(p, AST_UART_DMAOP_TRIGGER);
+    return 0;
+}
+EXPORT_SYMBOL_GPL(serial8250_tx_dma);
 
-	xmit->tail += dma->tx_size;
-	xmit->tail &= UART_XMIT_SIZE - 1;
-	p->port.icount.tx += dma->tx_size;
+int serial8250_rx_dma(struct uart_8250_port *p, unsigned int iir)
+{
+    return 0;
+}
+EXPORT_SYMBOL_GPL(serial8250_rx_dma);
 
-	if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
-		uart_write_wakeup(&p->port);
+int serial8250_request_dma(struct uart_8250_port *p)
+{
+    struct uart_8250_dma *dma = p->dma;
 
-	if (!uart_circ_empty(xmit) && !uart_tx_stopped(&p->port))
-		serial8250_tx_dma(p);
+    dma->rx.dma_buf.head = 0;
+    dma->rx.dma_buf.tail = 0;
+    dma->rx.dma_buf.buf = (unsigned char *)dma_alloc_coherent(NULL, AST_UART_RX_DMA_BUFFER_SIZE_VAL, &dma->rx.dma_virt_addr, GFP_KERNEL);
+    UART_DBG("RX buff vir = %p, phy = %x \n", dma->rx.dma_buf.buf, dma->rx.dma_virt_addr);
+    ast_uart_rx_udma_ctrl(p, AST_UART_DMAOP_STOP);
+    ast_uart_rx_udma_enqueue(p);
+    mdelay(300);
+    ast_uart_rx_udma_ctrl(p, AST_UART_DMAOP_TRIGGER);
+
+    dma->tx.dma_buf.head = 0;
+    dma->tx.dma_buf.tail = 0;
+    dma->tx.dma_buf.buf = p->port.state->xmit.buf;
+    dma->tx.dma_virt_addr = dma_map_single(p->port.dev,
+                                           dma->tx.dma_buf.buf,
+                                           UART_XMIT_SIZE,
+                                           DMA_TO_DEVICE);
+                        
+    ast_uart_tx_udma_ctrl(p, AST_UART_DMAOP_STOP);
+    ast_uart_tx_udma_enqueue(p);
+    return 0;
+}
+EXPORT_SYMBOL_GPL(serial8250_request_dma);
 
-	spin_unlock_irqrestore(&p->port.lock, flags);
+void serial8250_release_dma(struct uart_8250_port *p)
+{
+    ast_uart_rx_udma_ctrl(p, AST_UART_DMAOP_STOP);
+    ast_uart_tx_udma_ctrl(p, AST_UART_DMAOP_STOP);
+    UART_DBG("free TX , RX buffer \n");
+    dma_free_coherent(p->port.dev,
+                      AST_UART_RX_DMA_BUFFER_SIZE_VAL,
+                      p->dma->rx.dma_buf.buf,
+                      p->dma->rx.dma_virt_addr);
+    dma_unmap_single(p->port.dev, p->dma->tx.dma_virt_addr,
+                     AST_UART_TX_DMA_BUFFER_SIZE_VAL,
+                     DMA_TO_DEVICE);
 }
+EXPORT_SYMBOL_GPL(serial8250_release_dma);
 
-static void __dma_rx_complete(void *param)
+static inline void ast_udma_bufffdone(struct uart_8250_port *p, u8 dir)
 {
-	struct uart_8250_port	*p = param;
-	struct uart_8250_dma	*dma = p->dma;
-	struct tty_port		*tty_port = &p->port.state->port;
-	struct dma_tx_state	state;
-	int			count;
+    u32 len=0;
+    int ch = (dir==DMA_TO_DEVICE?p->dma->tx.dma_ch:p->dma->rx.dma_ch);
+    struct ast_uart_dma_info *udma = (dir==DMA_TO_DEVICE?(&(p->dma->tx)):(&(p->dma->rx)));
+
+    if(udma->enable == 0) {
+        //printk("udma Please check \n");
+        return;
+    }
+
+    if(dir==DMA_TO_DEVICE) {
+        len = ast_uart_udma_read(UART_TX_R_POINT(ch)) ;
+        UART_DBG("tx rp %x , wp %x \n", ast_uart_udma_read(UART_TX_R_POINT(ch)), ast_uart_udma_read(UART_TX_W_POINT(ch)));
+        
+    }
+
+    UART_DBG("<dma dwn>: ch[(%d:%d)] : %s ,len : %d \n", p->port.line, ch, (dir==DMA_TO_DEVICE ? "tx" : "rx"), len);
 
-	dma_sync_single_for_cpu(dma->rxchan->device->dev, dma->rx_addr,
-				dma->rx_size, DMA_FROM_DEVICE);
+    if (udma->callback_fn != NULL)
+        (udma->callback_fn)(p, len);
+}
+
+static irqreturn_t ast_uart_udma_irq(int irq, void *dev_id)
+{
+    struct uart_8250_port *p = (struct uart_8250_port *)dev_id;
+    int count=0;
 
-	dmaengine_tx_status(dma->rxchan, dma->rx_cookie, &state);
-	dmaengine_terminate_all(dma->rxchan);
+    u32 tx_sts = (u16) ast_uart_udma_read(UART_TX_UDMA_ISR);
+    u32 rx_sts = (u16) ast_uart_udma_read(UART_RX_UDMA_ISR);
 
-	count = dma->rx_size - state.residue;
+    UART_DBG("tx sts : %x, rx sts : %x \n",tx_sts, rx_sts);
 
-	tty_insert_flip_string(tty_port, dma->rx_buf, count);
-	p->port.icount.rx += count;
+    if((tx_sts == 0) && (rx_sts == 0)) {
+        printk("UDMA IRQ ERROR !!!\n");
+        return IRQ_HANDLED;    
+    }
+
+    while (rx_sts) {
+        /* BMC DEBUG PORT doesn't SUPPORT DMA mode */
+        if (p->dma==NULL) p++;
+        if (rx_sts & (1 << count)) {
+            /* clear the interrupt status */
+            ast_uart_udma_write((1 << count), UART_RX_UDMA_ISR);
+            ast_udma_bufffdone(p, DMA_FROM_DEVICE);
+        }
+        rx_sts &= ~(1 << count);
+        count++;
+        p++;
+    }
+
+
+    count=0;
+
+    while (tx_sts) {
+        /* BMC DEBUG PORT doesn't SUPPORT DMA mode */
+        if (p->dma==NULL) p++;
+        if (tx_sts & (1 << count)) {
+            /* clear the interrupt status */
+            ast_uart_udma_write((1 << count), UART_TX_UDMA_ISR);
+            ast_udma_bufffdone(p, DMA_TO_DEVICE);
+        }
+        tx_sts &= ~(1 << count);
+        count++;
+        p++;
+    }
 
-	tty_flip_buffer_push(tty_port);
+    return IRQ_HANDLED;
 }
 
-int serial8250_tx_dma(struct uart_8250_port *p)
+int ast_uart_udma_init(struct uart_8250_port *p)
 {
-	struct uart_8250_dma		*dma = p->dma;
-	struct circ_buf			*xmit = &p->port.state->xmit;
-	struct dma_async_tx_descriptor	*desc;
+    int ret;
+
+    ast_uart_udma_write(0x400, UART_UDMA_TIMER);
+    ast_uart_udma_write(0xfff, UART_TX_UDMA_ISR);
+    ast_uart_udma_write(0, UART_TX_UDMA_IER);
+    ast_uart_udma_write(0xfff, UART_RX_UDMA_ISR);
+    ast_uart_udma_write(0, UART_RX_UDMA_IER);
+    ast_uart_udma_write(UART_UDMA_TIMER_DEFAULT_VALUE, UART_UDMA_TIMER);
+
+    ret = request_irq(IRQ_UART_UDMA_INT,
+                      ast_uart_udma_irq, IRQF_DISABLED, 
+                      "ast_uart_udma", p);
+    if (ret) {
+        printk (KERN_ERR "Request UART UDMA IRQ Fail\n");
+        return -1;
+    }
+
+    ast_uart_udma_write(UDMA_SET_TX_BUFF_SIZE(AST_UART_TX_DMA_BUFFER_SIZE) | UDMA_SET_RX_BUFF_SIZE(AST_UART_RX_DMA_BUFFER_SIZE), UART_UDMA_CONF);
+
+    return 0;
+}                                                                              
+EXPORT_SYMBOL(ast_uart_udma_init);
 
-	if (uart_tx_stopped(&p->port) || dma->tx_running ||
-	    uart_circ_empty(xmit))
-		return 0;
+static int ast_uart_tx_udma_update(struct uart_8250_port *p, u16 point)
+{
+    unsigned long flags;
+    int ch = p->dma->tx.dma_ch;
+    UART_DBG("TX DMA CTRL [ch (%d:%d)] \n", p->port.line, ch);
+
+    local_irq_save(flags);
+    ast_uart_udma_write(point, UART_TX_W_POINT(ch));
+    local_irq_restore(flags);
+    return 0;
+}
 
-	dma->tx_size = CIRC_CNT_TO_END(xmit->head, xmit->tail, UART_XMIT_SIZE);
+void ast_uart_rx_udma_tasklet_func(unsigned long data)
+{
+    struct uart_8250_port *up = (struct uart_8250_port *)data;
+    struct circ_buf *rx_ring = &up->dma->rx.dma_buf;
+    struct tty_port *port = &(up->port.state->port);
+
+    u32 h=0,t=0;
+    u32 len=0;
+    int ch = up->dma->rx.dma_ch;
+    UART_DBG("rx rp %x , wp %x \n", ast_uart_udma_read(UART_RX_R_POINT(ch)), ast_uart_udma_read(UART_RX_W_POINT(ch)));
+    spin_lock_irq(&up->dma->rx.lock);
+    ast_uart_rx_udma_ctrl(up, AST_UART_DMAOP_TRIGGER);
+    h=ast_uart_udma_read(UART_RX_W_POINT(ch));
+    t=ast_uart_udma_read(UART_RX_R_POINT(ch));
+    ast_uart_udma_write(h,UART_RX_R_POINT(ch)) ;
+    if (t > h) {
+        len=(AST_UART_RX_DMA_BUFFER_SIZE_VAL-t)+h+1;
+    } else {
+        len=h-t;
+    }
+
+    if ((rx_ring->head+len) > AST_UART_RX_DMA_BUFFER_SIZE_VAL)
+        rx_ring->head=(rx_ring->head+len) - AST_UART_RX_DMA_BUFFER_SIZE_VAL -1;
+    else
+        rx_ring->head+= len;
+
+    {
+        if (rx_ring->head != rx_ring->tail) {
+            if (rx_ring->head < rx_ring->tail) {
+                tty_insert_flip_string(port, &rx_ring->buf[rx_ring->tail], AST_UART_RX_DMA_BUFFER_SIZE_VAL-rx_ring->tail);
+                spin_lock(&up->port.lock);
+                tty_flip_buffer_push(port);
+                spin_unlock(&up->port.lock);
+                rx_ring->tail = 0;
+            }
+
+            if (rx_ring->head != rx_ring->tail) {
+                tty_insert_flip_string(port, &rx_ring->buf[rx_ring->tail], rx_ring->head-rx_ring->tail);
+                rx_ring->tail = rx_ring->head;
+            }
+        }
+    }
+    spin_unlock_irq(&up->dma->rx.lock);
+
+    spin_lock(&up->port.lock);
+    tty_flip_buffer_push(port);
+    spin_unlock(&up->port.lock);
+}
+EXPORT_SYMBOL_GPL(ast_uart_rx_udma_tasklet_func);
 
-	desc = dmaengine_prep_slave_single(dma->txchan,
-					   dma->tx_addr + xmit->tail,
-					   dma->tx_size, DMA_MEM_TO_DEV,
-					   DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
-	if (!desc)
-		return -EBUSY;
+void ast_uart_rx_buffdone(void *dev_id, u16 len)
+{
+    struct uart_8250_port *up = (struct uart_8250_port *)dev_id;
 
-	dma->tx_running = 1;
+    UART_DBG("line [(%d:%d)],head = %d, len : %d\n",up->port.line,up->dma->rx.dma_ch,up->dma->rx.dma_buf.head, len);
+    tasklet_schedule(&up->dma->rx.tasklet);
+}
+EXPORT_SYMBOL_GPL(ast_uart_rx_buffdone);
 
-	desc->callback = __dma_tx_complete;
-	desc->callback_param = p;
+void ast_uart_tx_buffdone(void *dev_id, u16 len)
+{
+    struct uart_8250_port *up = (struct uart_8250_port *) dev_id;
+    struct circ_buf *xmit = &up->port.state->xmit;
 
-	dma->tx_cookie = dmaengine_submit(desc);
+    UART_DBG("line [(%d:%d)] : tx len = %d \n", up->port.line, up->dma->tx.dma_ch, len);    
 
-	dma_sync_single_for_device(dma->txchan->device->dev, dma->tx_addr,
-				   UART_XMIT_SIZE, DMA_TO_DEVICE);
+    spin_lock(&up->port.lock);
+    //-->get tail for update len 
+    xmit->tail = len;
+    UART_DBG("???? line [%d], xmit->head =%d, xmit->tail = %d\n",up->dma->tx.dma_ch,xmit->head, xmit->tail);
+
+    if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS)
+            uart_write_wakeup(&up->port);
+
+    if(xmit->head != xmit->tail) {
+        ast_uart_tx_udma_ctrl(up, AST_UART_DMAOP_PAUSE);
+        dma_sync_single_for_device(up->port.dev,
+                                   up->dma->tx.dma_virt_addr,
+                                   AST_UART_TX_DMA_BUFFER_SIZE_VAL,
+                                   DMA_TO_DEVICE);
+        //update xmit->head -->fot tx 
+        ast_uart_tx_udma_update(up, xmit->head);
+        ast_uart_tx_udma_ctrl(up, AST_UART_DMAOP_TRIGGER);
+    }
 
-	dma_async_issue_pending(dma->txchan);
+    spin_unlock(&up->port.lock);
 
-	return 0;
 }
-EXPORT_SYMBOL_GPL(serial8250_tx_dma);
+EXPORT_SYMBOL_GPL(ast_uart_tx_buffdone);
 
-int serial8250_rx_dma(struct uart_8250_port *p, unsigned int iir)
+int ast_uart_tx_udma_request(struct uart_8250_port *p, ast_uart_dma_cbfn_t rtn, void *id)
 {
-	struct uart_8250_dma		*dma = p->dma;
-	struct dma_async_tx_descriptor	*desc;
-	struct dma_tx_state		state;
-	int				dma_status;
-
-	dma_status = dmaengine_tx_status(dma->rxchan, dma->rx_cookie, &state);
-
-	switch (iir & 0x3f) {
-	case UART_IIR_RLSI:
-		/* 8250_core handles errors and break interrupts */
-		return -EIO;
-	case UART_IIR_RX_TIMEOUT:
-		/*
-		 * If RCVR FIFO trigger level was not reached, complete the
-		 * transfer and let 8250_core copy the remaining data.
-		 */
-		if (dma_status == DMA_IN_PROGRESS) {
-			dmaengine_pause(dma->rxchan);
-			__dma_rx_complete(p);
-		}
-		return -ETIMEDOUT;
-	default:
-		break;
-	}
-
-	if (dma_status)
-		return 0;
-
-	desc = dmaengine_prep_slave_single(dma->rxchan, dma->rx_addr,
-					   dma->rx_size, DMA_DEV_TO_MEM,
-					   DMA_PREP_INTERRUPT | DMA_CTRL_ACK);
-	if (!desc)
-		return -EBUSY;
-
-	desc->callback = __dma_rx_complete;
-	desc->callback_param = p;
+    unsigned long flags;
+    int ch = p->dma->tx.dma_ch;
+
+    UART_DBG("TX DMA REQUEST ch = (%d:%d) \n", p->port.line, ch);
+
+    local_irq_save(flags);
 
-	dma->rx_cookie = dmaengine_submit(desc);
+    p->dma->tx.priv = id;
+    p->dma->tx.callback_fn = rtn;
 
-	dma_sync_single_for_device(dma->rxchan->device->dev, dma->rx_addr,
-				   dma->rx_size, DMA_FROM_DEVICE);
+    //DMA IRQ En
+    ast_uart_udma_write(ast_uart_udma_read(UART_TX_UDMA_IER) | (1 << ch),
+                        UART_TX_UDMA_IER);
 
-	dma_async_issue_pending(dma->rxchan);
+    local_irq_restore(flags);
+
+    return 0;
 
-	return 0;
 }
-EXPORT_SYMBOL_GPL(serial8250_rx_dma);
 
-int serial8250_request_dma(struct uart_8250_port *p)
+EXPORT_SYMBOL(ast_uart_tx_udma_request);
+
+int ast_uart_rx_udma_request(struct uart_8250_port *p, ast_uart_dma_cbfn_t rtn, void *id)
 {
-	struct uart_8250_dma	*dma = p->dma;
-	dma_cap_mask_t		mask;
+    unsigned long flags;
+    int ch = p->dma->rx.dma_ch;
+
+    UART_DBG("RX DMA REQUEST ch = (%d:%d) \n", p->port.line,ch);
 
-	/* Default slave configuration parameters */
-	dma->rxconf.direction		= DMA_DEV_TO_MEM;
-	dma->rxconf.src_addr_width	= DMA_SLAVE_BUSWIDTH_1_BYTE;
-	dma->rxconf.src_addr		= p->port.mapbase + UART_RX;
-
-	dma->txconf.direction		= DMA_MEM_TO_DEV;
-	dma->txconf.dst_addr_width	= DMA_SLAVE_BUSWIDTH_1_BYTE;
-	dma->txconf.dst_addr		= p->port.mapbase + UART_TX;
-
-	dma_cap_zero(mask);
-	dma_cap_set(DMA_SLAVE, mask);
-
-	/* Get a channel for RX */
-	dma->rxchan = dma_request_slave_channel_compat(mask,
-						       dma->fn, dma->rx_param,
-						       p->port.dev, "rx");
-	if (!dma->rxchan)
-		return -ENODEV;
-
-	dmaengine_slave_config(dma->rxchan, &dma->rxconf);
-
-	/* Get a channel for TX */
-	dma->txchan = dma_request_slave_channel_compat(mask,
-						       dma->fn, dma->tx_param,
-						       p->port.dev, "tx");
-	if (!dma->txchan) {
-		dma_release_channel(dma->rxchan);
-		return -ENODEV;
-	}
-
-	dmaengine_slave_config(dma->txchan, &dma->txconf);
-
-	/* RX buffer */
-	if (!dma->rx_size)
-		dma->rx_size = PAGE_SIZE;
-
-	dma->rx_buf = dma_alloc_coherent(dma->rxchan->device->dev, dma->rx_size,
-					&dma->rx_addr, GFP_KERNEL);
-	if (!dma->rx_buf) {
-		dma_release_channel(dma->rxchan);
-		dma_release_channel(dma->txchan);
-		return -ENOMEM;
-	}
-
-	/* TX buffer */
-	dma->tx_addr = dma_map_single(dma->txchan->device->dev,
-					p->port.state->xmit.buf,
-					UART_XMIT_SIZE,
-					DMA_TO_DEVICE);
+    local_irq_save(flags);
 
-	dev_dbg_ratelimited(p->port.dev, "got both dma channels\n");
+    p->dma->rx.priv = id;
+    p->dma->rx.callback_fn = rtn;
+
+    //DMA IRQ En
+    ast_uart_udma_write(ast_uart_udma_read(UART_RX_UDMA_IER) | (1 << ch),
+                        UART_RX_UDMA_IER);
+
+    local_irq_restore(flags);
+
+    return 0;
 
-	return 0;
 }
-EXPORT_SYMBOL_GPL(serial8250_request_dma);
 
-void serial8250_release_dma(struct uart_8250_port *p)
+EXPORT_SYMBOL(ast_uart_rx_udma_request);
+
+
+static int ast_uart_tx_udma_ctrl(struct uart_8250_port *p, enum ast_uart_chan_op op)
+{
+    unsigned long flags;
+    int ch = p->dma->tx.dma_ch;
+    UART_DBG("TX DMA CTRL [ch (%d:%d)] \n", p->port.line, ch);
+
+    local_irq_save(flags);
+
+    switch (op) {
+        case AST_UART_DMAOP_TRIGGER:
+            UART_DBG("Trigger \n");
+            p->dma->tx.enable = 1;
+            //set enable 
+            ast_uart_udma_write(ast_uart_udma_read(UART_TX_UDMA_EN) | (0x1 << ch), UART_TX_UDMA_EN);
+            break;
+        case AST_UART_DMAOP_STOP:
+            UART_DBG("STOP \n");
+            p->dma->tx.enable = 0;
+            //disable engine 
+            ast_uart_udma_write(ast_uart_udma_read(UART_TX_UDMA_EN) & ~(0x1 << ch), UART_TX_UDMA_EN);
+
+            //set reset 
+            ast_uart_udma_write(ast_uart_udma_read(UART_TX_UDMA_REST) | (0x1 << ch), UART_TX_UDMA_REST);
+            ast_uart_udma_write(ast_uart_udma_read(UART_TX_UDMA_REST) & ~(0x1 << ch), UART_TX_UDMA_REST);
+            break;
+        case AST_UART_DMAOP_PAUSE:
+            //disable engine
+            ast_uart_udma_write(ast_uart_udma_read(UART_TX_UDMA_EN) & ~(0x1 << ch), UART_TX_UDMA_EN);
+    }
+
+    local_irq_restore(flags);
+    return 0;
+}
+
+static int ast_uart_tx_udma_enqueue(struct uart_8250_port *p)
+{
+    unsigned long flags;
+    int ch = p->dma->tx.dma_ch;
+
+    UART_DBG("ch = (%d:%d), rx buff = %x, len = %d \n", p->port.line, ch, p->dma->tx.dma_virt_addr, AST_UART_TX_DMA_BUFFER_SIZE_VAL);
+
+    local_irq_save(flags);
+
+    ast_uart_udma_write(p->dma->tx.dma_virt_addr, UART_TX_UDMA_ADDR(ch));
+
+    ast_uart_udma_write(0, UART_TX_W_POINT(ch));
+
+    local_irq_restore(flags);
+
+    return 0;
+}
+
+static int ast_uart_rx_udma_ctrl(struct uart_8250_port *p, enum ast_uart_chan_op op)
+{
+    unsigned long flags;
+    int ch = p->dma->rx.dma_ch;
+    UART_DBG("RX DMA CTRL [ch %d] \n", ch);
+
+    local_irq_save(flags);
+
+    switch (op) {
+        case AST_UART_DMAOP_TRIGGER:
+            UART_DBG("Trigger \n");
+            p->dma->rx.enable = 1;
+            //set enable 
+            ast_uart_udma_write(ast_uart_udma_read(UART_RX_UDMA_EN) | (0x1 << ch), UART_RX_UDMA_EN);
+            break;
+        case AST_UART_DMAOP_STOP:
+            //disable engine 
+            UART_DBG("STOP \n");
+            p->dma->rx.enable = 0;
+            ast_uart_udma_write(ast_uart_udma_read(UART_RX_UDMA_EN) & ~(0x1 << ch), UART_RX_UDMA_EN);
+
+            //set reset 
+            ast_uart_udma_write(ast_uart_udma_read(UART_RX_UDMA_REST) | (0x1 << ch), UART_RX_UDMA_REST);
+            ast_uart_udma_write(ast_uart_udma_read(UART_RX_UDMA_REST) & ~(0x1 << ch), UART_RX_UDMA_REST);
+            break;
+        case AST_UART_DMAOP_PAUSE:
+            //disable engine
+            ast_uart_udma_write(ast_uart_udma_read(UART_RX_UDMA_EN) & ~(0x1 << ch), UART_RX_UDMA_EN);
+            break;
+    }
+
+    local_irq_restore(flags);
+    return 0;
+}
+
+static int ast_uart_rx_udma_enqueue(struct uart_8250_port *p)
 {
-	struct uart_8250_dma *dma = p->dma;
+    unsigned long flags;
+    int ch = p->dma->rx.dma_ch;
+
+    UART_DBG("ch = %d, rx buff = %x, len = %d \n", ch, p->dma->rx.dma_virt_addr, AST_UART_RX_DMA_BUFFER_SIZE_VAL);
 
-	if (!dma)
-		return;
+    local_irq_save(flags);
 
-	/* Release RX resources */
-	dmaengine_terminate_all(dma->rxchan);
-	dma_free_coherent(dma->rxchan->device->dev, dma->rx_size, dma->rx_buf,
-			  dma->rx_addr);
-	dma_release_channel(dma->rxchan);
-	dma->rxchan = NULL;
-
-	/* Release TX resources */
-	dmaengine_terminate_all(dma->txchan);
-	dma_unmap_single(dma->txchan->device->dev, dma->tx_addr,
-			 UART_XMIT_SIZE, DMA_TO_DEVICE);
-	dma_release_channel(dma->txchan);
-	dma->txchan = NULL;
-	dma->tx_running = 0;
+    ast_uart_udma_write(p->dma->rx.dma_virt_addr, UART_RX_UDMA_ADDR(ch));
 
-	dev_dbg_ratelimited(p->port.dev, "dma channels released\n");
+    local_irq_restore(flags);
+
+    return 0;
 }
-EXPORT_SYMBOL_GPL(serial8250_release_dma);
+
+int serial8250_dma_config(struct uart_8250_port *p)
+{
+    struct uart_8250_dma *dma=NULL;
+    int ch = p->port.line;
+
+    if (ch == 4) {
+        p->dma = NULL;
+        //Select UART5 as BMC Console
+        return -EPERM;
+    }
+
+    dma = kmalloc(sizeof(struct uart_8250_dma), GFP_KERNEL);
+    if (!dma)
+        return -ENOMEM;
+
+    dma->rx.enable = 0;
+    dma->rx.dma_ch=dma_count;
+
+    dma->tx.enable = 0;
+    dma->tx.dma_ch=dma_count;
+    dma_count++;
+    p->dma = dma;
+    ast_uart_rx_udma_ctrl(p, AST_UART_DMAOP_STOP);
+    ast_uart_tx_udma_ctrl(p, AST_UART_DMAOP_STOP);
+    return 0;
+}
+EXPORT_SYMBOL_GPL(serial8250_dma_config);
