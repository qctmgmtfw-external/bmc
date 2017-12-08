/*****************************************************************
*	File		:	imagereader.c								 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Implements routines for presenting			 *
*					cdrom image as a cdrom device during		 *
*					redirection									 *
******************************************************************/

/**
 * @file   imagereader.c
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   18-June-2003
 *
 * @brief  Implements the cdrom redirection routines.
 */

#include <string.h>
#include <stdlib.h>

#ifdef _WIN32 /*Joy added*/
#include <wchar.h>
#include <stddef.h>
#endif
#include <dbgout.h>
#include "imagereader.h"
#include "iso9660.h"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <io.h>
#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif
#endif

#ifndef _WIN32
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>

#ifdef _WIN32
#define INT64	__int64
#define LSEEK64 _lseeki64
#define TELL64 _telli64
#endif
#ifdef ICC_OS_LINUX
//#else
#define INT64 off_t
#define LSEEK64 lseek64
#define TELL64 tell64
#define O_BINARY 0
#endif
#ifdef ICC_OS_MACH
#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif
#define LSEEK64 lseek
#define O_BINARY 0
#define INT64 off_t
#endif

/***                    Why all this "unaligned" stuff?                    ***/
/* Sparc processors refuse to read or write integers that are not aligned on */
/* a 4-byte boundary.  Attempting to do so generates a bus error and kills   */
/* the offending program.  Therefore, SolarisMediaReader.cpp defines a bunch */
/* of functions to read and write from unaligned integers by copying them    */
/* byte-wise into appropriately aligned structures.  See the                 */
/* SolarisMediaReader.* files for details.  For operating systems that are   */
/* not solaris, we fill in the read/write unaligned functions with simple    */
/* typecasts and copies.  This makes the assumption that solaris is the only */
/* OS we will support that runs on sparc hardware.  Keep this in mind if we  */
/* have to support something odd like linux on sparc in the future.          */
#ifdef ICC_OS_SOLARIS
#include "SolarisMediaReader.h"
#else
int16 cd_read_unaligned_int16( const uint8 *address )
{
	return( *( (int16 *)address ) );
}

void cd_write_unaligned_int16( uint8 *address, int16 value )
{
	*( (int16 *)address ) = value;
}

int32 cd_read_unaligned_int32( const uint8 *address )
{
	return( *( (int32 *)address ) );
}

void cd_write_unaligned_int32( uint8 *address, int32 value )
{
	*( (int32 *)address ) = value;
}
#endif

#define	MSF_BIT			0x02
#define IDBG_DBGVAR_DECLARE(libcdromreader)

/**
* \brief Sets the corresponding Error flags for USB packet.
* The function takes the error value and sets the error flags
* for the corresponding error for the USB packet.
*
* @param nError	the error value based on which the flags to be set.
* @param pResponsePkt USB packet in which the error flags to be stored.
*/
static void SetErrorStatus(int nError, IUSB_SCSI_PACKET *pResponsePkt)
{
	switch(nError)
	{
		case SUCCESS:
			pResponsePkt->StatusPkt.OverallStatus	= 0;
			pResponsePkt->StatusPkt.SenseKey		= 0x00;
			pResponsePkt->StatusPkt.SenseCode		= 0x00;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case SECTOR_RANGE_ERROR:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x05;
			pResponsePkt->StatusPkt.SenseCode		= 0x21;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case WRONG_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 0;
			pResponsePkt->StatusPkt.SenseKey		= 0x03;
			pResponsePkt->StatusPkt.SenseCode		= 0x30;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x01;
			break;
		case MEDIUM_CHANGE:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x06;
			pResponsePkt->StatusPkt.SenseCode		= 0x28;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case MEDIA_ERROR:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x03;
			pResponsePkt->StatusPkt.SenseCode		= 0x11;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case NO_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x02;
			pResponsePkt->StatusPkt.SenseCode		= 0x3A;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case INVALID_PARAMS:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x05;
			pResponsePkt->StatusPkt.SenseCode		= 0x26;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case UNREADABLE_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x03;
			pResponsePkt->StatusPkt.SenseCode		= 0x30;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x02;
			break;
		case REMOVAL_PREVENTED:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x05;
			pResponsePkt->StatusPkt.SenseCode		= 0x53;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x02;
			break;
		case UNSUPPORTED_COMMAND:
		default:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x05;
			pResponsePkt->StatusPkt.SenseCode		= 0x20;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
	}

	return;
}


