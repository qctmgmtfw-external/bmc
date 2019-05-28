--- u-boot-2013.07/include/_exports.h	2013-12-13 13:49:25.780958498 -0500
+++ uboot.new/include/_exports.h	2013-12-05 12:17:55.243503948 -0500
@@ -22,11 +22,21 @@
 EXPORT_FUNC(strict_strtoul)
 EXPORT_FUNC(simple_strtol)
 EXPORT_FUNC(strcmp)
+#if defined(CONFIG_CMD_I2C)
+EXPORT_FUNC(i2c_probe)
 EXPORT_FUNC(i2c_write)
 EXPORT_FUNC(i2c_read)
+#endif
 EXPORT_FUNC(spi_init)
 EXPORT_FUNC(spi_setup_slave)
 EXPORT_FUNC(spi_free_slave)
 EXPORT_FUNC(spi_claim_bus)
 EXPORT_FUNC(spi_release_bus)
 EXPORT_FUNC(spi_xfer)
+#ifdef CONFIG_EXPORT_ETH_FNS
+EXPORT_FUNC(eth_init)
+EXPORT_FUNC(eth_halt)
+EXPORT_FUNC(eth_send)
+EXPORT_FUNC(eth_rx)
+EXPORT_FUNC(net_set_udp_handler)
+#endif
