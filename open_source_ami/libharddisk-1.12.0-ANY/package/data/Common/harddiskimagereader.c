/*****************************************************************
*	File		:	harddiskimagereader.c							 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Implements routines for presenting			 *
*					harddisk image as a harddisk device during		 *
*					redirection									 *
******************************************************************/

/**
 * @file   harddiskimagereader.c
 * @author Rajasekhar (rajasekharj@amiindia.co.in)
 * @date   18-July-2003
 *
 * @brief  Implements the harddisk redirection routines.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef _WIN32 /*Joy added*/
#include <wchar.h>
#include <stddef.h>
#endif
#include "harddiskimagereader.h"
#include "libharddisk.h"
#include "harddiskerrors.h"
#include <dbgout.h>
#include "icc_what.h"

#ifdef ICC_PLATFORM_BIG_ENDIAN
#define GET_SHORT_VALUE(x) (uint16)((((uint16)(x) >> 8) | ((uint16)(x) << 8)))
#define GET_LONG_VALUE(x) ( ((x) >> 24) | ((x) << 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8))
#else
#define GET_SHORT_VALUE(x) (x)
#define GET_LONG_VALUE(x)  (x)
#endif

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
#define LSEEK64 _lseeki64
#define TELL64 _telli64
#define INT64	__int64
#define READHARDDISKIMAGE		_read
#define WRITEHARDDISKIMAGE	_write
#endif
#ifdef ICC_OS_LINUX
#define LSEEK64 lseek64
#define TELL64 tell64
#define INT64 off_t
#define READHARDDISKIMAGE		read
#define WRITEHARDDISKIMAGE	write
#define O_BINARY 0
#endif
#ifdef ICC_OS_MACH
#include <IOKit/storage/IOMediaBSDClient.h>
#define LSEEK64 lseek
#define TELL64 tell64
#define INT64 off_t
#define READHARDDISKIMAGE		read
#define WRITEHARDDISKIMAGE	write
#define O_BINARY 0
static int umountCount = 0;
#endif
/*Total sector offset info*/
#define FAT16_OFFSET 0x36
#define FAT32_OFFSET 0x52
#define NTFS_OFFSET	0x03

#define STR_LEN 32
int IsHDFSFAT32(unsigned char *data)
{
	if( data[0x42] == 0x28 || data[0x42] == 0x29 )
		return 1;
	
	return 0;
}

int IsHDFSFAT16(unsigned char *data)
{
	if( !IsHDFSFAT32(data) && ( data[0x26] == 0x28 || data[0x26] == 0x29 )  )
		return 1;

	return 0;
}

/***                    Why all this "unaligned" stuff?                    ***/
/* Sparc processors refuse to read or write integers that are not aligned on */
/* a 4-byte boundary.  Attempting to do so generates a bus error and kills   */
/* the offending program.  Therefore, SolarisFloppyReader.cpp defines a bunch*/
/* of functions to read and write from unaligned integers by copying them    */
/* byte-wise into appropriately aligned structures.  See the                 */
/* SolarisFloppyReader.* files for details.  For operating systems that are  */
/* not solaris, we fill in the read/write unaligned functions with simple    */
/* typecasts and copies.  This makes the assumption that solaris is the only */
/* OS we will support that runs on sparc hardware.  Keep this in mind if we  */
/* have to support something odd like linux on sparc in the future.          */
#ifdef ICC_OS_SOLARIS
#include "SolarisFloppyReader.h"
#else
int16 hd_read_unaligned_int16( const uint8 *address )
{
	return( *( (int16 *)address ) );
}

void hd_write_unaligned_int16( uint8 *address, int16 value )
{
	*( (int16 *)address ) = value;
}

int32 hd_read_unaligned_int32( const uint8 *address )
{
	return( *( (int32 *)address ) );
}

void hd_write_unaligned_int32( uint8 *address, int32 value )
{
	*( (int32 *)address ) = value;
}
#endif

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
			pResponsePkt->StatusPkt.SenseKey	= 0x00;
			pResponsePkt->StatusPkt.SenseCode	= 0x00;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x00;
			break;
		case SECTOR_RANGE_ERROR:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x05;
			pResponsePkt->StatusPkt.SenseCode	= 0x21;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x00;
			break;
		case WRONG_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 0;
			pResponsePkt->StatusPkt.SenseKey	= 0x03;
			pResponsePkt->StatusPkt.SenseCode	= 0x30;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x01;
			break;
		case MEDIUM_CHANGE:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x06;
			pResponsePkt->StatusPkt.SenseCode	= 0x28;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x00;
			break;
		case MEDIA_ERROR:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x03;
			pResponsePkt->StatusPkt.SenseCode	= 0x11;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x00;
			break;
		case MEDIA_IN_USE:
		case NO_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x02;
			pResponsePkt->StatusPkt.SenseCode	= 0x3A;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x00;
			break;
		case INVALID_PARAMS:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x05;
			pResponsePkt->StatusPkt.SenseCode	= 0x26;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x00;
			break;
		case UNREADABLE_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x03;
			pResponsePkt->StatusPkt.SenseCode	= 0x30;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x02;
			break;
		case REMOVAL_PREVENTED:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x05;
			pResponsePkt->StatusPkt.SenseCode	= 0x53;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x02;
			break;
		case WRITE_PROTECT_ERROR:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x07;
			pResponsePkt->StatusPkt.SenseCode	= 0x27;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x00;
			break;
		case UNSUPPORTED_COMMAND:
		default:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey	= 0x05;
			pResponsePkt->StatusPkt.SenseCode	= 0x20;
			pResponsePkt->StatusPkt.SenseCodeQ	= 0x00;
			break;
	}

	return;
}

/**
* \brief Attempts to open the image file in readonly mode and returns the status.
*
* @param pFDImage refers harddisk image file. It is of type \ref HARDDISKIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int OpenImageReadOnly(HARDDISKIMAGE *pFDImage)
{
#ifdef _WIN32 /*Joy added for windows*/
	HANDLE device_handle;