/**
* \brief Decodes the value read from ISO image file.
* The image file is read in terms of the characters and
* the decoding logic for certain value is provided in
* the ISO9660 document. Here 723 refers to the section
* number in ISO9660 document, where the decoding logic
* is specified.
*
* @param pnt pointer to CDROM image buffer.
*
* @return the decoded value
*/
unsigned int get_iso_723(char *pnt)
{
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
	return( ( ( pnt[1] & 0xff ) << 8 ) | ( pnt[0] & 0xff ) );
#else
	return( ( ( pnt[0] & 0xff ) << 8 ) | ( pnt[1] & 0xff ) );
#endif
}


/**
* \brief Attempts to open the image file and returns the status.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int OpenImage(CDROMIMAGE *pCDImage)
{
#ifdef _WIN32 /*Joy added for windows*/
	HANDLE device_handle;

/*Change ISO path from UTF-8 to UTF-16, UTF-16 is the default Unicode encoding by Windows*/
	wchar_t UnicodeStr[MAX_DEVICE_PATH_SIZE];
	MultiByteToWideChar(CP_UTF8,0,pCDImage->szImageFile,-1,UnicodeStr,MAX_DEVICE_PATH_SIZE);
		
	/* Open the image file */
	device_handle = CreateFileW(UnicodeStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);


	if (device_handle == INVALID_HANDLE_VALUE) {

		TWARN("Unable to open image file - %s\n", pCDImage->szImageFile);
		return NO_MEDIA;
	}
	pCDImage->fdImageFile = (int) device_handle;

	return SUCCESS;
#else /*Joy added for Linux*/
	pCDImage->fdImageFile = open( pCDImage->szImageFile, O_BINARY | O_RDONLY | O_LARGEFILE, S_IREAD );
	if(pCDImage->fdImageFile == -1)
	{
		TWARN("Unable to open image file - %s\n", pCDImage->szImageFile);
		return NO_MEDIA;
	}

	return SUCCESS;
#endif
}

/**
* \brief Closes the image file and returns the status.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int CloseImage(CDROMIMAGE *pCDImage)
{
	if (pCDImage->fdImageFile != -1) {

#ifdef _WIN32 /*Joy Added*/
		CloseHandle((HANDLE) pCDImage->fdImageFile);
#else
		close(pCDImage->fdImageFile);
#endif
	}

	return SUCCESS;
}

/**
* \brief Reads the CDROM image capacity
* Gets the information from the CDROM image like Total number of
* sectors and Size of each sector.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int ReadCapacity(CDROMIMAGE *pCDImage)
{
	char 				buffer[2048];
	struct iso_primary_descriptor 	*vol_desc;
	#ifdef _WIN32
	__int64 				dwFileSize;
	#else
	off_t				dwFileSize;
	struct stat stats;
	#endif
#ifdef _WIN32  /*Joy added*/
	HANDLE device_handle;
	DWORD num_read;
	LARGE_INTEGER iso_image_size;
	
	device_handle = (HANDLE) pCDImage->fdImageFile;
	iso_image_size.QuadPart = (LONGLONG) (16 * 2048); /* primary volume descriptor is in LBA 16 */

	if (SetFilePointerEx(device_handle, iso_image_size, NULL, FILE_BEGIN) == -1) 
#else /*Joy added for LINUX*/
	if( LSEEK64(pCDImage->fdImageFile, (16L*2048L), SEEK_SET) == -1 )
#endif
	{
		TCRIT("lseek64 failed for vol desc\n");
		return MEDIA_ERROR;
	}

	/* read primary volume descriptor */
#ifdef _WIN32  /*Joy added*/
	if (ReadFile(device_handle, buffer, sizeof(buffer), &num_read, NULL) == 0) 
#else /*Joy added for LINUX*/
	if( read(pCDImage->fdImageFile,buffer, 2048) != 2048 )
#endif
	{
		TCRIT("Read failed\n");
		return UNREADABLE_MEDIA;
	}

	vol_desc = (struct iso_primary_descriptor*)&buffer[0];

#ifdef _WIN32  /*Joy added*/
	iso_image_size.QuadPart = 0;
	if (SetFilePointerEx(device_handle, iso_image_size, NULL, FILE_END) == -1) 
