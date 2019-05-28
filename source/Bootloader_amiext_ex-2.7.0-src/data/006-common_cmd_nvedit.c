--- u-boot-2013.07/common/cmd_nvedit.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/cmd_nvedit.c	2013-12-05 12:17:55.223503948 -0500
@@ -55,6 +55,7 @@
 #if	!defined(CONFIG_ENV_IS_IN_EEPROM)	&& \
 	!defined(CONFIG_ENV_IS_IN_FLASH)	&& \
 	!defined(CONFIG_ENV_IS_IN_DATAFLASH)	&& \
+	!defined(CONFIG_ENV_IS_IN_SPI)		&& \
 	!defined(CONFIG_ENV_IS_IN_MMC)		&& \
 	!defined(CONFIG_ENV_IS_IN_FAT)		&& \
 	!defined(CONFIG_ENV_IS_IN_NAND)		&& \
@@ -65,7 +66,7 @@
 	!defined(CONFIG_ENV_IS_IN_UBI)		&& \
 	!defined(CONFIG_ENV_IS_NOWHERE)
 # error Define one of CONFIG_ENV_IS_IN_{EEPROM|FLASH|DATAFLASH|ONENAND|\
-SPI_FLASH|NVRAM|MMC|FAT|REMOTE|UBI} or CONFIG_ENV_IS_NOWHERE
+SPI_FLASH|NVRAM|MMC|FAT|REMOTE|UBI|SPI} or CONFIG_ENV_IS_NOWHERE
 #endif
 
 /*