/*Change ISO path from UTF-8 to UTF-16, UTF-16 is the default Unicode encoding by Windows*/
	wchar_t UnicodeStr[MAX_DEVICE_PATH_SIZE];
	MultiByteToWideChar(CP_UTF8,0,pFDImage->szImageFile,-1,UnicodeStr,MAX_DEVICE_PATH_SIZE);
		
	/* Open the image file */
	device_handle = CreateFileW(UnicodeStr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);


	if (device_handle == INVALID_HANDLE_VALUE) {

		TWARN("Unable to open image file - %s\n", pFDImage->szImageFile);
		return NO_MEDIA;
	}
	pFDImage->pImageFile = (int) device_handle;

	return SUCCESS;
#else /*Joy added for Linux*/
	/*Fopen call fails when the USB is Unplugged from the linux client it crashes the JViewer application.
        opencall checks whether th device is available or not while unplug the USB.
        */
	int        nRetval = SUCCESS;
	int fd;
	
	fd=open(pFDImage->szImageFile,O_RDONLY);
	if(fd<0)
	{
		return NO_MEDIA;
	}
	close(fd);
	
	/* Open the image file */
	pFDImage->pImageFile	=	fopen(pFDImage->szImageFile, "rb" );

	if(pFDImage->pImageFile == NULL)
	{
		TWARN("Unable to open image file - %s\n", pFDImage->szImageFile);
		return NO_MEDIA;
	}

	return SUCCESS;
#endif
}

/**
* \brief Attempts to open the image file in write mode and returns the status.
*
* @param pFDImage refers harddisk image file. It is of type \ref HARDDISKIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int OpenImageWritable(HARDDISKIMAGE *pFDImage)
{
#ifdef _WIN32 /*Joy added for windows*/
	HANDLE device_handle;

/*Change ISO path from UTF-8 to UTF-16, UTF-16 is the default Unicode encoding by Windows*/
	wchar_t UnicodeStr[MAX_DEVICE_PATH_SIZE];
	MultiByteToWideChar(CP_UTF8,0,pFDImage->szImageFile,-1,UnicodeStr,MAX_DEVICE_PATH_SIZE);
		
	/* Open the image file */
	device_handle = CreateFileW(UnicodeStr, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);


	if (device_handle == INVALID_HANDLE_VALUE) {

		TWARN("Unable to open image file - %s\n", pFDImage->szImageFile);
		return NO_MEDIA;
	}

	pFDImage->pImageFile = (int) device_handle;

	return SUCCESS;
#else /*Joy added for Linux*/
	int fd;
#if defined ICC_OS_MACH
	char cmd[50];
#endif
	if( pFDImage->bIsReadOnly == true)
	{
		TDBG("Attempted to modify RAM image");
		return WRITE_PROTECT_ERROR;
	}
	fd=open(pFDImage->szImageFile,O_RDONLY);
	if(fd<0)
	{
		return NO_MEDIA;
	}
	close(fd);
#if defined ICC_OS_MACH
	if(umountCount == 1)
	{
		sprintf(cmd, "diskutil unmountDisk %s", pFDImage->szImageFile);
		system(cmd);
		umountCount = 0;
	}
#endif
	/* Open the image file */
	pFDImage->pImageFile = fopen(pFDImage->szImageFile, "rb+" );

	if(pFDImage->pImageFile == NULL)
	{
		if(errno == EROFS)
			return WRITE_PROTECT_ERROR;
		else
		{
			TWARN("Unable to open image file - %s\n", pFDImage->szImageFile);
			return NO_MEDIA;
		}
	}

	return SUCCESS;
#endif
}

