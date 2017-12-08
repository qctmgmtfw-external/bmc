/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

#ifndef AMI_COMPRESS_H
#define AMI_COMPRESS_H

/* Soft compression types */
#define SOFT_COMPRESSION_NONE 			0x00
#define SOFT_COMPRESSION_MINILZO		0x01
#define SOFT_COMPRESSION_GZIP			0x02
#define SOFT_COMPRESSION_BZIP2			0x03
#define SOFT_COMPRESSION_QLZW			0x04
#define SOFT_COMPRESSION_QLZW_MINILZO		0x05
#define SOFT_COMPRESSION_RLE			0x06
#define SOFT_COMPRESSION_RLE_QLZW		0x07
#define SOFT_COMPRESSION_RLE_PIII		0x08
#define SOFT_COMPRESSION_NONE_PIII		0x0A

int InitCompression(void);
unsigned long DoCompression(unsigned char *InBuf, unsigned long InSize, unsigned char *OutBuf, unsigned long OutSize);

#endif /* AMI_COMPRESS_H */
