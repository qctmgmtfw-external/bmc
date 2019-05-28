--- uboot_old/oem/ami/fmh/fmhcore.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot/oem/ami/fmh/fmhcore.c	2014-12-18 10:33:16.085771521 -0500
@@ -0,0 +1,276 @@
+#ifdef __KERNEL__
+	#ifdef __UBOOT__
+		#include <common.h>
+	#else
+		#include <linux/kernel.h>
+		#include <linux/string.h>
+	#endif
+#else
+	#include <stdio.h>
+	#include <string.h>
+#endif
+#include <oem/ami/fmh/fmh.h>
+#include "crc32.h"
+
+
+FMH fmh_buffer;
+ALT_FMH altfmh_buffer;
+
+static unsigned char  CalculateModule100(unsigned char *Buffer, unsigned long Size);
+static FMH * CheckForNormalFMH(FMH *fmh);
+static unsigned long  CheckForAlternateFMH(ALT_FMH *altfmh);
+
+static
+unsigned char 
+CalculateModule100(unsigned char *Buffer, unsigned long Size)
+{
+	unsigned char Sum=0;
+
+	while (Size--)
+	{
+		Sum+=(*Buffer);
+		Buffer++;
+	}	
+
+	return (~Sum)+1;
+}
+
+static
+unsigned char 
+ValidateModule100(unsigned char *Buffer, unsigned long Size)
+{
+	unsigned char Sum=0;
+
+	while (Size--)
+	{
+		Sum+=(*Buffer);
+		Buffer++;
+	}
+
+	return Sum;
+}
+
+static
+FMH *
+CheckForNormalFMH(FMH *fmh)
+{
+	if (strncmp((char *)fmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
+			return NULL;
+
+	if (le16_to_host(fmh->FMH_End_Signature) != FMH_END_SIGNATURE)
+			return NULL;
+
+	if (ValidateModule100((unsigned char *)fmh,sizeof(FMH)) != 0)
+			return NULL;
+	
+	return fmh;
+			
+}
+
+static
+unsigned long 
+CheckForAlternateFMH(ALT_FMH *altfmh)
+{
+
+	if (strncmp((char *)altfmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
+			return INVALID_FMH_OFFSET;
+
+	if (le16_to_host(altfmh->FMH_End_Signature) != FMH_END_SIGNATURE)
+			return INVALID_FMH_OFFSET;
+
+	if (ValidateModule100((unsigned char *)altfmh,sizeof(ALT_FMH)) != 0)
+			return INVALID_FMH_OFFSET;
+	
+	return le32_to_host(altfmh->FMH_Link_Address);
+
+}
+
+
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+/*
+ * Check For Alternate FMH in RAM
+ */
+static
+unsigned long
+CheckForAlternateFMH_RAM(ALT_FMH *altfmh)
+{
+
+	if (strncmp((char *)altfmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
+		return INVALID_FMH_OFFSET;
+
+	if (le16_to_host(altfmh->FMH_End_Signature) != FMH_END_SIGNATURE)
+		return INVALID_FMH_OFFSET;
+
+	if (ValidateModule100((unsigned char *)altfmh,sizeof(ALT_FMH)) != 0)
+		return INVALID_FMH_OFFSET;
+
+	return le32_to_host(altfmh->FMH_Link_Address);
+
+}
+
+
+/*
+ * Check For Normal FMH in RAM
+ */
+static
+FMH *
+CheckForNormalFMH_RAM(FMH *fmh)
+{
+
+
+	if (strncmp((char *)fmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
+		return NULL;
+
+
+	if (le16_to_host(fmh->FMH_End_Signature) != FMH_END_SIGNATURE)
+		return NULL;
+
+
+	if (ValidateModule100((unsigned char *)fmh,sizeof(FMH)) != 0)
+		return NULL;
+
+
+	return fmh;
+
+}
+
+/*
+ * Scan for FMH in the RAM
+ */
+FMH*
+ScanforFMH_RAM(unsigned char *SectorAddr, unsigned long SectorSize)
+{
+	FMH *fmh;
+	ALT_FMH *altfmh;
+	unsigned long FMH_Offset;
+
+
+	/* Check if Normal FMH is found */
+	fmh = (FMH *)SectorAddr;
+	fmh = CheckForNormalFMH_RAM(fmh);
+	if (fmh != NULL)
+	{
+		return fmh;
+	}
+
+	/* If Normal FMH is not found, check for alternate FMH */
+	altfmh = (ALT_FMH *)(SectorAddr+SectorSize - sizeof(ALT_FMH));
+	FMH_Offset = CheckForAlternateFMH_RAM(altfmh);
+	if (FMH_Offset == INVALID_FMH_OFFSET)
+	{
+		return NULL;
+	}
+	fmh = (FMH *)(SectorAddr +FMH_Offset);
+
+	/* If alternate FMH is found, validate it */
+	fmh = CheckForNormalFMH_RAM(fmh);
+	return fmh;
+}
+#endif
+
+FMH *
+ScanforFMH(unsigned char *SectorAddr, unsigned long SectorSize)
+{
+	FMH *fmh;
+	ALT_FMH *altfmh;
+	unsigned long FMH_Offset;
+
+	/* Check if Normal FMH is found */
+	flash_read((ulong)SectorAddr,sizeof(FMH),(char*)&fmh_buffer);
+	fmh = (FMH *)(&fmh_buffer);
+	fmh = CheckForNormalFMH(fmh);
+	if (fmh != NULL)
+		return fmh;
+
+	/* If Normal FMH is not found, check for alternate FMH */
+	flash_read((ulong)SectorAddr+SectorSize - sizeof(ALT_FMH),sizeof(ALT_FMH),(char *)&altfmh_buffer);
+	altfmh = (ALT_FMH*)&altfmh_buffer;
+	FMH_Offset = CheckForAlternateFMH(altfmh);
+	if (FMH_Offset == INVALID_FMH_OFFSET)
+		return NULL;
+
+	flash_read((ulong)CONFIG_SYS_FLASH_BASE+FMH_Offset,sizeof(FMH),(char *)&fmh_buffer);	
+	fmh = (FMH*)&fmh_buffer;
+	
+	/* If alternate FMH is found, validate it */
+	fmh = CheckForNormalFMH(fmh);
+	return fmh;
+}
+
+void
+CreateFMH(FMH *fmh,unsigned long AllocatedSize, MODULE_INFO *mod)
+{
+	/* Clear the Structure */	
+	memset((void *)fmh,0,sizeof(FMH));
+
+	/* Copy the module information */
+	memcpy((void *)&(fmh->Module_Info),(void *)mod,sizeof(MODULE_INFO));
+					
+	/* Fill the FMH Fields */		
+	strncpy((char *)fmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1);
+	fmh->FMH_Ver_Major 		= FMH_MAJOR;
+	fmh->FMH_Ver_Minor 		= FMH_MINOR;
+	fmh->FMH_Size	   		= FMH_SIZE;
+	fmh->FMH_End_Signature	= host_to_le16(FMH_END_SIGNATURE);
+	
+	fmh->FMH_AllocatedSize	= host_to_le32(AllocatedSize);
+
+	/*Calculate Header Checksum*/
+	fmh->FMH_Header_Checksum = CalculateModule100((unsigned char *)fmh,sizeof(FMH));
+		
+	return;
+}
+
+void
+CreateAlternateFMH(ALT_FMH *altfmh,unsigned long FMH_Offset) 
+{
+	/* Clear the Structure */	
+	memset((void *)altfmh,0,sizeof(ALT_FMH));
+					
+	/* Fill the FMH Fields */		
+	strncpy((char *)altfmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1);
+	altfmh->FMH_End_Signature	= host_to_le16(FMH_END_SIGNATURE);
+	
+	altfmh->FMH_Link_Address	= host_to_le32(FMH_Offset);
+
+	/*Calculate Header Checksum*/
+	altfmh->FMH_Header_Checksum = CalculateModule100((unsigned char *)altfmh,
+										sizeof(ALT_FMH));
+	return;
+}
+
+
+
+unsigned long
+CalculateCRC32(unsigned char *Buffer, unsigned long Size)
+{
+    unsigned long i,crc32 = 0xFFFFFFFF;
+
+	/* Read the data and calculate crc32 */	
+    for(i = 0; i < Size; i++)
+		crc32 = ((crc32) >> 8) ^ CrcLookUpTable[(Buffer[i]) 
+								^ ((crc32) & 0x000000FF)];
+	return ~crc32;
+}
+
+void
+BeginCRC32(unsigned long *crc32)
+{
+	*crc32 = 0xFFFFFFFF;
+	return;
+}
+
+void
+DoCRC32(unsigned long *crc32, unsigned char Data)
+{
+	*crc32=((*crc32) >> 8) ^ CrcLookUpTable[Data ^ ((*crc32) & 0x000000FF)];
+	return;
+}
+
+void
+EndCRC32(unsigned long *crc32)
+{
+	*crc32 = ~(*crc32);
+	return;
+}
+
