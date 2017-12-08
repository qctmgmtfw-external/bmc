/*****************************************************************
*	File		:	HarddiskMediaReader.cpp						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Implements routines for redirecting a		 *
*						Harddisk media.							 *
******************************************************************/

/**
* @file HarddiskMediaReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Implements routines for redirecting a Harddisk media.
*/
#include "HarddiskMediaReader.h"
#include "harddiskerrors.h"
#include "libharddisk.h"
#include <dbgout.h>

/*****************************************************************
	Function	:	SetErrorStatus
	
	Parameters	:	nError		 - Error type
					pResponsePkt - response packet in which the
						error flags to be filled in.
				
	Description	:	sets the error flags in the response packet
					based on the error type.
******************************************************************/
void CHarddiskMediaReader::SetErrorStatus(int nError, IUSB_SCSI_PACKET *pResponsePkt)
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
		case MEDIA_IN_USE:
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
		case WRITE_PROTECT_ERROR:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x07;
			pResponsePkt->StatusPkt.SenseCode		= 0x27;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
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

/*****************************************************************
	Function	:	CreateHarddiskImage
	
	Parameters	:	pszOutputfile - output image file
				
	Description	:	Generates an harddisk image and stores in a 
					disk file.
******************************************************************/
int	CHarddiskMediaReader::CreateHarddiskImage(char *pszOutputfile)
{
	FILE			*pImageFile;
	int				nRetval;
#if defined (__x86_64__) || defined (WIN64)
	unsigned int	dwSectorSize;
	unsigned int	dwTotalSectors;
	unsigned int	dwNotUsed;
#else
	unsigned long	dwSectorSize;
	unsigned long	dwTotalSectors;
	unsigned long	dwNotUsed;
#endif
	char			*Buffer;
	int				nResult;

	/* If OpenDevice function is not used before calling this
		function then the following condition will fail.
	*/
	if( !m_bIsDeviceOpen )
	{
		return NO_MEDIA;
	}

	/* Create/Open an image file in writable mode */
	pImageFile = fopen(pszOutputfile, "wb");

	if(pImageFile == NULL)
	{
		return INVALID_ACCESS;
	}

	/* Get the sector size and sector count parameters .*/
	nRetval = ReadCapacity(&dwSectorSize, &dwTotalSectors);

	if(nRetval == SUCCESS && dwSectorSize > HARDDISK_BLOCK_SIZE)
			nRetval = INVALID_PARAMS;

	if( nRetval != SUCCESS)
	{
		fclose(pImageFile);
		return nRetval;
	}

	nRetval = SUCCESS;
	m_bFlagStop = false;
	m_ProgressPercentage = 0;
	
	Buffer = new char[dwTotalSectors * dwSectorSize];
	
	if(Buffer == NULL)
	{
		fclose(pImageFile);
		return INVALID_PARAMS;	
	}

	int nReadIndex;
	int nWriteIndex;
	int nNumberofSectorsRead = 0;
	int nNumberofSectorsWritten = 0;
	int nNumberofSectorsToWrite = 0;
	int nNumofSectorsToBeRead = dwTotalSectors / 4;

	while(nNumberofSectorsWritten < (int)dwTotalSectors)
	{
		/* Check if user wants to abort the current operation */
		if(m_bFlagStop == true)
		{
			nRetval = USER_ABORT_ERROR;
			break;
		}
		
		nReadIndex = nNumberofSectorsRead * dwSectorSize;
		
		/* Read bulk of sectors at a stretch to save time */
		nRetval = ReadHarddisk(nNumberofSectorsRead,nNumofSectorsToBeRead,&dwNotUsed, &Buffer[nReadIndex]);
		
		if(nRetval != SUCCESS)
			break;
		
		nNumberofSectorsRead += (dwNotUsed /  dwSectorSize);

		/* Write to image file */
		/* Write may take one or more calls to write the data */
		while(nNumberofSectorsWritten < nNumberofSectorsRead)
		{
			nWriteIndex = nNumberofSectorsWritten * dwSectorSize;
			nNumberofSectorsToWrite = nNumberofSectorsRead - nNumberofSectorsWritten;
			
			nResult = fwrite(&Buffer[nWriteIndex], dwSectorSize,nNumberofSectorsToWrite,pImageFile);
			if(nResult == 0)
			{
				nRetval = IMAGE_FILE_WRITE_ERROR;
				break;
			}
			
			nNumberofSectorsWritten += nResult;
		}
		
		
		m_ProgressPercentage = (nNumberofSectorsWritten * 100) / (dwTotalSectors - 1);
	}

	delete[] Buffer;
	fclose(pImageFile);
	return nRetval;
}

