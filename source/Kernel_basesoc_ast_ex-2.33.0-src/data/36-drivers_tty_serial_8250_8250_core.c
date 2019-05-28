--- linux-3.14.17/drivers/tty/serial/8250/8250_core.c	2014-08-14 09:38:34.000000000 +0800
+++ linux-3.14.17-ami/drivers/tty/serial/8250/8250_core.c	2015-05-12 15:29:25.277211158 +0800
@@ -325,6 +325,10 @@
 	},
 };
 
+#if defined(CONFIG_SERIAL_8250_DMA) && defined(CONFIG_ASTSERIAL)
+static struct uart_8250_port *serial8250_find_match_or_unused(struct uart_port *port);
+#endif
+
 /* Uart divisor latch read */
 static int default_serial_dl_read(struct uart_8250_port *up)
 {
@@ -1513,8 +1517,9 @@
 	DEBUG_INTR("status = %x...", status);
 
 	if (status & (UART_LSR_DR | UART_LSR_BI)) {
-		if (up->dma)
+		if (up->dma) {
 			dma_err = serial8250_rx_dma(up, iir);
+        }
 
 		if (!up->dma || dma_err)
 			status = serial8250_rx_chars(up, status);
@@ -1595,8 +1600,9 @@
 		if (port->handle_irq(port)) {
 			handled = 1;
 			end = NULL;
-		} else if (end == NULL)
+		} else if (end == NULL) {
 			end = l;
+        }
 
 		l = l->next;
 
@@ -1683,6 +1689,10 @@
 		i->head = &up->list;
 		spin_unlock_irq(&i->lock);
 		irq_flags |= up->port.irqflags;
+#ifdef CONFIG_ASTSERIAL
+        IRQ_SET_HIGH_LEVEL(up->port.irq);
+        IRQ_SET_LEVEL_TRIGGER(up->port.irq);
+#endif
 		ret = request_irq(up->port.irq, serial8250_interrupt,
 				  irq_flags, "serial", i);
 		if (ret < 0)
@@ -2177,7 +2187,10 @@
 	 * are set via set_termios(), which will be occurring imminently
 	 * anyway, so we don't enable them here.
 	 */
-	up->ier = UART_IER_RLSI | UART_IER_RDI;
+	if (!up->dma)
+        up->ier = UART_IER_RLSI | UART_IER_RDI;
+    else
+        up->ier = 0;
 	serial_port_out(port, UART_IER, up->ier);
 
 	if (port->flags & UPF_FOURPORT) {
@@ -2811,11 +2824,18 @@
 		port->uartclk  = old_serial_port[i].baud_base * 16;
 		port->flags    = old_serial_port[i].flags;
 		port->hub6     = old_serial_port[i].hub6;
+#ifdef CONFIG_ASTSERIAL
+        up->port.mapbase  = old_serial_port[i].port;
+#endif
 		port->membase  = old_serial_port[i].iomem_base;
 		port->iotype   = old_serial_port[i].io_type;
 		port->regshift = old_serial_port[i].iomem_reg_shift;
 		set_io_from_upio(port);
 		port->irqflags |= irqflag;
+#if defined(CONFIG_SERIAL_8250_DMA) && defined(CONFIG_ASTSERIAL)
+        serial8250_dma_config(up);
+#endif
+
 		if (serial8250_isa_config != NULL)
 			serial8250_isa_config(i, &up->port, &up->capabilities);
 
@@ -3121,8 +3141,47 @@
 				p->iobase, (unsigned long long)p->mapbase,
 				p->irq, ret);
 		}
-	}
+
+#if defined(CONFIG_SERIAL_8250_DMA) && defined(CONFIG_ASTSERIAL)
+    }
+
+    for (i = 0; i < nr_uarts; i++) {
+        struct uart_8250_port *p=&serial8250_ports[i];
+        if (p->dma) {
+
+            tasklet_init(&p->dma->rx.tasklet, ast_uart_rx_udma_tasklet_func,
+                    (unsigned long)p);
+
+            ret = ast_uart_rx_udma_request(p, ast_uart_rx_buffdone, &serial8250_ports[i]);
+            if (ret < 0) {
+                printk("Error : failed to get rx dma channel[%d]\n", p->port.line);
+                goto out_ast_uart_unregister_port;
+            }
+
+            ret = ast_uart_tx_udma_request(p, ast_uart_tx_buffdone, &serial8250_ports[i]);
+            if (ret < 0) {
+                printk("Error : failed to get tx dma channel[%d]\n", p->port.line);
+                return ret;
+            }
+        }
+    }
+
+	return 0;
+
+out_ast_uart_unregister_port:
+    for (i = 0; i < nr_uarts; i++) {
+        struct uart_8250_port *up = &serial8250_ports[i];
+
+        if (up->port.dev == &dev->dev)
+            serial8250_unregister_port(i);
+    };
+    return ret;
+#else
+
+    }
 	return 0;
+#endif
+
 }
 
 /*
@@ -3308,6 +3367,7 @@
 		ret = uart_add_one_port(&serial8250_reg, &uart->port);
 		if (ret == 0)
 			ret = uart->port.line;
+
 	}
 	mutex_unlock(&serial_mutex);
 
@@ -3346,6 +3406,10 @@
 	int ret;
 
 	serial8250_isa_init_ports();
+#if defined(CONFIG_SERIAL_8250_DMA) && defined(CONFIG_ASTSERIAL)
+    ast_uart_udma_init(serial8250_ports);
+#endif
+
 
 	printk(KERN_INFO "Serial: 8250/16550 driver, "
 		"%d ports, IRQ sharing %sabled\n", nr_uarts,
