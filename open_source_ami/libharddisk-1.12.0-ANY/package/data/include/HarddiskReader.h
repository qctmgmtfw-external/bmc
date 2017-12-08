/*****************************************************************
*	File		:	HarddiskReader.h								 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-July-2003								 *
*	Description	:	Defines Harddisk redirection routines			 *
******************************************************************/

/** @mainpage Documentation for Harddisk Library
*
* @author Rajasekhar (rajasekharj@amiindia.co.in)
* @date   18-July-2003
*
* @brief Libraries for Harddisk Redirection.
* Harddisk redirection involves presenting a virtual Harddisk device to 
* the managed server. This virtual Harddisk may actually be serviced 
* from various sources to accomplish Harddisk redirection.
*
* <br>The following sources are considered for now <br><br>
*		<b>1)</b>	Actual Physical Harddisk media in Remote Console machine<br>
*		<b>2)</b>	Harddisk image file in Remote Console Machine<br>
*		<b>3)</b>	Harddisk Image file in G3 cards network accessible via CIFS/NFS share<br>
*/

/**
* @file HarddiskReader.h
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-July-2003
*
* @brief Implements routines for Harddisk Redirection.
*/

# ifndef __HARDDISK_READER_H__
#define __HARDDISK_READER_H__

#include <string>

#include "coreTypes.h"
#include "libmacro.h"
#include "harddiskerrors.h"
#include "iusb.h"
#include "harddiskimagereader.h"

/**
 @brief Contains routines for reading a Harddisk Media/Harddisk Image
*/
class CHarddiskReader {

	protected:
		/** @brief Byte order of megarac-card */
		enum byteOrder		m_nByteOrder;
	
	public:
		/** @brief Useful when called
			\a CreateHarddiskImage or \a LoadFromImage. 
			It can be used by the caller to know the 
			progress of work completed. After calling 
			the functions \aCreateHarddiskImage or 
			\a LoadFromImage, the user can poll this 
			variable for every particular amount of 
			time to know the progress of work completed.
		*/
		unsigned int		m_ProgressPercentage;
		
		/** @brief Useful when called
			\a CreateHarddiskImage or \a LoadFromImage.
			By default this value will be set to false.
			The functions \aCreateHarddiskImage and 
			\a LoadFromImage takes quiet long time to
			complete the operation. If the user wants
			to abort the operation in between, he can set
			the value for this variable to true which will abort
			the operation.
		*/
		bool				m_bFlagStop;

	public:

		/**@brief Opens Harddisk device for reading */
		virtual int		OpenDevice(char *pszDevicePath, enum byteOrder nByteOrder)=0;

		/**@brief Closes Harddisk device */
		virtual int		CloseDevice()=0;

#if defined (__x86_64__) || defined (WIN64)
		/**@brief Execute an operation on Harddisk Device. */
		virtual	int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
											IUSB_SCSI_PACKET *pResponsePkt,
											unsigned int *dwDataLen)=0;
#else
		/**@brief Execute an operation on Harddisk Device. */
		virtual	int		ExecuteSCSICmd(IUSB_SCSI_PACKET *pRequestPkt,
											IUSB_SCSI_PACKET *pResponsePkt,
											unsigned long *dwDataLen)=0;
#endif

		/**@brief List all Harddisk devices connected to the system */
		virtual int		ListHardDrives(char *DriveList,int type)=0;

		/** @brief Generates a harddisk image and stores in a disk file. 
			
			@param pszOutputfile	Path of an image file to be created.
			
			@retval SUCCESS - on success
			@retval nError	on Error \ref ERROR "ERROR_VALUES"
		*/
		virtual int		CreateHarddiskImage(char *pszOutputfile)=0;

		/** @brief Reads a harddisk image file and loads the data
			into the harddisk disk media.
			
			@param pszOutputfile Path of an image file to be copied.
			
			@retval SUCCESS - on success
			@retval nError	on Error \ref ERROR "ERROR_VALUES"
		*/
		virtual	int		LoadFromImage(char *pszImageFile)=0;

		virtual std::string GetVersion()=0;
};

#endif


