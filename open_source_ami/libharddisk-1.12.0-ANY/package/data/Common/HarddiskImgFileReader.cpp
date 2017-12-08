/*****************************************************************
*	File		:	HarddiskImgFileReader.cpp						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Implements routines for redirecting a		 *
*						Harddisk image.							 *
******************************************************************/

/**
* @file HarddiskImgFileReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Implements routines for redirecting a Harddisk image.
* This is a C++ wrapper class above the "C" routines implemented
* in harddiskimagereader.c
*/

#include "coreTypes.h"
#include "libharddisk.h"
#include "harddiskimagereader.h"
#include "HarddiskImgFileReader.h"
#include <dbgout.h>

CHarddiskImgFileReader::CHarddiskImgFileReader(int devicetype,bool physicaldrive)
{
	m_ProgressPercentage=	0;
	m_bFlagStop			=	false;
	m_HarddiskImage.bIsOpened = 0;
	m_HarddiskImage.bIsReadOnly = devicetype;
	m_HarddiskImage.bIsPhysicalDrive = physicaldrive;
}

CHarddiskImgFileReader::~CHarddiskImgFileReader()
{
}

/*****************************************************************
	Function	:	OpenDevice
	
	Parameters	:	pszDevicePath - full path of the Harddisk image.
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
int CHarddiskImgFileReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
{
	int		nRetval;

	nRetval = OpenHarddiskImage(pszDevicePath, &m_HarddiskImage);
	SetHarddiskByteOrder(nByteOrder, &m_HarddiskImage);

	return nRetval;
}

/*****************************************************************
	Function	:	CloseDevice
				
	Returns		:	SUCCESS	- on success.
	
	Description	:	close the image file.
******************************************************************/
int CHarddiskImgFileReader::CloseDevice()
{
	int		nRetval;

	nRetval = CloseHarddiskImage(&m_HarddiskImage);

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

	Description	:	Execute the command on a harddisk image.
					Get the status and fill the response packet.
					Add the data to reponse packet as a result if any.
					fill data-length field.
******************************************************************/
#if defined (__x86_64__)|| defined (WIN64) 
int CHarddiskImgFileReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned int *dwDataLen)
#else
int CHarddiskImgFileReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned long *dwDataLen)
#endif
{
	int		nRetval;

	nRetval = ::ExecuteHarddiskImageSCSICmd(pRequestPkt, pResponsePkt, dwDataLen, &m_HarddiskImage);

	return nRetval;
}
/*
	This is function has no effect.
*/
int CHarddiskImgFileReader::ListHardDrives(char *DriveList,int type)
{

	return SUCCESS;
}

/*****************************************************************
	Function	:	CreateHarddiskImage
	
	Parameters	:	pszOutputfile - output image file
				
	Description	:	Generates another copy of image file
******************************************************************/
int CHarddiskImgFileReader::CreateHarddiskImage(char *pszOutputfile)
{
	FILE		*pImageFile;
	int		nData;

	/* If OpenDevice function is not used before calling this
		function then the following condition will fail.
	*/
	if( m_HarddiskImage.bIsOpened != 1 )
	{
		return NO_MEDIA;
	}

	/* Create/Open an image file in read mode (source)*/
	m_HarddiskImage.pImageFile = fopen(m_HarddiskImage.szImageFile, "rb");

	if(m_HarddiskImage.pImageFile == NULL)
	{
		return UNREADABLE_MEDIA;
	}

	/* Create/Open an image file in writable mode (target)*/
	pImageFile = fopen(pszOutputfile, "wb");

	if(pImageFile == NULL)
	{
		fclose(m_HarddiskImage.pImageFile);
		return INVALID_ACCESS;
	}

	/* read byte by byte */
	while(!feof(m_HarddiskImage.pImageFile))
	{
		nData = fgetc(m_HarddiskImage.pImageFile);
		fputc(nData, pImageFile);
	}

	fclose(m_HarddiskImage.pImageFile);
	fclose(pImageFile);

	return SUCCESS;
}

/*****************************************************************
	Function	:	LoadFromImage
	
	Parameters	:	pszOutputfile -  path of image file
				
	Description	:	copy the image content.
******************************************************************/
int CHarddiskImgFileReader::LoadFromImage(char *pszImageFile)
{
	FILE		*pImageFile;
	int 		nData;

	/* If OpenDevice function is not used before calling this
		function then the following condition will fail.
	*/
	if( m_HarddiskImage.bIsOpened != 1 )
	{
		return NO_MEDIA;
	}

	/* Create/Open an image file in write mode (target)*/
	m_HarddiskImage.pImageFile = fopen(m_HarddiskImage.szImageFile, "wb");

	if(m_HarddiskImage.pImageFile == NULL)
	{
		return UNREADABLE_MEDIA;
	}

	/* Create/Open an image file in read mode (source)*/
	pImageFile = fopen(pszImageFile, "rb");

	if(pImageFile == NULL)
	{
		fclose(m_HarddiskImage.pImageFile);
		return INVALID_ACCESS;
	}

	/* read byte by byte */
	while(!feof(m_HarddiskImage.pImageFile))
	{
		nData = fgetc(pImageFile);
		fputc(nData, m_HarddiskImage.pImageFile);
	}

	fclose(m_HarddiskImage.pImageFile);
	fclose(pImageFile);

	return SUCCESS;
}

std::string CHarddiskImgFileReader::GetVersion()
{
	std::string version( HARDDISK_VERSION );
	return( version );
}
