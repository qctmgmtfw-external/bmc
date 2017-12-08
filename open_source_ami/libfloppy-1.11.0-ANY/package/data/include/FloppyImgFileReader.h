/*****************************************************************
*	File		:	FloppyImgFileReader.h						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Defines routines for redirecting			 *
						a Floppy image.							 *
******************************************************************/

/**
* @file FloppyImgFileReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Defines routines for redirecting a Floppy image.
* This is a C++ wrapper class above the "C" routines implemented
* in imagereader.c
*/

#ifndef __FLOPPY_IMAGE_READER_CLASS_H__
#define __FLOPPY_IMAGE_READER_CLASS_H__

#include <string>
#include "FloppyReader.h"

/**
 @brief Contains routines for redirecting Floppy Image
*/
class CFloppyImgFileReader:public CFloppyReader{

	private:
		/** @brief represents a Floppy Image. */
		FLOPPYIMAGE 	m_FloppyImage;
	public:

		/** @brief Constructor. */
		CFloppyImgFileReader();
		
		/** @brief Destructor. */
		virtual ~CFloppyImgFileReader();

		/**@brief Opens Floppy image for reading.
			Opens the Floppy image file specified by \a pszDevicePath.

			@param pszDevicePath Full path of the Floppy image file.
			@param nByteOrder byte order of the orginating system.

		    @retval SUCCESS if the file is successfully opened.
		    @retval DEVICE_ALREADY_OPEN if the device is already in open state.
		    @retval DEVICE_PATH_TOO_LONG if the length of path string exceeds \ref MAX_DEVICE_PATH_SIZE.
		    @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
		*/
		int		OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder);

		/** @brief Close the Floppy image file.
		    	Close the Floppy image file and Stops all the transactions.

		    @retval SUCCESS
		*/
		int		CloseDevice();

		/** @brief Executes the Command by redirecting it to the Floppy image file.
			Execute the command on the image file and fills the result in respomse packet.
			It also returns the datalength if any data added to	the response packet.

		    @param pRequestPkt refers to the Request command packet that is to executed.
		    @param pResponsePkt refers the result of the operation.
		    @param dwDataLen	refers to the length of data if any attached to the response packet.

		    @retval DEVICE_ERROR if the device is not opened.
		    @retval Error value if any other error occurs \ref SUCCESS "ERROR_VALUES"
		*/
#if defined (__x86_64__) || defined (WIN64)
		int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
											IUSB_SCSI_PACKET *pResponsePkt,
											unsigned int *dwDataLen);
#else
		int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
											IUSB_SCSI_PACKET *pResponsePkt,
											unsigned long *dwDataLen);
#endif

		/**@brief This function has no effect. */
		int		ListFloppyDrives(char *DriveList);
		
		/**@brief Creates a copy an image file. */
		int		CreateFloppyImage(char *pszOutputfile);
		
		/**@brief copys the content another image file. */
		int		LoadFromImage(char *pszImageFile);

		std::string GetVersion();
};

#endif