#else /*Joy added for LINUX*/
	if(LSEEK64(pCDImage->fdImageFile, 0, SEEK_END) == -1)
#endif
	{
		TCRIT("Second lseek failed\n");
		return MEDIA_ERROR;
	}

	#ifndef _WIN32
	stat(pCDImage->szImageFile,&stats);
	dwFileSize = stats.st_size;
	#else
		#ifdef _WIN32 /*Joy added*/
	GetFileSizeEx(device_handle, &iso_image_size);
	dwFileSize = iso_image_size.QuadPart;
		#else
	dwFileSize = TELL64(pCDImage->fdImageFile);
		#endif
	#endif

	/* Block size may not always equal to Sector Size */
	pCDImage->BlockSize	=	2048;//get_iso_723(vol_desc->logical_block_size);
#if defined (__x86_64__) || defined(WIN64)
	pCDImage->TotalSectors	=	(unsigned int)((dwFileSize) / (pCDImage->BlockSize));
#else
	pCDImage->TotalSectors	=	(unsigned long)((dwFileSize) / (pCDImage->BlockSize));
#endif

	return SUCCESS;
}

/**
* \brief Validates the image file by checking its signature.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int ValidateISOImage(CDROMIMAGE *pCDImage)
{
	char				buffer[2048];
	struct iso_primary_descriptor	*vol_desc;
#ifdef _WIN32 /*Joy added*/
	HANDLE device_handle;
	DWORD num_read;
	LARGE_INTEGER iso_image_size;
	device_handle = (HANDLE) pCDImage->fdImageFile;
	iso_image_size.QuadPart = (LONGLONG) (16 * 2048); /* primary volume descriptor is in LBA 16 */
	if (SetFilePointerEx(device_handle, iso_image_size, NULL, FILE_BEGIN) == -1) {
		return MEDIA_ERROR;
	}
	if (ReadFile(device_handle, buffer, sizeof(buffer), &num_read, NULL) == 0) {
		return UNREADABLE_MEDIA;
	}
#else /*Joy added*/
	if(LSEEK64(pCDImage->fdImageFile, 16*2048L, SEEK_SET) == -1L )
	{
		return MEDIA_ERROR;
	}

	if( read( pCDImage->fdImageFile,buffer, 2048) != 2048 )
	{
		return UNREADABLE_MEDIA;
	}
#endif
	vol_desc = (struct iso_primary_descriptor*)&buffer[0];

	/* signature not found */
	if( strstr(vol_desc->id, "CD001") == NULL )
	{
		//ARN("Not a valid ISO image - %s\n", pCDImage->szImageFile);
		return WRONG_MEDIA;
	}

	return SUCCESS;
}

/**
* \brief Checks the ready state of a cdrom image.
* TEST_UNIT_READY is a command to check the the
* ready state of a CDROM
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int TestUnitReady(CDROMIMAGE *pCDImage)
{
	int		nRetval;

	/* Read Capacity */
	nRetval = ReadCapacity(pCDImage);
	if (nRetval != SUCCESS)
	{
		TWARN("Unable to read ISO image.\n");
		return nRetval;
	}

	return SUCCESS;
}

/**
* \brief Reads the desired number of sectors from an image file.
* Reads the data from an image file and returns the size of the
* data (\a dwDataLen) read from an image file.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
* @param SectorNo refers to the Starting sector from which the data is to be read.
* @param Sectors refers to the number of sectors to be read from the the Starting sector \a SectorNo
* @param dwDataLen it is an OUT parameter filled with size of the data read or 0 if any error occurs.
* @param Buffer pointer to the data read from the cdrom image file.
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
#if defined (__x86_64__) || defined(WIN64)
static int ReadCdrom(CDROMIMAGE *pCDImage, unsigned int SectorNo,unsigned int Sectors, unsigned int *dwDataLen, char *Buffer)
#else
static int ReadCdrom(CDROMIMAGE *pCDImage, unsigned long SectorNo,unsigned long Sectors, unsigned long *dwDataLen, char *Buffer)
#endif
{
#if defined (__x86_64__) || defined(WIN64)
	unsigned int	dwReadSize;
	unsigned int	nResult;
#else
	unsigned long	dwReadSize;
	unsigned long	nResult;
#endif	
	int		nRetval;
	unsigned char	*lpSector;
	INT64		nSeekSector;
#ifdef _WIN32 /*Joy added*/
	HANDLE device_handle;
	DWORD num_read;
	LARGE_INTEGER iso_image_size;
