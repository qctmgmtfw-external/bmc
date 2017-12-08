/*****************************************************************
*	File		:	harddiskimagereader.h								 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Defines routines for presenting				 *
*					Harddisk image as a Harddisk device during		 *
*					redirection									 *
******************************************************************/

/**
 * @file   harddiskimagereader.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   18-July-2003
 *
 * @brief  Defines the Harddisk redirection routines.
 * The routines in this section are used to
 * present a Harddisk image file as a Virtual
 * Harddisk to the managed server. All the commands from
 * the managed server are answered by the taking the
 * information from the Harddisk image file. The Harddisk image
 * can be anywhere either on a harddisk or in a network.<br>
 * Not All the routines specified in the section are open
 * to the user. There are only some routines which are open
 * to the user.
 *
 *	<LI> \ref OpenHarddiskImage </LI>
 *	<LI> \ref CloseHarddiskImage </LI>
 *	<LI> \ref ExecuteHarddiskSCSICmd </LI>
 * <BR>
 * User must call \ref OpenHarddiskImage to open the Harddisk image file
 * and then \ref ExecuteSCSICmd to carry out any transactions with image
 * file. Once the transactions are completed he must call CloseHarddiskImage
 * to end all transactions with the image file.
 */

#ifndef __HARDDISK_IMAGE_READER_H__
#define __HARDDISK_IMAGE_READER_H__

#include "coreTypes.h"
#include "libmacro.h"
#include "iusb.h"

#include <stdio.h>

/**
* @brief refers the HARDDISK image file.
* It contains the full path of image file and image attributes
*/
typedef struct tagHARDDISKIMAGE {

	/** full path of Harddiskimage file */
	char		szImageFile[MAX_DEVICE_PATH_SIZE+1];
	/** file pointer for Harddiskimage file */
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
	/** identify whether OpenHARDDISKDevice is used before any transaction */
	unsigned char	bIsOpened;
	/** image is readonly */
	unsigned char	bIsReadOnly;
	bool	bIsPhysicalDrive;

}HARDDISKIMAGE, *pHARDDISKIMAGE;

#define HARDDISK_BLOCK_SIZE (512)
#define HARDDISK_TOTAL_BLOCKS (2880)

typedef struct
{
	uint8	Reserved[3];
	uint8	ListLength;
	uint32	TotalBlocks;
	uint32	BlockSize;
} HD_READ_FORMAT_CAPACITIES;

#define MAX_HARDDISK_READ_SECTORS   	(256)
#define MAX_HARDDISK_READ_SIZE		(512*MAX_HARDDISK_READ_SECTORS)

#ifdef __cplusplus
extern "C" {
#endif

int	OpenHarddiskImage(char *pszImageFilePath, HARDDISKIMAGE *pFDImage);
int	CloseHarddiskImage(HARDDISKIMAGE *pFDImage);
void	SetHarddiskByteOrder(enum byteOrder ByteOrder, HARDDISKIMAGE *pFDImage );
void	SetHDImageReadOnly(bool status, HARDDISKIMAGE *pFDImage ); /* true or false*/
#if defined (__x86_64__) || defined (WIN64)
int	ExecuteHarddiskImageSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt,
			unsigned int *dwDataLen, HARDDISKIMAGE *pFDImage);
#else
int	ExecuteHarddiskImageSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt,
			unsigned long *dwDataLen, HARDDISKIMAGE *pFDImage);
#endif

int	transferharddiskimage(char *pszDevice, char *pszImageFile);
int	createharddiskimage(char *pszDevice, char *pszImageFile);
int IsHDFSFAT16(unsigned char *data);
int IsHDFSFAT32(unsigned char *data);

#ifdef __cplusplus
	}
#endif

#endif