/**
* \brief Closes the image file and returns the status.
*
* @param pFDImage refers harddisk image file. It is of type \ref HARDDISKIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int CloseImage(HARDDISKIMAGE *pFDImage)
{
	if(pFDImage->pImageFile != NULL )
	{
		#ifdef _WIN32 /*Joy Added*/
		CloseHandle((HANDLE) pFDImage->pImageFile);
		#else
		fclose(pFDImage->pImageFile);
		pFDImage->pImageFile = NULL;
		#endif
	}

	return SUCCESS;
}
/**
*
*OnReadImage
*
**/
int OncheckImageType(HARDDISKIMAGE *pFDImage,char *buffer)
{
	BPB	*pBootSector;
	BPB	*pBootSector_fat;
	BPB_NTFS	*pBootSector_ntfs;
	
	pBootSector = (BPB*)buffer;
		
	if(strncmp(buffer+NTFS_OFFSET,"NTFS",4)== 0)
	{ 
		pBootSector_ntfs = (BPB_NTFS*)&buffer[0];
#if defined (__x86_64__) || defined (WIN64)
		pFDImage->TotalSectors =  (unsigned int)pBootSector_ntfs->Totalsector;;
#else
		pFDImage->TotalSectors =  (unsigned long)pBootSector_ntfs->Totalsector;;
#endif
		pFDImage->BlockSize= pBootSector_ntfs->bsBytesPerSec;
	}
	else if((strncmp(buffer+FAT16_OFFSET,"FAT16",5)== 0) || (strncmp(buffer+FAT16_OFFSET,"FAT12",5)== 0) || (strncmp(buffer+FAT32_OFFSET,"FAT32",5)== 0))
	{
		pBootSector_fat = (BPB*)&buffer[0];
#if defined (__x86_64__) || defined (WIN64)
		pFDImage->TotalSectors =  (unsigned int)pBootSector_fat->bsSectors;
#else
		pFDImage->TotalSectors =  (unsigned long)pBootSector_fat->bsSectors;
#endif
		if(pFDImage->TotalSectors == 0)
		{
			#if defined (__x86_64__) || defined (WIN64)
			pFDImage->TotalSectors =  (unsigned int)pBootSector_fat->bsHugeSectors;
			#else
			pFDImage->TotalSectors =  (unsigned long)pBootSector_fat->bsHugeSectors;
			#endif
		}
		pFDImage->BlockSize= pBootSector_fat->bsBytesPerSec;
	}
	else
	{
		return SECTOR_RANGE_ERROR;
	}

	return SUCCESS;
}
/**
* \brief Reads the Floppy image capacity
* Gets the information from the Floppy image like Total number of
* sectors and Size of each sector.
*
* @param pFDImage refers harddisk image file. It is of type \ref HARDDISKIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int ReadCapacity(HARDDISKIMAGE *pFDImage)
{
	char buffer[512]={0};

	MASTER_BOOT_REC *master_boot_record;
	int sectorcount = 0;
	int ret;
	ret=OnReadImage(pFDImage,sectorcount,&buffer[0]);
	if(pFDImage->bIsPhysicalDrive)
	{
#if defined ICC_OS_MACH
	return ret;
#endif
	}
	if(ret != SUCCESS)
	{
		return  ret;
	}
	if(OncheckImageType(pFDImage,&buffer[0]) == SECTOR_RANGE_ERROR)
	{
		master_boot_record = (MASTER_BOOT_REC *)&buffer[0];
		sectorcount = (int)master_boot_record->partentry1.part_btween_mbr_first_sector;
		ret= OnReadImage(pFDImage,sectorcount,&buffer[0]);
		if(ret != SUCCESS){
			return  ret;
		}
		if(OncheckImageType(pFDImage,&buffer[0]) == SECTOR_RANGE_ERROR)
		{
			pFDImage->BlockSize = 512;
			pFDImage->TotalSectors = 2880;
		}
	}
			
	return SUCCESS;

	
}

/**
*
*OnReadImage
*
**/
int OnReadImage(HARDDISKIMAGE *pFDImage,long sectorcount,char *buffer)
{
	INT64		dwFileSize;
#ifdef _WIN32  /*Joy added*/
	HANDLE device_handle;
	DWORD num_read;
	LARGE_INTEGER iso_image_size;
	
	device_handle = (HANDLE) pFDImage->pImageFile;
	iso_image_size.QuadPart = (LONGLONG) (sectorcount*512); /* primary volume descriptor is in LBA 16 */
	if (SetFilePointerEx(device_handle, iso_image_size, NULL, FILE_BEGIN) == -1) 
#else /*Joy added for LINUX*/
	/* Make sure we're at the beginning of the disk */
#if defined ICC_OS_MACH
	long offset = (long)sectorcount* 512;	
	if( LSEEK64( fileno(pFDImage->pImageFile),offset, SEEK_SET ) == -1 )
#else
	char off_set[STR_LEN];
	memset(off_set,0,STR_LEN);
	off64_t file_posn;
	off64_t offset = (off64_t)sectorcount * (off64_t)512;
	sprintf(off_set, "offset=%llu\n", (unsigned long long)offset);
	if( LSEEK64( fileno(pFDImage->pImageFile),offset, SEEK_SET ) == -1 )
#endif
#endif
	{
		return MEDIA_ERROR;
	}

#ifdef _WIN32  /*Joy added*/
	if (ReadFile(device_handle, buffer,512, &num_read, NULL) == 0) 
#else /*Joy added for LINUX*/
	/* read BOOT SECTOR (0th sector) */
	if( READHARDDISKIMAGE(fileno(pFDImage->pImageFile), buffer, HARDDISK_BLOCK_SIZE) != HARDDISK_BLOCK_SIZE )
#endif
	{
		return UNREADABLE_MEDIA;
	}
	

#if defined ICC_OS_MACH
	if(pFDImage->bIsPhysicalDrive)
	{
	if(ioctl( fileno(pFDImage->pImageFile), DKIOCGETBLOCKCOUNT, &dwFileSize) >= 0)
       	{
		pFDImage->BlockSize = 512;
		pFDImage->TotalSectors = dwFileSize;
		return SUCCESS;
       	}	
	}
#endif

	return SUCCESS;
}

/**
* \brief Checks the ready state of a harddisk image.
* TEST_UNIT_READY is a command to check the the
* ready state of a Floppy
*
* @param pFDImage refers harddisk image file. It is of type \ref HARDDISKIMAGE
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
static int TestUnitReady(HARDDISKIMAGE *pFDImage)
{
	int		nRetval;

	/* Read Capacity */
	nRetval = ReadCapacity(pFDImage);

	return nRetval;
}

/**
* \brief Reads the desired number of sectors from an image file.
* Reads the data from an image file and returns the size of the
* data (\a dwDataLen) read from an image file.
*
* @param pFDImage refers harddisk image file. It is of type \ref HARDDISKIMAGE
* @param SectorNo refers to the Starting sector from which the data is to be read.
* @param Sectors refers to the number of sectors to be read from the the Starting sector \a SectorNo
* @param dwDataLen it is an OUT parameter filled with size of the data read or 0 if any error occurs.
* @param Buffer pointer to the data read from the harddisk image file.
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
#if defined (__x86_64__) || defined (WIN64)
static int ReadHarddisk(HARDDISKIMAGE *pFDImage, unsigned int SectorNo,
			unsigned int Sectors, unsigned int *dwDataLen, char *Buffer)
#else
static int ReadHarddisk(HARDDISKIMAGE *pFDImage, unsigned long SectorNo,
			unsigned long Sectors, unsigned long *dwDataLen, char *Buffer)
#endif			
{

#if defined (__x86_64__) || defined (WIN64)
	unsigned int	dwReadSize = 0;
#else
	unsigned long	dwReadSize = 0;
#endif
	int		nResult;
	int		nRetval;
	unsigned char	*lpSector;
	INT64		nSeekSector;

#ifdef _WIN32 /*Joy added*/
	HANDLE device_handle;
	DWORD num_read;
	LARGE_INTEGER iso_image_size;
	*dwDataLen = 0;
#endif
	
	if(Sectors > pFDImage->TotalSectors)
		return SECTOR_RANGE_ERROR; /* SetSectorRange */