#endif
	if(Sectors > pCDImage->TotalSectors){
		
		return SECTOR_RANGE_ERROR; 
	}

#ifdef _WIN32 
	/*Calculate Buffer Requirements*/
	dwReadSize = Sectors * pCDImage->BlockSize;

	nSeekSector = (INT64)(((INT64)SectorNo) * ((INT64)pCDImage->BlockSize));
	device_handle = (HANDLE) pCDImage->fdImageFile;
	iso_image_size.QuadPart = (LONGLONG) nSeekSector;

	/* Seek to the Desired Sector */
	if (SetFilePointerEx(device_handle, iso_image_size, NULL, FILE_BEGIN) == -1) {
		return SECTOR_RANGE_ERROR; /* SetSectorRange */
	}

	lpSector = (unsigned char *)malloc(dwReadSize);
	if( lpSector == NULL )
	{
		return INVALID_PARAMS;
	}

	/* Read the CD */
	nResult = ReadFile(device_handle, lpSector, dwReadSize, &num_read, NULL);
	nRetval = SUCCESS;
	if(num_read != dwReadSize)
#else /*Joy added*/
	/*Calculate Buffer Requirements*/
	dwReadSize = Sectors * pCDImage->BlockSize;

	nSeekSector = (INT64)(((INT64)SectorNo) * ((INT64)pCDImage->BlockSize));

	/* Seek to the Desired Sector */
	if( LSEEK64(pCDImage->fdImageFile, nSeekSector, SEEK_SET) == -1 )
	{
		return SECTOR_RANGE_ERROR; /* SetSectorRange */
	}

	lpSector = (unsigned char *)malloc(dwReadSize);
	if( lpSector == NULL )
	{
		return INVALID_PARAMS;
	}

	/* Read the CD */
	nResult = read(pCDImage->fdImageFile,lpSector, dwReadSize);
	nRetval = SUCCESS;
	if(nResult != dwReadSize)
#endif
	{
		if(nResult == 0)
		{
			nRetval = SECTOR_RANGE_ERROR; /* SetSectorRange */
		}
		else
		{
			nRetval = MEDIA_ERROR; /* SetMediaError */
		}
	}

	if ( nRetval == SUCCESS )
	{
		*dwDataLen = dwReadSize;
		memcpy(Buffer,lpSector,dwReadSize);
	}
	else
	{
		*dwDataLen = 0;
	}

	free(lpSector);

	return nRetval;
}

