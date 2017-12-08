/*****************************************************************
*	File		:	CDROMImgFileReader.cpp						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Implements routines for redirecting a		 *
*						CDROM image.							 *
******************************************************************/

/**
* @file CDROMImgFileReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Implements routines for redirecting a CDROM image.
* This is a C++ wrapper class above the "C" routines implemented
* in imagereader.c
*/
#include <string>
#include "CDROMImgFileReader.h"
#include "imagereader.h"
#include "dbgout.h"

/*****************************************************************
	Function	:	OpenDevice
	
	Parameters	:	pszDevicePath - full path of the cdrom image.
						The path size must not exceed the limit
						specified.
					nByteOrder	- byte order used by the MegaRAC
						card.
				
	Returns		:	SUCCESS				- on success.
					DEVICE_ALREADY_OPEN - if already in open state.
					ERROR_CODE			- error value if any other error

	Description	:	Checks for the existance of image in specified
						location.
					Validates the format of the image. It should
						be an ISO9660 image.
******************************************************************/
int CCDROMImgFileReader::OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)
{
	int		nRetval;

	nRetval = OpenCDROMImage(pszDevicePath, &m_CDROMImage);
	SetByteOrder(nByteOrder, &m_CDROMImage);

	return nRetval;
}

/*****************************************************************
	Function	:	CloseDevice
				
	Returns		:	SUCCESS	- on success.
	
	Description	:	close the image file.
******************************************************************/
int	CCDROMImgFileReader::CloseDevice()
{
	int		nRetval;

	nRetval = CloseCDROMImage(&m_CDROMImage);

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

	Description	:	Execute the command on a cdrom image.
					Get the status and fill the response packet.
					Add the data to reponse packet as a result if any.
					fill data-length field.
******************************************************************/
#if defined (__x86_64__) || defined (WIN64)
int CCDROMImgFileReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned int *dwDataLen)
#else
int CCDROMImgFileReader::ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt, IUSB_SCSI_PACKET *pResponsePkt, unsigned long *dwDataLen)
#endif
{
	int		nRetval;

	nRetval = ::ExecuteSCSICmd(pRequestPkt, pResponsePkt, dwDataLen, &m_CDROMImage);

	return nRetval;
}

/*	Constructor */
CCDROMImgFileReader::CCDROMImgFileReader()
{
    /* Initialize needed variables */
    m_CDROMImage.bIsOpened = 0;
}

/*	Destructor */
CCDROMImgFileReader::~CCDROMImgFileReader()
{
}

/*
	This is function has no effect.
*/
int CCDROMImgFileReader::ListCDROMDrives(char *DriveList)
{
	return SUCCESS;
}

using std::string;
string CCDROMImgFileReader::GetVersion()
{
    string version( CDROM_VERSION );
    return( version );
}