#ifdef _WIN32 
	/*Calculate Buffer Requirements*/
	dwReadSize = Sectors * pFDImage->BlockSize;

	nSeekSector = (INT64)(((INT64)SectorNo) * ((INT64)pFDImage->BlockSize));
	device_handle = (HANDLE) pFDImage->pImageFile;
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
	*dwDataLen = 0;
	nSeekSector = (INT64)(((INT64)SectorNo) * ((INT64)pFDImage->BlockSize));
	/*Calculate Buffer Requirements*/
	dwReadSize = Sectors * pFDImage->BlockSize;

	/* Seek to the Desired Sector */
	if( LSEEK64( fileno(pFDImage->pImageFile), nSeekSector, SEEK_SET) == -1 )
	{
		return SECTOR_RANGE_ERROR; /* SetSectorRange */
	}

	lpSector = (unsigned char *)malloc(dwReadSize);

	if( lpSector == NULL )
	{
		return INVALID_PARAMS;
	}

	nResult = READHARDDISKIMAGE( fileno(pFDImage->pImageFile), lpSector, dwReadSize);
	if(nResult != (int)dwReadSize)
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
	else
	{
		nRetval = SUCCESS;
	}

	if(nRetval == SUCCESS)
	{
		*dwDataLen = dwReadSize;
		memcpy(Buffer,lpSector,dwReadSize);
	}

	free(lpSector);

	return nRetval;
}

/**
* \brief Writes data to an image file sector wise.
* Writes the data to an image file and returns the size of the
* data (\a dwDataLen) written to image file.
*
* @param pFDImage refers harddisk image file. It is of type \ref HARDDISKIMAGE
* @param SectorNo refers to the Starting sector to which the data is to be written.
* @param Sectors refers to the number of sectors to be written to the the Starting sector \a SectorNo
* @param dwDataLen size of the data to be written to.
* @param Buffer pointer to a data to be written to the harddisk image file.
*
* @retval SUCCESS upon successful completion
* @retval Error Code upon failure.
*/
#if defined (__x86_64__) || defined (WIN64)
static int WriteHarddisk(HARDDISKIMAGE *pFDImage, unsigned int SectorNo,
				unsigned int Sectors, char *Buffer)

#else
static int WriteHarddisk(HARDDISKIMAGE *pFDImage, unsigned long SectorNo,
				unsigned long Sectors, char *Buffer)
#endif				
{
#if defined (__x86_64__) || defined (WIN64)
	unsigned int	dwReadSize;
#else
	unsigned long	dwReadSize;
#endif
	int		nRetval;
	unsigned char	*lpSector;
	int		nResult;

	INT64		nSeekSector;
#ifdef _WIN32 /*Joy added*/
	HANDLE device_handle;
	DWORD num_read;
	LARGE_INTEGER iso_image_size;
#endif
	
	if(Sectors > pFDImage->TotalSectors)
		return SECTOR_RANGE_ERROR; /* SetSectorRange */

#ifdef _WIN32 

	/*Calculate Buffer Requirements*/
	dwReadSize = Sectors * pFDImage->BlockSize;

	nSeekSector = (INT64)(((INT64)SectorNo) * ((INT64)pFDImage->BlockSize));
	device_handle = (HANDLE) pFDImage->pImageFile;
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
	memcpy(lpSector,Buffer,dwReadSize);
	/* Read the CD */
	nResult = WriteFile(device_handle, lpSector, dwReadSize, &num_read, NULL);
	nRetval = SUCCESS;
	if(num_read != dwReadSize)
#else /*Joy added*/
	nSeekSector = (INT64)(((INT64)SectorNo) * ((INT64)pFDImage->BlockSize));
	/*Calculate Buffer Requirements*/
	dwReadSize = Sectors * pFDImage->BlockSize;

	/* Seek to the Desired Sector */
	if( LSEEK64( fileno(pFDImage->pImageFile), nSeekSector, SEEK_SET) == -1 )
	{
		return SECTOR_RANGE_ERROR; /* SetSectorRange */
	}

	lpSector = (unsigned char *)malloc(dwReadSize);

	if( lpSector == NULL )
	{
		return INVALID_PARAMS;
	}

	memcpy(lpSector,Buffer,dwReadSize);

	nResult = WRITEHARDDISKIMAGE(fileno(pFDImage->pImageFile), lpSector, dwReadSize);

	if(nResult != (int)dwReadSize)
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
	else
	{
		nRetval = SUCCESS;
	}

    	free(lpSector);

	return SUCCESS;
}

/***************************************************************************************
	Function	:	OpenHarddiskImage

	Parameters	:	pszImageFilePath - full path of harddisk image file
				pFDImage - specifies the harddisk image object

	Returns		:	SUCCESS upon successful completion
				DEVICE_ALREADY_OPEN if the device is already in open state.
				DEVICE_PATH_TOO_LONG if the length of path string exceeds \ref MAX_DEVICE_PATH_SIZE.
				WRONG_MEDIA if the image file is not a valid ISO9660 image file.
				NO_MEDIA if no file exists in specified location.
				MEDIA_ERROR if invalid file
				UNREADABLE_MEDIA if image file is unreadable.

	Description	:	Opens the harddisk image file.
				validates the harddisk image file.
				returns the object identifying the harddisk image.
****************************************************************************************/
int	OpenHarddiskImage(char *pszImageFilePath, HARDDISKIMAGE *pFDImage)
{
	int		nRetval;

	if ( pFDImage->bIsOpened == 1)
	{
		TWARN("CloseHarddiskImage is not called for previous image file\n");
		return DEVICE_ALREADY_OPEN;
	}

	pFDImage->bIsOpened = 0;

	/* Check the boundaries */
	if( strlen(pszImageFilePath) > MAX_DEVICE_PATH_SIZE )
	{
		TWARN("Image file path is too long.\n");
		return DEVICE_PATH_TOO_LONG;
	}
	strcpy(pFDImage->szImageFile, pszImageFilePath);

	/* Open Harddisk Image*/
	nRetval = OpenImageReadOnly(pFDImage);

	if(nRetval != SUCCESS)
	{
		return nRetval;
	}

	/* Test Unit Ready*/
	nRetval = TestUnitReady(pFDImage);
	if(nRetval != SUCCESS)
	{
		#ifdef _WIN32 /*Joy added*/
		CloseHandle((HANDLE)pFDImage->pImageFile);
		#else
  		fclose(pFDImage->pImageFile);
		#endif
		return nRetval;
	}

	/* Bit of confusion about this */
//	pFDImage->ByteOrder	= littleendian;
	pFDImage->ByteOrder	= bigendian;

	pFDImage->bFirstTime	= 1;
	pFDImage->bIsOpened	= 1;
	//if(pFDImage->bIsReadOnly )
	//pFDImage->bIsReadOnly	= false;
	#ifdef _WIN32 /*Joy added*/
	CloseHandle((HANDLE)pFDImage->pImageFile);
	#else
	fclose(pFDImage->pImageFile);
	#endif
	return SUCCESS;
}

