/*****************************************************************
*	File		:	FloppyImgFileReader.cpp						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Implements routines for redirecting a		 *
*						Floppy image.							 *
******************************************************************/

/**
* @file FloppyImgFileReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Implements routines for redirecting a Floppy image.
* This is a C++ wrapper class above the "C" routines implemented
* in floppyimagereader.c
*/

#include "coreTypes.h"
#include "libfloppy.h"
#include "floppyimagereader.h"
#include "FloppyImgFileReader.h"
#include <dbgout.h>

CFloppyImgFileReader::CFloppyImgFileReader()
{
	m_ProgressPercentage=	0;
	m_bFlagStop			=	false;
    m_FloppyImage.bIsOpened = 0;
}

CFloppyImgFileReader::~CFloppyImgFileReader()
{
}

/*****************************************************************
	Function	:	OpenDevice
	
	Parameters	:	pszDevicePath - full path of the Floppy image.
						The path size must not exceed the limit
						specified.
					nByteOrder	- byte order used by the MegaRAC
						card.
				
	Returns		:	SUCCESS				- on success.
					DEVICE_ALREADY_OPEN - if already in open state.
					ERROR_CODE			- error value if any other error

	Description	:	Checks for the existance of image in specified
						location.
******************************************************************/
int CFloppyImgFileReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
{
	int		nRetval;

	nRetval = OpenFloppyImage(pszDevicePath, &m_FloppyImage);
	SetFloppyByteOrder(nByteOrder, &m_FloppyImage);

	return nRetval;
}

/*****************************************************************
	Function	:	CloseDevice
				
	Returns		:	SUCCESS	- on success.
	
	Description	:	close the image file.
******************************************************************/
int CFloppyImgFileReader::CloseDevice()
{
	int		nRetval;

	nRetval = CloseFloppyImage(&m_FloppyImage);

	return nRetval;
}

/*****************************************************************
	Function	:	ExecuteSCSICmd
	
	Parameters	:	pRequestPkt - command request packet sent by
						the MegaRAC Card.
					pResponsePkt - response packet sent to the
						MegaRAC Card.
					pdwDataLen	- length of data if any data 
						added to the response packet. This will
						be useful while sending the reponse packet
						to the card.
				
	Returns		:	SUCCESS				- on success.
					ERROR_CODE			- error value if any other error

	Description	:	Execute the command on a floppy image.
					Get the status and fill the response packet.
					Add the data to reponse packet as a result if any.
					fill data-length field.
******************************************************************/
#if defined (__x86_64__)|| defined (WIN64) 
int CFloppyImgFileReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned int *dwDataLen)
#else
int CFloppyImgFileReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned long *dwDataLen)
#endif
{
	int		nRetval;

	nRetval = ::ExecuteFloppyImageSCSICmd(pRequestPkt, pResponsePkt, dwDataLen, &m_FloppyImage);

	return nRetval;
}
/*
	This is function has no effect.
*/
int CFloppyImgFileReader::ListFloppyDrives(char *DriveList)
{
	return SUCCESS;
}

/*****************************************************************
	Function	:	CreateFloppyImage
	
	Parameters	:	pszOutputfile - output image file
				
	Description	:	Generates another copy of image file
******************************************************************/
int CFloppyImgFileReader::CreateFloppyImage(char *pszOutputfile)
{
	FILE		*pImageFile;
	int		nData;

	/* If OpenDevice function is not used before calling this
		function then the following condition will fail.
	*/
	if( m_FloppyImage.bIsOpened != 1 )
	{
		return NO_MEDIA;
	}

	/* Create/Open an image file in read mode (source)*/
	m_FloppyImage.pImageFile = fopen(m_FloppyImage.szImageFile, "rb");

	if(m_FloppyImage.pImageFile == NULL)
	{
		return UNREADABLE_MEDIA;
	}

	/* Create/Open an image file in writable mode (target)*/
	pImageFile = fopen(pszOutputfile, "wb");

	if(pImageFile == NULL)
	{
		fclose(m_FloppyImage.pImageFile);
		return INVALID_ACCESS;
	}

	/* read byte by byte */
	while(!feof(m_FloppyImage.pImageFile))
	{
		nData = fgetc(m_FloppyImage.pImageFile);
		fputc(nData, pImageFile);
	}

	fclose(m_FloppyImage.pImageFile);
	fclose(pImageFile);

	return SUCCESS;
}

/*****************************************************************
	Function	:	LoadFromImage
	
	Parameters	:	pszOutputfile -  path of image file
				
	Description	:	copy the image content.
******************************************************************/
int CFloppyImgFileReader::LoadFromImage(char *pszImageFile)
{
	FILE		*pImageFile;
	int 		nData;

	/* If OpenDevice function is not used before calling this
		function then the following condition will fail.
	*/
	if( m_FloppyImage.bIsOpened != 1 )
	{
		return NO_MEDIA;
	}

	/* Create/Open an image file in write mode (target)*/
	m_FloppyImage.pImageFile = fopen(m_FloppyImage.szImageFile, "wb");

	if(m_FloppyImage.pImageFile == NULL)
	{
		return UNREADABLE_MEDIA;
	}

	/* Create/Open an image file in read mode (source)*/
	pImageFile = fopen(pszImageFile, "rb");

	if(pImageFile == NULL)
	{
		fclose(m_FloppyImage.pImageFile);
		return INVALID_ACCESS;
	}

	/* read byte by byte */
	while(!feof(m_FloppyImage.pImageFile))
	{
		nData = fgetc(pImageFile);
		fputc(nData, m_FloppyImage.pImageFile);
	}

	fclose(m_FloppyImage.pImageFile);
	fclose(pImageFile);

	return SUCCESS;
}

std::string CFloppyImgFileReader::GetVersion()
{
    std::string version( FLOPPY_VERSION );
    return( version );
}
