/*****************************************************************
*	File		:	FloppyReader.h								 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Defines Floppy redirection routines			 *
******************************************************************/

/** @mainpage Documentation for Floppy Library
*
* @author Rajasekhar (rajasekharj@amiindia.co.in)
* @date   18-July-2003
*
* @brief Libraries for Floppy Redirection.
* Floppy redirection involves presenting a virtual Floppy device to 
* the managed server. This virtual Floppy may actually be serviced 
* from various sources to accomplish Floppy redirection.
*
* <br>The following sources are considered for now <br><br>
*		<b>1)</b>	Actual Physical Floppy media in Remote Console machine<br>
*		<b>2)</b>	Floppy image file in Remote Console Machine<br>
*		<b>3)</b>	Floppy Image file in G3 cards network accessible via CIFS/NFS share<br>
*/

/**
* @file FloppyReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Implements routines for Floppy Redirection.
*/

# ifndef __FLOPPY_READER_H__
#define __FLOPPY_READER_H__

#include <string>

#include "coreTypes.h"
#include "libmacro.h"
#include "floppyerrors.h"
#include "iusb.h"
#include "floppyimagereader.h"

/**
 @brief Contains routines for reading a Floppy Media/Floppy Image
*/
class CFloppyReader {

	protected:
		/** @brief Byte order of megarac-card */
		enum byteOrder		m_nByteOrder;
	
	public:
		/** @brief Useful when called
			\a CreateFloppyImage or \a LoadFromImage. 
			It can be used by the caller to know the 
			progress of work completed. After calling 
			the functions \aCreateFloppyImage or 
			\a LoadFromImage, the user can poll this 
			variable for every particular amount of 
			time to know the progress of work completed.
		*/
		unsigned int		m_ProgressPercentage;
		
		/** @brief Useful when called
			\a CreateFloppyImage or \a LoadFromImage.
			By default this value will be set to false.
			The functions \aCreateFloppyImage and 
			\a LoadFromImage takes quiet long time to
			complete the operation. If the user wants
			to abort the operation in between, he can set
			the value for this variable to true which will abort
			the operation.
		*/
		bool				m_bFlagStop;

	public:

		/**@brief Opens Floppy device for reading */
		virtual int		OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)=0;

		/**@brief Closes Floppy device */
		virtual int		CloseDevice()=0;

#if defined (__x86_64__) || defined (WIN64)
		/**@brief Execute an operation on Floppy Device. */
		virtual	int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
											IUSB_SCSI_PACKET *pResponsePkt,
											unsigned int *dwDataLen)=0;
#else
		/**@brief Execute an operation on Floppy Device. */
		virtual	int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
											IUSB_SCSI_PACKET *pResponsePkt,
											unsigned long *dwDataLen)=0;
#endif

		/**@brief List all Floppy devices connected to the system */
		virtual int		ListFloppyDrives(char *DriveList)=0;

		/** @brief Generates a floppy image and stores in a disk file. 
			
			@param pszOutputfile	Path of an image file to be created.
			
			@retval SUCCESS - on success
			@retval nError	on Error \ref ERROR "ERROR_VALUES"
		*/
		virtual int		CreateFloppyImage(char *pszOutputfile)=0;

		/** @brief Reads a floppy image file and loads the data
			into the floppy disk media.
			
			@param pszOutputfile Path of an image file to be copied.
			
			@retval SUCCESS - on success
			@retval nError	on Error \ref ERROR "ERROR_VALUES"
		*/
		virtual	int		LoadFromImage(char *pszImageFile)=0;

		virtual std::string GetVersion()=0;
};

#endif


