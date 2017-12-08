/*****************************************************************
*	File		:	imagereader.h								 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Defines routines for presenting				 *
*					cdrom image as a cdrom device during		 *
*					redirection									 *
******************************************************************/

/**
 * @file   imagereader.h
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   18-June-2003
 *
 * @brief  Defines the cdrom redirection routines.
 * The routines in this section are used to
 * present a ISO9660 CDROM image file as a Virtual
 * CDROM to the managed server. All the commands from
 * the managed server are answered by the taking the
 * information from the CDROM image file. The CDROM image
 * can be anywhere either on a harddisk or in a network.<br>
 * Not All the routines specified in the section are open
 * to the user. There are only some routines which are open
 * to the user.
 *
 *	<LI> \ref OpenCDROMImage </LI>
 *	<LI> \ref CloseCDROMImage </LI>
 *	<LI> \ref ExecuteSCSICmd </LI>
 * <BR>
 * User must call \ref OpenCDROMImage to open the cdrom image file
 * and then \ref ExecuteSCSICmd to carry out any transactions with image
 * file. Once the transactions are completed he must call CloseCDROMImage
 * to end all transactions with the image file.
 */

# ifndef __IMAGEREADER_H__
#define __IMAGEREADER_H__

#include "coreTypes.h"
#include "libmacro.h"
#include "iusb.h"
#include "scsi.h"

#include <stdio.h>

/**
* @brief refers the CDROM image file.
* It contains the full path of image file and image attributes
*/
typedef struct tagCDROMIMAGE {

	/** full path of cdimage file */
	char		szImageFile[MAX_DEVICE_PATH_SIZE+1];
	/** file pointer for cdimage file */
	//FILE 		*pImageFile;

	int fdImageFile;
#if defined (__x86_64__) || defined (WIN64)
	/** Total sectors for cdimage file */
	unsigned int	TotalSectors;
	/** size of each sector in cdimage file */
	unsigned int	BlockSize;
#else
	/** Total sectors for cdimage file */
	unsigned long	TotalSectors;
	/** size of each sector in cdimage file */
	unsigned long	BlockSize;
#endif
	/** byte order used by MegaRAC card */
	enum byteOrder	ByteOrder;
	/** used internally to intimate status to the host */
	unsigned char	bFirstTime;
	/** identify whether OpenCDROMDevice is used before any transaction */
	unsigned char	bIsOpened;

	unsigned char IndicateMediaChange;

}CDROMIMAGE, *pCDROMIMAGE;

#ifdef __cplusplus
extern "C" {
#endif

/** @name API for redirecting CDROM Image */
/** @{ */
/**
* @brief Opens the CDROM image.
* Opens the cdrom image file specified by \a pszImageFilePath. It
* opens the cdrom image file and validates it by identifying the
* signature in predefined location(refer ISO9660 document). Once
* the validation is completed, it fills attributes in \a pCDImage
* required for later transactions with the CDROM image. The user
* can use ExecuteSCSICmd only if this fucntion returns SUCCESS.
*
* @param pszImageFilePath Full path of the cdrom image file. It should not exceed \ref MAX_DEVICE_PATH_SIZE.
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
*
* @retval SUCCESS upon successful completion
* @retval DEVICE_ALREADY_OPEN if the device is already in open state.
* @retval DEVICE_PATH_TOO_LONG if the length of path string exceeds \ref MAX_DEVICE_PATH_SIZE.
* @retval WRONG_MEDIA if the image file is not a valid ISO9660 image file.
* @retval NO_MEDIA if no file exists in specified location.
* @retval MEDIA_ERROR if invalid file
* @retval UNREADABLE_MEDIA if image file is unreadable.
* @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
*/
int	OpenCDROMImage(char *pszImageFilePath, CDROMIMAGE *pCDImage);

/**
* @brief Closes the CDROM Image.
* Closes the CDROM Image so that no other transactions
* can be made there after.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
*
* @retval SUCCESS upon successful completion
*/
int	CloseCDROMImage(CDROMIMAGE *pCDImage);

/**
* @brief Identifies the byteorder of system from where the transactions commands are originated.
* By default All the API uses the default byte order. This will be useful to specify the byteorder
* of the MegaRAC card(from where the transactions commands are originated).
* while using these API from a Client machine to redirect the CDROM.
* MegaRAC-G2 uses bigendian format. If these API are used in MegaRAC card itself,
* then it is not required to set the byteorder because it uses the default byte order.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
* @param ByteOrder byteorder of the system from where the transactions will be orginated.
*
* @retval SUCCESS upon successful completion
*/
void	SetByteOrder(enum byteOrder ByteOrder, CDROMIMAGE *pCDImage );

/**
* @brief Executes the transaction command.
* Takes the command packet, executes the command and fills the response packet
* with the result. It returns the size of the data if any attached to the response packet.
* This size is used while sending the response packet to the originating system.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
* @param pRequestPkt the command request packet
* @param pResponsePkt the response packet filled with the result data after execution of the command.
* @param dwDataLen refers the length of the data if any added to the response packet.
*
* @retval SUCCESS upon successful completion
* @retval DEVICE_ERROR if this function is used before calling OpenCDROMImage.
* @retval WRONG_MEDIA if the image file is not a valid ISO9660 image file.
* @retval NO_MEDIA if no file exists in specified location.
* @retval MEDIA_ERROR if invalid file
* @retval UNREADABLE_MEDIA if image file is unreadable.
* @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
*/
#if defined (__x86_64__) || defined(WIN64)
int	ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned int *dwDataLen, CDROMIMAGE *pCDImage);
#else
int	ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned long *dwDataLen, CDROMIMAGE *pCDImage);
#endif
/** @} */

#ifdef __cplusplus
	}
#endif

#endif


