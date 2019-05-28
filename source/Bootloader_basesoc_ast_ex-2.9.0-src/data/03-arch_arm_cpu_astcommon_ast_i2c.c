--- u-boot-2013.07/arch/arm/cpu/astcommon/ast_i2c.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/ast_i2c.c	2013-12-11 11:08:01.204598852 -0500
@@ -0,0 +1,327 @@
+#include <common.h>
+#include <i2c.h>
+#include "soc_hw.h"
+
+
+#define AST_I2C_REG_BASE			0x1E78A000 /* I2C controller registers base address */
+
+/* I2C device registers offset */
+#define I2C_DISR	AST_I2C_REG_BASE 			  /* I2C controller device interrupt status register */
+#define I2C_FCR		(i2c_bus_base_addr + 0x00) /* function control register */
+#define I2C_CATCR1	(i2c_bus_base_addr + 0x04) /* clock and AT timming control register 1 */
+#define I2C_CATCR2	(i2c_bus_base_addr + 0x08) /* clock and AT timming control register 2 */
+#define I2C_ICR		(i2c_bus_base_addr + 0x0C) /* interrupt control register */
+#define I2C_ISR		(i2c_bus_base_addr + 0x10) /* interrupt status register */
+#define I2C_CSR		(i2c_bus_base_addr + 0x14) /* command/status register */
+#define I2C_DR		(i2c_bus_base_addr + 0x18) /* device address register */
+#define I2C_BCR		(i2c_bus_base_addr + 0x1C) /* buffer control register */
+#define I2C_TRBB	(i2c_bus_base_addr + 0x20) /* transmit/receive byte buffer */
+#define I2C_DMCR	(i2c_bus_base_addr + 0x24) /* DMA mode control register */
+#define I2C_DMSR	(i2c_bus_base_addr + 0x28) /* DMA mode status register */
+
+
+
+
+/*----------------------------------------------------------------------------------------------*/
+/*------------------------------------  Common stuff for All SOC -------------------------------*/ 
+/*----------------------------------------------------------------------------------------------*/
+
+static void real_i2c_init (int speed, int slaveadd);
+static int i2c_init_pending= 1;
+
+static int i2c_bus_base_addr   = 0;
+static int i2c_bus_num = 0;
+
+#ifdef CONFIG_SYS_I2C_SLAVE
+static int i2c_bus_slaveaddr   = CONFIG_SYS_I2C_SLAVE;
+#else
+static int i2c_bus_slaveaddr   = 0x20;		/* Default BMC Address */
+#endif
+#ifdef CONFIG_SYS_I2C_SPEED
+static int i2c_bus_speed  = CONFIG_SYS_I2C_SPEED;
+#else
+static int i2c_bus_speed  = 100000;		/* Default 100Khz */
+#endif
+
+
+unsigned int i2c_get_bus_speed(void)
+{
+	return i2c_bus_speed;	
+}
+
+int 
+i2c_set_bus_speed(unsigned int speed)
+{
+	i2c_bus_speed = speed;
+	real_i2c_init(i2c_bus_speed,i2c_bus_slaveaddr);
+	return 0;
+}
+
+unsigned int i2c_get_bus_num(void)
+{
+	return i2c_bus_num;
+}
+
+int i2c_set_bus_num(unsigned int bus)
+{
+	i2c_bus_num = bus;
+	real_i2c_init(i2c_bus_speed,i2c_bus_slaveaddr);
+	return 0;
+}
+
+#if 0 /* Already defined in i2c.h */
+uchar i2c_reg_read(uchar i2c_addr, uchar reg)
+{
+	unsigned char buf;
+	i2c_read(i2c_addr, reg, 1, &buf, 1);
+	return buf;
+}
+
+void i2c_reg_write(uchar i2c_addr, uchar reg, uchar val)
+{
+	i2c_write(i2c_addr, reg, 1, &val, 1);
+	return;
+}
+#endif
+
+/*----------------------------------------------------------------------------------------------*/
+
+
+/*----------------------------------------------------------------------------------------------*/
+/* ---------------------  i2c interface functions (SOC Specific --------------------------------*/ 
+/*----------------------------------------------------------------------------------------------*/
+
+static int i2c_start(unsigned char chip, unsigned char read_write);
+static void i2c_stop(void);
+static unsigned long i2c_interrupt_status(void);
+static int i2c_send_byte(unsigned char byte);
+static int i2c_receive_byte(unsigned char *byte, int last);
+
+/* I2C operations */
+#define I2C_WRITE 0
+#define I2C_READ 1
+
+void i2c_init (int speed, int slaveadd)
+{
+	/* This is called before relocation and hence we cannot access global variables */
+	return;
+}
+
+static void real_i2c_init (int speed, int slaveadd)
+{
+	volatile unsigned long scu_reg;
+
+	/* Calculate bus address based on bus number */
+	if (i2c_bus_num  >= 7)   
+		i2c_bus_base_addr = 	(AST_I2C_REG_BASE + 0x100 + (0x040 * (i2c_bus_num + 1)));
+	else
+		i2c_bus_base_addr = 	(AST_I2C_REG_BASE + (0x040 * (i2c_bus_num + 1)));
+
+	*((volatile ulong *) SCU_KEY_CONTROL_REG) = 0x1688A8A8; /* unlock SCU */
+
+	if (i2c_bus_num >=13)
+	{
+		scu_reg = *((volatile ulong*) (AST_SCU_VA_BASE+ 0x90));
+		*((volatile ulong*) (AST_SCU_VA_BASE+ 0x90)) = scu_reg | (0x4000 << i2c_bus_num); /*enable multi function pin*/
+ 	}
+	else if (i2c_bus_num >=9)
+	{
+		scu_reg = *((volatile ulong*) (AST_SCU_VA_BASE+ 0x90));
+		*((volatile ulong*) (AST_SCU_VA_BASE+ 0x90)) = (scu_reg & ~1) | (0x4000 << i2c_bus_num); /*enable multi function pin*/
+	}
+	else if (i2c_bus_num >=2)
+	{
+		scu_reg = *((volatile ulong*) (AST_SCU_VA_BASE+ 0x90));
+		*((volatile ulong*) (AST_SCU_VA_BASE+ 0x90)) = scu_reg | (0x4000 << i2c_bus_num); /*enable multi function pin*/
+ 	}
+
+	/* stop I2C controller reset */
+	scu_reg = *((volatile ulong *) SCU_SYS_RESET_REG);
+	*((volatile ulong *) SCU_SYS_RESET_REG) = scu_reg & ~(0x04);
+
+//	*((volatile ulong *) SCU_KEY_CONTROL_REG) = 0; /* lock SCU */
+
+    /* Set Clock and AC timing Control Register 1 and 2 */
+    /* the divisor is 256 */
+	*((volatile unsigned long *) I2C_CATCR1) = 0x77777344;
+	*((volatile unsigned long *) I2C_CATCR2) = 0x00000000;
+
+    /* Set Slave address */
+    *((volatile unsigned long *) I2C_DR) = slaveadd;
+
+    /* Clear Interrupt Status register */
+    *((volatile unsigned long *) I2C_ISR) = 0xFFFFFFFF; /* write 1 to clear bits */
+
+    /* Set Master or Slave mode */
+    *((volatile unsigned long *) I2C_FCR) = 0x01; /* master function enable only */
+
+    /* Set Interrupt Control register */
+	*((volatile unsigned long *) I2C_ICR) = 0x0000002F;
+
+	i2c_init_pending=0;
+}
+
+int i2c_probe(uchar chip)
+{
+	int ret;
+
+	real_i2c_init(i2c_bus_speed,i2c_bus_slaveaddr);
+
+	ret = 0;
+	if (i2c_start(chip, I2C_WRITE)) /* Do not use I2C_READ or the bus will be blocked by salve */
+		ret = 1;
+
+	i2c_stop();
+
+	return ret;
+}
+
+int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
+{
+	int i;
+	int last;
+	int ret;
+
+	ret = 0;
+
+	if (i2c_init_pending)
+		real_i2c_init(i2c_bus_speed,i2c_bus_slaveaddr);
+
+	if (i2c_start(chip, I2C_WRITE))
+		return 1;
+
+	for (i = 0; i < alen; i ++) 
+	{
+		if (i2c_send_byte((addr >> ((alen -1 - i) * 8)) & 0xFF)) 
+		{
+			return 1;
+		}
+	}
+
+	if (i2c_start(chip, I2C_READ))
+		return 1;
+
+	for (i = 0; i < len; i ++) 
+	{
+		last = (i < (len - 1)) ? 0 : 1;
+		if (i2c_receive_byte(&(buffer[i]), last)) 
+		{
+			ret = 1;
+		}
+	}
+
+	i2c_stop();
+
+	return ret;
+}
+
+int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
+{
+	int i;
+	//int last;
+	int ret;
+
+	ret = 0;
+
+	if (i2c_init_pending)
+		real_i2c_init(i2c_bus_speed,i2c_bus_slaveaddr);
+
+	if (i2c_start(chip, I2C_WRITE))
+		return 1;
+
+	for (i = 0; i < alen; i ++) 
+	{
+		if (i2c_send_byte((addr >> ((alen -1 - i) * 8)) & 0xFF)) 
+		{
+			return 1;
+		}
+	}
+
+	for (i = 0; i < len; i ++) 
+	{
+		//last = (i < (len - 1)) ? 0 : 1;
+		if (i2c_send_byte(buffer[i])) 
+		{
+			ret = 1;
+		}
+	}
+
+	i2c_stop();
+
+	return ret;
+}
+
+/*----------------------------------------------------------------------------------------------*/
+/* ---------------------  i2c internal fuctions for SOC         --------------------------------*/ 
+/*----------------------------------------------------------------------------------------------*/
+static int i2c_send_byte(unsigned char byte)
+{
+	*((volatile unsigned long *) I2C_TRBB) = byte & 0x000000FF;
+	*((volatile unsigned long *) I2C_CSR) = 0x02; /* master transmit command */
+
+	if (!(i2c_interrupt_status() & 0x01)) { /* transmit done with ACK returned */
+		return 1;
+	}
+
+    return 0;
+}
+
+static int i2c_receive_byte(unsigned char *byte, int last)
+{
+	*((volatile unsigned long *) I2C_CSR) = 0x08 | (last << 4); /* master receive command, master/slave receive command last */
+
+	if (!(i2c_interrupt_status() & 0x04)) { /* receive done interrupt */
+		return 1;
+	}
+
+	*byte = (unsigned char) ((*((volatile unsigned long *) I2C_TRBB) & 0x0000FF00) >> 8);
+
+	return 0;
+}
+
+static int i2c_start(unsigned char chip, unsigned char read_write)
+{
+	*((volatile unsigned long *) I2C_TRBB) = (chip  << 1) | (read_write & 0x1); /* load address to buffer and specify read/write */
+	*((volatile unsigned long *) I2C_CSR) = 0x03; /* master start command, master transmit command */
+
+	if (i2c_interrupt_status() & 0x01) { /* transmit done with ACK returned interrupt */
+		return 0;
+	}
+
+	return 1;
+}
+
+static void i2c_stop(void)
+{
+	*((volatile unsigned long *) I2C_CSR) = 0x0030; /* master stop command */
+}
+
+static unsigned long i2c_interrupt_status(void)
+{
+	unsigned long isr; /* device interrupt status register */
+	unsigned long device_bit;
+	unsigned long interrup_status;
+	long i;
+
+	interrup_status = 0;
+	
+	for (i = 0; i < 100000; i ++) { /* poll device interrupt status */
+		isr = *((volatile unsigned long *) I2C_DISR);
+
+		device_bit = 0x01 << i2c_bus_num;
+		if ((isr & device_bit) != 0) { /* interrupt occurs */
+			interrup_status = *((volatile unsigned long *) I2C_ISR);
+			break;
+		}
+	}
+
+	#if 0
+	if (i == 100000)
+		printf("i2c time out! ISR = 0x%X\n", *((volatile unsigned long *) I2C_ISR]);
+	#endif
+
+	*((volatile unsigned long *) I2C_ISR) = 0xFFFFFFFF; /* clear bits */
+
+	return interrup_status;
+}
+