/***************************************************************************************
	Function	:	CloseHarddiskImage

	Parameters	:	pFDImage - specifies the harddisk image object

	Returns		:	SUCCESS always

	Description	:	Close the harddisk image and reset the object
****************************************************************************************/
int CloseHarddiskImage(HARDDISKIMAGE *pFDImage)
{
	if( pFDImage->bIsOpened )
	{
		pFDImage->bIsOpened = 0;
	}

	return SUCCESS;
}

/***************************************************************************************
	Function	:	SetFloppyByteOrder

	Parameters	:	ByteOrder - set the byte order of the card.
				pFDImage - specifies the harddisk image object

	Returns		:	SUCCESS always

	Description	:	This function is useful when used from client side.
				it is not useful when used in the card(firmware).
				It is used to take a note of byte order used by
				the card. This byte order will be useful when
				parsing the commands from MegaRAC-Card.
****************************************************************************************/
void SetHarddiskByteOrder(enum byteOrder ByteOrder, HARDDISKIMAGE *pFDImage )
{
	pFDImage->ByteOrder = ByteOrder;
}


void	SetHDImageReadOnly(bool status, HARDDISKIMAGE *pFDImage )
{
	pFDImage->bIsReadOnly	= true;
}
/***************************************************************************************
	Function	:	ExecuteSCSICmd

	Parameters	:	pRequestPkt  - command request from host/MegaRAC-Card
				pResponsePkt - response packet to host/MegaRAC-Card.
				pdwDataLen - Length of the data added to response packet.
				pFDImage - specifies the harddisk image object


	Returns		:	SUCCESS upon successful completion
				Error Codes on any other Error.

	Description	:	Execute the command sent from host/card.
				Form a response packet and add data to it if any.
****************************************************************************************/
#if defined (__x86_64__) || defined (WIN64)
int ExecuteHarddiskImageSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,IUSB_SCSI_PACKET *pResponsePkt,
				unsigned int *pdwDataLen, HARDDISKIMAGE *pFDImage)
#else
int ExecuteHarddiskImageSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,IUSB_SCSI_PACKET *pResponsePkt,
				unsigned long *pdwDataLen, HARDDISKIMAGE *pFDImage)
