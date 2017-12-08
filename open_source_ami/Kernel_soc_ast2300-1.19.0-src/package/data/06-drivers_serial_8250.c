diff -Naur linux-2.6.28.10/drivers/serial/8250.c linux-2.6.28.10-ami/drivers/serial/8250.c
--- linux-2.6.28.10/drivers/serial/8250.c	2009-05-03 02:54:43.000000000 +0800
+++ linux-2.6.28.10-ami/drivers/serial/8250.c	2010-05-10 14:15:36.000000000 +0800
@@ -1598,6 +1598,10 @@
 		i->head = &up->list;
 		spin_unlock_irq(&i->lock);
 
+		#ifdef CONFIG_ASTSERIAL
+		IRQ_SET_HIGH_LEVEL(up->port.irq);
+		IRQ_SET_LEVEL_TRIGGER(up->port.irq);
+		#endif
 		ret = request_irq(up->port.irq, serial8250_interrupt,
 				  irq_flags, "serial", i);
 		if (ret < 0)
@@ -2588,6 +2592,9 @@
 		up->port.uartclk  = old_serial_port[i].baud_base * 16;
 		up->port.flags    = old_serial_port[i].flags;
 		up->port.hub6     = old_serial_port[i].hub6;
+		#ifdef CONFIG_ASTSERIAL
+		up->port.mapbase  = old_serial_port[i].port;
+		#endif
 		up->port.membase  = old_serial_port[i].iomem_base;
 		up->port.iotype   = old_serial_port[i].io_type;
 		up->port.regshift = old_serial_port[i].iomem_reg_shift;