/**
* \brief Reads the Table of Contents from an image file.
*
* @param pCDImage refers cdrom image file. It is of type \ref CDROMIMAGE
* @param ScsiPkt It is the command packet received from server. It contains the starting track information.
* @param Buffer pointer to the data read from the cdrom image file.
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
#if defined (__x86_64__) || defined(WIN64)
static int ReadTOC(CDROMIMAGE *pCDImage, SCSI_COMMAND_PACKET *ScsiPkt, unsigned int *dwDataLen, char *Buffer)
#else
static int ReadTOC(CDROMIMAGE *pCDImage, SCSI_COMMAND_PACKET *ScsiPkt, unsigned long *dwDataLen, char *Buffer)
#endif
{
	int		StartTrack;
	unsigned int nLength = 4;
	unsigned char	*TOC;
#if defined (__x86_64__) || defined(WIN64)
	unsigned int AllocLen;
#else
	unsigned long AllocLen;
#endif

	StartTrack	= ScsiPkt->CmdLen.Cmd10.Reserved6;
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
	AllocLen = mac2bshort(ScsiPkt->CmdLen.Cmd10.Length);
#else
	AllocLen = cd_read_unaligned_int16( (uint8 *)&ScsiPkt->CmdLen.Cmd10.Length );
#endif

	TDBG("allocation length from initiatir is %x\n",AllocLen);

	TOC	= (unsigned char*)Buffer;

	TDBG("Start rack for TOC is %d\n",StartTrack);

	if( (StartTrack > 1) && (StartTrack != 0xaa) )
	{
    		return 0;
	}

	TOC[2] = 1;
    TOC[3] = 1;

	if (StartTrack <= 1)
	{
		TOC[nLength++] = 0;	 // Reserved
		TOC[nLength++] = 0x14; // ADR, control
		TOC[nLength++] = 1;    // Track number
		TOC[nLength++] = 0;    // Reserved

		if( ScsiPkt->Lun & MSF_BIT )
		{
			TDBG("MSF is set\n");
			/* Send in MSF Format */
			/* Logical ABS ADDR = (((MIN*60)+SEC)*75 + FRAC ) - 150 */
			TOC[nLength++] = 0; // reserved
			TOC[nLength++] = 0; // minute
			TOC[nLength++] = 2; // second
			TOC[nLength++] = 0; // frame
		}
		else
		{
			TDBG("MSF is not set\n");
			/* Send Direct Absolute address  */
			TOC[nLength++] = 0; // reserved
			TOC[nLength++] = 0; // minute
			TOC[nLength++] = 0; // second
			TOC[nLength++] = 0; // frame
		}
	}


	TOC[nLength++] = 0;		// Reserved
	TOC[nLength++] = 0x16;  // ADR, control
	TOC[nLength++] = 0xaa;  // Track number
	TOC[nLength++] = 0;		// Reserved

	TOC[nLength++] = 0;									// reserved
	TOC[nLength++] = (uint8)(((pCDImage->TotalSectors + 150) / 75) / 60); // minute
	TOC[nLength++] = (uint8)(((pCDImage->TotalSectors + 150) / 75) % 60); // second
	TOC[nLength++] = (uint8)((pCDImage->TotalSectors + 150) % 75);		// frame;

	TDBG("nLength before is %x\n",nLength);
	if(nLength > AllocLen)
	{
		nLength = AllocLen;
	}
	TDBG("nlength is now %x\n",nLength);

#ifdef ICC_PLATFORM_LITTLE_ENDIAN
	TOC[0] = ((nLength-2) >> 8) & 0xff;
	TOC[1] = (nLength-2) & 0xff;
#else
	TOC[1] = ((nLength-2) >> 8) & 0xff;
	TOC[0] = (nLength-2) & 0xff;
#endif

	*dwDataLen = nLength;

	return SUCCESS;
}

/***************************************************************************************
	Function	:	OpenCDROMImage

	Parameters	:	pszImageFilePath - full path of cdrom image file
				pCDImage - specifies the cdrom image object

	Returns		:	SUCCESS upon successful completion
				DEVICE_ALREADY_OPEN if the device is already in open state.
				DEVICE_PATH_TOO_LONG if the length of path string exceeds \ref MAX_DEVICE_PATH_SIZE.
				WRONG_MEDIA if the image file is not a valid ISO9660 image file.
				NO_MEDIA if no file exists in specified location.
				MEDIA_ERROR if invalid file
				UNREADABLE_MEDIA if image file is unreadable.

	Description	:	Opens the cdrom image file.
				validates the cdrom image file.
				returns the object identifying the cdrom image.
****************************************************************************************/
int OpenCDROMImage(char *pszImageFilePath, CDROMIMAGE *pCDImage)
{
	int		nRetval;

	if ( pCDImage->bIsOpened == 1)
	{
		TWARN("CloseCDROMImage is not called for previous image file\n");
		return DEVICE_ALREADY_OPEN;
	}

	pCDImage->bIsOpened = 0;

	/* Check the boundaries */
	if( strlen(pszImageFilePath) > MAX_DEVICE_PATH_SIZE )
	{
		TWARN("Image file path is too long.\n");
		return DEVICE_PATH_TOO_LONG;
	}

	strcpy(pCDImage->szImageFile, pszImageFilePath);

	/* Open CD Image*/
	nRetval = OpenImage(pCDImage);
	if(nRetval != SUCCESS)
	{
		return nRetval;
	}

	/* Check if valid ISO image */
	nRetval = ValidateISOImage(pCDImage);
	if (nRetval != SUCCESS)
	{
#ifdef _WIN32 /*Joy added*/
		CloseHandle((HANDLE)pCDImage->fdImageFile);
#else
		close(pCDImage->fdImageFile);
#endif
		return nRetval;
	}


	/* Test Unit Ready*/
	nRetval = TestUnitReady(pCDImage);
	if(nRetval != SUCCESS)
	{
#ifdef _WIN32 /*Joy added*/
		CloseHandle((HANDLE)pCDImage->fdImageFile);
#else
		close(pCDImage->fdImageFile);
#endif
		return nRetval;
	}

	pCDImage->ByteOrder	= littleendian;
	pCDImage->bFirstTime	= 1;
	pCDImage->bIsOpened	= 1;
	pCDImage->IndicateMediaChange = 1;
#ifdef _WIN32 /*Joy added*/
	CloseHandle((HANDLE)pCDImage->fdImageFile);
#else
	close(pCDImage->fdImageFile);
#endif
	return SUCCESS;
}