#endif				
{
	int			nRetval=SUCCESS;
    int			nRet=SUCCESS;
	SCSI_COMMAND_PACKET	*ScsiPkt;
	READ_CAPACITY		*ScsiReadCap;
	HD_READ_FORMAT_CAPACITIES	*ReadFmtCaps;
#if defined (__x86_64__)|| defined (WIN64)
	unsigned int		dwDataLen=0;
	unsigned int		dwSectorNo, dwSectors;
	unsigned int	*pdwData;
#else
	unsigned long		dwDataLen=0;
	unsigned long		dwSectorNo, dwSectors;
	DWORD	*pdwData;
#endif
	/* Some Fields has to be sent back. So copy all as of now */
	memcpy(pResponsePkt,pRequestPkt,sizeof(IUSB_SCSI_PACKET));

	ScsiPkt=(SCSI_COMMAND_PACKET *)(&(pRequestPkt->CommandPkt));

	dwDataLen	=	0;

	/* It fails here if OpenDevice function is not used before using this
	function. So instead of returning error, just say NO_MEDIA.
	*/
	if( !pFDImage->bIsOpened )
	{
		TWARN("ExecuteSCSICmd called before using OpenDevice\n");
		nRetval = NO_MEDIA;
	}
	else if( pFDImage->bFirstTime == 1 )
	{
		TDBG( "Setting medium change for first time run\n" );
		nRetval = MEDIUM_CHANGE;
		pFDImage->bFirstTime = 0;
#if defined ICC_OS_MACH
		if(pFDImage->bIsPhysicalDrive)
			umountCount = 1;
		else
			umountCount = 0;
#endif
	}
	else
	{
		nRetval = OpenImageReadOnly(pFDImage);
	}

 	if(nRetval == SUCCESS )
	{
		/* process each command */
		switch (ScsiPkt->OpCode)
		{
			case SCSI_FORMAT_UNIT:
				nRetval = SUCCESS;
				break;
			case SCSI_START_STOP_UNIT:
				nRetval = SUCCESS;
				break;
			case SCSI_TEST_UNIT_READY:
				nRetval = TestUnitReady(pFDImage);
				break;
			case SCSI_MEDIUM_REMOVAL:
				nRetval = SUCCESS;
				break;
			case SCSI_READ_FORMAT_CAPACITIES:
				ReadFmtCaps = (HD_READ_FORMAT_CAPACITIES *)(&(pResponsePkt->Data));
				ReadFmtCaps->ListLength  = 8;

				nRetval = ReadCapacity(pFDImage);

				if(nRetval == SUCCESS )
				{
					ReadFmtCaps->BlockSize   = pFDImage->BlockSize | 0x02000000;
					ReadFmtCaps->TotalBlocks = pFDImage->TotalSectors;
				}
				else
				{
					ReadFmtCaps->BlockSize   = 512 | 0x03000000;
					ReadFmtCaps->TotalBlocks = 2880;
				}

				if( pFDImage->ByteOrder == bigendian )
				{
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
					ReadFmtCaps->BlockSize   = mac2blong( ReadFmtCaps->BlockSize );
					ReadFmtCaps->TotalBlocks = mac2blong( ReadFmtCaps->TotalBlocks );
#endif
				}
				dwDataLen = sizeof( HD_READ_FORMAT_CAPACITIES );
				break;

			case SCSI_READ_CAPACITY:
				ScsiReadCap = (READ_CAPACITY *)(&(pResponsePkt->Data));

				nRetval = ReadCapacity(pFDImage);

				if(nRetval == SUCCESS )
				{
					if( pFDImage->ByteOrder == bigendian )
					{
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
						ScsiReadCap->BlockSize   = mac2blong( pFDImage->BlockSize );
						ScsiReadCap->TotalBlocks = mac2blong( pFDImage->TotalSectors - 1 );
#else
						hd_write_unaligned_int32( (uint8 *)&ScsiReadCap->BlockSize, pFDImage->BlockSize );
						hd_write_unaligned_int32( (uint8 *)&ScsiReadCap->TotalBlocks, pFDImage->TotalSectors - 1 );
#endif
					}
					else
					{
						ScsiReadCap->BlockSize   = pFDImage->BlockSize;
						ScsiReadCap->TotalBlocks = pFDImage->TotalSectors-1;
					}

					dwDataLen = 8;
				}
				else
					dwDataLen = 0;
				break;

			case SCSI_READ_10:
			case SCSI_READ_12:
				dwSectorNo = hd_read_unaligned_int32( (uint8 *)&ScsiPkt->Lba );
				if(ScsiPkt->OpCode == SCSI_READ_10 )
				dwSectors = hd_read_unaligned_int16( (uint8 *)&ScsiPkt->CmdLen.Cmd10.Length );
				else
				dwSectors = hd_read_unaligned_int16( (uint8 *)&ScsiPkt->CmdLen.Cmd12.Length32 );

				/* convert the from bigendian*/
				if( pFDImage->ByteOrder == bigendian )
				{
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
					dwSectorNo = mac2blong(dwSectorNo);
					dwSectors  = mac2bshort(dwSectors);
#endif
				}

				if( dwSectors <= MAX_HARDDISK_READ_SECTORS )
				{
					/* Read till successfull */
					nRetval = ReadHarddisk(pFDImage, dwSectorNo,
								dwSectors,
								&dwDataLen, (char *)(&(pResponsePkt->Data)));

					// To support Virtual harddisk boot, the following
					// values must be modified everytime we supply 
					// the zeroth sector
					//  bsHiddenSectors must be 0
					//  bsHeads must be 0
					//  bsSecPerTrack must be 0x3f for devices larger than 8mb
					//  bsHeads must be 0xff for devices  larger than 8mb
					if( nRetval == SUCCESS && dwSectorNo == 0)
					{
					#if defined (__x86_64__) || defined (WIN64)
						unsigned int SectorSize = pFDImage->BlockSize;
						unsigned int SectorCount = pFDImage->TotalSectors;					
					#else
						unsigned long SectorSize = pFDImage->BlockSize;
						unsigned long SectorCount = pFDImage->TotalSectors;
					#endif
						
						if( SectorCount >= (NUM_OF_TRACKS*NUM_OF_HEADS) )
						{
							if( IsHDFSFAT32((unsigned char *)(&(pResponsePkt->Data)) ) )
							{
								BOOTSECTOR_32 *pBootSector = (BOOTSECTOR_32 *)(&(pResponsePkt->Data));
								pBootSector->bsParamBlock.bsHiddenSectors = 0;
								pBootSector->bsDriveNumber = 0;

// this is the actual harddisk data and not scsi protocol.
// the boot sector structure will always be in little endian.
#ifdef ICC_PLATFORM_LITTLE_ENDIAN 
								pBootSector->bsParamBlock.bsSecPerTrack = NUM_OF_TRACKS;
								pBootSector->bsParamBlock.bsHeads = NUM_OF_HEADS;
#else
								pBootSector->bsParamBlock.bsSecPerTrack = mac2bshort(NUM_OF_TRACKS);
								pBootSector->bsParamBlock.bsHeads = mac2bshort(NUM_OF_HEADS);
#endif
							}
							else if( IsHDFSFAT16((unsigned char *)(&(pResponsePkt->Data)) ) )
							{
								BOOTSECTOR_12_16 *pBootSector = (BOOTSECTOR_12_16 *)(&(pResponsePkt->Data));
								pBootSector->bsParamBlock.bsHiddenSectors = 0;
								pBootSector->bsDriveNumber = 0;

// this is the actual harddisk data and not scsi protocol.
// the boot sector structure will always be in little endian.
#ifdef ICC_PLATFORM_LITTLE_ENDIAN 
								pBootSector->bsParamBlock.bsSecPerTrack = NUM_OF_TRACKS;
								pBootSector->bsParamBlock.bsHeads = NUM_OF_HEADS;
#else
								pBootSector->bsParamBlock.bsSecPerTrack = mac2bshort(NUM_OF_TRACKS);
								pBootSector->bsParamBlock.bsHeads = mac2bshort(NUM_OF_HEADS);
#endif
							}

						}
					}
				}
				else
				{
					nRetval = INVALID_PARAMS;
				}
#if defined (__x86_64__) || defined (WIN64)
				pdwData = (unsigned int *)(&(pResponsePkt->Data));
#else
				pdwData = (DWORD *)(&(pResponsePkt->Data));
#endif
				//TINFO("Read 0x%x sectors starting from 0x%x\n", dwSectors, dwSectorNo);
				//TINFO("Start:0x%x, End:0x%x\n", pdwData[0], pdwData[(dwDataLen/4)-1]);
				break;

			case SCSI_WRITE_10:
				/* Close the drive and open in write mode */
				CloseImage(pFDImage);

				nRetval = OpenImageWritable(pFDImage);
				if( nRetval == SUCCESS )
				{
					dwSectorNo = ScsiPkt->Lba;
					dwSectors  = ScsiPkt->CmdLen.Cmd10.Length;

					/* convert the from bigendian*/
					if( pFDImage->ByteOrder == bigendian )
					{
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
						dwSectorNo = mac2blong(dwSectorNo);
						dwSectors  = mac2bshort(dwSectors);
#endif
					}

					if( dwSectors <= MAX_HARDDISK_READ_SECTORS)
					{
						// ATTENTION ATTENTION
						// We are doing a stunt while reading sector zero
						// by masking HiddenSectors and drive number.
						// In the same way, we should not change this values
						// while writing sector zero.
						// So while writing sector zero, we have to replace
						// the above values with the actual values.
						if( dwSectorNo == 0 )
						{
							BYTE SectZero[512];
							int nRet;
					#if defined (__x86_64__) || defined (WIN64)
							unsigned int dwTemp=0;
						#else
							DWORD dwTemp=0;
						#endif

							nRet = ReadHarddisk(pFDImage,0, 1, &dwTemp, (char*)SectZero);

							if( nRet== SUCCESS )
							{
					#if defined (__x86_64__) || defined (WIN64)
								unsigned int SectorSize = pFDImage->BlockSize;
								unsigned int SectorCount = pFDImage->TotalSectors;							
							#else
								unsigned long SectorSize = pFDImage->BlockSize;
								unsigned long SectorCount = pFDImage->TotalSectors;
							#endif
						

								if( SectorCount >= (NUM_OF_TRACKS*NUM_OF_HEADS) )
								{
									if( IsHDFSFAT32((unsigned char *)(&(pResponsePkt->Data)) ) )
									{	
										BOOTSECTOR_32 *pBootSector = (BOOTSECTOR_32 *)(&(pResponsePkt->Data));
										BOOTSECTOR_32 *pOrigBootSector = (BOOTSECTOR_32 *)(SectZero);

										pBootSector->bsParamBlock.bsHiddenSectors = pOrigBootSector->bsParamBlock.bsHiddenSectors;
										pBootSector->bsDriveNumber = pOrigBootSector->bsDriveNumber;
										pBootSector->bsParamBlock.bsSecPerTrack = pOrigBootSector->bsParamBlock.bsSecPerTrack;
										pBootSector->bsParamBlock.bsHeads = pOrigBootSector->bsParamBlock.bsHeads;
									}
									else if( IsHDFSFAT16((unsigned char *)(&(pResponsePkt->Data)) ) )
									{
										BOOTSECTOR_12_16 *pBootSector = (BOOTSECTOR_12_16 *)(&(pResponsePkt->Data));
										BOOTSECTOR_12_16 *pOrigBootSector = (BOOTSECTOR_12_16 *)(SectZero);

										pBootSector->bsParamBlock.bsHiddenSectors = pOrigBootSector->bsParamBlock.bsHiddenSectors;
										pBootSector->bsDriveNumber = pOrigBootSector->bsDriveNumber;
										pBootSector->bsParamBlock.bsSecPerTrack = pOrigBootSector->bsParamBlock.bsSecPerTrack;
										pBootSector->bsParamBlock.bsHeads = pOrigBootSector->bsParamBlock.bsHeads;
									}

								}
							}
						}
						/* Read till successfull */
						nRetval = WriteHarddisk(pFDImage, dwSectorNo,
									dwSectors,
									(char *)(&(pRequestPkt->Data)));
					}
					else
					{
						nRetval = INVALID_PARAMS;
					}
				}

#if defined (__x86_64__) || defined (WIN64)
				pdwData = (unsigned int *)(&(pResponsePkt->Data));
#else
				pdwData = (DWORD *)(&(pResponsePkt->Data));
#endif
				//TINFO("Write 0x%x sectors starting from 0x%x\n", dwSectors, dwSectorNo);
				//TINFO("Start:0x%x, End:0x%x\n", pdwData[0], pdwData[(dwDataLen/4)-1]);
				dwDataLen = 0;
				break;
			/* Mode sense */
			case SCSI_MODE_SENSE:
				{
					unsigned char	nPageCode = (unsigned char)((ScsiPkt->Lba)&0x3f);

					dwDataLen=0;
					// we support only Flexible Disk page (05h)
					if( nPageCode == FLEXIBLE_DISK_PAGE_CODE )
					{
						MODE_SENSE_RESPONSE	*ModeSenseResp = 
									(MODE_SENSE_RESPONSE *) (&(pResponsePkt->Data));
					#if defined (__x86_64__) || defined (WIN64)
						unsigned int SectorSize=0;
						unsigned int SectorCount=0;					
					#else
						unsigned long SectorSize=0;
						unsigned long SectorCount=0;
					#endif
						FLEXIBLE_DISK_PAGE	*DiskPage;
						unsigned short NumberofCylinders;
						unsigned char NumofHeads=1;
						unsigned char SectorsPerTrack=1;
						
						nRetval = ReadCapacity(pFDImage);

						if( nRetval == SUCCESS )
						{
							unsigned char FirstSector[2048];
					#if defined (__x86_64__) || defined (WIN64)
							unsigned int	dwTemp=0;						
						#else
							unsigned long	dwTemp=0;
						#endif

							SectorSize = pFDImage->BlockSize;
							SectorCount = pFDImage->TotalSectors;

							nRetval = ReadHarddisk(pFDImage, 0,1,&dwTemp, (char *)FirstSector);

							if( nRetval == SUCCESS )
							{
								BPB *pBootSector = (BPB *)FirstSector;

								if( SectorCount >= (NUM_OF_TRACKS*NUM_OF_HEADS) )
								{
									SectorsPerTrack = NUM_OF_TRACKS;
									NumofHeads = NUM_OF_HEADS;
								}
								else
								{
// this is the actual harddisk data and not scsi protocol.
// the boot sector structure will always be in little endian.
#ifdef ICC_PLATFORM_LITTLE_ENDIAN								
									NumofHeads = (unsigned char) pBootSector->bsHeads;
									SectorsPerTrack = (unsigned char)pBootSector->bsSecPerTrack;
#else
									NumofHeads = (unsigned char) mac2bshort(pBootSector->bsHeads);
									SectorsPerTrack = (unsigned char) mac2bshort(pBootSector->bsSecPerTrack);
#endif
								}

								// to avoid divide by zero
								if( NumofHeads == 0 ) NumofHeads = 1;
								if( SectorsPerTrack == 0 ) SectorsPerTrack = 1;
							}
						}

						if( nRetval == SUCCESS )
						{
							memset(ModeSenseResp, 0, sizeof(MODE_SENSE_RESPONSE));
							//Mode sense header
							ModeSenseResp->ModeSenseRespHdr.ModeDataLen = 
								(FLEXIBLE_DISK_PAGE_LEN+sizeof(MODE_SENSE_RESP_HDR)-2);
#ifdef ICC_PLATFORM_LITTLE_ENDIAN								
								ModeSenseResp->ModeSenseRespHdr.ModeDataLen = 
								mac2bshort(ModeSenseResp->ModeSenseRespHdr.ModeDataLen);
#endif
							
							if( SectorSize == 512 && SectorCount == 2880 )
							{ //harddisk
								ModeSenseResp->ModeSenseRespHdr.MediumTypeCode = MEDIUM_TYPE_144_MB;
							}
							else
							{//usbdisk
								ModeSenseResp->ModeSenseRespHdr.MediumTypeCode = MEDIUM_TYPE_DEFAULT;
							}

							// Mode sense data
							DiskPage = 
											(FLEXIBLE_DISK_PAGE*)(&ModeSenseResp->PageData[0]);

							DiskPage->PageCode = FLEXIBLE_DISK_PAGE_CODE;
							DiskPage->PageLength = 0x1E;
							DiskPage->NumberofHeads	= NumofHeads;
							DiskPage->SectorsPerTrack = SectorsPerTrack;
							if( SectorSize == 512 && SectorCount == 2880 )
							{ //harddisk
								DiskPage->TransferRate = 0x01f4; //500kbit/s
								DiskPage->MotorONDelay = 0x05;
								DiskPage->MotorOFFDelay = 0x1e;
								DiskPage->MediumRotationRate = 0x12c;
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
								DiskPage->TransferRate = mac2bshort(DiskPage->TransferRate);
								DiskPage->MediumRotationRate = mac2bshort(DiskPage->MediumRotationRate);
#endif
							}
							else
							{//usbdisk
								DiskPage->TransferRate = 0xf000; //500kbit/s
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
								DiskPage->TransferRate = mac2bshort(DiskPage->TransferRate);
#endif
							}
							
							NumberofCylinders = 
									(unsigned short)(SectorCount/(DiskPage->NumberofHeads*DiskPage->SectorsPerTrack));
							DiskPage->NumberofCylinders = NumberofCylinders;
							DiskPage->DataBytesPerSector = (unsigned short)SectorSize;
#ifdef ICC_PLATFORM_LITTLE_ENDIAN
							DiskPage->NumberofCylinders = mac2bshort(DiskPage->NumberofCylinders);
							DiskPage->DataBytesPerSector = mac2bshort(DiskPage->DataBytesPerSector);
#endif
							dwDataLen = FLEXIBLE_DISK_PAGE_LEN + sizeof(MODE_SENSE_RESP_HDR);
						}
						
					}
					else
					{
						nRetval = UNSUPPORTED_COMMAND;
					}
				}
				break;
			case SCSI_MODE_SENSE_6 :
				{
					unsigned long SectorSize=0;
					unsigned long SectorCount=0;
					
					SCSI_MODE_SENSE_6_HEADER *ModeSenseResp = 
					(SCSI_MODE_SENSE_6_HEADER *) (&(pResponsePkt->Data));
			
					dwDataLen=0;

					nRetval = ReadCapacity(pFDImage);
			 
					if(nRetval ==SUCCESS)
					{
				
						memset(ModeSenseResp, 0, sizeof(SCSI_MODE_SENSE_6_HEADER));
				
						ModeSenseResp->Datalen=sizeof(SCSI_MODE_SENSE_6_HEADER)-1;
						nRet = OpenImageWritable(pFDImage);
				
						if(nRet == WRITE_PROTECT_ERROR)
						{
							ModeSenseResp->Devicespecific = 0x80;
						}
						else
						{
							ModeSenseResp->Devicespecific = 0;								
					
						}
						if( SectorSize == 512 && SectorCount == 2880 )
						{ //harddisk
							ModeSenseResp->Mediumtype = MEDIUM_TYPE_144_MB;
						}
						else
						{//usbdisk
							ModeSenseResp->Mediumtype = MEDIUM_TYPE_DEFAULT;
						}	
					dwDataLen = sizeof(SCSI_MODE_SENSE_6_HEADER);
					}
					else
					nRetval = UNSUPPORTED_COMMAND;
				}	

			break;
			default:
				nRetval = UNSUPPORTED_COMMAND;
				break;
		}/* end switch case*/

		CloseImage(pFDImage);
	}/* nRetval; == SUCCESS */

	/* Set the error flags for response packet based on the error */
	SetErrorStatus(nRetval, pResponsePkt);

	*pdwDataLen = dwDataLen;
#ifdef ICC_PLATFORM_BIG_ENDIAN
    /* The data length passed to the remote device must be little endian */
    dwDataLen = mac2blong( dwDataLen );
#endif

	hd_write_unaligned_int32( (uint8 *)&pResponsePkt->DataLen, dwDataLen );

	return nRetval;
}
