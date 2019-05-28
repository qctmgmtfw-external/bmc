--- linux-3.14.17/drivers/tty/serial/8250/8250.h	2014-12-03 16:07:13.545519966 +0800
+++ linux-3.14.17-ami/drivers/tty/serial/8250/8250.h	2014-12-17 18:20:56.982920009 +0800
@@ -14,53 +14,58 @@
 #include <linux/serial_8250.h>
 #include <linux/dmaengine.h>
 
-struct uart_8250_dma {
-	dma_filter_fn		fn;
-	void			*rx_param;
-	void			*tx_param;
-
-	int			rx_chan_id;
-	int			tx_chan_id;
-
-	struct dma_slave_config	rxconf;
-	struct dma_slave_config	txconf;
-
-	struct dma_chan		*rxchan;
-	struct dma_chan		*txchan;
-
-	dma_addr_t		rx_addr;
-	dma_addr_t		tx_addr;
+/* ast_uart_dma_cbfn_t *  * buffer callback routine type */
+typedef void (*ast_uart_dma_cbfn_t)(void *dev_id, u16 len);
 
-	dma_cookie_t		rx_cookie;
-	dma_cookie_t		tx_cookie;
-
-	void			*rx_buf;
+struct ast_uart_dma_info {
+    void                    *priv;
+    dma_addr_t              dma_virt_addr;
+    struct circ_buf         dma_buf;
+    /* cdriver callbacks */
+    ast_uart_dma_cbfn_t     callback_fn;   /* buffer done callback */
+    struct tasklet_struct   tasklet;
+    int                     tasklet_done;
+    u8                      enable;
+    u8                      dma_ch;
+    spinlock_t              lock;
+};
 
-	size_t			rx_size;
-	size_t			tx_size;
+struct uart_8250_dma {
+    struct ast_uart_dma_info    tx;
+    struct ast_uart_dma_info    rx;
+};
 
-	unsigned char		tx_running:1;
+/* enum ast_uart_chan_op
+   *
+   * operation codes passed to the DMA code by the user, and also used
+   * to inform the current channel owner of any changes to the system state
+   */
+
+enum ast_uart_chan_op {
+    AST_UART_DMAOP_TRIGGER,
+    AST_UART_DMAOP_STOP,
+    AST_UART_DMAOP_PAUSE,
 };
 
 struct old_serial_port {
-	unsigned int uart;
-	unsigned int baud_base;
-	unsigned int port;
-	unsigned int irq;
-	unsigned int flags;
-	unsigned char hub6;
-	unsigned char io_type;
-	unsigned char *iomem_base;
-	unsigned short iomem_reg_shift;
-	unsigned long irqflags;
+    unsigned int uart;
+    unsigned int baud_base;
+    unsigned int port;
+    unsigned int irq;
+    unsigned int flags;
+    unsigned char hub6;
+    unsigned char io_type;
+    unsigned char *iomem_base;
+    unsigned short iomem_reg_shift;
+    unsigned long irqflags;
 };
 
 struct serial8250_config {
-	const char	*name;
-	unsigned short	fifo_size;
-	unsigned short	tx_loadsz;
-	unsigned char	fcr;
-	unsigned int	flags;
+    const char    *name;
+    unsigned short    fifo_size;
+    unsigned short    tx_loadsz;
+    unsigned char    fcr;
+    unsigned int    flags;
 };
 
 #define UART_CAP_FIFO	(1 << 8)	/* UART has FIFO */
@@ -90,24 +95,24 @@
 
 static inline int serial_in(struct uart_8250_port *up, int offset)
 {
-	return up->port.serial_in(&up->port, offset);
+    return up->port.serial_in(&up->port, offset);
 }
 
 static inline void serial_out(struct uart_8250_port *up, int offset, int value)
 {
-	up->port.serial_out(&up->port, offset, value);
+    up->port.serial_out(&up->port, offset, value);
 }
 
 void serial8250_clear_and_reinit_fifos(struct uart_8250_port *p);
 
 static inline int serial_dl_read(struct uart_8250_port *up)
 {
-	return up->dl_read(up);
+    return up->dl_read(up);
 }
 
 static inline void serial_dl_write(struct uart_8250_port *up, int value)
 {
-	up->dl_write(up, value);
+    up->dl_write(up, value);
 }
 
 #if defined(__alpha__) && !defined(CONFIG_PCI)
@@ -132,37 +137,37 @@
 #ifdef CONFIG_ARCH_OMAP1
 static inline int is_omap1_8250(struct uart_8250_port *pt)
 {
-	int res;
+    int res;
 
-	switch (pt->port.mapbase) {
-	case OMAP1_UART1_BASE:
-	case OMAP1_UART2_BASE:
-	case OMAP1_UART3_BASE:
-		res = 1;
-		break;
-	default:
-		res = 0;
-		break;
-	}
+    switch (pt->port.mapbase) {
+    case OMAP1_UART1_BASE:
+    case OMAP1_UART2_BASE:
+    case OMAP1_UART3_BASE:
+        res = 1;
+        break;
+    default:
+        res = 0;
+        break;
+    }
 
-	return res;
+    return res;
 }
 
 static inline int is_omap1510_8250(struct uart_8250_port *pt)
 {
-	if (!cpu_is_omap1510())
-		return 0;
+    if (!cpu_is_omap1510())
+        return 0;
 
-	return is_omap1_8250(pt);
+    return is_omap1_8250(pt);
 }
 #else
 static inline int is_omap1_8250(struct uart_8250_port *pt)
 {
-	return 0;
+    return 0;
 }
 static inline int is_omap1510_8250(struct uart_8250_port *pt)
 {
-	return 0;
+    return 0;
 }
 #endif
 
@@ -171,18 +176,46 @@
 extern int serial8250_rx_dma(struct uart_8250_port *, unsigned int iir);
 extern int serial8250_request_dma(struct uart_8250_port *);
 extern void serial8250_release_dma(struct uart_8250_port *);
+extern void ast_uart_tx_buffdone(void *dev_id, u16 len);
+extern void ast_uart_rx_buffdone(void *dev_id, u16 len);
+extern void ast_uart_tx_udma_tasklet_func(unsigned long data);
+extern void ast_uart_rx_udma_tasklet_func(unsigned long data);
+extern int serial8250_dma_config(struct uart_8250_port *p);
+extern int ast_uart_tx_udma_request(struct uart_8250_port *p, ast_uart_dma_cbfn_t rtn, void *id);
+extern int ast_uart_rx_udma_request(struct uart_8250_port *p, ast_uart_dma_cbfn_t rtn, void *id);
+extern int ast_uart_udma_init(struct uart_8250_port *p);
 #else
 static inline int serial8250_tx_dma(struct uart_8250_port *p)
 {
-	return -1;
+    return -1;
 }
 static inline int serial8250_rx_dma(struct uart_8250_port *p, unsigned int iir)
 {
-	return -1;
+    return -1;
 }
 static inline int serial8250_request_dma(struct uart_8250_port *p)
 {
-	return -1;
+    return -1;
 }
 static inline void serial8250_release_dma(struct uart_8250_port *p) { }
+static inline void ast_uart_tx_buffdone(void *dev_id, u16 len) { }
+static inline void ast_uart_rx_buffdone(void *dev_id, u16 len) { }
+static inline void ast_uart_tx_udma_tasklet_func(unsigned long data) { }
+static inline void ast_uart_rx_udma_tasklet_func(unsigned long data) { }
+static inline int serial8250_dma_config(struct uart_8250_port *p)
+{
+    return -1;
+}
+static inline int ast_uart_tx_udma_request(struct uart_8250_port *p, ast_uart_dma_cbfn_t rtn, void *id)
+{
+    return -1;
+}
+static inline int ast_uart_rx_udma_request(struct uart_8250_port *p, ast_uart_dma_cbfn_t rtn, void *id)
+{
+    return -1;
+}
+static inline int ast_uart_udma_init(struct uart_8250_port *p)
+{
+    return -1;
+}
 #endif