/***************************************************************************************
	Function	:	CloseCDROMImage

	Parameters	:	pCDImage - specifies the cdrom image object

	Returns		:	SUCCESS always

	Description	:	Close the cdrom image and reset the object
****************************************************************************************/
int	CloseCDROMImage(CDROMIMAGE *pCDImage)
{
	if( pCDImage->bIsOpened )
	{
		pCDImage->bIsOpened	= 0;
	}

	return SUCCESS;
}

/***************************************************************************************
	Function	:	SetByteOrder

	Parameters	:	ByteOrder - set the byte order of the card.
				pCDImage - specifies the cdrom image object

	Returns		:	SUCCESS always

	Description	:	This function is useful when used from client side.
				it is not useful when used in the card(firmware).
				It is used to take a note of byte order used by
				the card. This byte order will be useful when
				parsing the commands from MegaRAC-Card.
****************************************************************************************/
void SetByteOrder(enum byteOrder ByteOrder, CDROMIMAGE *pCDImage )
{
	pCDImage->ByteOrder = ByteOrder;
}

/***************************************************************************************
	Function	:	ExecuteSCSICmd

	Parameters	:	pRequestPkt  - command request from host/MegaRAC-Card
				pResponsePkt - response packet to host/MegaRAC-Card.
				pdwDataLen - Length of the data added to response packet.
				pCDImage - specifies the cdrom image object


	Returns		:	SUCCESS upon successful completion
				Error Codes on any other Error.

	Description	:	Execute the command sent from host/card.
				Form a response packet and add data to it if any.
****************************************************************************************/
#if defined (__x86_64__) || defined(WIN64)
int ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned int *pdwDataLen, CDROMIMAGE *pCDImage)
#else
int ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned long *pdwDataLen, CDROMIMAGE *pCDImage)
#endif
{
	SCSI_COMMAND_PACKET	*ScsiPkt;
	int			nRetval;
#if defined (__x86_64__) || defined (WIN64)
	unsigned int		dwDataLen=0;
	unsigned int		dwSectorNo;
	unsigned int		dwSectors;
	unsigned int		dwMaxTOCSize;
#else
	unsigned long		dwDataLen=0;
	unsigned long		dwSectorNo;
	unsigned long		dwSectors;
	unsigned long		dwMaxTOCSize;
#endif
	READ_CAPACITY		*ScsiReadCap;


	*pdwDataLen = 0;

	/* Some Fields has to be sent back. So copy all as of now */
	memcpy(pResponsePkt,pRequestPkt,sizeof(IUSB_SCSI_PACKET));

	if( pCDImage->bIsOpened != 1)
	{
		TWARN("ExecuteSCSICmd called before using OpenCDROMImage\n");
		nRetval = NO_MEDIA;
	}
	else
	{
		/* Open Image file */
		nRetval = OpenImage(pCDImage);
		if(nRetval != SUCCESS)
		{
			TDBG("Connection lost for CDROM imagefile - %s\n",
				pCDImage->szImageFile);
		}
	}

	if( nRetval != SUCCESS)
	{
		pCDImage->bFirstTime = 1;
	}
	else if(nRetval == SUCCESS && pCDImage->bFirstTime )
	{
		TDBG("Connection got for CDROM imagefile - %s\n",
			pCDImage->szImageFile);

		pCDImage->bFirstTime = 0;

		/* Close Image file */
		CloseImage(pCDImage);

	}
	else if( nRetval == SUCCESS )
	{
		char *buf;
		/* Get the Scsi CDB  */
		ScsiPkt=(SCSI_COMMAND_PACKET *)(&(pRequestPkt->CommandPkt));
		switch (ScsiPkt->OpCode)
		{
			case SCSI_TEST_UNIT_READY:
				if(pCDImage->IndicateMediaChange)
				{
					TDBG("test unit ready for first time..Indicating medium change\n");
					pCDImage->IndicateMediaChange = 0;
					nRetval = MEDIUM_CHANGE;
				}
				else
				{
					nRetval = TestUnitReady(pCDImage);
				}
				break;
			case SCSI_READ_CAPACITY:
				if( pCDImage->IndicateMediaChange == 1)
				{
					pCDImage->IndicateMediaChange = 0;
					TDBG("read capfor first time..Indicating medium change\n");
					nRetval = MEDIUM_CHANGE;
					break;
				}

				nRetval = ReadCapacity(pCDImage);

				ScsiReadCap = (READ_CAPACITY *)(&(pResponsePkt->Data));
				if( pCDImage->ByteOrder == bigendian )
				{
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
					ScsiReadCap->BlockSize = mac2blong(pCDImage->BlockSize);
					ScsiReadCap->TotalBlocks = mac2blong(pCDImage->TotalSectors-1);
#else
					cd_write_unaligned_int32( (uint8 *)&ScsiReadCap->BlockSize, pCDImage->BlockSize );
					cd_write_unaligned_int32( (uint8 *)&ScsiReadCap->TotalBlocks, pCDImage->TotalSectors-1 );
#endif
				}
				else
				{
					cd_write_unaligned_int32( (uint8 *)&ScsiReadCap->BlockSize, pCDImage->BlockSize );
					cd_write_unaligned_int32( (uint8 *)&ScsiReadCap->TotalBlocks, pCDImage->TotalSectors-1 );
				}
				dwDataLen = 8;
				break;

			case SCSI_READ_10:
			case SCSI_READ_12:
				dwSectorNo	= ScsiPkt->Lba;
				dwSectors = (ScsiPkt->OpCode == SCSI_READ_10) ? ScsiPkt->CmdLen.Cmd10.Length : ScsiPkt->CmdLen.Cmd12.Length32;

				if( pCDImage->ByteOrder == bigendian )
				{
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
					dwSectorNo	= mac2blong(ScsiPkt->Lba);
					dwSectors	= mac2bshort(ScsiPkt->CmdLen.Cmd10.Length);
#else
					dwSectorNo	= ScsiPkt->Lba;
					dwSectors	= ScsiPkt->CmdLen.Cmd10.Length;
#endif
				}
				else
				{
					dwSectorNo	= ScsiPkt->Lba;
					dwSectors	= ScsiPkt->CmdLen.Cmd10.Length;
				}

				nRetval=ReadCdrom(pCDImage, dwSectorNo, dwSectors, &dwDataLen, (char *)(&(pResponsePkt->Data)) );
				break;

			case SCSI_READ_TOC:

				TDBG("TOC request is for 0x%x bytes\n",ScsiPkt->CmdLen.Cmd10.Length);

				nRetval = ReadTOC(pCDImage, ScsiPkt, &dwDataLen, (char *)(&(pResponsePkt->Data)) );
				buf = ((char *)(&(pResponsePkt->Data)));

				dwMaxTOCSize = cd_read_unaligned_int16( (uint8 *)&ScsiPkt->CmdLen.Cmd10.Length );

				if( pCDImage->ByteOrder == bigendian )
				{
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
					dwMaxTOCSize = mac2bshort(dwMaxTOCSize);
#endif
				}

				if (dwDataLen > dwMaxTOCSize)
					dwDataLen = dwMaxTOCSize;

				TDBG("dwData len is therefore %x\n",dwDataLen);

				break;
			case SCSI_START_STOP_UNIT:
				nRetval = SUCCESS;
				break;
			case SCSI_MEDIUM_REMOVAL:
			default: /* Unsupported */
				nRetval = UNSUPPORTED_COMMAND;
				break;
		}

		/* Close Image file */
		CloseImage(pCDImage);
	}

	SetErrorStatus(nRetval, pResponsePkt);

	*pdwDataLen = dwDataLen;

#ifdef ICC_PLATFORM_BIG_ENDIAN
    /* The data length passed to the remote device must be little endian */
    dwDataLen = mac2blong( dwDataLen );
#endif

	cd_write_unaligned_int32( (uint8 *)&pResponsePkt->DataLen, dwDataLen );

	return nRetval;
}