/*****************************************************************
	Function	:	LoadFromImage
	
	Parameters	:	pszOutputfile -  path of image file
				
	Description	:	Loads the image file to a harddisk media.
******************************************************************/
int	CHarddiskMediaReader::LoadFromImage(char *pszImageFile)
{
	
	FILE			*pImageFile;
	int				nRetval;
#if defined (__x86_64__) || defined (WIN64)
	unsigned int	dwSectorSize;
	unsigned int	dwTotalSectors;
	unsigned int	dwImageSectorSize;
	unsigned int	dwImageTotalSectors;
#else
	unsigned long	dwSectorSize;
	unsigned long	dwTotalSectors;
	unsigned long	dwImageSectorSize;
	unsigned long	dwImageTotalSectors;
#endif
	char			*Buffer;
	int				nResult;

	/* If OpenDevice function is not used before calling this
		function then the following condition will fail.
	*/
	if( !m_bIsDeviceOpen )
	{
		return NO_MEDIA;
	}

	/* Open an image file in a read mode */
	pImageFile = fopen(pszImageFile, "rb");

	if(pImageFile == NULL)
	{
		return INVALID_ACCESS;
	}

	/* Read the capacity for image file.*/
	nResult = fseek(pImageFile,0L, SEEK_END);
	if(nResult)
	{
		fclose(pImageFile);
		return UNREADABLE_IMAGE_FILE;
	}

	/* get the sector size and total sectors for image file*/
	dwImageSectorSize = HARDDISK_BLOCK_SIZE;
	dwImageTotalSectors = ftell(pImageFile)/dwImageSectorSize;

	/* again set back the pointer to the begining of image file.*/
	/* Read the capacity for image file.*/
	nResult = fseek(pImageFile,0L, SEEK_SET);
	if(nResult)
	{
		fclose(pImageFile);
		return UNREADABLE_IMAGE_FILE;
	}

	/* Read the capacity for harddisk media */
	nRetval = ReadCapacity(&dwSectorSize, &dwTotalSectors);

	if(nRetval != SUCCESS)
	{
		fclose(pImageFile);
		return nRetval;
	}
	
	/* Check whether the image file is compatible with
	harddisk media
	*/
	if( dwSectorSize != dwImageSectorSize ||
			dwImageTotalSectors != dwTotalSectors )
	{
		fclose(pImageFile);
		return INCOMPATIBLE_IMAGE_FILE;
	}


	/* Allocate Buffer for reading image file */
	Buffer = new char[dwTotalSectors * dwSectorSize];
	
	if(Buffer == NULL)
	{
		fclose(pImageFile);
		return INVALID_PARAMS;	
	}

	nRetval = SUCCESS;
	m_bFlagStop = false;
	m_ProgressPercentage = 0;

	int nReadIndex;
	int nWriteIndex;
	int nNumberofSectorsRead = 0;
	int nNumberofSectorsWritten = 0;
	int nNumberofSectorsToWrite = 0;
	int nNumofSectorsToBeRead = dwTotalSectors / 4;
	
	while(nNumberofSectorsWritten < (int)dwTotalSectors)
	{
		/* Check if user wants to abort the current operation */
		if(m_bFlagStop == true)
		{
			nRetval = USER_ABORT_ERROR;
			break;
		}
		
		nReadIndex = nNumberofSectorsRead * dwSectorSize;
		
		/* Read bulk of sectors at a stretch to save time */
		nResult = fread(&Buffer[nReadIndex], dwSectorSize,nNumofSectorsToBeRead,pImageFile);
		//nRetval = ReadHarddisk(nNumberofSectorsRead,nNumofSectorsToBeRead,&dwNotUsed, &Buffer[nReadIndex]);
		
		if(nResult == 0)
		{
			nResult = UNREADABLE_IMAGE_FILE;
			break;
		}
		else if(nResult == nNumofSectorsToBeRead)
			nNumofSectorsToBeRead = dwTotalSectors / 4;
		else
			nNumofSectorsToBeRead -= nResult;
		
		nNumberofSectorsRead += nResult;

		/* Write to harddisk media */
		/* Write may take one or more calls to write the data */
		while(nNumberofSectorsWritten < nNumberofSectorsRead)
		{
			nWriteIndex = nNumberofSectorsWritten * dwSectorSize;
			nNumberofSectorsToWrite = nNumberofSectorsRead - nNumberofSectorsWritten;
			
			nRetval = WriteHarddisk(nNumberofSectorsWritten, nNumberofSectorsToWrite, &Buffer[nWriteIndex]);
			if(nRetval != SUCCESS)
				break;
			
			nNumberofSectorsWritten += nNumberofSectorsToWrite;
		}
		
		m_ProgressPercentage = (nNumberofSectorsWritten * 100) / (dwTotalSectors - 1);
	}
	
	delete[] Buffer;
	fclose(pImageFile);
	
	return nRetval;
}


std::string CHarddiskMediaReader::GetVersion()
{
    std::string version( HARDDISK_VERSION );
    return( version );
}
