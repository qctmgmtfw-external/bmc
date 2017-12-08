/*****************************************************************
*	File		:	floppyimagereader.h								 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Defines routines for presenting				 *
*					Floppy image as a Floppy device during		 *
*					redirection									 *
******************************************************************/

/**
 * @file   floppyimagereader.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   18-July-2003
 *
 * @brief  Defines the Floppy redirection routines.
 * The routines in this section are used to
 * present a Floppy image file as a Virtual
 * Floppy to the managed server. All the commands from
 * the managed server are answered by the taking the
 * information from the Floppy image file. The Floppy image
 * can be anywhere either on a harddisk or in a network.<br>
 * Not All the routines specified in the section are open
 * to the user. There are only some routines which are open
 * to the user.
 *
 *	<LI> \ref OpenFloppyImage </LI>
 *	<LI> \ref CloseFloppyImage </LI>
 *	<LI> \ref ExecuteFloppySCSICmd </LI>
 * <BR>
 * User must call \ref OpenFloppyImage to open the Floppy image file
 * and then \ref ExecuteSCSICmd to carry out any transactions with image
 * file. Once the transactions are completed he must call CloseFloppyImage
 * to end all transactions with the image file.
 */

#ifndef __FLOPPY_IMAGE_READER_H__
#define __FLOPPY_IMAGE_READER_H__

#include "coreTypes.h"
#include "libmacro.h"
#include "iusb.h"

#include <stdio.h>

/**
* @brief refers the FLOPPY image file.
* It contains the full path of image file and image attributes
*/
typedef struct tagFLOPPYIMAGE {

	/** full path of Floppyimage file */
	char		szImageFile[MAX_DEVICE_PATH_SIZE+1];
	/** file pointer for Floppyimage file */
	FILE 		*pImageFile;
	/** byte order used by MegaRAC card */
	enum byteOrder	ByteOrder;

#if defined (__x86_64__) || defined (WIN64)
	/** Size of each sectot */
	unsigned int	BlockSize;
	/** Total number of sectors */
	unsigned int	TotalSectors;
#else
	/** Size of each sectot */
	unsigned long	BlockSize;
	/** Total number of sectors */
	unsigned long	TotalSectors;
#endif	
	/** used internally to intimate status to the host */
	unsigned char	bFirstTime;
	/** identify whether OpenFLOPPYDevice is used before any transaction */
	unsigned char	bIsOpened;
	/** image is readonly */
	unsigned char	bIsReadOnly;

}FLOPPYIMAGE, *pFLOPPYIMAGE;

#define FLOPPY_BLOCK_SIZE (512)
#define FLOPPY_TOTAL_BLOCKS (2880)

typedef struct
{
	uint8	Reserved[3];
	uint8	ListLength;
	uint32	TotalBlocks;
	uint32	BlockSize;
} READ_FORMAT_CAPACITIES;

#define MAX_FLOPPY_READ_SECTORS   	(256)
#define MAX_FLOPPY_READ_SIZE		(512*MAX_FLOPPY_READ_SECTORS)

#ifdef __cplusplus
extern "C" {
#endif

int	OpenFloppyImage(char *pszImageFilePath, FLOPPYIMAGE *pFDImage);
int	CloseFloppyImage(FLOPPYIMAGE *pFDImage);
void	SetFloppyByteOrder(enum byteOrder ByteOrder, FLOPPYIMAGE *pFDImage );
void	SetImageReadOnly(bool status, FLOPPYIMAGE *pFDImage ); /* true or false*/
#if defined (__x86_64__) || defined (WIN64)
int	ExecuteFloppyImageSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt,
			unsigned int *dwDataLen, FLOPPYIMAGE *pFDImage);
#else
int	ExecuteFloppyImageSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt,
			unsigned long *dwDataLen, FLOPPYIMAGE *pFDImage);
#endif

int	transferfloppyimage(char *pszDevice, char *pszImageFile);
int	createfloppyimage(char *pszDevice, char *pszImageFile);
int IsFSFAT16(unsigned char *data);
int IsFSFAT32(unsigned char *data);

#ifdef __cplusplus
	}
#endif

#endif
