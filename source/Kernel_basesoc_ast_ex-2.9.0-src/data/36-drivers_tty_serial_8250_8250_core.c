--- linux-3.14.17/drivers/tty/serial/8250/8250_core.c	2014-12-03 16:07:13.545519966 +0800
+++ linux-3.14.17-ami/drivers/tty/serial/8250/8250_core.c	2014-12-17 18:23:42.266920290 +0800
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
@@ -1683,6 +1687,10 @@
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
@@ -2811,11 +2819,18 @@
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
 
@@ -3121,8 +3136,50 @@
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
+            tasklet_init(&p->dma->tx.tasklet, ast_uart_tx_udma_tasklet_func,
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
 	return 0;
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
+	return 0;
+#endif
+
 }
 
 /*
@@ -3308,6 +3365,7 @@
 		ret = uart_add_one_port(&serial8250_reg, &uart->port);
 		if (ret == 0)
 			ret = uart->port.line;
+
 	}
 	mutex_unlock(&serial_mutex);
 
@@ -3346,6 +3404,10 @@
 	int ret;
 
 	serial8250_isa_init_ports();
+#if defined(CONFIG_SERIAL_8250_DMA) && defined(CONFIG_ASTSERIAL)
+    ast_uart_udma_init(serial8250_ports);
+#endif
+
 
 	printk(KERN_INFO "Serial: 8250/16550 driver, "
 		"%d ports, IRQ sharing %sabled\n", nr_uarts,
